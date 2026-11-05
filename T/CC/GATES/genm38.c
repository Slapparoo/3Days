/* M38 fixture generator (host tool, gcc-compiled; NOT part of the tree).
 *
 * Reads mthrows.txt — the KERNEL's own `%.17f` renderings of the 34 M38 math
 * cells, each measured by calling the kernel entry (Sqrt/Pow/ACos/...) direct-
 * ly and printing the result through StrPrintJoin — and emits the two self-
 * checking fixtures that run those same 34 expressions through lite:
 *
 *   T:/CC/TEST/mth.c    the sprintf sink.  `n=sprintf(b,"%.17f",call);` then
 *                       the bytes compared against the pinned text, exactly as
 *                       T:/CC/TEST/ffmt.c counts a cell, and a
 *                       `Ms<nn>|<got>|<want>|` row for whoever reads the log.
 *   T:/CC/TEST/mthp.c   the printf sink.  Same cells and the same expectation;
 *                       the rendering UNDER TEST is printf's own `%f`, printed
 *                       in the row as `Mp<nn>|<got>|<want>|`, while the cell is
 *                       counted by a sprintf of the same call against the same
 *                       text — the way gsd.HC's sweep programs score a sink
 *                       that reports its answer only as stdout.
 *
 * The table below carries the C call, the kernel expression that row of
 * mthrows.txt measured, and what the cell is for.  It is CHECKED against the
 * file it generates from: a row whose expression is not the call the fixture is
 * about to make means the expectation and the program have silently come
 * apart, which is the one failure a generator can hide.  So the generator
 * refuses to write anything unless it read exactly 34 `K` rows, numbered 01 to
 * 34 with no gap and no duplicate, each with a non-empty rendering that fits a
 * fixture buffer and holds nothing a C string literal cannot.  A gate that
 * quietly generates a short program is worse than one that stops.
 *
 * Expectation text is loaded into `w` in pieces of at most 68 characters, as in
 * genm37.c: a literal over CC_TOK_MAX (128, T:/CC/CC.HH:139) is refused by
 * lite's lexer, and a longer piece pushes the emitted line past this tree's 83
 * column discipline.  Both hold trivially for these renderings — the widest is
 * 21 characters — and the rule is here for the day one is not.
 *
 *   ./genm38 [mthrows] [out-sprintf] [out-printf]
 *
 * All three default to the T:/ paths above; `T:/x` is the host file
 * <repo root>/T/x, and env CC_REPO points at a different checkout (a worktree,
 * for instance) exactly as it does for genm37.c and genffmt.c.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NCELL 34		/* the cell count, pinned; see the checks in Read */
#define PMAX 1024		/* any path or scratch buffer */
#define WMAX 1200		/* a rendering, and the fixture's b[] and w[] */

/* The cells, in the order both fixtures emit them.  fn/arg spell the C call,
 * kor names the kernel expression mthrows.txt measured, why is the comment.
 * `arg` is exactly what C source says, integer literals included: widening an
 * int operand to the double the kernel entry wants is part of what is tested. */
