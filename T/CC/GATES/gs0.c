#include <stdio.h>
#include <string.h>
/* M37 delegation sweep (T:/CC/GATES/gs0.c).  Expectations are the kernel's own
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
  /* id 0 bits 3ff8000000000000 */
  x=6755399441055744; q=p2e(-52); x=x*q;
  strcpy(w,"1.500000");
  printf("P 0 0|%s|",w); printf("|%f|\n",x);
  n=sprintf(b,"%f",x); ck(0,0,w,n,b,"L");
  fm="%f"; n=sprintf(b,fm,x); ck(0,0,w,n,b,"R");
  strcpy(w,"2");
  printf("P 0 1|%s|",w); printf("|%.0f|\n",x);
  n=sprintf(b,"%.0f",x); ck(0,1,w,n,b,"L");
  fm="%.0f"; n=sprintf(b,fm,x); ck(0,1,w,n,b,"R");
  strcpy(w,"1.5");
  printf("P 0 2|%s|",w); printf("|%.1f|\n",x);
  n=sprintf(b,"%.1f",x); ck(0,2,w,n,b,"L");
  fm="%.1f"; n=sprintf(b,fm,x); ck(0,2,w,n,b,"R");
  strcpy(w,"1.500");
  printf("P 0 3|%s|",w); printf("|%.3f|\n",x);
  n=sprintf(b,"%.3f",x); ck(0,3,w,n,b,"L");
  fm="%.3f"; n=sprintf(b,fm,x); ck(0,3,w,n,b,"R");
  strcpy(w,"1.500000");
  printf("P 0 4|%s|",w); printf("|%.6f|\n",x);
  n=sprintf(b,"%.6f",x); ck(0,4,w,n,b,"L");
  fm="%.6f"; n=sprintf(b,fm,x); ck(0,4,w,n,b,"R");
  strcpy(w,"1.499999999999998");
  printf("P 0 5|%s|",w); printf("|%.15f|\n",x);
  n=sprintf(b,"%.15f",x); ck(0,5,w,n,b,"L");
  fm="%.15f"; n=sprintf(b,fm,x); ck(0,5,w,n,b,"R");
  strcpy(w,"1.4999999999999970");
  printf("P 0 6|%s|",w); printf("|%.16f|\n",x);
  n=sprintf(b,"%.16f",x); ck(0,6,w,n,b,"L");
  fm="%.16f"; n=sprintf(b,fm,x); ck(0,6,w,n,b,"R");
  strcpy(w,"     1.500");
  printf("P 0 7|%s|",w); printf("|%10.3f|\n",x);
  n=sprintf(b,"%10.3f",x); ck(0,7,w,n,b,"L");
  fm="%10.3f"; n=sprintf(b,fm,x); ck(0,7,w,n,b,"R");
  strcpy(w,"1.49999999999999648000");
  printf("P 0 8|%s|",w); printf("|%.20f|\n",x);
  n=sprintf(b,"%.20f",x); ck(0,8,w,n,b,"L");
  fm="%.20f"; n=sprintf(b,fm,x); ck(0,8,w,n,b,"R");
  /* id 1 bits bff8000000000000 */
  x=6755399441055744; q=p2e(-52); x=x*q;
  x=-x;
  strcpy(w,"-1.500000");
  printf("P 1 0|%s|",w); printf("|%f|\n",x);
  n=sprintf(b,"%f",x); ck(1,0,w,n,b,"L");
  fm="%f"; n=sprintf(b,fm,x); ck(1,0,w,n,b,"R");
  strcpy(w,"-2");
  printf("P 1 1|%s|",w); printf("|%.0f|\n",x);
  n=sprintf(b,"%.0f",x); ck(1,1,w,n,b,"L");
  fm="%.0f"; n=sprintf(b,fm,x); ck(1,1,w,n,b,"R");
  strcpy(w,"-1.5");
  printf("P 1 2|%s|",w); printf("|%.1f|\n",x);
  n=sprintf(b,"%.1f",x); ck(1,2,w,n,b,"L");
  fm="%.1f"; n=sprintf(b,fm,x); ck(1,2,w,n,b,"R");
  strcpy(w,"-1.500");
  printf("P 1 3|%s|",w); printf("|%.3f|\n",x);
  n=sprintf(b,"%.3f",x); ck(1,3,w,n,b,"L");
  fm="%.3f"; n=sprintf(b,fm,x); ck(1,3,w,n,b,"R");
  strcpy(w,"-1.500000");
  printf("P 1 4|%s|",w); printf("|%.6f|\n",x);
  n=sprintf(b,"%.6f",x); ck(1,4,w,n,b,"L");
  fm="%.6f"; n=sprintf(b,fm,x); ck(1,4,w,n,b,"R");
  strcpy(w,"-1.499999999999998");
  printf("P 1 5|%s|",w); printf("|%.15f|\n",x);
  n=sprintf(b,"%.15f",x); ck(1,5,w,n,b,"L");
  fm="%.15f"; n=sprintf(b,fm,x); ck(1,5,w,n,b,"R");
  strcpy(w,"-1.4999999999999970");
  printf("P 1 6|%s|",w); printf("|%.16f|\n",x);
  n=sprintf(b,"%.16f",x); ck(1,6,w,n,b,"L");
  fm="%.16f"; n=sprintf(b,fm,x); ck(1,6,w,n,b,"R");
  strcpy(w,"    -1.500");
  printf("P 1 7|%s|",w); printf("|%10.3f|\n",x);
  n=sprintf(b,"%10.3f",x); ck(1,7,w,n,b,"L");
  fm="%10.3f"; n=sprintf(b,fm,x); ck(1,7,w,n,b,"R");
  strcpy(w,"-1.49999999999999648000");
  printf("P 1 8|%s|",w); printf("|%.20f|\n",x);
  n=sprintf(b,"%.20f",x); ck(1,8,w,n,b,"L");
  fm="%.20f"; n=sprintf(b,fm,x); ck(1,8,w,n,b,"R");
  /* id 2 bits 0000000000000000 */
  x=0; q=p2e(-1074); x=x*q;
  strcpy(w,"0.000000");
  printf("P 2 0|%s|",w); printf("|%f|\n",x);
  n=sprintf(b,"%f",x); ck(2,0,w,n,b,"L");
  fm="%f"; n=sprintf(b,fm,x); ck(2,0,w,n,b,"R");
  strcpy(w,"0");
  printf("P 2 1|%s|",w); printf("|%.0f|\n",x);
  n=sprintf(b,"%.0f",x); ck(2,1,w,n,b,"L");
  fm="%.0f"; n=sprintf(b,fm,x); ck(2,1,w,n,b,"R");
  strcpy(w,"0.0");
  printf("P 2 2|%s|",w); printf("|%.1f|\n",x);
  n=sprintf(b,"%.1f",x); ck(2,2,w,n,b,"L");
  fm="%.1f"; n=sprintf(b,fm,x); ck(2,2,w,n,b,"R");
  strcpy(w,"0.000");
  printf("P 2 3|%s|",w); printf("|%.3f|\n",x);
  n=sprintf(b,"%.3f",x); ck(2,3,w,n,b,"L");
  fm="%.3f"; n=sprintf(b,fm,x); ck(2,3,w,n,b,"R");
  strcpy(w,"0.000000");
  printf("P 2 4|%s|",w); printf("|%.6f|\n",x);
  n=sprintf(b,"%.6f",x); ck(2,4,w,n,b,"L");
  fm="%.6f"; n=sprintf(b,fm,x); ck(2,4,w,n,b,"R");
  strcpy(w,"0.000000000000000");
  printf("P 2 5|%s|",w); printf("|%.15f|\n",x);
  n=sprintf(b,"%.15f",x); ck(2,5,w,n,b,"L");
  fm="%.15f"; n=sprintf(b,fm,x); ck(2,5,w,n,b,"R");
  strcpy(w,"0.0000000000000000");
  printf("P 2 6|%s|",w); printf("|%.16f|\n",x);
  n=sprintf(b,"%.16f",x); ck(2,6,w,n,b,"L");
  fm="%.16f"; n=sprintf(b,fm,x); ck(2,6,w,n,b,"R");
  strcpy(w,"     0.000");
  printf("P 2 7|%s|",w); printf("|%10.3f|\n",x);
  n=sprintf(b,"%10.3f",x); ck(2,7,w,n,b,"L");
  fm="%10.3f"; n=sprintf(b,fm,x); ck(2,7,w,n,b,"R");
  strcpy(w,"0.00000000000000000000");
  printf("P 2 8|%s|",w); printf("|%.20f|\n",x);
  n=sprintf(b,"%.20f",x); ck(2,8,w,n,b,"L");
  fm="%.20f"; n=sprintf(b,fm,x); ck(2,8,w,n,b,"R");
  /* id 3 bits 8000000000000000 */
  x=0; q=p2e(-1074); x=x*q;
  x=-x;
  strcpy(w,"-0.000000");
  printf("P 3 0|%s|",w); printf("|%f|\n",x);
  n=sprintf(b,"%f",x); ck(3,0,w,n,b,"L");
  fm="%f"; n=sprintf(b,fm,x); ck(3,0,w,n,b,"R");
  strcpy(w,"-0");
  printf("P 3 1|%s|",w); printf("|%.0f|\n",x);
  n=sprintf(b,"%.0f",x); ck(3,1,w,n,b,"L");
  fm="%.0f"; n=sprintf(b,fm,x); ck(3,1,w,n,b,"R");
  strcpy(w,"-0.0");
  printf("P 3 2|%s|",w); printf("|%.1f|\n",x);
  n=sprintf(b,"%.1f",x); ck(3,2,w,n,b,"L");
  fm="%.1f"; n=sprintf(b,fm,x); ck(3,2,w,n,b,"R");
  strcpy(w,"-0.000");
  printf("P 3 3|%s|",w); printf("|%.3f|\n",x);
  n=sprintf(b,"%.3f",x); ck(3,3,w,n,b,"L");
  fm="%.3f"; n=sprintf(b,fm,x); ck(3,3,w,n,b,"R");
  strcpy(w,"-0.000000");
  printf("P 3 4|%s|",w); printf("|%.6f|\n",x);
  n=sprintf(b,"%.6f",x); ck(3,4,w,n,b,"L");
  fm="%.6f"; n=sprintf(b,fm,x); ck(3,4,w,n,b,"R");
  strcpy(w,"-0.000000000000000");
  printf("P 3 5|%s|",w); printf("|%.15f|\n",x);
  n=sprintf(b,"%.15f",x); ck(3,5,w,n,b,"L");
  fm="%.15f"; n=sprintf(b,fm,x); ck(3,5,w,n,b,"R");
  strcpy(w,"-0.0000000000000000");
  printf("P 3 6|%s|",w); printf("|%.16f|\n",x);
  n=sprintf(b,"%.16f",x); ck(3,6,w,n,b,"L");
  fm="%.16f"; n=sprintf(b,fm,x); ck(3,6,w,n,b,"R");
  strcpy(w,"    -0.000");
  printf("P 3 7|%s|",w); printf("|%10.3f|\n",x);
  n=sprintf(b,"%10.3f",x); ck(3,7,w,n,b,"L");
  fm="%10.3f"; n=sprintf(b,fm,x); ck(3,7,w,n,b,"R");
  strcpy(w,"-0.00000000000000000000");
  printf("P 3 8|%s|",w); printf("|%.20f|\n",x);
  n=sprintf(b,"%.20f",x); ck(3,8,w,n,b,"L");
  fm="%.20f"; n=sprintf(b,fm,x); ck(3,8,w,n,b,"R");
  printf("TOT T:/CC/GATES/gs %d cells %d fails\n",cells,fails);
  return fails;
}
