/* M38 edge cells: the domains T:/CC/TEST/mth.c and mthp.c never enter -- NaN,
 * the infinities, signed zero, overflow and underflow.  GENERATED WITH the
 * probe driver T:/CC/GATES/edgemth.HC, which prints the kernel's own answer
 * for every cell here, in the same session, so the comparison needs no C
 * library and no expectation about what a NaN SHOULD print as.  Run:
 *   ./3d_loader -t T -c T:/CC/GATES/edgemth.HC   ->  15 `Es<nn>` rows and 15
 *   `O<nn>` rows; each Es field 2 must equal the O field 2 of the same number.
 * These rows are NOT pinned into mthrows.txt: they are a probe, and what they
 * record is in T:/CC/PLAN.md (kernel Round, the pow domain, how a NaN and an
 * infinity and a signed zero come out of the delegated printer).
 */
#include <math.h>
#include <stdio.h>

int main(void)
{
  char b[128];
  int n;

  /* Es01: sqrt(-1.0) -- a NaN out of a builtin */
  n = sprintf(b, "%.17f", sqrt(-1.0));
  printf("Es01|%s|%d|\n", b, n);
  /* Es02: sqrt(0.0) -- exactly zero */
  n = sprintf(b, "%.17f", sqrt(0.0));
  printf("Es02|%s|%d|\n", b, n);
  /* Es03: sqrt(-0.0) -- does the sign of -0.0 survive? */
  n = sprintf(b, "%.17f", sqrt(-0.0));
  printf("Es03|%s|%d|\n", b, n);
  /* Es04: log(0.0) -- log of zero */
  n = sprintf(b, "%.17f", log(0.0));
  printf("Es04|%s|%d|\n", b, n);
  /* Es05: log(-1.0) -- log of a negative: a NaN */
  n = sprintf(b, "%.17f", log(-1.0));
  printf("Es05|%s|%d|\n", b, n);
  /* Es06: exp(1000.0) -- overflow */
  n = sprintf(b, "%.17f", exp(1000.0));
  printf("Es06|%s|%d|\n", b, n);
  /* Es07: exp(-1000.0) -- underflow */
  n = sprintf(b, "%.17f", exp(-1000.0));
  printf("Es07|%s|%d|\n", b, n);
  /* Es08: pow(0.0,0.0) -- 0 to the 0 */
  n = sprintf(b, "%.17f", pow(0.0,0.0));
  printf("Es08|%s|%d|\n", b, n);
  /* Es09: pow(0.0,-1.0) -- a zero base with a negative exponent */
  n = sprintf(b, "%.17f", pow(0.0,-1.0));
  printf("Es09|%s|%d|\n", b, n);
  /* Es10: pow(2.0,1000.0) -- overflow the other way */
  n = sprintf(b, "%.17f", pow(2.0,1000.0));
  printf("Es10|%s|%d|\n", b, n);
  /* Es11: trunc(-0.5) -- rounds toward a SIGNED zero */
  n = sprintf(b, "%.17f", trunc(-0.5));
  printf("Es11|%s|%d|\n", b, n);
  /* Es12: floor(-0.5) -- the integer below -0.5 */
  n = sprintf(b, "%.17f", floor(-0.5));
  printf("Es12|%s|%d|\n", b, n);
  /* Es13: round(-0.5) -- nearest-even on a tie below zero */
  n = sprintf(b, "%.17f", round(-0.5));
  printf("Es13|%s|%d|\n", b, n);
  /* Es14: fabs(-0.0) -- does fabs clear the sign bit? */
  n = sprintf(b, "%.17f", fabs(-0.0));
  printf("Es14|%s|%d|\n", b, n);
  /* Es15: log10(0.0) -- log10 of zero */
  n = sprintf(b, "%.17f", log10(0.0));
  printf("Es15|%s|%d|\n", b, n);
  return 0;
}