static const struct {
    const char *fn;
    const char *arg;
    const char *kor;
    const char *why;
} Cell[NCELL] = {
    {"sqrt", "2.0", "Sqrt(2.0)", "double literal operand"},
    {"sqrt", "2", "Sqrt(2.0)", "INTEGER literal operand (must be widened)"},
    {"sqrt", "v", "Sqrt(3.0)", "double variable operand (no widening)"},
    {"sqrt", "k", "Sqrt(4.0)", "int variable operand (k=4, must be widened)"},
    {"sqrt", "4.0", "Sqrt(4.0)", "exactly representable: the answer is 2"},
    {"sqrt", "f", "Sqrt(0.25)", "double variable, value below 1"},
    {"fabs", "-2.5", "Abs(-2.5)", "absolute value, C fabs -> kernel Abs"},
    {"exp", "1.0", "Exp(1.0)", "exp(1): the kernel e, not the glibc e"},
    {"exp", "0.0", "Exp(0.0)", "exactly 1"},
    {"log", "1.0", "Ln(1.0)", "log(1) is exactly 0; C log -> kernel Ln"},
    {"log", "2.7", "Ln(2.7)", "natural log"},
    {"log10", "10.0", "Log10(10.0)", "C log10 -> kernel Log10"},
    {"log10", "1000.0", "Log10(1000.0)",
        "should be 3; the kernel says 2.99999"},
    {"floor", "2.7", "Floor(2.7)", "toward -inf"},
    {"floor", "-2.7", "Floor(-2.7)", "toward -inf, not toward zero"},
    {"ceil", "2.1", "Ceil(2.1)", "toward +inf"},
    {"ceil", "-2.1", "Ceil(-2.1)", "toward +inf, not away from zero"},
    {"trunc", "2.7", "Trunc(2.7)", "toward zero"},
    {"pow", "2,0.5", "Pow(2.0,0.5)", "INTEGER base, double exponent"},
    {"pow", "2.0,3", "Pow(2.0,3.0)", "double base, INTEGER exponent"},
    {"pow", "k,0.5", "Pow(4.0,0.5)", "int variable base (k=4)"},
    {"pow", "2.0,10.0", "Pow(2.0,10.0)", "must be exactly 1024"},
    {"pow", "-2.0,0.5", "Pow(-2.0,0.5)",
        "C says NaN; the kernel returns a real"},
    {"round", "2.5", "Round(2.5)", "kernel Round is nearest-even; C rounds away"},
    {"round", "3.5", "Round(3.5)", "the other half of that pair"},
    {"sin", "0.0", "Sin(0.0)", "exactly 0"},
    {"sin", "1.0", "Sin(1.0)", "radians, like C"},
    {"cos", "1.0", "Cos(1.0)", "radians, like C"},
    {"tan", "1.0", "Tan(1.0)", "radians, like C"},
    {"atan", "1.0", "ATan(1.0)", "C atan -> kernel ATan"},
    {"asin", "0.5", "ASin(0.5)", "C asin -> kernel ASin"},
    {"acos", "0.5", "ACos(0.5)", "measured 1 ulp from C: recorded, not fixed"},
    {"sinh", "1.0", "Sinh(1.0)", "C sinh -> kernel Sinh"},
    {"cosh", "1.0", "Cosh(1.0)", "C cosh -> kernel Cosh"},
};

/* Indexed by the cell number on the `K` row, so 1..NCELL. */
static char Expr[NCELL + 1][64];
static char Rend[NCELL + 1][WMAX];
static int Have[NCELL + 1];
static int Nrow;

/* The gate programs and the table they read live IN the repo (T:/CC/...),
 * because /tmp on the host is wiped on reboot and a gate that lives there
 * evaporates with it.  Same two helpers as genm37.c: `T:/CC/GATES/x` is
 * <repo>/T/CC/GATES/x. */
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

static void Stop(const char *msg, const char *a, const char *b)
{
    fflush(stdout);
    fprintf(stderr, "genm38: %s", msg);
    if (a)
        fprintf(stderr, " %s", a);
    if (b)
        fprintf(stderr, " %s", b);
    fputc('\n', stderr);
    exit(1);
}

static FILE *Open(const char *tos)
{
    char host[PMAX];
    FILE *f;
    HostOf(tos, host, sizeof host);
    f = fopen(host, "w");
    if (!f)
        Stop("cannot write", host, NULL);
    return f;
}

/* A rendering the fixtures can carry: non-empty, short enough for w[], and
 * free of the two characters a C string literal cannot hold as written. */
static void CheckRend(int id, const char *s)
{
    int L = strlen(s), i;
    char num[16];
    if (L <= 0)
    {
        sprintf(num, "%02d", id);
        Stop("empty expected rendering for cell", num, NULL);
    }
    if (L >= WMAX)
    {
        sprintf(num, "%02d", id);
        Stop("rendering too wide for a fixture buffer, cell", num, NULL);
    }
    for (i = 0; i < L; i++)
        if (s[i] == '"' || s[i] == '\\' || (unsigned char) s[i] < 0x20)
        {
            sprintf(num, "%02d", id);
            Stop("rendering needs escaping in a C literal, cell", num, NULL);
        }
}

/* Read mthrows.txt into Expr[]/Rend[] and refuse anything that is not exactly
 * the table the fixtures are about to be built from. */
