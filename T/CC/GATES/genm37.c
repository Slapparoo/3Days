/* M37 gate generator (host tool, gcc-compiled; NOT part of the tree).
 *
 * Reads rows.txt — the KERNEL's own %f renderings, measured in TempleOS by
 * StrPrintJoin over 38 bit patterns x 9 directives — and emits chunked C
 * programs that run the same (value, directive) cells through lite and compare
 * them against those renderings.  Two things make the mapping non-trivial and
 * both are the delegation contract, not a hedge:
 *
 *   lite never hands the engine a directive without a precision: C's `%f`
 *   defaults to 6 and the engine's defaults to 0, so a cell whose directive
 *   spells no precision is expected to match the engine's answer for the SAME
 *   directive with `.6` written in.  rows.txt supplies that row (%f -> %.6f).
 *   Values enter as (significand, exponent) so lite and the kernel see one and
 *   the same double: an integer below 2^53 scaled by a power of two reached by
 *   exact doubling.  A NaN comes from 0.0/-0.0, because x87's 0.0/0.0 yields a
 *   NEGATIVE quiet NaN and the measured pattern is the positive one.
 *
 * Expectation text is built in a buffer by pieces under 128 characters, since
 * a string literal longer than that is refused by the lexer — and the kernel
 * renders 1e300 at `%f` as 248 characters (T:/CC/GATES/rows.txt), so this is
 * not hypothetical.
 *
 *   mode sweep <base>      -> <base>0.c .. + a chunk list on stderr
 *   mode driver <base> <n> -> a HolyC driver that CCRunFile's each chunk
 *   mode expect <base>     -> the expected printf-sink lines, for the harness
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static const char *Dir[9] = {"%f", "%.0f", "%.1f", "%.3f", "%.6f", "%.15f",
    "%.16f", "%10.3f", "%.20f"};
static unsigned long long Bits[64];
static char Ans[64][9][512];
static int Have[64][9];
static int Nids;

/* The gate programs and the measurement table live IN the repo (T:/CC/GATES/),
 * because /tmp on the host is wiped on reboot and a gate that lives there
 * evaporates with it.  These two map a TempleOS path to the host file the
 * generator reads or writes: `T:/CC/GATES/gs` is <repo>/T/CC/GATES/gs.  Override
 * the root with env CC_REPO when running from a worktree. */
static const char *Repo(void)
{
    const char *r = getenv("CC_REPO");
    return (r && *r) ? r : "/mnt/d/Source/3Days";
}

static void HostOf(const char *tos, char *out, size_t n)
{
    if (!strncmp(tos, "T:/", 3))
        snprintf(out, n, "%s/T/%s", Repo(), tos + 3);
    else if (tos[0] == '/')
        snprintf(out, n, "%s", tos);
    else
        snprintf(out, n, "%s/%s", Repo(), tos);
}

static void RowsOf(const char *tos, char *out, size_t n)
{
    char dir[512], *s;
    snprintf(dir, sizeof dir, "%s", tos);
    s = strrchr(dir, '/');
    if (s) *s = 0;
    snprintf(out, n, "%s/rows.txt", dir);
}

static void Parse(const char *path)
{
    FILE *f = fopen(path, "r");
    static char ln[1200];
    int id, k;
    long long bb;
    char *c, *o, *t;
    if (!f) { fprintf(stderr, "no %s\n", path); exit(1); }
    while (fgets(ln, sizeof ln, f))
    {
        if (strncmp(ln, "K ", 2))
            continue;
        t = ln + 2;
        id = atoi(t);
        while (*t && *t != ' ') t++;
        if (!*t) continue;
        t++;
        k = atoi(t);
        while (*t && *t != ' ') t++;
        if (!*t) continue;
        t++;
        bb = strtoll(t, &c, 10);
        o = strchr(c, '[');
        if (!o) continue;
        t = strrchr(o, ']');
        if (!t) continue;
        if (t - o - 1 > 500) { fprintf(stderr, "row too long\n"); exit(1); }
        memcpy(Ans[id][k], o + 1, t - o - 1);
        Ans[id][k][t - o - 1] = 0;
        Have[id][k] = 1;
        Bits[id] = (unsigned long long) bb;
        if (id + 1 > Nids) Nids = id + 1;
    }
    fclose(f);
}

/* The directive lite will actually hand the engine, spelled the way rows.txt
 * spells it: a directive with no precision becomes the same text with `.6`
 * written in ahead of the letter.  Returns the row index, or -1 if that
 * composed directive was never measured (then the cell is skipped). */
