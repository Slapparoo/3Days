#include <stdio.h>
#include <string.h>
/* M37 delegation sweep (T:/CC/GATES/gs2.c).  Expectations are the kernel's own
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
  /* id 8 bits 3fe5555555555555 */
  x=6004799503160661; q=p2e(-53); x=x*q;
  strcpy(w,"0.666667");
  printf("P 8 0|%s|",w); printf("|%f|\n",x);
  n=sprintf(b,"%f",x); ck(8,0,w,n,b,"L");
  fm="%f"; n=sprintf(b,fm,x); ck(8,0,w,n,b,"R");
  strcpy(w,"1");
  printf("P 8 1|%s|",w); printf("|%.0f|\n",x);
  n=sprintf(b,"%.0f",x); ck(8,1,w,n,b,"L");
  fm="%.0f"; n=sprintf(b,fm,x); ck(8,1,w,n,b,"R");
  strcpy(w,"0.7");
  printf("P 8 2|%s|",w); printf("|%.1f|\n",x);
  n=sprintf(b,"%.1f",x); ck(8,2,w,n,b,"L");
  fm="%.1f"; n=sprintf(b,fm,x); ck(8,2,w,n,b,"R");
  strcpy(w,"0.667");
  printf("P 8 3|%s|",w); printf("|%.3f|\n",x);
  n=sprintf(b,"%.3f",x); ck(8,3,w,n,b,"L");
  fm="%.3f"; n=sprintf(b,fm,x); ck(8,3,w,n,b,"R");
  strcpy(w,"0.666667");
  printf("P 8 4|%s|",w); printf("|%.6f|\n",x);
  n=sprintf(b,"%.6f",x); ck(8,4,w,n,b,"L");
  fm="%.6f"; n=sprintf(b,fm,x); ck(8,4,w,n,b,"R");
  strcpy(w,"0.666666666666666");
  printf("P 8 5|%s|",w); printf("|%.15f|\n",x);
  n=sprintf(b,"%.15f",x); ck(8,5,w,n,b,"L");
  fm="%.15f"; n=sprintf(b,fm,x); ck(8,5,w,n,b,"R");
  strcpy(w,"0.6666666666666653");
  printf("P 8 6|%s|",w); printf("|%.16f|\n",x);
  n=sprintf(b,"%.16f",x); ck(8,6,w,n,b,"L");
  fm="%.16f"; n=sprintf(b,fm,x); ck(8,6,w,n,b,"R");
  strcpy(w,"     0.667");
  printf("P 8 7|%s|",w); printf("|%10.3f|\n",x);
  n=sprintf(b,"%10.3f",x); ck(8,7,w,n,b,"L");
  fm="%10.3f"; n=sprintf(b,fm,x); ck(8,7,w,n,b,"R");
  strcpy(w,"0.66666666666666504000");
  printf("P 8 8|%s|",w); printf("|%.20f|\n",x);
  n=sprintf(b,"%.20f",x); ck(8,8,w,n,b,"L");
  fm="%.20f"; n=sprintf(b,fm,x); ck(8,8,w,n,b,"R");
  /* id 9 bits 4024000000000000 */
  x=5629499534213120; q=p2e(-49); x=x*q;
  strcpy(w,"10.000000");
  printf("P 9 0|%s|",w); printf("|%f|\n",x);
  n=sprintf(b,"%f",x); ck(9,0,w,n,b,"L");
  fm="%f"; n=sprintf(b,fm,x); ck(9,0,w,n,b,"R");
  strcpy(w,"10");
  printf("P 9 1|%s|",w); printf("|%.0f|\n",x);
  n=sprintf(b,"%.0f",x); ck(9,1,w,n,b,"L");
  fm="%.0f"; n=sprintf(b,fm,x); ck(9,1,w,n,b,"R");
  strcpy(w,"10.0");
  printf("P 9 2|%s|",w); printf("|%.1f|\n",x);
  n=sprintf(b,"%.1f",x); ck(9,2,w,n,b,"L");
  fm="%.1f"; n=sprintf(b,fm,x); ck(9,2,w,n,b,"R");
  strcpy(w,"10.000");
  printf("P 9 3|%s|",w); printf("|%.3f|\n",x);
  n=sprintf(b,"%.3f",x); ck(9,3,w,n,b,"L");
  fm="%.3f"; n=sprintf(b,fm,x); ck(9,3,w,n,b,"R");
  strcpy(w,"10.000000");
  printf("P 9 4|%s|",w); printf("|%.6f|\n",x);
  n=sprintf(b,"%.6f",x); ck(9,4,w,n,b,"L");
  fm="%.6f"; n=sprintf(b,fm,x); ck(9,4,w,n,b,"R");
  strcpy(w,"9.999999999999984");
  printf("P 9 5|%s|",w); printf("|%.15f|\n",x);
  n=sprintf(b,"%.15f",x); ck(9,5,w,n,b,"L");
  fm="%.15f"; n=sprintf(b,fm,x); ck(9,5,w,n,b,"R");
  strcpy(w,"9.9999999999999808");
  printf("P 9 6|%s|",w); printf("|%.16f|\n",x);
  n=sprintf(b,"%.16f",x); ck(9,6,w,n,b,"L");
  fm="%.16f"; n=sprintf(b,fm,x); ck(9,6,w,n,b,"R");
  strcpy(w,"    10.000");
  printf("P 9 7|%s|",w); printf("|%10.3f|\n",x);
  n=sprintf(b,"%10.3f",x); ck(9,7,w,n,b,"L");
  fm="%10.3f"; n=sprintf(b,fm,x); ck(9,7,w,n,b,"R");
  strcpy(w,"9.99999999999998080000");
  printf("P 9 8|%s|",w); printf("|%.20f|\n",x);
  n=sprintf(b,"%.20f",x); ck(9,8,w,n,b,"L");
  fm="%.20f"; n=sprintf(b,fm,x); ck(9,8,w,n,b,"R");
  /* id 10 bits 40f86a0000000000 */
  x=6871947673600000; q=p2e(-36); x=x*q;
  strcpy(w,"100000.000000");
  printf("P 10 0|%s|",w); printf("|%f|\n",x);
  n=sprintf(b,"%f",x); ck(10,0,w,n,b,"L");
  fm="%f"; n=sprintf(b,fm,x); ck(10,0,w,n,b,"R");
  strcpy(w,"100000");
  printf("P 10 1|%s|",w); printf("|%.0f|\n",x);
  n=sprintf(b,"%.0f",x); ck(10,1,w,n,b,"L");
  fm="%.0f"; n=sprintf(b,fm,x); ck(10,1,w,n,b,"R");
  strcpy(w,"100000.0");
  printf("P 10 2|%s|",w); printf("|%.1f|\n",x);
  n=sprintf(b,"%.1f",x); ck(10,2,w,n,b,"L");
  fm="%.1f"; n=sprintf(b,fm,x); ck(10,2,w,n,b,"R");
  strcpy(w,"100000.000");
  printf("P 10 3|%s|",w); printf("|%.3f|\n",x);
  n=sprintf(b,"%.3f",x); ck(10,3,w,n,b,"L");
  fm="%.3f"; n=sprintf(b,fm,x); ck(10,3,w,n,b,"R");
  strcpy(w,"100000.000000");
  printf("P 10 4|%s|",w); printf("|%.6f|\n",x);
  n=sprintf(b,"%.6f",x); ck(10,4,w,n,b,"L");
  fm="%.6f"; n=sprintf(b,fm,x); ck(10,4,w,n,b,"R");
  strcpy(w,"99999.999999999968000");
  printf("P 10 5|%s|",w); printf("|%.15f|\n",x);
  n=sprintf(b,"%.15f",x); ck(10,5,w,n,b,"L");
  fm="%.15f"; n=sprintf(b,fm,x); ck(10,5,w,n,b,"R");
  strcpy(w,"99999.9999999999680000");
  printf("P 10 6|%s|",w); printf("|%.16f|\n",x);
  n=sprintf(b,"%.16f",x); ck(10,6,w,n,b,"L");
  fm="%.16f"; n=sprintf(b,fm,x); ck(10,6,w,n,b,"R");
  strcpy(w,"100000.000");
  printf("P 10 7|%s|",w); printf("|%10.3f|\n",x);
  n=sprintf(b,"%10.3f",x); ck(10,7,w,n,b,"L");
  fm="%10.3f"; n=sprintf(b,fm,x); ck(10,7,w,n,b,"R");
  strcpy(w,"99999.99999999996800000000");
  printf("P 10 8|%s|",w); printf("|%.20f|\n",x);
  n=sprintf(b,"%.20f",x); ck(10,8,w,n,b,"L");
  fm="%.20f"; n=sprintf(b,fm,x); ck(10,8,w,n,b,"R");
  /* id 11 bits 41cdcd6500000000 */
  x=8388608000000000; q=p2e(-23); x=x*q;
  strcpy(w,"1000000000.000000");
  printf("P 11 0|%s|",w); printf("|%f|\n",x);
  n=sprintf(b,"%f",x); ck(11,0,w,n,b,"L");
  fm="%f"; n=sprintf(b,fm,x); ck(11,0,w,n,b,"R");
  strcpy(w,"1000000000");
  printf("P 11 1|%s|",w); printf("|%.0f|\n",x);
  n=sprintf(b,"%.0f",x); ck(11,1,w,n,b,"L");
  fm="%.0f"; n=sprintf(b,fm,x); ck(11,1,w,n,b,"R");
  strcpy(w,"1000000000.0");
  printf("P 11 2|%s|",w); printf("|%.1f|\n",x);
  n=sprintf(b,"%.1f",x); ck(11,2,w,n,b,"L");
  fm="%.1f"; n=sprintf(b,fm,x); ck(11,2,w,n,b,"R");
  strcpy(w,"1000000000.000");
  printf("P 11 3|%s|",w); printf("|%.3f|\n",x);
  n=sprintf(b,"%.3f",x); ck(11,3,w,n,b,"L");
  fm="%.3f"; n=sprintf(b,fm,x); ck(11,3,w,n,b,"R");
  strcpy(w,"1000000000.000000");
  printf("P 11 4|%s|",w); printf("|%.6f|\n",x);
  n=sprintf(b,"%.6f",x); ck(11,4,w,n,b,"L");
  fm="%.6f"; n=sprintf(b,fm,x); ck(11,4,w,n,b,"R");
  strcpy(w,"999999999.999998880000000");
  printf("P 11 5|%s|",w); printf("|%.15f|\n",x);
  n=sprintf(b,"%.15f",x); ck(11,5,w,n,b,"L");
  fm="%.15f"; n=sprintf(b,fm,x); ck(11,5,w,n,b,"R");
  strcpy(w,"999999999.9999988800000000");
  printf("P 11 6|%s|",w); printf("|%.16f|\n",x);
  n=sprintf(b,"%.16f",x); ck(11,6,w,n,b,"L");
  fm="%.16f"; n=sprintf(b,fm,x); ck(11,6,w,n,b,"R");
  strcpy(w,"1000000000.000");
  printf("P 11 7|%s|",w); printf("|%10.3f|\n",x);
  n=sprintf(b,"%10.3f",x); ck(11,7,w,n,b,"L");
  fm="%10.3f"; n=sprintf(b,fm,x); ck(11,7,w,n,b,"R");
  strcpy(w,"999999999.99999888000000000000");
  printf("P 11 8|%s|",w); printf("|%.20f|\n",x);
  n=sprintf(b,"%.20f",x); ck(11,8,w,n,b,"L");
  fm="%.20f"; n=sprintf(b,fm,x); ck(11,8,w,n,b,"R");
  printf("TOT T:/CC/GATES/gs %d cells %d fails\n",cells,fails);
  return fails;
}
