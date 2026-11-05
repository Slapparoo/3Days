/* M37: printf's and sprintf's %f, delegated to the kernel's own float printer.
 * Every rendering pinned below is what T:/STRPRINT.HC's `case 'f':` writes for
 * these bit patterns — measured in TempleOS, not derived from what C would
 * print.  Where the kernel and C disagree, the comment says so and the kernel
 * wins here, because lite hands the kernel the directive and emits the answer.
 *
 * Values are built from an exact significand and an exact power of two: p2e
 * doubles 1.0 (or halves it) so no rounding is involved, which is how this
 * file, the sweep and CC_TEST.HC's CCProbeFloat all agree on one bit pattern
 * without any of them needing `double *` — a type lite refuses.  The NaN comes
 * from 0.0/0.0 negated, because x87's real indefinite is a NEGATIVE quiet NaN
 * and the pattern the kernel was measured with is the positive one.
 *
 * The directives below are what C SOURCE spells.  CCProbeFloat pins the same
 * cells with the directives lite SENDS: `.6` written in when the source stated
 * no precision (C defaults to 6, the engine defaults to 0), `*` dropped, and a
 * field width or precision above 1000 or 600 capped to those, because
 * StrPrintJoin takes no buffer length and the buffer is a 1024-byte field of
 * cc.  The pair pins both halves: lite sends what C means, and the kernel's
 * rendering is the answer.
 *
 * Expect 63: one pass per cell, and TOT prints the count.  The ffmtP line is
 * the compile-time path (printf) for 1.5; the suite run compares it with
 * CCProbeFloat's cells for those same four directives. */
#include <stdio.h>
#include <string.h>
static double p2e(int n)
{
  double p;
  int i,j;
  p=1.0;
  j=n;
  if (j<0)
  {
    j=-j;
    for (i=0;i!=j;i=i+1)
      p=p*0.5;
  }
  else
  {
    for (i=0;i!=j;i=i+1)
      p=p+p;
  }
  return p;
}