static const char *Want(int id, int k)
{
    char want[64];
    int i, j, L, dot = 0;
    if (!Have[id][k])
        return 0;
    L = strlen(Dir[k]);
    for (i = 0; i < L; i++)
        if (Dir[k][i] == '.')
            dot = 1;
    if (dot)
        return Ans[id][k];
    j = 0;
    while (j < L - 1)
    {
        want[j] = Dir[k][j];	/* flags and width first, then the precision */
        j++;
    }
    want[j++] = '.';
    want[j++] = '6';
    want[j++] = Dir[k][L - 1];
    want[j] = 0;
    for (j = 0; j < 9; j++)
        if (!strcmp(Dir[j], want))
            return Have[id][j] ? Ans[id][j] : 0;
    return Ans[id][k];
}

/* C code that produces the value whose bit pattern is b. */
static void Value(FILE *o, int id)
{
    unsigned long long b = Bits[id], m;
    int e, k, neg;
    neg = (b >> 63) != 0;
    e = (int) ((b >> 52) & 0x7FF);
    m = b & 0xFFFFFFFFFFFFFULL;
    fprintf(o, "  /* id %d bits %016llx */\n", id, b);
    if (e == 0x7FF)
    {
        if (m)
            fputs("  x=0.0/0.0;\n  x=-x;\t/* x87s real indefinite is a NEGATIVE"
                " quiet\n\t   NaN, so it is flipped to reach the pattern"
                " measured\n\t   here.  gcc gets the other NaN. */\n", o);
        else
            fputs("  x=1.0/0.0;\n", o);
    }
    else if (e == 0)
        fprintf(o, "  x=%llu; q=p2e(-1074); x=x*q;\n", m);
    else
    {
        k = e - 1075;
        fprintf(o, "  x=%llu; q=p2e(%d); x=x*q;\n", m + (1ULL << 52), k);
    }
    if (neg)
        fputs("  x=-x;\n", o);
}

static void Head(FILE *o, const char *tag)
{
    fprintf(o, "#include <stdio.h>\n#include <string.h>\n");
    fprintf(o, "/* M37 delegation sweep (%s).  Expectations are the kernel's own\n"
        " * renderings of these bit patterns; the value enters as an exact\n"
        " * integer below 2^53 scaled by an exact power of two, so lite and the\n"
        " * kernel see the same double.  Three sinks per cell: printf (the\n"
        " * compile-time lowering, reported as a P line for the harness to\n"
        " * compare), sprintf with a literal directive, and sprintf with the\n"
        " * same directive in a variable (the runtime engine).  A failure\n"
        " * prints FAIL and the program returns the failure count. */\n", tag);
    fputs("static double p2e(int n)\n{\n  double p;\n  int i,j;\n  p=1.0;\n"
        "  j=n;\n  if (j<0)\n  {\n    j=-j;\n"
        "    for (i=0;i!=j;i=i+1)\n      p=p*0.5;\n  }\n"
        "  else\n  {\n    for (i=0;i!=j;i=i+1)\n      p=p+p;\n  }\n"
        "  return p;\n}\n\n", o);
    fputs("static int cells,fails;\nstatic char b[1200],w[512];\n\n"
        "static void ck(int id,int k,const char *w,int n,const char *g,"
        "const char *s)\n{\n  if (((n!=strlen(w))||strcmp(g,w)))\n"
        "  {\n    printf(\"FAIL %d %d %s [%s] want [%s] n=%d\\n\",id,k,s,g,w,n);\n"
        "    fails=fails+1;\n  }\n  cells=cells+1;\n}\n\n", o);
    fputs("int main()\n{\n  double x,q;\n  char *fm;\n  int n;\n", o);
}

/* A want string is loaded into w in pieces.  Two reasons, not one: a literal
 * longer than CC_TOK_MAX (128) is refused by LITE's lexer — 1e300 at `%f`
 * measures 248 characters (T:/CC/GATES/rows.txt) — and a piece over 68
 * characters pushes the emitted `strcpy(w,"...");` line past this tree's 83
 * column discipline, which a generated program has no excuse for breaking. */
static void EmitWant(FILE *o, const char *s)
{
    char piece[128];
    int n = strlen(s), i = 0, m;
    while (i < n)
    {
        m = n - i;
        if (m > 68) m = 68;	/* 14 chars of `  strcpy(w,"");` + NUL <= 83 */
        memcpy(piece, s + i, m);
        piece[m] = 0;
        fprintf(o, "  %s(w,\"%s\");\n", i ? "strcat" : "strcpy", piece);
        i += m;
    }
}

