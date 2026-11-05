#include <stdio.h>
#include <string.h>
/* M37 delegation sweep (T:/CC/GATES/gs9.c).  Expectations are the kernel's own
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
  /* id 36 bits c004000000000000 */
  x=5629499534213120; q=p2e(-51); x=x*q;
  x=-x;
  strcpy(w,"-2.500000");
  printf("P 36 0|%s|",w); printf("|%f|\n",x);
  n=sprintf(b,"%f",x); ck(36,0,w,n,b,"L");
  fm="%f"; n=sprintf(b,fm,x); ck(36,0,w,n,b,"R");
  strcpy(w,"-2");
  printf("P 36 1|%s|",w); printf("|%.0f|\n",x);
  n=sprintf(b,"%.0f",x); ck(36,1,w,n,b,"L");
  fm="%.0f"; n=sprintf(b,fm,x); ck(36,1,w,n,b,"R");
  strcpy(w,"-2.5");
  printf("P 36 2|%s|",w); printf("|%.1f|\n",x);
  n=sprintf(b,"%.1f",x); ck(36,2,w,n,b,"L");
  fm="%.1f"; n=sprintf(b,fm,x); ck(36,2,w,n,b,"R");
  strcpy(w,"-2.500");
  printf("P 36 3|%s|",w); printf("|%.3f|\n",x);
  n=sprintf(b,"%.3f",x); ck(36,3,w,n,b,"L");
  fm="%.3f"; n=sprintf(b,fm,x); ck(36,3,w,n,b,"R");
  strcpy(w,"-2.500000");
  printf("P 36 4|%s|",w); printf("|%.6f|\n",x);
  n=sprintf(b,"%.6f",x); ck(36,4,w,n,b,"L");
  fm="%.6f"; n=sprintf(b,fm,x); ck(36,4,w,n,b,"R");
  strcpy(w,"-2.499999999999996");
  printf("P 36 5|%s|",w); printf("|%.15f|\n",x);
  n=sprintf(b,"%.15f",x); ck(36,5,w,n,b,"L");
  fm="%.15f"; n=sprintf(b,fm,x); ck(36,5,w,n,b,"R");
  strcpy(w,"-2.4999999999999952");
  printf("P 36 6|%s|",w); printf("|%.16f|\n",x);
  n=sprintf(b,"%.16f",x); ck(36,6,w,n,b,"L");
  fm="%.16f"; n=sprintf(b,fm,x); ck(36,6,w,n,b,"R");
  strcpy(w,"    -2.500");
  printf("P 36 7|%s|",w); printf("|%10.3f|\n",x);
  n=sprintf(b,"%10.3f",x); ck(36,7,w,n,b,"L");
  fm="%10.3f"; n=sprintf(b,fm,x); ck(36,7,w,n,b,"R");
  strcpy(w,"-2.49999999999999392000");
  printf("P 36 8|%s|",w); printf("|%.20f|\n",x);
  n=sprintf(b,"%.20f",x); ck(36,8,w,n,b,"L");
  fm="%.20f"; n=sprintf(b,fm,x); ck(36,8,w,n,b,"R");
  /* id 37 bits 000730d67819e8d2 */
  x=2024022533073106; q=p2e(-1074); x=x*q;
  strcpy(w,"0.000000");
  printf("P 37 0|%s|",w); printf("|%f|\n",x);
  n=sprintf(b,"%f",x); ck(37,0,w,n,b,"L");
  fm="%f"; n=sprintf(b,fm,x); ck(37,0,w,n,b,"R");
  strcpy(w,"0");
  printf("P 37 1|%s|",w); printf("|%.0f|\n",x);
  n=sprintf(b,"%.0f",x); ck(37,1,w,n,b,"L");
  fm="%.0f"; n=sprintf(b,fm,x); ck(37,1,w,n,b,"R");
  strcpy(w,"0.0");
  printf("P 37 2|%s|",w); printf("|%.1f|\n",x);
  n=sprintf(b,"%.1f",x); ck(37,2,w,n,b,"L");
  fm="%.1f"; n=sprintf(b,fm,x); ck(37,2,w,n,b,"R");
  strcpy(w,"0.000");
  printf("P 37 3|%s|",w); printf("|%.3f|\n",x);
  n=sprintf(b,"%.3f",x); ck(37,3,w,n,b,"L");
  fm="%.3f"; n=sprintf(b,fm,x); ck(37,3,w,n,b,"R");
  strcpy(w,"0.000000");
  printf("P 37 4|%s|",w); printf("|%.6f|\n",x);
  n=sprintf(b,"%.6f",x); ck(37,4,w,n,b,"L");
  fm="%.6f"; n=sprintf(b,fm,x); ck(37,4,w,n,b,"R");
  strcpy(w,"0.000000000000000");
  printf("P 37 5|%s|",w); printf("|%.15f|\n",x);
  n=sprintf(b,"%.15f",x); ck(37,5,w,n,b,"L");
  fm="%.15f"; n=sprintf(b,fm,x); ck(37,5,w,n,b,"R");
  strcpy(w,"0.0000000000000000");
  printf("P 37 6|%s|",w); printf("|%.16f|\n",x);
  n=sprintf(b,"%.16f",x); ck(37,6,w,n,b,"L");
  fm="%.16f"; n=sprintf(b,fm,x); ck(37,6,w,n,b,"R");
  strcpy(w,"     0.000");
  printf("P 37 7|%s|",w); printf("|%10.3f|\n",x);
  n=sprintf(b,"%10.3f",x); ck(37,7,w,n,b,"L");
  fm="%10.3f"; n=sprintf(b,fm,x); ck(37,7,w,n,b,"R");
  strcpy(w,"0.00000000000000000000");
  printf("P 37 8|%s|",w); printf("|%.20f|\n",x);
  n=sprintf(b,"%.20f",x); ck(37,8,w,n,b,"L");
  fm="%.20f"; n=sprintf(b,fm,x); ck(37,8,w,n,b,"R");
  printf("TOT T:/CC/GATES/gs %d cells %d fails\n",cells,fails);
  return fails;
}
