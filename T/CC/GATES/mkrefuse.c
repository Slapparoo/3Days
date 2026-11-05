/* M37 refusal/acceptance gate generator (host tool, gcc-compiled; NOT part of
 * the compiled tree).  Emits, INTO THIS DIRECTORY:
 *
 *   r01.c .. rNN.c   one program per directive lite is expected to REFUSE at
 *                    compile time — a compile error, not a wrong string
 *   acc.c            the acceptance program: every flag/width/precision
 *                    directive that must COMPILE, printed twice (sprintf into a
 *                    buffer, then printf), plus the three cells that pin what
 *                    the runtime engine does with a flag or a width in front of
 *                    one of ITS OWN conversions
 *   rfdriver.HC      the HolyC driver: runs acc.c, renders the same acceptance
 *                    directives through the KERNEL itself, then runs each
 *                    refusal case and reports its return code
 *
 * Nothing here decides what is correct — the emitted programs and the driver
 * do, and the driver's own K-lines are the oracle for the C lines.  The files
 * land here in the tree (T:/CC/GATES/) and not in the host's /tmp, because the
 * host's /tmp is wiped on reboot: `T:/CC/GATES/x` is written to
 * <repo root>/T/CC/GATES/x (override the root with env CC_REPO).
 *
 *   cc mkrefuse.c -o mkrefuse && ./mkrefuse
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/* Directives printf must refuse: %e and %g are letters the kernel spells
 * differently from C and lite does not delegate them at all; %lf %hd %ld %u are
 * type suffixes lite has no notion of; %x is lowered by lite and not delegated;
 * %5d %010d %10.3d %.2d % c %n %5s are a field width, a precision or a flag on
 * a conversion lite writes itself.  Each is the ONLY directive in its program,
 * spelled verbatim in the format string, which is the whole point. */
static const char *R[] = {
    "%e", "%g", "%lf", "%hd", "%5d", "%010d", "%10.3d", "%.2d", "%x", "% c",
    "%n", "%.2e", "%ld", "%u", "%5s", 0
};

/* Directives printf must ACCEPT and hand over, because the letter is `f`: a
 * field width, a `0` pad, a `-`, a `#`, a space, a `+`, a precision and a width
 * past what a buffer can hold.  The driver renders these same strings through
 * the kernel in its K-lines, so the comparison needs no external oracle. */
static const char *K[] = {
    "%20.15f", "%010.4f", "%-10.4f", "%#10.2f", "% 10.2f", "%+10.2f", "%.600f",
    "%999.999f", "%01000.3f", "%3.1f", "%-3.1f", 0
};

/* The bit pattern of 1.5 as a quad.  The driver hands StrPrintJoin that quad
 * directly; the C programs build the same double as 3.0/2.0, so both sides
 * format one and the same value. */
static const char BITS_1_5[] = "4609434218613702656";

static const char *Repo(void)
{
    const char *r = getenv("CC_REPO");
    return (r && *r) ? r : "/mnt/d/Source/3Days";
}

/* T:/CC/GATES/<name> -> the host file <repo>/T/CC/GATES/<name>. */
static FILE *Open(const char *name, char *shown)
{
    char host[512];
    snprintf(host, sizeof host, "%s/T/CC/GATES/%s", Repo(), name);
    if (shown)
        snprintf(shown, 64, "T:/CC/GATES/%s", name);
    return fopen(host, "w");
}

int main(void)
{
    char name[32], shown[64];
    FILE *o;
    int i, n, nk;

    /* One refusal case per file: the directive is the whole format string, so
     * a wrong ANSWER is impossible — the compile either fails or it does not. */
    for (i = 0; R[i]; i++)
    {
        sprintf(name, "r%02d.c", i + 1);
        o = Open(name, shown);
        if (!o) { perror(name); return 1; }
        fputs("#include <stdio.h>\nint main()\n{\n  double da,db,x;\n  char "
            "s[8];\n  int n;\n  da=3;db=2;x=da/db;\n  s[0]='a';\n"
            "  n=printf(\"", o);
        fputs(R[i], o);
        fputs("\",x,10,s);\n  printf(\"OK|%d|\",n);\n  return 0;\n}\n", o);
        fclose(o);
    }
    n = i;

    /* The acceptance program: the same directive through sprintf (A-lines) and
     * through printf (B-lines), which is how "both sinks answer the same" is
     * checked, then the three engine-model cells. */
    o = Open("acc.c", shown);
    if (!o) { perror("acc.c"); return 1; }
    fputs("#include <stdio.h>\nint main()\n{\n  double da,db,x;\n  char b[1200];\n"
        "  int n;\n  da=3;db=2;x=da/db;\n", o);
    for (i = 0; K[i]; i++)
    {
        fprintf(o, "  n=sprintf(b,\"%s\",x);\n", K[i]);
        fprintf(o, "  printf(\"A%02d|%%s|%%d|\\n\",b,n);\n", i + 1);
    }
    nk = i;		/* the K[] count.  `i` is reused by the refusal loop
                         * below, so the tally is parked here. */
    for (i = 0; K[i]; i++)
        fprintf(o, "  printf(\"B%02d|%s|\\n\",x);\n", i + 1, K[i]);
    fputs("  n=sprintf(b,\"%5d %d\",5,7);\n  printf(\"ARITY1|%s|%d|\\n\",b,n);\n"
        "  n=sprintf(b,\"%-2d\",7);\n  printf(\"ARITY2|%s|%d|\\n\",b,n);\n"
        "  n=sprintf(b,\"%*d\",10,7);\n  printf(\"ARITY3|%s|%d|\\n\",b,n);\n"
        "  return 0;\n}\n", o);
    fclose(o);

    /* The driver: acc.c first, then the kernel's own rendering of every
     * acceptance directive, then the refusal cases.  The K-lines ARE the
     * oracle, produced in the same session as the A- and B-lines. */
    o = Open("rfdriver.HC", shown);
    if (!o) { perror("rfdriver.HC"); return 1; }
    fputs("#include \"T:/CC/CC.HC\"\n", o);
    fputs("\"ACC rc=%d\\n\",CCRunFile(\"T:/CC/GATES/acc.c\");\n", o);
    fprintf(o, "I64 q;\nU8 out[1100];\n\nq=%s;\n", BITS_1_5);
    for (i = 0; K[i]; i++)
        fprintf(o, "MemSet(out,0,1100);\nStrPrintJoin(out,\"%s\",1,&q);\n"
            "\"K%02d|%%s|\\n\",out;\n", K[i], i + 1);
    fputs("q=10;\nMemSet(out,0,1100);\nStrPrintJoin(out,\"%.6f\",1,&q);\n"
        "\"K12|%s|\\n\",out;\n", o);
    for (i = 0; i < n; i++)
        fprintf(o, "\"R%02d rc=%%d\\n\",CCRunFile(\"T:/CC/GATES/r%02d.c\");\n",
            i + 1, i + 1);
    fputs("Exit;\n", o);
    fclose(o);

    fprintf(stderr, "refusal cases %d, acceptance %d float directives\n",
        n, nk);
    return 0;
}