static void Read(const char *path)
{
    FILE *f = fopen(path, "r");
    static char ln[WMAX + PMAX];
    char tok[64], num[16];
    char *o, *e, *p;
    int id, L;
    if (!f)
        Stop("no measurement table at", path, NULL);
    while (fgets(ln, sizeof ln, f))
    {
        L = strlen(ln);
        while (L && (ln[L - 1] == '\n' || ln[L - 1] == '\r'))
            ln[--L] = 0;
        if (strncmp(ln, "K ", 2))
            continue;		/* the header comments, and nothing else */
        Nrow++;
        p = ln + 2;
        if (sscanf(p, "%d %63s", &id, tok) != 2)
            Stop("K row without a cell number and an expression:", ln, NULL);
        if (id < 1 || id > NCELL)
            Stop("cell number out of range 01..34 on row", p, NULL);
        if (Have[id])
            Stop("duplicate cell number — one K row each, not two:", p, NULL);
        o = strchr(p, '[');
        e = strrchr(p, ']');
        if (!o || !e || e < o)
            Stop("K row has no [rendering] field:", ln, NULL);
        L = (int) (e - o - 1);
        memcpy(Rend[id], o + 1, L);
        Rend[id][L] = 0;
        sprintf(num, "%02d", id);
        CheckRend(id, Rend[id]);
        strcpy(Expr[id], tok);
        Have[id] = 1;
    }
    fclose(f);
    if (Nrow != NCELL)
    {
        sprintf(num, "%d", Nrow);
        Stop("read", num, "K rows; exactly 34 are expected");
    }
    for (id = 1; id <= NCELL; id++)
    {
        if (!Have[id])
        {
            sprintf(num, "%02d", id);
            Stop("no K row for cell", num, NULL);
        }
        if (strcmp(Expr[id], Cell[id - 1].kor))
            Stop("cell expression does not match the table it pins:",
                Expr[id], Cell[id - 1].kor);
    }
}

/* A want string goes into w in pieces: past 68 characters the emitted line
 * leaves the 83-column discipline, and a literal past CC_TOK_MAX (128) is
 * refused by lite's lexer outright. */
static void EmitWant(FILE *o, const char *s)
{
    char piece[128];
    int n = strlen(s), i = 0, m;
    while (i < n)
    {
        m = n - i;
        if (m > 68)
            m = 68;		/* 15 chars of `  strcpy(w,"");` + NUL <= 83 */
        memcpy(piece, s + i, m);
        piece[m] = 0;
        fprintf(o, "  %s(w,\"%s\");\n", i ? "strcat" : "strcpy", piece);
        i += m;
    }
}

/* One comment, broken at a space so no emitted line reaches 83 columns: the
 * two spaces of indent, three after the opener and three before the close
 * leave 75 characters for the text itself. */
static void EmitComment(FILE *o, const char *s)
{
    int at = 0, take, brk;
    while (s[at])
    {
        take = (int) strlen(s + at);
        if (take > 75)
        {
            brk = 75;
            while (brk > 0 && s[at + brk] != ' ')
                brk--;
            take = (brk > 0) ? brk : 75;
        }
        fprintf(o, "  /* %.*s */\n", take, s + at);
        at += take;
        while (s[at] == ' ')
            at++;
    }
}

