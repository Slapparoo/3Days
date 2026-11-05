#include <stdio.h>
#include <string.h>
/* M37 delegation sweep (T:/CC/GATES/gs1.c).  Expectations are the kernel's own
 * renderings of these bit patterns; the value enters as an exact
 * integer below 2^53 scaled by an exact power of two, so lite and the
 * kernel see the same double.  Three sinks per cell: printf (the
 * compile-time lowering, reported as a P line for the harness to
 * compare), sprintf with a literal directive, and sprintf with the
 * same directive in a variable (the runtime engine).  A failure
 * prints FAIL and the program returns the failure count. */
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

static int cells,fails;
static char b[1200],w[512];

static void ck(int id,int k,const char *w,int n,const char *g,const char *s)
{
  if (((n!=strlen(w))||strcmp(g,w)))
  {
    printf("FAIL %d %d %s [%s] want [%s] n=%d\n",id,k,s,g,w,n);
    fails=fails+1;
  }
  cells=cells+1;
}

int main()
{
  double x,q;
  char *fm;
  int n;
  /* id 4 bits 3ff0000000000000 */
  x=4503599627370496; q=p2e(-52); x=x*q;
  strcpy(w,"1.000000");
  printf("P 4 0|%s|",w); printf("|%f|\n",x);
  n=sprintf(b,"%f",x); ck(4,0,w,n,b,"L");
  fm="%f"; n=sprintf(b,fm,x); ck(4,0,w,n,b,"R");
  strcpy(w,"1");
  printf("P 4 1|%s|",w); printf("|%.0f|\n",x);
  n=sprintf(b,"%.0f",x); ck(4,1,w,n,b,"L");
  fm="%.0f"; n=sprintf(b,fm,x); ck(4,1,w,n,b,"R");
  strcpy(w,"1.0");
  printf("P 4 2|%s|",w); printf("|%.1f|\n",x);
  n=sprintf(b,"%.1f",x); ck(4,2,w,n,b,"L");
  fm="%.1f"; n=sprintf(b,fm,x); ck(4,2,w,n,b,"R");
  strcpy(w,"1.000");
  printf("P 4 3|%s|",w); printf("|%.3f|\n",x);
  n=sprintf(b,"%.3f",x); ck(4,3,w,n,b,"L");
  fm="%.3f"; n=sprintf(b,fm,x); ck(4,3,w,n,b,"R");
  strcpy(w,"1.000000");
  printf("P 4 4|%s|",w); printf("|%.6f|\n",x);
  n=sprintf(b,"%.6f",x); ck(4,4,w,n,b,"L");
  fm="%.6f"; n=sprintf(b,fm,x); ck(4,4,w,n,b,"R");
  strcpy(w,"0.999999999999998");
  printf("P 4 5|%s|",w); printf("|%.15f|\n",x);
  n=sprintf(b,"%.15f",x); ck(4,5,w,n,b,"L");
  fm="%.15f"; n=sprintf(b,fm,x); ck(4,5,w,n,b,"R");
  strcpy(w,"0.9999999999999980");
  printf("P 4 6|%s|",w); printf("|%.16f|\n",x);
  n=sprintf(b,"%.16f",x); ck(4,6,w,n,b,"L");
  fm="%.16f"; n=sprintf(b,fm,x); ck(4,6,w,n,b,"R");
  strcpy(w,"     1.000");
  printf("P 4 7|%s|",w); printf("|%10.3f|\n",x);
  n=sprintf(b,"%10.3f",x); ck(4,7,w,n,b,"L");
  fm="%10.3f"; n=sprintf(b,fm,x); ck(4,7,w,n,b,"R");
  strcpy(w,"0.99999999999999760000");
  printf("P 4 8|%s|",w); printf("|%.20f|\n",x);
  n=sprintf(b,"%.20f",x); ck(4,8,w,n,b,"L");
  fm="%.20f"; n=sprintf(b,fm,x); ck(4,8,w,n,b,"R");
  /* id 5 bits 3fb999999999999a */
  x=7205759403792794; q=p2e(-56); x=x*q;
  strcpy(w,"0.100000");
  printf("P 5 0|%s|",w); printf("|%f|\n",x);
  n=sprintf(b,"%f",x); ck(5,0,w,n,b,"L");
  fm="%f"; n=sprintf(b,fm,x); ck(5,0,w,n,b,"R");
  strcpy(w,"0");
  printf("P 5 1|%s|",w); printf("|%.0f|\n",x);
  n=sprintf(b,"%.0f",x); ck(5,1,w,n,b,"L");
  fm="%.0f"; n=sprintf(b,fm,x); ck(5,1,w,n,b,"R");
  strcpy(w,"0.1");
  printf("P 5 2|%s|",w); printf("|%.1f|\n",x);
  n=sprintf(b,"%.1f",x); ck(5,2,w,n,b,"L");
  fm="%.1f"; n=sprintf(b,fm,x); ck(5,2,w,n,b,"R");
  strcpy(w,"0.100");
  printf("P 5 3|%s|",w); printf("|%.3f|\n",x);
  n=sprintf(b,"%.3f",x); ck(5,3,w,n,b,"L");
  fm="%.3f"; n=sprintf(b,fm,x); ck(5,3,w,n,b,"R");
  strcpy(w,"0.100000");
  printf("P 5 4|%s|",w); printf("|%.6f|\n",x);
  n=sprintf(b,"%.6f",x); ck(5,4,w,n,b,"L");
  fm="%.6f"; n=sprintf(b,fm,x); ck(5,4,w,n,b,"R");
  strcpy(w,"0.100000000000000");
  printf("P 5 5|%s|",w); printf("|%.15f|\n",x);
  n=sprintf(b,"%.15f",x); ck(5,5,w,n,b,"L");
  fm="%.15f"; n=sprintf(b,fm,x); ck(5,5,w,n,b,"R");
  strcpy(w,"0.0999999999999998");
  printf("P 5 6|%s|",w); printf("|%.16f|\n",x);
  n=sprintf(b,"%.16f",x); ck(5,6,w,n,b,"L");
  fm="%.16f"; n=sprintf(b,fm,x); ck(5,6,w,n,b,"R");
  strcpy(w,"     0.100");
  printf("P 5 7|%s|",w); printf("|%10.3f|\n",x);
  n=sprintf(b,"%10.3f",x); ck(5,7,w,n,b,"L");
  fm="%10.3f"; n=sprintf(b,fm,x); ck(5,7,w,n,b,"R");
  strcpy(w,"0.09999999999999972800");
  printf("P 5 8|%s|",w); printf("|%.20f|\n",x);
  n=sprintf(b,"%.20f",x); ck(5,8,w,n,b,"L");
  fm="%.20f"; n=sprintf(b,fm,x); ck(5,8,w,n,b,"R");
  /* id 6 bits 3fd0000000000000 */
  x=4503599627370496; q=p2e(-54); x=x*q;
  strcpy(w,"0.250000");
  printf("P 6 0|%s|",w); printf("|%f|\n",x);
  n=sprintf(b,"%f",x); ck(6,0,w,n,b,"L");
  fm="%f"; n=sprintf(b,fm,x); ck(6,0,w,n,b,"R");
  strcpy(w,"0");
  printf("P 6 1|%s|",w); printf("|%.0f|\n",x);
  n=sprintf(b,"%.0f",x); ck(6,1,w,n,b,"L");
  fm="%.0f"; n=sprintf(b,fm,x); ck(6,1,w,n,b,"R");
  strcpy(w,"0.2");
  printf("P 6 2|%s|",w); printf("|%.1f|\n",x);
  n=sprintf(b,"%.1f",x); ck(6,2,w,n,b,"L");
  fm="%.1f"; n=sprintf(b,fm,x); ck(6,2,w,n,b,"R");
  strcpy(w,"0.250");
  printf("P 6 3|%s|",w); printf("|%.3f|\n",x);
  n=sprintf(b,"%.3f",x); ck(6,3,w,n,b,"L");
  fm="%.3f"; n=sprintf(b,fm,x); ck(6,3,w,n,b,"R");
  strcpy(w,"0.250000");
  printf("P 6 4|%s|",w); printf("|%.6f|\n",x);
  n=sprintf(b,"%.6f",x); ck(6,4,w,n,b,"L");
  fm="%.6f"; n=sprintf(b,fm,x); ck(6,4,w,n,b,"R");
  strcpy(w,"0.250000000000000");
  printf("P 6 5|%s|",w); printf("|%.15f|\n",x);
  n=sprintf(b,"%.15f",x); ck(6,5,w,n,b,"L");
  fm="%.15f"; n=sprintf(b,fm,x); ck(6,5,w,n,b,"R");
  strcpy(w,"0.2499999999999995");
  printf("P 6 6|%s|",w); printf("|%.16f|\n",x);
  n=sprintf(b,"%.16f",x); ck(6,6,w,n,b,"L");
  fm="%.16f"; n=sprintf(b,fm,x); ck(6,6,w,n,b,"R");
  strcpy(w,"     0.250");
  printf("P 6 7|%s|",w); printf("|%10.3f|\n",x);
  n=sprintf(b,"%10.3f",x); ck(6,7,w,n,b,"L");
  fm="%10.3f"; n=sprintf(b,fm,x); ck(6,7,w,n,b,"R");
  strcpy(w,"0.24999999999999940000");
  printf("P 6 8|%s|",w); printf("|%.20f|\n",x);
  n=sprintf(b,"%.20f",x); ck(6,8,w,n,b,"L");
  fm="%.20f"; n=sprintf(b,fm,x); ck(6,8,w,n,b,"R");
  /* id 7 bits 3fd5555555555555 */
  x=6004799503160661; q=p2e(-54); x=x*q;
  strcpy(w,"0.333333");
  printf("P 7 0|%s|",w); printf("|%f|\n",x);
  n=sprintf(b,"%f",x); ck(7,0,w,n,b,"L");
  fm="%f"; n=sprintf(b,fm,x); ck(7,0,w,n,b,"R");
  strcpy(w,"0");
  printf("P 7 1|%s|",w); printf("|%.0f|\n",x);
  n=sprintf(b,"%.0f",x); ck(7,1,w,n,b,"L");
  fm="%.0f"; n=sprintf(b,fm,x); ck(7,1,w,n,b,"R");
  strcpy(w,"0.3");
  printf("P 7 2|%s|",w); printf("|%.1f|\n",x);
  n=sprintf(b,"%.1f",x); ck(7,2,w,n,b,"L");
  fm="%.1f"; n=sprintf(b,fm,x); ck(7,2,w,n,b,"R");
  strcpy(w,"0.333");
  printf("P 7 3|%s|",w); printf("|%.3f|\n",x);
  n=sprintf(b,"%.3f",x); ck(7,3,w,n,b,"L");
  fm="%.3f"; n=sprintf(b,fm,x); ck(7,3,w,n,b,"R");
  strcpy(w,"0.333333");
  printf("P 7 4|%s|",w); printf("|%.6f|\n",x);
  n=sprintf(b,"%.6f",x); ck(7,4,w,n,b,"L");
  fm="%.6f"; n=sprintf(b,fm,x); ck(7,4,w,n,b,"R");
  strcpy(w,"0.333333333333333");
  printf("P 7 5|%s|",w); printf("|%.15f|\n",x);
  n=sprintf(b,"%.15f",x); ck(7,5,w,n,b,"L");
  fm="%.15f"; n=sprintf(b,fm,x); ck(7,5,w,n,b,"R");
  strcpy(w,"0.3333333333333326");
  printf("P 7 6|%s|",w); printf("|%.16f|\n",x);
  n=sprintf(b,"%.16f",x); ck(7,6,w,n,b,"L");
  fm="%.16f"; n=sprintf(b,fm,x); ck(7,6,w,n,b,"R");
  strcpy(w,"     0.333");
  printf("P 7 7|%s|",w); printf("|%10.3f|\n",x);
  n=sprintf(b,"%10.3f",x); ck(7,7,w,n,b,"L");
  fm="%10.3f"; n=sprintf(b,fm,x); ck(7,7,w,n,b,"R");
  strcpy(w,"0.33333333333333252000");
  printf("P 7 8|%s|",w); printf("|%.20f|\n",x);
  n=sprintf(b,"%.20f",x); ck(7,8,w,n,b,"L");
  fm="%.20f"; n=sprintf(b,fm,x); ck(7,8,w,n,b,"R");
  printf("TOT T:/CC/GATES/gs %d cells %d fails\n",cells,fails);
  return fails;
}
