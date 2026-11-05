/* M37 fixture + probe emitter (host tool, gcc-compiled; NOT part of the tree).
 *
 * Emits two views of ONE cell list, derived from rows.txt (the kernel's own
 * measured %f output for 38 bit patterns x 9 directives) plus a small table of
 * directives that were measured for the value 1.5 alone:
 *
 *   ./genffmt          -> T:/CC/TEST/ffmt.c  : C, run by the suite.  Builds
 *                         each value from an exact significand and power of
 *                         two, formats it with the directive as C SOURCE spells
 *                         it, and compares against the rendering.
 *   ./genffmt probe    -> the CCProbeFloat body: HolyC, in the suite.  Calls
 *                         StrPrintJoin on the same bits with the directive lite
 *                         WILL SEND (composed, `*` stripped, numbers clamped)
 *                         and compares against the same rendering.
 *
 * Together they say: lite's output equals the kernel's output, and the
 * composition rules — C's default precision written out, no `*`, the buffer
 * caps — are pinned as part of that equality.  Nothing here is "what C says":
 * where the kernel and C differ, the number is the kernel's and the comment in
 * the emitted file says so.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static unsigned long long Bits[64];
static char Ans[64][9][512];
static int Have[64][9], Nids;
static const char *Dir[9] = {"%f", "%.0f", "%.1f", "%.3f", "%.6f", "%.15f",
    "%.16f", "%10.3f", "%.20f"};

/* The measurement table lives IN the repo (T:/CC/GATES/rows.txt) so the gate
 * survives a host reboot; `T:/CC/GATES/rows.txt` is the host file
 * <repo root>/T/CC/GATES/rows.txt.  Override the root with env CC_REPO. */
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
        if (strncmp(ln, "K ", 2)) continue;
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
        memcpy(Ans[id][k], o + 1, t - o - 1);
        Ans[id][k][t - o - 1] = 0;
        Have[id][k] = 1;
        Bits[id] = (unsigned long long) bb;
        if (id + 1 > Nids) Nids = id + 1;
    }
    fclose(f);
}

/* Directives measured for value 0 (1.5) alone.  PADS is a placeholder the
 * builder expands: the kernel pads a field to the WIDTH AS A TOTAL, which the
 * measured `%010.3f` cell shows with ten characters. */
static int Extra(int id, const char *sent, char *out)
{
    static const struct { int id; const char *sent, *ans; } T[] = {
        {0, "%010.3f", "000001.500"},
        {0, "%-10.3f", "     1.500"},
        {0, "%+.6f", ".6f"},
        {0, "%01000.3f", "P995+1.500"},
        {0, "%100.50f", "P48SP+1.49999999999999648+31Z"},
        {-1, 0, 0}
    };
    int i;
    for (i = 0; T[i].sent; i++)
    {
        if (T[i].id != id || strcmp(T[i].sent, sent))
            continue;
        if (!strncmp(T[i].ans, "P995", 4))
        {
            memset(out, '0', 995);
            strcpy(out + 995, "1.500");
            return 1;
        }
        if (!strncmp(T[i].ans, "P48SP", 5))
        {
            memset(out, ' ', 48);
            strcpy(out + 48,
                "1.49999999999999648000000000000000000000000000000000");
            return 1;
        }
        strcpy(out, T[i].ans);
        return 1;
    }
    return 0;
}

/* What the kernel is handed for a directive written in C source: `.6` written
 * in when no precision is stated (C's default is 6, the engine's is 0), `*`
 * removed (lite cannot say which argument a width would come from), and the
 * field width and the precision clamped to what the destination buffer holds —
 * 1000 and 600 against a 1024-byte field, because StrPrintJoin takes no length.
 * A width run that begins with `0` keeps that zero: the engine reads it as its
 * pad-with-zeros flag and not as a digit. */