int main()
{
  double x,q;
  char b[1200],w[1200];
  char *fm;
  int n,tot,cells;

  tot=0;
  cells=0;
  /* cell 0: value 2, bits 0000000000000000, directive %f */
  x=0; q=p2e(-1074); x=x*q;
strcpy(w,"0.000000");
  n=sprintf(b,"%f",x);
  tot=tot+(((n==strlen(w))&&(strcmp(b,w)==0)));
  cells=cells+1;
  /* cell 1: value 2, bits 0000000000000000, directive %.3f */
  x=0; q=p2e(-1074); x=x*q;
strcpy(w,"0.000");
  n=sprintf(b,"%.3f",x);
  tot=tot+(((n==strlen(w))&&(strcmp(b,w)==0)));
  cells=cells+1;
  /* cell 2: value 2, bits 0000000000000000, directive %.15f */
  x=0; q=p2e(-1074); x=x*q;
strcpy(w,"0.000000000000000");
  n=sprintf(b,"%.15f",x);
  tot=tot+(((n==strlen(w))&&(strcmp(b,w)==0)));
  cells=cells+1;
  /* cell 3: value 2, bits 0000000000000000, directive %10.3f */
  x=0; q=p2e(-1074); x=x*q;
strcpy(w,"     0.000");
  n=sprintf(b,"%10.3f",x);
  tot=tot+(((n==strlen(w))&&(strcmp(b,w)==0)));
  cells=cells+1;
  /* cell 4: value 3, bits 8000000000000000, directive %f */
  x=0; q=p2e(-1074); x=x*q;
  x=-x;
strcpy(w,"-0.000000");
  n=sprintf(b,"%f",x);
  tot=tot+(((n==strlen(w))&&(strcmp(b,w)==0)));
  cells=cells+1;
  /* cell 5: value 3, bits 8000000000000000, directive %.3f */
  x=0; q=p2e(-1074); x=x*q;
  x=-x;
strcpy(w,"-0.000");
  n=sprintf(b,"%.3f",x);
  tot=tot+(((n==strlen(w))&&(strcmp(b,w)==0)));
  cells=cells+1;
  /* cell 6: value 3, bits 8000000000000000, directive %.15f */
  x=0; q=p2e(-1074); x=x*q;
  x=-x;
strcpy(w,"-0.000000000000000");
  n=sprintf(b,"%.15f",x);
  tot=tot+(((n==strlen(w))&&(strcmp(b,w)==0)));
  cells=cells+1;
  /* cell 7: value 3, bits 8000000000000000, directive %10.3f */
  x=0; q=p2e(-1074); x=x*q;
  x=-x;
strcpy(w,"    -0.000");
  n=sprintf(b,"%10.3f",x);
  tot=tot+(((n==strlen(w))&&(strcmp(b,w)==0)));
  cells=cells+1;
  /* cell 8: value 4, bits 3ff0000000000000, directive %f */
  x=4503599627370496; q=p2e(-52); x=x*q;
strcpy(w,"1.000000");
  n=sprintf(b,"%f",x);
  tot=tot+(((n==strlen(w))&&(strcmp(b,w)==0)));
  cells=cells+1;
  /* cell 9: value 4, bits 3ff0000000000000, directive %.3f */
  x=4503599627370496; q=p2e(-52); x=x*q;
strcpy(w,"1.000");
  n=sprintf(b,"%.3f",x);
  tot=tot+(((n==strlen(w))&&(strcmp(b,w)==0)));
  cells=cells+1;
  /* cell 10: value 4, bits 3ff0000000000000, directive %.15f */
  x=4503599627370496; q=p2e(-52); x=x*q;
strcpy(w,"0.999999999999998");
  n=sprintf(b,"%.15f",x);
  tot=tot+(((n==strlen(w))&&(strcmp(b,w)==0)));
  cells=cells+1;
  /* cell 11: value 4, bits 3ff0000000000000, directive %10.3f */
  x=4503599627370496; q=p2e(-52); x=x*q;
strcpy(w,"     1.000");
  n=sprintf(b,"%10.3f",x);
  tot=tot+(((n==strlen(w))&&(strcmp(b,w)==0)));
  cells=cells+1;
  /* cell 12: value 5, bits 3fb999999999999a, directive %f */
  x=7205759403792794; q=p2e(-56); x=x*q;
strcpy(w,"0.100000");
  n=sprintf(b,"%f",x);
  tot=tot+(((n==strlen(w))&&(strcmp(b,w)==0)));
  cells=cells+1;
  /* cell 13: value 5, bits 3fb999999999999a, directive %.3f */
  x=7205759403792794; q=p2e(-56); x=x*q;
strcpy(w,"0.100");
  n=sprintf(b,"%.3f",x);
  tot=tot+(((n==strlen(w))&&(strcmp(b,w)==0)));
  cells=cells+1;
  /* cell 14: value 5, bits 3fb999999999999a, directive %.15f */
  x=7205759403792794; q=p2e(-56); x=x*q;
strcpy(w,"0.100000000000000");
  n=sprintf(b,"%.15f",x);
  tot=tot+(((n==strlen(w))&&(strcmp(b,w)==0)));
  cells=cells+1;
  /* cell 15: value 5, bits 3fb999999999999a, directive %10.3f */
  x=7205759403792794; q=p2e(-56); x=x*q;
strcpy(w,"     0.100");
  n=sprintf(b,"%10.3f",x);
  tot=tot+(((n==strlen(w))&&(strcmp(b,w)==0)));
  cells=cells+1;
  /* cell 16: value 9, bits 4024000000000000, directive %f */
  x=5629499534213120; q=p2e(-49); x=x*q;
strcpy(w,"10.000000");
  n=sprintf(b,"%f",x);
  tot=tot+(((n==strlen(w))&&(strcmp(b,w)==0)));
  cells=cells+1;
  /* cell 17: value 9, bits 4024000000000000, directive %.3f */
  x=5629499534213120; q=p2e(-49); x=x*q;
strcpy(w,"10.000");
  n=sprintf(b,"%.3f",x);
  tot=tot+(((n==strlen(w))&&(strcmp(b,w)==0)));
  cells=cells+1;
  /* cell 18: value 9, bits 4024000000000000, directive %.15f */
  x=5629499534213120; q=p2e(-49); x=x*q;
strcpy(w,"9.999999999999984");
  n=sprintf(b,"%.15f",x);
  tot=tot+(((n==strlen(w))&&(strcmp(b,w)==0)));
  cells=cells+1;
  /* cell 19: value 9, bits 4024000000000000, directive %10.3f */
  x=5629499534213120; q=p2e(-49); x=x*q;
strcpy(w,"    10.000");
  n=sprintf(b,"%10.3f",x);
  tot=tot+(((n==strlen(w))&&(strcmp(b,w)==0)));
  cells=cells+1;
  /* cell 20: value 12, bits 4202a05f20000000, directive %f */
  x=5242880000000000; q=p2e(-19); x=x*q;
strcpy(w,"9999999999.999998");
  n=sprintf(b,"%f",x);
  tot=tot+(((n==strlen(w))&&(strcmp(b,w)==0)));
  cells=cells+1;
  /* cell 21: value 12, bits 4202a05f20000000, directive %.3f */
  x=5242880000000000; q=p2e(-19); x=x*q;
strcpy(w,"10000000000.000");
  n=sprintf(b,"%.3f",x);
  tot=tot+(((n==strlen(w))&&(strcmp(b,w)==0)));
  cells=cells+1;
  /* cell 22: value 12, bits 4202a05f20000000, directive %.15f */
  x=5242880000000000; q=p2e(-19); x=x*q;
strcpy(w,"9999999999.999995200000000");
  n=sprintf(b,"%.15f",x);
  tot=tot+(((n==strlen(w))&&(strcmp(b,w)==0)));
  cells=cells+1;
  /* cell 23: value 12, bits 4202a05f20000000, directive %10.3f */
  x=5242880000000000; q=p2e(-19); x=x*q;
strcpy(w,"10000000000.000");
  n=sprintf(b,"%10.3f",x);
  tot=tot+(((n==strlen(w))&&(strcmp(b,w)==0)));
  cells=cells+1;
  /* cell 24: value 21, bits 4132d687e4189375, directive %f */
  x=5302428716536693; q=p2e(-32); x=x*q;
strcpy(w,"1234567.891000");
  n=sprintf(b,"%f",x);
  tot=tot+(((n==strlen(w))&&(strcmp(b,w)==0)));
  cells=cells+1;
  /* cell 25: value 21, bits 4132d687e4189375, directive %.3f */
  x=5302428716536693; q=p2e(-32); x=x*q;
strcpy(w,"1234567.891");
  n=sprintf(b,"%.3f",x);
  tot=tot+(((n==strlen(w))&&(strcmp(b,w)==0)));
  cells=cells+1;
  /* cell 26: value 21, bits 4132d687e4189375, directive %.15f */
  x=5302428716536693; q=p2e(-32); x=x*q;
strcpy(w,"1234567.890999994240000");
  n=sprintf(b,"%.15f",x);
  tot=tot+(((n==strlen(w))&&(strcmp(b,w)==0)));
  cells=cells+1;
  /* cell 27: value 21, bits 4132d687e4189375, directive %10.3f */
  x=5302428716536693; q=p2e(-32); x=x*q;
strcpy(w,"1234567.891");
  n=sprintf(b,"%10.3f",x);
  tot=tot+(((n==strlen(w))&&(strcmp(b,w)==0)));
  cells=cells+1;
  /* cell 28: value 24, bits 7e37e43c8800759c, directive %f */
  x=6724873095247260; q=p2e(944); x=x*q;
strcpy(w,"000000000000000000000000000000000000000000000000000000000000");
strcat(w,"000000000000000000000000000000000000000000000000000000000000");
strcat(w,"000000000000000000000000000000000000000000000000000000000000");
strcat(w,"000000000000000000000000000000000000000000000000000000000000");
strcat(w,"0.000000");
  n=sprintf(b,"%f",x);
  tot=tot+(((n==strlen(w))&&(strcmp(b,w)==0)));
  cells=cells+1;
  /* cell 29: value 24, bits 7e37e43c8800759c, directive %.3f */
  x=6724873095247260; q=p2e(944); x=x*q;
strcpy(w,"000000000000000000000000000000000000000000000000000000000000");
strcat(w,"000000000000000000000000000000000000000000000000000000000000");
strcat(w,"000000000000000000000000000000000000000000000000000000000000");
strcat(w,"000000000000000000000000000000000000000000000000000000000000");
strcat(w,"0000.000");
  n=sprintf(b,"%.3f",x);
  tot=tot+(((n==strlen(w))&&(strcmp(b,w)==0)));
  cells=cells+1;
  /* cell 30: value 24, bits 7e37e43c8800759c, directive %.15f */
  x=6724873095247260; q=p2e(944); x=x*q;
strcpy(w,"000000000000000000000000000000000000000000000000000000000000");
strcat(w,"000000000000000000000000000000000000000000000000000000000000");
strcat(w,"000000000000000000000000000000000000000000000000000000000000");
strcat(w,"0000000000000000000000000000000000000000000000000000.0000000");
strcat(w,"00000000");
  n=sprintf(b,"%.15f",x);
  tot=tot+(((n==strlen(w))&&(strcmp(b,w)==0)));
  cells=cells+1;
  /* cell 31: value 24, bits 7e37e43c8800759c, directive %10.3f */
  x=6724873095247260; q=p2e(944); x=x*q;
strcpy(w,"000000000000000000000000000000000000000000000000000000000000");
strcat(w,"000000000000000000000000000000000000000000000000000000000000");
strcat(w,"000000000000000000000000000000000000000000000000000000000000");
strcat(w,"000000000000000000000000000000000000000000000000000000000000");
strcat(w,"0000.000");
  n=sprintf(b,"%10.3f",x);
  tot=tot+(((n==strlen(w))&&(strcmp(b,w)==0)));
  cells=cells+1;
  /* cell 32: value 27, bits 7ff0000000000000, directive %f */
  x=1.0/0.0;
strcpy(w,"i");
  n=sprintf(b,"%f",x);
  tot=tot+(((n==strlen(w))&&(strcmp(b,w)==0)));
  cells=cells+1;
  /* cell 33: value 27, bits 7ff0000000000000, directive %.3f */
  x=1.0/0.0;
strcpy(w,"i");
  n=sprintf(b,"%.3f",x);
  tot=tot+(((n==strlen(w))&&(strcmp(b,w)==0)));
  cells=cells+1;
  /* cell 34: value 27, bits 7ff0000000000000, directive %.15f */
  x=1.0/0.0;
strcpy(w,"i");
  n=sprintf(b,"%.15f",x);
  tot=tot+(((n==strlen(w))&&(strcmp(b,w)==0)));
  cells=cells+1;
  /* cell 35: value 27, bits 7ff0000000000000, directive %10.3f */
  x=1.0/0.0;
strcpy(w,"         i");
  n=sprintf(b,"%10.3f",x);
  tot=tot+(((n==strlen(w))&&(strcmp(b,w)==0)));
  cells=cells+1;
  /* cell 36: value 28, bits fff0000000000000, directive %f */
  x=1.0/0.0;
  x=-x;
strcpy(w,"-i");
  n=sprintf(b,"%f",x);
  tot=tot+(((n==strlen(w))&&(strcmp(b,w)==0)));
  cells=cells+1;
  /* cell 37: value 28, bits fff0000000000000, directive %.3f */
  x=1.0/0.0;
  x=-x;
strcpy(w,"-i");
  n=sprintf(b,"%.3f",x);
  tot=tot+(((n==strlen(w))&&(strcmp(b,w)==0)));
  cells=cells+1;
  /* cell 38: value 28, bits fff0000000000000, directive %.15f */
  x=1.0/0.0;
  x=-x;
strcpy(w,"-i");
  n=sprintf(b,"%.15f",x);
  tot=tot+(((n==strlen(w))&&(strcmp(b,w)==0)));
  cells=cells+1;
  /* cell 39: value 28, bits fff0000000000000, directive %10.3f */
  x=1.0/0.0;
  x=-x;
strcpy(w,"        -i");
  n=sprintf(b,"%10.3f",x);
  tot=tot+(((n==strlen(w))&&(strcmp(b,w)==0)));
  cells=cells+1;
  /* cell 40: value 29, bits 7ff8000000000000, directive %f */
  x=0.0/0.0;
  x=-x;		/* x87s real indefinite is a
		   NEGATIVE quiet NaN */
strcpy(w,"9223372036854.775808");
  n=sprintf(b,"%f",x);
  tot=tot+(((n==strlen(w))&&(strcmp(b,w)==0)));
  cells=cells+1;
  /* cell 41: value 29, bits 7ff8000000000000, directive %.3f */
  x=0.0/0.0;
  x=-x;		/* x87s real indefinite is a
		   NEGATIVE quiet NaN */
strcpy(w,"9223372036854775.808");
  n=sprintf(b,"%.3f",x);
  tot=tot+(((n==strlen(w))&&(strcmp(b,w)==0)));
  cells=cells+1;
  /* cell 42: value 29, bits 7ff8000000000000, directive %.15f */
  x=0.0/0.0;
  x=-x;		/* x87s real indefinite is a
		   NEGATIVE quiet NaN */
strcpy(w,"9223.372036854775808");
  n=sprintf(b,"%.15f",x);
  tot=tot+(((n==strlen(w))&&(strcmp(b,w)==0)));
  cells=cells+1;
  /* cell 43: value 29, bits 7ff8000000000000, directive %10.3f */
  x=0.0/0.0;
  x=-x;		/* x87s real indefinite is a
		   NEGATIVE quiet NaN */
strcpy(w,"9223372036854775.808");
  n=sprintf(b,"%10.3f",x);
  tot=tot+(((n==strlen(w))&&(strcmp(b,w)==0)));
  cells=cells+1;
  /* cell 44: value 0, bits 3ff8000000000000, directive %f */
  x=6755399441055744; q=p2e(-52); x=x*q;
strcpy(w,"1.500000");
  n=sprintf(b,"%f",x);
  tot=tot+(((n==strlen(w))&&(strcmp(b,w)==0)));
  cells=cells+1;
  /* cell 45: value 0, bits 3ff8000000000000, directive %.3f */
  x=6755399441055744; q=p2e(-52); x=x*q;
strcpy(w,"1.500");
  n=sprintf(b,"%.3f",x);
  tot=tot+(((n==strlen(w))&&(strcmp(b,w)==0)));
  cells=cells+1;
  /* cell 46: value 0, bits 3ff8000000000000, directive %.15f */
  x=6755399441055744; q=p2e(-52); x=x*q;
strcpy(w,"1.499999999999998");
  n=sprintf(b,"%.15f",x);
  tot=tot+(((n==strlen(w))&&(strcmp(b,w)==0)));
  cells=cells+1;
  /* cell 47: value 0, bits 3ff8000000000000, directive %10.3f */
  x=6755399441055744; q=p2e(-52); x=x*q;
strcpy(w,"     1.500");
  n=sprintf(b,"%10.3f",x);
  tot=tot+(((n==strlen(w))&&(strcmp(b,w)==0)));
  cells=cells+1;
  /* cell 48: value 0, bits 3ff8000000000000, directive %.0f */
  x=6755399441055744; q=p2e(-52); x=x*q;
strcpy(w,"2");
  n=sprintf(b,"%.0f",x);
  tot=tot+(((n==strlen(w))&&(strcmp(b,w)==0)));
  cells=cells+1;
  /* cell 49: value 0, bits 3ff8000000000000, directive %.1f */
  x=6755399441055744; q=p2e(-52); x=x*q;
strcpy(w,"1.5");
  n=sprintf(b,"%.1f",x);
  tot=tot+(((n==strlen(w))&&(strcmp(b,w)==0)));
  cells=cells+1;
  /* cell 50: value 0, bits 3ff8000000000000, directive %.16f */
  x=6755399441055744; q=p2e(-52); x=x*q;
strcpy(w,"1.4999999999999970");
  n=sprintf(b,"%.16f",x);
  tot=tot+(((n==strlen(w))&&(strcmp(b,w)==0)));
  cells=cells+1;
  /* cell 51: value 0, bits 3ff8000000000000, directive %.20f */
  x=6755399441055744; q=p2e(-52); x=x*q;
strcpy(w,"1.49999999999999648000");
  n=sprintf(b,"%.20f",x);
  tot=tot+(((n==strlen(w))&&(strcmp(b,w)==0)));
  cells=cells+1;
  /* cell 52: value 0, bits 3ff8000000000000, directive %010.3f */
  x=6755399441055744; q=p2e(-52); x=x*q;
strcpy(w,"000001.500");
  n=sprintf(b,"%010.3f",x);
  tot=tot+(((n==strlen(w))&&(strcmp(b,w)==0)));
  cells=cells+1;
  /* cell 53: value 0, bits 3ff8000000000000, directive %-10.3f */
  x=6755399441055744; q=p2e(-52); x=x*q;
strcpy(w,"     1.500");
  n=sprintf(b,"%-10.3f",x);
  tot=tot+(((n==strlen(w))&&(strcmp(b,w)==0)));
  cells=cells+1;
  /* cell 54: value 0, bits 3ff8000000000000, directive %+f */
  x=6755399441055744; q=p2e(-52); x=x*q;
strcpy(w,".6f");
  n=sprintf(b,"%+f",x);
  tot=tot+(((n==strlen(w))&&(strcmp(b,w)==0)));
  cells=cells+1;
  /* cell 55: value 0, bits 3ff8000000000000, directive %*f */
  x=6755399441055744; q=p2e(-52); x=x*q;
strcpy(w,"1.500000");
  n=sprintf(b,"%*f",x);
  tot=tot+(((n==strlen(w))&&(strcmp(b,w)==0)));
  cells=cells+1;
  /* cell 56: value 0, bits 3ff8000000000000, directive %100.50f */
  x=6755399441055744; q=p2e(-52); x=x*q;
strcpy(w,"                                                1.4999999999");
strcat(w,"9999648000000000000000000000000000000000");
  n=sprintf(b,"%100.50f",x);
  tot=tot+(((n==strlen(w))&&(strcmp(b,w)==0)));
  cells=cells+1;
  /* cell 57: value 0, bits 3ff8000000000000, directive %01500.3f */
  x=6755399441055744; q=p2e(-52); x=x*q;
strcpy(w,"000000000000000000000000000000000000000000000000000000000000");
strcat(w,"000000000000000000000000000000000000000000000000000000000000");
strcat(w,"000000000000000000000000000000000000000000000000000000000000");
strcat(w,"000000000000000000000000000000000000000000000000000000000000");
strcat(w,"000000000000000000000000000000000000000000000000000000000000");
strcat(w,"000000000000000000000000000000000000000000000000000000000000");
strcat(w,"000000000000000000000000000000000000000000000000000000000000");
strcat(w,"000000000000000000000000000000000000000000000000000000000000");
strcat(w,"000000000000000000000000000000000000000000000000000000000000");
strcat(w,"000000000000000000000000000000000000000000000000000000000000");
strcat(w,"000000000000000000000000000000000000000000000000000000000000");
strcat(w,"000000000000000000000000000000000000000000000000000000000000");
strcat(w,"000000000000000000000000000000000000000000000000000000000000");
strcat(w,"000000000000000000000000000000000000000000000000000000000000");
strcat(w,"000000000000000000000000000000000000000000000000000000000000");
strcat(w,"000000000000000000000000000000000000000000000000000000000000");
strcat(w,"000000000000000000000000000000000001.500");
  n=sprintf(b,"%01500.3f",x);
  tot=tot+(((n==strlen(w))&&(strcmp(b,w)==0)));
  cells=cells+1;
  fm="%f";	/* the engine reads this format at run time, and the
	   helper caps and composes it exactly as it did the
	   literal one above */
  n=sprintf(b,fm,x);
strcpy(w,"1.500000");
  tot=tot+(((n==strlen(w))&&(strcmp(b,w)==0)));
  cells=cells+1;
  n=sprintf(b,"%%%c-%.3f-%d",97,x,7);
  strcpy(w,"%a-1.500-7");
  tot=tot+(((n==10)&&(strcmp(b,w)==0)));
  cells=cells+1;
  n=sprintf(b,"%5d %d",5,7);	/* the width run is text, the
	   letter is now recognised, and the argument count is
	   unchanged: this printed `5d 7` before M37 */
  strcpy(w,"55 7");
  tot=tot+(((n==4)&&(strcmp(b,w)==0)));
  cells=cells+1;
  n=sprintf(b,"%-2d",7);	/* was `-2d`: the flag run reaches
	   the output as text either way, but the conversion
	   happens now instead of the letter being copied too */
  strcpy(w,"-27");
  tot=tot+(((n==3)&&(strcmp(b,w)==0)));
  cells=cells+1;
  n=sprintf(b,"%*d",10,7);	/* `*` is dropped from the text a
	   float hands over; for an integer it is simply not
	   there, so the first argument is what %d prints.  Was
	   `*d`.  C would print `         7` either way. */
  strcpy(w,"10");
  tot=tot+(((n==2)&&(strcmp(b,w)==0)));
  cells=cells+1;
  printf("ffmtP|%f|%.3f|%.15f|%10.3f|\n",x,x,x,x);
  printf("TOT ffmt %d %d\n",cells,tot);
  return tot;
}