static void Cells(FILE *o, int id, const int *dk, int ndk, int sinks)
{
    int i, k;
    const char *ans;
    for (i = 0; i < ndk; i++)
    {
        k = dk[i];
        ans = Want(id, k);
        if (!ans) continue;
        EmitWant(o, ans);
        if (sinks & 1)
            fprintf(o, "  printf(\"P %d %d|%%s|\",w); printf(\"|%s|\\n\",x);\n",
                id, k, Dir[k]);
        if (sinks & 2)
            fprintf(o, "  n=sprintf(b,\"%s\",x); ck(%d,%d,w,n,b,\"L\");\n",
                Dir[k], id, k);
        if (sinks & 4)
            fprintf(o, "  fm=\"%s\"; n=sprintf(b,fm,x); ck(%d,%d,w,n,b,\"R\");\n",
                Dir[k], id, k);
    }
}

int main(int argc, char **argv)
{
    const char *mode, *base;
    int a, id, nchunk, dk[9], ndk, i;
    char path[256], host[512], tbase[256], *slash;
    FILE *o;

    RowsOf(base = (argc > 2 ? argv[2] : "T:/CC/GATES/gs"), path, sizeof path);
    HostOf(path, host, sizeof host);
    Parse(host);
    /* The label the sweep programs and the driver print is the T:/ form, so a
     * log names the file inside the tree and not a host path that changes
     * between machines. */
    if (!strncmp(base, "T:/", 3))
        snprintf(tbase, sizeof tbase, "%s", base);
    else
    {
        slash = strstr(base, "/T/");
        if (slash)
        {
            strcpy(tbase, "T:");
            strcat(tbase, slash + 1);   /* "/CC/GATES/gs" -> "T:/CC/GATES/gs" */
        }
        else
            snprintf(tbase, sizeof tbase, "%s", base);
    }
    if (argc < 3)
    {
        fprintf(stderr, "usage: gen sweep|driver|expect <T:/CC/GATES/base>"
            " [dir-indices]\n");
        fprintf(stderr, "  e.g. gen sweep T:/CC/GATES/gs | gen driver"
            " T:/CC/GATES/gs 10\n");
        fprintf(stderr, "  gen expect T:/CC/GATES/gs -> the golden printf lines;"
            " T:/x is\n");
        fprintf(stderr, "  written to the host file <repo root>/T/x (env CC_REPO"
            " overrides)\n");
        return 1;
    }
    mode = argv[1];
    ndk = 0;
    for (i = 0; i < 9; i++) dk[ndk++] = i;
    if (argc > 3)
    {
        ndk = 0;
        for (i = 3; i < argc; i++) dk[ndk++] = atoi(argv[i]);
    }
    if (!strcmp(mode, "sweep"))
    {
        nchunk = 0;
        for (a = 0; a < Nids; a += 4)
        {
            sprintf(path, "%s%d.c", tbase, nchunk);
            HostOf(path, host, sizeof host);
            o = fopen(host, "w");
            if (!o) { perror(host); return 1; }
            Head(o, path);
            for (i = a; i < a + 4 && i < Nids; i++)
            {
                Value(o, i);
                Cells(o, i, dk, ndk, 7);
            }
            fprintf(o, "  printf(\"TOT %s %%d cells %%d fails\\n\",cells,fails);\n"
                "  return fails;\n}\n", tbase);
            fclose(o);
            fprintf(stderr, "chunk %d ids %d-%d -> %s\n", nchunk, a,
                (a + 3 < Nids ? a + 3 : Nids - 1), path);
            nchunk++;
        }
        fprintf(stderr, "CHUNKS %d\n", nchunk);
        return 0;
    }
    if (!strcmp(mode, "driver"))
    {
        nchunk = atoi(argv[3]);
        printf("#include \"T:/CC/CC.HC\"\n");
        for (i = 0; i < nchunk; i++)
            printf("\"RUN %%s rc=%%d\\n\",\"%s%d.c\",CCRunFile(\"%s%d.c\");\n",
                tbase, i, tbase, i);
        printf("Exit;\n");
        return 0;
    }
    if (!strcmp(mode, "expect"))
    {
        for (id = 0; id < Nids; id++)
            for (i = 0; i < ndk; i++)
            {
                const char *a2 = Want(id, i);
                if (a2)
                    printf("P %d %d|%s|\n", id, i, a2);
            }
        return 0;
    }
    return 1;
}
