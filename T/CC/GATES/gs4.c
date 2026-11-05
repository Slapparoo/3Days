#include <stdio.h>
#include <string.h>
/* M37 delegation sweep (T:/CC/GATES/gs4.c).  Expectations are the kernel's own
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
  /* id 16 bits 4376345785d8a000 */
  x=6250000000000000; q=p2e(4); x=x*q;
  strcpy(w,"100000000000000000.000000");
  printf("P 16 0|%s|",w); printf("|%f|\n",x);
  n=sprintf(b,"%f",x); ck(16,0,w,n,b,"L");
  fm="%f"; n=sprintf(b,fm,x); ck(16,0,w,n,b,"R");
  strcpy(w,"100000000000000000");
  printf("P 16 1|%s|",w); printf("|%.0f|\n",x);
  n=sprintf(b,"%.0f",x); ck(16,1,w,n,b,"L");
  fm="%.0f"; n=sprintf(b,fm,x); ck(16,1,w,n,b,"R");
  strcpy(w,"100000000000000000.0");
  printf("P 16 2|%s|",w); printf("|%.1f|\n",x);
  n=sprintf(b,"%.1f",x); ck(16,2,w,n,b,"L");
  fm="%.1f"; n=sprintf(b,fm,x); ck(16,2,w,n,b,"R");
  strcpy(w,"100000000000000000.000");
  printf("P 16 3|%s|",w); printf("|%.3f|\n",x);
  n=sprintf(b,"%.3f",x); ck(16,3,w,n,b,"L");
  fm="%.3f"; n=sprintf(b,fm,x); ck(16,3,w,n,b,"R");
  strcpy(w,"100000000000000000.000000");
  printf("P 16 4|%s|",w); printf("|%.6f|\n",x);
  n=sprintf(b,"%.6f",x); ck(16,4,w,n,b,"L");
  fm="%.6f"; n=sprintf(b,fm,x); ck(16,4,w,n,b,"R");
  strcpy(w,"100000000000000000.000000000000000");
  printf("P 16 5|%s|",w); printf("|%.15f|\n",x);
  n=sprintf(b,"%.15f",x); ck(16,5,w,n,b,"L");
  fm="%.15f"; n=sprintf(b,fm,x); ck(16,5,w,n,b,"R");
  strcpy(w,"100000000000000000.0000000000000000");
  printf("P 16 6|%s|",w); printf("|%.16f|\n",x);
  n=sprintf(b,"%.16f",x); ck(16,6,w,n,b,"L");
  fm="%.16f"; n=sprintf(b,fm,x); ck(16,6,w,n,b,"R");
  strcpy(w,"100000000000000000.000");
  printf("P 16 7|%s|",w); printf("|%10.3f|\n",x);
  n=sprintf(b,"%10.3f",x); ck(16,7,w,n,b,"L");
  fm="%10.3f"; n=sprintf(b,fm,x); ck(16,7,w,n,b,"R");
  strcpy(w,"100000000000000000.00000000000000000000");
  printf("P 16 8|%s|",w); printf("|%.20f|\n",x);
  n=sprintf(b,"%.20f",x); ck(16,8,w,n,b,"L");
  fm="%.20f"; n=sprintf(b,fm,x); ck(16,8,w,n,b,"R");
  /* id 17 bits 43abc16d674ec800 */
  x=7812500000000000; q=p2e(7); x=x*q;
  strcpy(w,"1000000000000000000.000000");
  printf("P 17 0|%s|",w); printf("|%f|\n",x);
  n=sprintf(b,"%f",x); ck(17,0,w,n,b,"L");
  fm="%f"; n=sprintf(b,fm,x); ck(17,0,w,n,b,"R");
  strcpy(w,"1000000000000000000");
  printf("P 17 1|%s|",w); printf("|%.0f|\n",x);
  n=sprintf(b,"%.0f",x); ck(17,1,w,n,b,"L");
  fm="%.0f"; n=sprintf(b,fm,x); ck(17,1,w,n,b,"R");
  strcpy(w,"1000000000000000000.0");
  printf("P 17 2|%s|",w); printf("|%.1f|\n",x);
  n=sprintf(b,"%.1f",x); ck(17,2,w,n,b,"L");
  fm="%.1f"; n=sprintf(b,fm,x); ck(17,2,w,n,b,"R");
  strcpy(w,"1000000000000000000.000");
  printf("P 17 3|%s|",w); printf("|%.3f|\n",x);
  n=sprintf(b,"%.3f",x); ck(17,3,w,n,b,"L");
  fm="%.3f"; n=sprintf(b,fm,x); ck(17,3,w,n,b,"R");
  strcpy(w,"1000000000000000000.000000");
  printf("P 17 4|%s|",w); printf("|%.6f|\n",x);
  n=sprintf(b,"%.6f",x); ck(17,4,w,n,b,"L");
  fm="%.6f"; n=sprintf(b,fm,x); ck(17,4,w,n,b,"R");
  strcpy(w,"1000000000000000000.000000000000000");
  printf("P 17 5|%s|",w); printf("|%.15f|\n",x);
  n=sprintf(b,"%.15f",x); ck(17,5,w,n,b,"L");
  fm="%.15f"; n=sprintf(b,fm,x); ck(17,5,w,n,b,"R");
  strcpy(w,"1000000000000000000.0000000000000000");
  printf("P 17 6|%s|",w); printf("|%.16f|\n",x);
  n=sprintf(b,"%.16f",x); ck(17,6,w,n,b,"L");
  fm="%.16f"; n=sprintf(b,fm,x); ck(17,6,w,n,b,"R");
  strcpy(w,"1000000000000000000.000");
  printf("P 17 7|%s|",w); printf("|%10.3f|\n",x);
  n=sprintf(b,"%10.3f",x); ck(17,7,w,n,b,"L");
  fm="%10.3f"; n=sprintf(b,fm,x); ck(17,7,w,n,b,"R");
  strcpy(w,"1000000000000000000.00000000000000000000");
  printf("P 17 8|%s|",w); printf("|%.20f|\n",x);
  n=sprintf(b,"%.20f",x); ck(17,8,w,n,b,"L");
  fm="%.20f"; n=sprintf(b,fm,x); ck(17,8,w,n,b,"R");
  /* id 18 bits 43e158e460913d00 */
  x=4882812500000000; q=p2e(11); x=x*q;
  strcpy(w,"10000000000000001600.000000");
  printf("P 18 0|%s|",w); printf("|%f|\n",x);
  n=sprintf(b,"%f",x); ck(18,0,w,n,b,"L");
  fm="%f"; n=sprintf(b,fm,x); ck(18,0,w,n,b,"R");
  strcpy(w,"10000000000000001600");
  printf("P 18 1|%s|",w); printf("|%.0f|\n",x);
  n=sprintf(b,"%.0f",x); ck(18,1,w,n,b,"L");
  fm="%.0f"; n=sprintf(b,fm,x); ck(18,1,w,n,b,"R");
  strcpy(w,"10000000000000001600.0");
  printf("P 18 2|%s|",w); printf("|%.1f|\n",x);
  n=sprintf(b,"%.1f",x); ck(18,2,w,n,b,"L");
  fm="%.1f"; n=sprintf(b,fm,x); ck(18,2,w,n,b,"R");
  strcpy(w,"10000000000000001600.000");
  printf("P 18 3|%s|",w); printf("|%.3f|\n",x);
  n=sprintf(b,"%.3f",x); ck(18,3,w,n,b,"L");
  fm="%.3f"; n=sprintf(b,fm,x); ck(18,3,w,n,b,"R");
  strcpy(w,"10000000000000001600.000000");
  printf("P 18 4|%s|",w); printf("|%.6f|\n",x);
  n=sprintf(b,"%.6f",x); ck(18,4,w,n,b,"L");
  fm="%.6f"; n=sprintf(b,fm,x); ck(18,4,w,n,b,"R");
  strcpy(w,"10000000000000001600.000000000000000");
  printf("P 18 5|%s|",w); printf("|%.15f|\n",x);
  n=sprintf(b,"%.15f",x); ck(18,5,w,n,b,"L");
  fm="%.15f"; n=sprintf(b,fm,x); ck(18,5,w,n,b,"R");
  strcpy(w,"10000000000000001600.0000000000000000");
  printf("P 18 6|%s|",w); printf("|%.16f|\n",x);
  n=sprintf(b,"%.16f",x); ck(18,6,w,n,b,"L");
  fm="%.16f"; n=sprintf(b,fm,x); ck(18,6,w,n,b,"R");
  strcpy(w,"10000000000000001600.000");
  printf("P 18 7|%s|",w); printf("|%10.3f|\n",x);
  n=sprintf(b,"%10.3f",x); ck(18,7,w,n,b,"L");
  fm="%10.3f"; n=sprintf(b,fm,x); ck(18,7,w,n,b,"R");
  strcpy(w,"10000000000000001600.00000000000000000000");
  printf("P 18 8|%s|",w); printf("|%.20f|\n",x);
  n=sprintf(b,"%.20f",x); ck(18,8,w,n,b,"L");
  fm="%.20f"; n=sprintf(b,fm,x); ck(18,8,w,n,b,"R");
  /* id 19 bits 4415af1d78b58c40 */
  x=6103515625000000; q=p2e(14); x=x*q;
  strcpy(w,"100000000000000000000.000000");
  printf("P 19 0|%s|",w); printf("|%f|\n",x);
  n=sprintf(b,"%f",x); ck(19,0,w,n,b,"L");
  fm="%f"; n=sprintf(b,fm,x); ck(19,0,w,n,b,"R");
  strcpy(w,"100000000000000000000");
  printf("P 19 1|%s|",w); printf("|%.0f|\n",x);
  n=sprintf(b,"%.0f",x); ck(19,1,w,n,b,"L");
  fm="%.0f"; n=sprintf(b,fm,x); ck(19,1,w,n,b,"R");
  strcpy(w,"100000000000000000000.0");
  printf("P 19 2|%s|",w); printf("|%.1f|\n",x);
  n=sprintf(b,"%.1f",x); ck(19,2,w,n,b,"L");
  fm="%.1f"; n=sprintf(b,fm,x); ck(19,2,w,n,b,"R");
  strcpy(w,"100000000000000000000.000");
  printf("P 19 3|%s|",w); printf("|%.3f|\n",x);
  n=sprintf(b,"%.3f",x); ck(19,3,w,n,b,"L");
  fm="%.3f"; n=sprintf(b,fm,x); ck(19,3,w,n,b,"R");
  strcpy(w,"100000000000000000000.000000");
  printf("P 19 4|%s|",w); printf("|%.6f|\n",x);
  n=sprintf(b,"%.6f",x); ck(19,4,w,n,b,"L");
  fm="%.6f"; n=sprintf(b,fm,x); ck(19,4,w,n,b,"R");
  strcpy(w,"100000000000000000000.000000000000000");
  printf("P 19 5|%s|",w); printf("|%.15f|\n",x);
  n=sprintf(b,"%.15f",x); ck(19,5,w,n,b,"L");
  fm="%.15f"; n=sprintf(b,fm,x); ck(19,5,w,n,b,"R");
  strcpy(w,"100000000000000000000.0000000000000000");
  printf("P 19 6|%s|",w); printf("|%.16f|\n",x);
  n=sprintf(b,"%.16f",x); ck(19,6,w,n,b,"L");
  fm="%.16f"; n=sprintf(b,fm,x); ck(19,6,w,n,b,"R");
  strcpy(w,"100000000000000000000.000");
  printf("P 19 7|%s|",w); printf("|%10.3f|\n",x);
  n=sprintf(b,"%10.3f",x); ck(19,7,w,n,b,"L");
  fm="%10.3f"; n=sprintf(b,fm,x); ck(19,7,w,n,b,"R");
  strcpy(w,"100000000000000000000.00000000000000000000");
  printf("P 19 8|%s|",w); printf("|%.20f|\n",x);
  n=sprintf(b,"%.20f",x); ck(19,8,w,n,b,"L");
  fm="%.20f"; n=sprintf(b,fm,x); ck(19,8,w,n,b,"R");
  printf("TOT T:/CC/GATES/gs %d cells %d fails\n",cells,fails);
  return fails;
}