static void Sent(const char *src, char *out)
{
    int i = 1, j = 1, dot = 0, n, k;
    const char L = src[strlen(src) - 1];
    out[0] = '%';
    while (src[i])
    {
        if (src[i] == '*') { i++; continue; }
        if (src[i] == '-' || src[i] == '+' || src[i] == ' ' || src[i] == '#')
        {
            out[j++] = src[i++];
            continue;
        }
        if ('0' <= src[i] && src[i] <= '9')
        {
            n = 0; k = i;
            while (src[i] && '0' <= src[i] && src[i] <= '9')
            {
                if (n < 1000000) n = n * 10 + (src[i] - '0');
                i++;
            }
            if (!dot && n > 1000)
            {
                if (src[k] == '0') out[j++] = '0';
                j += sprintf(out + j, "%d", 1000);
            }
            else if (dot && n > 600)
                j += sprintf(out + j, "%d", 600);
            else
                while (k < i) out[j++] = src[k++];
            continue;
        }
        if (src[i] == '.' && !dot)
        {
            dot = 1;
            out[j++] = '.';
            i++;
            continue;
        }
        out[j++] = src[i++];
    }
    out[j] = 0;				//the scan does not terminate, and the
						//precision-less branch below counts
						//on strlen of what it just wrote
    if (!dot)
    {
        /* C writes the precision before the conversion letter, so the letter
         * moves right by two rather than `.6` being pasted at the end. */
        j = 1;
        while (out[j] != L) j++;
        out[j + 2] = out[j];
        out[j] = '.';
        out[j + 1] = '6';
        out[j + 3] = 0;
        j += 3;
    }
    else
        j = strlen(out);
    out[j] = 0;
}

static int WantFor(int id, const char *sent, char *out)
{
    int j;
    if (Extra(id, sent, out))
        return 1;
    for (j = 0; j < 9; j++)
        if (!strcmp(Dir[j], sent) && Have[id][j])
        {
            strcpy(out, Ans[id][j]);
            return 1;
        }
    return 0;
}

static const int Ids[] = {0, 2, 3, 4, 5, 9, 12, 21, 24, 27, 28, 29, -1};
static const char *DirsCore[] = {"%f", "%.3f", "%.15f", "%10.3f", 0};
static const char *DirsWide[] = {"%.0f", "%.1f", "%.16f", "%.20f", "%010.3f",
    "%-10.3f", "%+f", "%*f", "%100.50f", "%01500.3f", 0};

/* The pieces of a rendering: short enough to sit on a source line inside the
 * 83-column style this tree keeps, and short enough to be a C string literal. */
static void Pieces(const char *s)
{
    int n = strlen(s), at = 0, take;
    while (at < n)
    {
        take = n - at;
        if (take > 60) take = 60;
        printf("%s\"%.*s\"", at ? "strcat(w," : "strcpy(w,", take, s + at);
        printf(");\n");
        at += take;
    }
}