static void Head(FILE *o, const char *tag, const char *tos)
{
    fputs("/* M38 math builtins, the ", o);
    fprintf(o, "%s sink.  GENERATED by\n", tag);
    fputs(" * T:/CC/GATES/genm38.c from T:/CC/GATES/mthrows.txt: the renderings\n"
        " * below are what the KERNEL wrote for these same expressions, from a\n"
        " * direct call to its own entry, rendered by StrPrintJoin at `%.17f`.\n"
        " * Not a C library — the kernel is the engine lite hands a `%f` to, so\n"
        " * the kernel's text IS the answer.  Do not hand-edit ", o);
    fprintf(o, "%s:\n", tos);
    fputs(" * regenerate.\n"
        " *\n"
        " * A bare `%f` is precision 0 in this kernel, so the directive is\n"
        " * spelled out as `%.17f`, which exposes every bit the two engines\n"
        " * disagree on.  Operands are variables as well as literals (v=3.0,\n"
        " * f=0.25, k=4): the widening a math argument may need is a property\n"
        " * of the VALUE's kind, and a fixture of literals alone cannot show\n"
        " * whether a non-constant answers correctly.\n", o);
    if (!strcmp(tag, "sprintf"))
        fputs(" *\n"
            " * Each cell renders its call with sprintf into b and compares the\n"
            " * bytes against the pinned text, counting a pass into tot exactly\n"
            " * as T:/CC/TEST/ffmt.c counts one, so a clean run prints\n"
            " * `TOT mth 34 34` and returns 34.  The return value is the PASS\n"
            " * count, so a short count is itself the failure.  The\n"
            " * `Ms<nn>|<got>|<want>|` row is for whoever reads the log.\n", o);
    else
        fputs(" *\n"
            " * The rendering under test is printf's own `%f` — the path M37's\n"
            " * float fixtures did not cover, because they printed the buffer\n"
            " * through %s.  The `Mp<nn>|<got>|<want>|` row carries lite's\n"
            " * answer next to the kernel's.  tot still comes from a sprintf of\n"
            " * the same call against the same text, the way gsd.HC's sweep\n"
            " * programs score a sink that reports only to stdout, so a clean\n"
            " * run prints `TOT mthp 34 34` and returns 34.\n", o);
    fputs(" */\n#include <stdio.h>\n#include <string.h>\n#include <math.h>\n\n",
        o);
    fputs("int main()\n{\n  char b[1200],w[1200];\n  double v;\n  double f;\n"
        "  int k;\n  int n,tot,cells;\n\n  tot=0;\n  cells=0;\n"
        "  v=3.0;\n  f=0.25;\n  k=4;\n\n", o);
}

static void Cells(FILE *o, int sink)
{
    char note[PMAX], call[128], pin[PMAX];
    int i;
    for (i = 0; i < NCELL; i++)
    {
        snprintf(call, sizeof call, "%s(%s)", Cell[i].fn, Cell[i].arg);
        snprintf(note, sizeof note, "cell %02d: %s -- %s", i + 1, call,
            Cell[i].why);
        snprintf(pin, sizeof pin, "oracle: kernel %s, mthrows.txt row %02d",
            Cell[i].kor, i + 1);
        EmitComment(o, note);
        EmitComment(o, pin);
        EmitWant(o, Rend[i + 1]);
        if (sink == 0)
        {
            fprintf(o, "  n=sprintf(b,\"%%.17f\",%s);\n", call);
            fputs("  tot=tot+(((n==strlen(w))&&(strcmp(b,w)==0)));\n"
                "  cells=cells+1;\n", o);
            fprintf(o, "  printf(\"Ms%02d|%%s|%%s|\\n\",b,w);\n\n", i + 1);
        }
        else
        {
            fprintf(o, "  printf(\"Mp%02d|%%.17f|%%s|\\n\",%s,w);\n",
                i + 1, call);
            fprintf(o, "  n=sprintf(b,\"%%.17f\",%s);\n", call);
            fputs("  tot=tot+(((n==strlen(w))&&(strcmp(b,w)==0)));\n"
                "  cells=cells+1;\n\n", o);
        }
    }
}

int main(int argc, char **argv)
{
    const char *tbl = "T:/CC/GATES/mthrows.txt";
    const char *out0 = "T:/CC/TEST/mth.c";
    const char *out1 = "T:/CC/TEST/mthp.c";
    char host[PMAX];
    FILE *o;

    if (argc > 1)
        tbl = argv[1];
    if (argc > 2)
        out0 = argv[2];
    if (argc > 3)
        out1 = argv[3];
    HostOf(tbl, host, sizeof host);
    Read(host);
    o = Open(out0);
    Head(o, "sprintf", out0);
    Cells(o, 0);
    fputs("  printf(\"TOT mth %d %d\\n\",cells,tot);\n  return tot;\n}\n", o);
    fclose(o);
    o = Open(out1);
    Head(o, "printf", out1);
    Cells(o, 1);
    fputs("  printf(\"TOT mthp %d %d\\n\",cells,tot);\n  return tot;\n}\n", o);
    fclose(o);
    fprintf(stderr, "CELLS %d -> %s %s\n", NCELL, out0, out1);
    return 0;
}
