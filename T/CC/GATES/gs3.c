#include <stdio.h>
#include <string.h>
/* M37 delegation sweep (T:/CC/GATES/gs3.c).  Expectations are the kernel's own
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
  /* id 12 bits 4202a05f20000000 */
  x=5242880000000000; q=p2e(-19); x=x*q;
  strcpy(w,"9999999999.999998");
  printf("P 12 0|%s|",w); printf("|%f|\n",x);
  n=sprintf(b,"%f",x); ck(12,0,w,n,b,"L");
  fm="%f"; n=sprintf(b,fm,x); ck(12,0,w,n,b,"R");
  strcpy(w,"10000000000");
  printf("P 12 1|%s|",w); printf("|%.0f|\n",x);
  n=sprintf(b,"%.0f",x); ck(12,1,w,n,b,"L");
  fm="%.0f"; n=sprintf(b,fm,x); ck(12,1,w,n,b,"R");
  strcpy(w,"10000000000.0");
  printf("P 12 2|%s|",w); printf("|%.1f|\n",x);
  n=sprintf(b,"%.1f",x); ck(12,2,w,n,b,"L");
  fm="%.1f"; n=sprintf(b,fm,x); ck(12,2,w,n,b,"R");
  strcpy(w,"10000000000.000");
  printf("P 12 3|%s|",w); printf("|%.3f|\n",x);
  n=sprintf(b,"%.3f",x); ck(12,3,w,n,b,"L");
  fm="%.3f"; n=sprintf(b,fm,x); ck(12,3,w,n,b,"R");
  strcpy(w,"9999999999.999998");
  printf("P 12 4|%s|",w); printf("|%.6f|\n",x);
  n=sprintf(b,"%.6f",x); ck(12,4,w,n,b,"L");
  fm="%.6f"; n=sprintf(b,fm,x); ck(12,4,w,n,b,"R");
  strcpy(w,"9999999999.999995200000000");
  printf("P 12 5|%s|",w); printf("|%.15f|\n",x);
  n=sprintf(b,"%.15f",x); ck(12,5,w,n,b,"L");
  fm="%.15f"; n=sprintf(b,fm,x); ck(12,5,w,n,b,"R");
  strcpy(w,"9999999999.9999952000000000");
  printf("P 12 6|%s|",w); printf("|%.16f|\n",x);
  n=sprintf(b,"%.16f",x); ck(12,6,w,n,b,"L");
  fm="%.16f"; n=sprintf(b,fm,x); ck(12,6,w,n,b,"R");
  strcpy(w,"10000000000.000");
  printf("P 12 7|%s|",w); printf("|%10.3f|\n",x);
  n=sprintf(b,"%10.3f",x); ck(12,7,w,n,b,"L");
  fm="%10.3f"; n=sprintf(b,fm,x); ck(12,7,w,n,b,"R");
  strcpy(w,"9999999999.99999520000000000000");
  printf("P 12 8|%s|",w); printf("|%.20f|\n",x);
  n=sprintf(b,"%.20f",x); ck(12,8,w,n,b,"L");
  fm="%.20f"; n=sprintf(b,fm,x); ck(12,8,w,n,b,"R");
  /* id 13 bits 42d6bcc41e900000 */
  x=6400000000000000; q=p2e(-6); x=x*q;
  strcpy(w,"99999999999999.984000");
  printf("P 13 0|%s|",w); printf("|%f|\n",x);
  n=sprintf(b,"%f",x); ck(13,0,w,n,b,"L");
  fm="%f"; n=sprintf(b,fm,x); ck(13,0,w,n,b,"R");
  strcpy(w,"100000000000000");
  printf("P 13 1|%s|",w); printf("|%.0f|\n",x);
  n=sprintf(b,"%.0f",x); ck(13,1,w,n,b,"L");
  fm="%.0f"; n=sprintf(b,fm,x); ck(13,1,w,n,b,"R");
  strcpy(w,"100000000000000.0");
  printf("P 13 2|%s|",w); printf("|%.1f|\n",x);
  n=sprintf(b,"%.1f",x); ck(13,2,w,n,b,"L");
  fm="%.1f"; n=sprintf(b,fm,x); ck(13,2,w,n,b,"R");
  strcpy(w,"99999999999999.984");
  printf("P 13 3|%s|",w); printf("|%.3f|\n",x);
  n=sprintf(b,"%.3f",x); ck(13,3,w,n,b,"L");
  fm="%.3f"; n=sprintf(b,fm,x); ck(13,3,w,n,b,"R");
  strcpy(w,"99999999999999.984000");
  printf("P 13 4|%s|",w); printf("|%.6f|\n",x);
  n=sprintf(b,"%.6f",x); ck(13,4,w,n,b,"L");
  fm="%.6f"; n=sprintf(b,fm,x); ck(13,4,w,n,b,"R");
  strcpy(w,"99999999999999.984000000000000");
  printf("P 13 5|%s|",w); printf("|%.15f|\n",x);
  n=sprintf(b,"%.15f",x); ck(13,5,w,n,b,"L");
  fm="%.15f"; n=sprintf(b,fm,x); ck(13,5,w,n,b,"R");
  strcpy(w,"99999999999999.9840000000000000");
  printf("P 13 6|%s|",w); printf("|%.16f|\n",x);
  n=sprintf(b,"%.16f",x); ck(13,6,w,n,b,"L");
  fm="%.16f"; n=sprintf(b,fm,x); ck(13,6,w,n,b,"R");
  strcpy(w,"99999999999999.984");
  printf("P 13 7|%s|",w); printf("|%10.3f|\n",x);
  n=sprintf(b,"%10.3f",x); ck(13,7,w,n,b,"L");
  fm="%10.3f"; n=sprintf(b,fm,x); ck(13,7,w,n,b,"R");
  strcpy(w,"99999999999999.98400000000000000000");
  printf("P 13 8|%s|",w); printf("|%.20f|\n",x);
  n=sprintf(b,"%.20f",x); ck(13,8,w,n,b,"L");
  fm="%.20f"; n=sprintf(b,fm,x); ck(13,8,w,n,b,"R");
  /* id 14 bits 430c6bf526340000 */
  x=8000000000000000; q=p2e(-3); x=x*q;
  strcpy(w,"999999999999999.680000");
  printf("P 14 0|%s|",w); printf("|%f|\n",x);
  n=sprintf(b,"%f",x); ck(14,0,w,n,b,"L");
  fm="%f"; n=sprintf(b,fm,x); ck(14,0,w,n,b,"R");
  strcpy(w,"1000000000000000");
  printf("P 14 1|%s|",w); printf("|%.0f|\n",x);
  n=sprintf(b,"%.0f",x); ck(14,1,w,n,b,"L");
  fm="%.0f"; n=sprintf(b,fm,x); ck(14,1,w,n,b,"R");
  strcpy(w,"999999999999999.8");
  printf("P 14 2|%s|",w); printf("|%.1f|\n",x);
  n=sprintf(b,"%.1f",x); ck(14,2,w,n,b,"L");
  fm="%.1f"; n=sprintf(b,fm,x); ck(14,2,w,n,b,"R");
  strcpy(w,"999999999999999.680");
  printf("P 14 3|%s|",w); printf("|%.3f|\n",x);
  n=sprintf(b,"%.3f",x); ck(14,3,w,n,b,"L");
  fm="%.3f"; n=sprintf(b,fm,x); ck(14,3,w,n,b,"R");
  strcpy(w,"999999999999999.680000");
  printf("P 14 4|%s|",w); printf("|%.6f|\n",x);
  n=sprintf(b,"%.6f",x); ck(14,4,w,n,b,"L");
  fm="%.6f"; n=sprintf(b,fm,x); ck(14,4,w,n,b,"R");
  strcpy(w,"999999999999999.680000000000000");
  printf("P 14 5|%s|",w); printf("|%.15f|\n",x);
  n=sprintf(b,"%.15f",x); ck(14,5,w,n,b,"L");
  fm="%.15f"; n=sprintf(b,fm,x); ck(14,5,w,n,b,"R");
  strcpy(w,"999999999999999.6800000000000000");
  printf("P 14 6|%s|",w); printf("|%.16f|\n",x);
  n=sprintf(b,"%.16f",x); ck(14,6,w,n,b,"L");
  fm="%.16f"; n=sprintf(b,fm,x); ck(14,6,w,n,b,"R");
  strcpy(w,"999999999999999.680");
  printf("P 14 7|%s|",w); printf("|%10.3f|\n",x);
  n=sprintf(b,"%10.3f",x); ck(14,7,w,n,b,"L");
  fm="%10.3f"; n=sprintf(b,fm,x); ck(14,7,w,n,b,"R");
  strcpy(w,"999999999999999.68000000000000000000");
  printf("P 14 8|%s|",w); printf("|%.20f|\n",x);
  n=sprintf(b,"%.20f",x); ck(14,8,w,n,b,"L");
  fm="%.20f"; n=sprintf(b,fm,x); ck(14,8,w,n,b,"R");
  /* id 15 bits 4341c37937e08000 */
  x=5000000000000000; q=p2e(1); x=x*q;
  strcpy(w,"9999999999999998.400000");
  printf("P 15 0|%s|",w); printf("|%f|\n",x);
  n=sprintf(b,"%f",x); ck(15,0,w,n,b,"L");
  fm="%f"; n=sprintf(b,fm,x); ck(15,0,w,n,b,"R");
  strcpy(w,"10000000000000000");
  printf("P 15 1|%s|",w); printf("|%.0f|\n",x);
  n=sprintf(b,"%.0f",x); ck(15,1,w,n,b,"L");
  fm="%.0f"; n=sprintf(b,fm,x); ck(15,1,w,n,b,"R");
  strcpy(w,"9999999999999998.4");
  printf("P 15 2|%s|",w); printf("|%.1f|\n",x);
  n=sprintf(b,"%.1f",x); ck(15,2,w,n,b,"L");
  fm="%.1f"; n=sprintf(b,fm,x); ck(15,2,w,n,b,"R");
  strcpy(w,"9999999999999998.400");
  printf("P 15 3|%s|",w); printf("|%.3f|\n",x);
  n=sprintf(b,"%.3f",x); ck(15,3,w,n,b,"L");
  fm="%.3f"; n=sprintf(b,fm,x); ck(15,3,w,n,b,"R");
  strcpy(w,"9999999999999998.400000");
  printf("P 15 4|%s|",w); printf("|%.6f|\n",x);
  n=sprintf(b,"%.6f",x); ck(15,4,w,n,b,"L");
  fm="%.6f"; n=sprintf(b,fm,x); ck(15,4,w,n,b,"R");
  strcpy(w,"9999999999999998.400000000000000");
  printf("P 15 5|%s|",w); printf("|%.15f|\n",x);
  n=sprintf(b,"%.15f",x); ck(15,5,w,n,b,"L");
  fm="%.15f"; n=sprintf(b,fm,x); ck(15,5,w,n,b,"R");
  strcpy(w,"9999999999999998.4000000000000000");
  printf("P 15 6|%s|",w); printf("|%.16f|\n",x);
  n=sprintf(b,"%.16f",x); ck(15,6,w,n,b,"L");
  fm="%.16f"; n=sprintf(b,fm,x); ck(15,6,w,n,b,"R");
  strcpy(w,"9999999999999998.400");
  printf("P 15 7|%s|",w); printf("|%10.3f|\n",x);
  n=sprintf(b,"%10.3f",x); ck(15,7,w,n,b,"L");
  fm="%10.3f"; n=sprintf(b,fm,x); ck(15,7,w,n,b,"R");
  strcpy(w,"9999999999999998.40000000000000000000");
  printf("P 15 8|%s|",w); printf("|%.20f|\n",x);
  n=sprintf(b,"%.20f",x); ck(15,8,w,n,b,"L");
  fm="%.20f"; n=sprintf(b,fm,x); ck(15,8,w,n,b,"R");
  printf("TOT T:/CC/GATES/gs %d cells %d fails\n",cells,fails);
  return fails;
}