int main(int argc, char **argv)
{
    char sent[64], wd[1100];
    int ids[256], nc = 0, i, j, k0;
    char *srcs[256], *sents[256], *wants[256];

    char rp[512];
    HostOf(argc > 2 ? argv[2] : "T:/CC/GATES/rows.txt", rp, sizeof rp);
    Parse(rp);
    for (i = 0; Ids[i] >= 0; i++)
    {
        if (Ids[i] == 0) continue;
        for (j = 0; DirsCore[j]; j++)
        {
            ids[nc] = Ids[i];
            srcs[nc] = strdup(DirsCore[j]);
            nc++;
        }
    }
    for (j = 0; DirsCore[j]; j++)
    {
        ids[nc] = 0;
        srcs[nc] = strdup(DirsCore[j]);
        nc++;
    }
    for (j = 0; DirsWide[j]; j++)
    {
        ids[nc] = 0;
        srcs[nc] = strdup(DirsWide[j]);
        nc++;
    }
    k0 = -1;
    for (i = 0; i < nc; i++)
    {
        Sent(srcs[i], sent);
        sents[i] = strdup(sent);
        if (!WantFor(ids[i], sent, wd))
        {
            fprintf(stderr, "no measurement for id %d directive %s (sent %s)\n",
                ids[i], srcs[i], sent);
            return 1;
        }
        wants[i] = strdup(wd);
        if (k0 < 0 && ids[i] == 0 && !strcmp(srcs[i], "%f"))
            k0 = i;
    }
    fprintf(stderr, "CELLS %d  RUNTIME-CELL-MODEL %d\n", nc, k0);
    if (argc > 1 && !strcmp(argv[1], "probe"))
    {
        fputs("/* M37: the KERNEL side of the ffmt table.\n"
            " * bits, same renderings, no lite anywhere in the middle.\n"
            " * StrPrintJoin is called exactly as cci_fmt calls it: argv is the\n"
            " * address of one quad holding the value's bits, and the directive\n"
            " * is the one lite's two sinks COMPOSE: `.6` written in when the\n"
            " * source stated none, `*` dropped, and the width and the precision\n"
            " * capped to what the destination holds.  This says what the kernel\n"
            " * answers; ffmt.c says lite answers the same.  The pair pins the\n"
            " * delegation and the composition.  A rendering too wide for one\n"
            " * source line is pinned by its length, its first character and its\n"
            " * last eight, which is all a padding rule fixes; the digits are\n"
            " * pinned by the short cells of the same directive nearby. */\n"
            "I64 CCProbeFloat()\n{\n  U8 out[1100];\n  I64 q,nf,L;\n\n"
            "  q=0;\n  nf=0;\n", stdout);
        for (i = 0; i < nc; i++)
        {
            int lw = strlen(wants[i]);
            printf("  q=%lld;\n", (long long) Bits[ids[i]]);
            fputs("  MemSet(out,0,1100);\n", stdout);
            printf("  StrPrintJoin(out,\"%s\",1,&q);\n", sents[i]);
            if (lw <= 70)
            {
                printf("  if (StrCmp(out,\"%s\"))\n  {\n", wants[i]);
                printf("    \"FAIL float cell %d [%%s] want [%s]\\n\",out;\n",
                    i, wants[i]);
            }
            else
            {
                printf("  L=%d;\t\t/* the measured length of this rendering */\n",
                    lw);
                printf("  if ((StrLen(out)!=L)||(out[0]!='%c')||\n"
                    "      StrCmp(out + L - 8,\"%.8s\"))\n  {\n",
                    wants[i][0], wants[i] + lw - 8);
                printf("    \"FAIL float cell %d len %%d [%%s]\\n\""
                    ",StrLen(out);\n", i);
            }
            fputs("    nf=nf+1;\n  }\n", stdout);
        }
        fputs("  return nf;\n}\n", stdout);
        return 0;
    }
    fputs("static double p2e(int n)\n{\n  double p;\n  int i,j;\n  p=1.0;\n"
        "  j=n;\n  if (j<0)\n  {\n    j=-j;\n"
        "    for (i=0;i!=j;i=i+1)\n      p=p*0.5;\n  }\n"
        "  else\n  {\n    for (i=0;i!=j;i=i+1)\n      p=p+p;\n  }\n"
        "  return p;\n}\n\n"
        "int main()\n{\n  double x,q;\n  char b[1200],w[1200];\n  char *fm;\n"
        "  int n,tot,cells;\n\n  tot=0;\n  cells=0;\n", stdout);
    for (i = 0; i < nc; i++)
    {
        unsigned long long bb = Bits[ids[i]];
        int e = (int) ((bb >> 52) & 0x7FF), neg = (bb >> 63) != 0;
        unsigned long long m = bb & 0xFFFFFFFFFFFFFULL;
        printf("  /* cell %d: value %d, bits %016llx, directive %s */\n",
            i, ids[i], bb, srcs[i]);
        if (e == 0x7FF)
        {
            if (m)
                fputs("  x=0.0/0.0;\n  x=-x;\t\t/* x87s real indefinite is a\n"
                    "\t\t   NEGATIVE quiet NaN */\n", stdout);
            else
                fputs("  x=1.0/0.0;\n", stdout);
        }
        else if (e == 0)
            printf("  x=%llu; q=p2e(-1074); x=x*q;\n", m);
        else
            printf("  x=%llu; q=p2e(%d); x=x*q;\n", m + (1ULL << 52), e - 1075);
        if (neg) fputs("  x=-x;\n", stdout);
        Pieces(wants[i]);
        printf("  n=sprintf(b,\"%s\",x);\n", srcs[i]);
        fputs("  tot=tot+(((n==strlen(w))&&(strcmp(b,w)==0)));\n"
            "  cells=cells+1;\n", stdout);
    }
    /* two more cells built from the model above: the same directive read out of
     * a variable, and a format where %%, %c and %d keep the engine's own arms
     * while only the f goes to the kernel. */
    printf("  fm=\"%s\";\t/* the engine reads this format at run time, and the\n",
        srcs[k0]);
    fputs("\t   helper caps and composes it exactly as it did the\n"
        "\t   literal one above */\n", stdout);
    printf("  n=sprintf(b,fm,x);\n");
    Pieces(wants[k0]);
    fputs("  tot=tot+(((n==strlen(w))&&(strcmp(b,w)==0)));\n"
        "  cells=cells+1;\n", stdout);
    fputs("  n=sprintf(b,\"%%%c-%.3f-%d\",97,x,7);\n", stdout);
    fputs("  strcpy(w,\"%a-1.500-7\");\n", stdout);
    fputs("  tot=tot+(((n==10)&&(strcmp(b,w)==0)));\n"
        "  cells=cells+1;\n", stdout);
    /* Three cells pin what the RUNTIME engine does with a flag, a width or a
     * `*` in front of one of ITS OWN conversions.  M37 changed this because
     * reading a %f directive needs a scan, and the scan now runs for every
     * directive: the characters before the letter are buffered and emitted as
     * text, and the letter is then recognised.  Before, the argument was
     * selected, nothing in the chain matched the flag or width characters, so
     * they fell through to the plain-text copy AND the letter fell through
     * with them: `%5d` printed `5d`, `%-2d` printed `-2d`, `%*d` printed `*d`,
     * each having already consumed an argument.  The argument count is the
     * same as it was — one per percent sign — and none of these is C's answer
     * (`%5d` of 5 is `    5`), which is why they are refused outright by
     * printf instead. */
    fputs("  n=sprintf(b,\"%5d %d\",5,7);\t/* the width run is text, the\n"
        "\t   letter is now recognised, and the argument count is\n"
        "\t   unchanged: this printed `5d 7` before M37 */\n"
        "  strcpy(w,\"55 7\");\n"
        "  tot=tot+(((n==4)&&(strcmp(b,w)==0)));\n  cells=cells+1;\n", stdout);
    fputs("  n=sprintf(b,\"%-2d\",7);\t/* was `-2d`: the flag run reaches\n"
        "\t   the output as text either way, but the conversion\n"
        "\t   happens now instead of the letter being copied too */\n"
        "  strcpy(w,\"-27\");\n"
        "  tot=tot+(((n==3)&&(strcmp(b,w)==0)));\n  cells=cells+1;\n", stdout);
    fputs("  n=sprintf(b,\"%*d\",10,7);\t/* `*` is dropped from the text a\n"
        "\t   float hands over; for an integer it is simply not\n"
        "\t   there, so the first argument is what %d prints.  Was\n"
        "\t   `*d`.  C would print `         7` either way. */\n"
        "  strcpy(w,\"10\");\n"
        "  tot=tot+(((n==2)&&(strcmp(b,w)==0)));\n  cells=cells+1;\n", stdout);
    fputs("  printf(\"ffmtP|%f|%.3f|%.15f|%10.3f|\\n\",x,x,x,x);\n"
        "  printf(\"TOT ffmt %d %d\\n\",cells,tot);\n"
        "  return tot;\n}\n", stdout);
    (void) k0;
    return 0;
}
