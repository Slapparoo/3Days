#include <stdio.h>
#include <string.h>
/* M37 delegation sweep (T:/CC/GATES/gs7.c).  Expectations are the kernel's own
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
  /* id 28 bits fff0000000000000 */
  x=1.0/0.0;
  x=-x;
  strcpy(w,"-i");
  printf("P 28 0|%s|",w); printf("|%f|\n",x);
  n=sprintf(b,"%f",x); ck(28,0,w,n,b,"L");
  fm="%f"; n=sprintf(b,fm,x); ck(28,0,w,n,b,"R");
  strcpy(w,"-i");
  printf("P 28 1|%s|",w); printf("|%.0f|\n",x);
  n=sprintf(b,"%.0f",x); ck(28,1,w,n,b,"L");
  fm="%.0f"; n=sprintf(b,fm,x); ck(28,1,w,n,b,"R");
  strcpy(w,"-i");
  printf("P 28 2|%s|",w); printf("|%.1f|\n",x);
  n=sprintf(b,"%.1f",x); ck(28,2,w,n,b,"L");
  fm="%.1f"; n=sprintf(b,fm,x); ck(28,2,w,n,b,"R");
  strcpy(w,"-i");
  printf("P 28 3|%s|",w); printf("|%.3f|\n",x);
  n=sprintf(b,"%.3f",x); ck(28,3,w,n,b,"L");
  fm="%.3f"; n=sprintf(b,fm,x); ck(28,3,w,n,b,"R");
  strcpy(w,"-i");
  printf("P 28 4|%s|",w); printf("|%.6f|\n",x);
  n=sprintf(b,"%.6f",x); ck(28,4,w,n,b,"L");
  fm="%.6f"; n=sprintf(b,fm,x); ck(28,4,w,n,b,"R");
  strcpy(w,"-i");
  printf("P 28 5|%s|",w); printf("|%.15f|\n",x);
  n=sprintf(b,"%.15f",x); ck(28,5,w,n,b,"L");
  fm="%.15f"; n=sprintf(b,fm,x); ck(28,5,w,n,b,"R");
  strcpy(w,"-i");
  printf("P 28 6|%s|",w); printf("|%.16f|\n",x);
  n=sprintf(b,"%.16f",x); ck(28,6,w,n,b,"L");
  fm="%.16f"; n=sprintf(b,fm,x); ck(28,6,w,n,b,"R");
  strcpy(w,"        -i");
  printf("P 28 7|%s|",w); printf("|%10.3f|\n",x);
  n=sprintf(b,"%10.3f",x); ck(28,7,w,n,b,"L");
  fm="%10.3f"; n=sprintf(b,fm,x); ck(28,7,w,n,b,"R");
  strcpy(w,"-i");
  printf("P 28 8|%s|",w); printf("|%.20f|\n",x);
  n=sprintf(b,"%.20f",x); ck(28,8,w,n,b,"L");
  fm="%.20f"; n=sprintf(b,fm,x); ck(28,8,w,n,b,"R");
  /* id 29 bits 7ff8000000000000 */
  x=0.0/0.0;
  x=-x;	/* x87s real indefinite is a NEGATIVE quiet
	   NaN, so it is flipped to reach the pattern measured
	   here.  gcc gets the other NaN. */
  strcpy(w,"9223372036854.775808");
  printf("P 29 0|%s|",w); printf("|%f|\n",x);
  n=sprintf(b,"%f",x); ck(29,0,w,n,b,"L");
  fm="%f"; n=sprintf(b,fm,x); ck(29,0,w,n,b,"R");
  strcpy(w,"9223372036854775808");
  printf("P 29 1|%s|",w); printf("|%.0f|\n",x);
  n=sprintf(b,"%.0f",x); ck(29,1,w,n,b,"L");
  fm="%.0f"; n=sprintf(b,fm,x); ck(29,1,w,n,b,"R");
  strcpy(w,"922337203685477580.8");
  printf("P 29 2|%s|",w); printf("|%.1f|\n",x);
  n=sprintf(b,"%.1f",x); ck(29,2,w,n,b,"L");
  fm="%.1f"; n=sprintf(b,fm,x); ck(29,2,w,n,b,"R");
  strcpy(w,"9223372036854775.808");
  printf("P 29 3|%s|",w); printf("|%.3f|\n",x);
  n=sprintf(b,"%.3f",x); ck(29,3,w,n,b,"L");
  fm="%.3f"; n=sprintf(b,fm,x); ck(29,3,w,n,b,"R");
  strcpy(w,"9223372036854.775808");
  printf("P 29 4|%s|",w); printf("|%.6f|\n",x);
  n=sprintf(b,"%.6f",x); ck(29,4,w,n,b,"L");
  fm="%.6f"; n=sprintf(b,fm,x); ck(29,4,w,n,b,"R");
  strcpy(w,"9223.372036854775808");
  printf("P 29 5|%s|",w); printf("|%.15f|\n",x);
  n=sprintf(b,"%.15f",x); ck(29,5,w,n,b,"L");
  fm="%.15f"; n=sprintf(b,fm,x); ck(29,5,w,n,b,"R");
  strcpy(w,"922.3372036854775808");
  printf("P 29 6|%s|",w); printf("|%.16f|\n",x);
  n=sprintf(b,"%.16f",x); ck(29,6,w,n,b,"L");
  fm="%.16f"; n=sprintf(b,fm,x); ck(29,6,w,n,b,"R");
  strcpy(w,"9223372036854775.808");
  printf("P 29 7|%s|",w); printf("|%10.3f|\n",x);
  n=sprintf(b,"%10.3f",x); ck(29,7,w,n,b,"L");
  fm="%10.3f"; n=sprintf(b,fm,x); ck(29,7,w,n,b,"R");
  strcpy(w,"0.09223372036854775808");
  printf("P 29 8|%s|",w); printf("|%.20f|\n",x);
  n=sprintf(b,"%.20f",x); ck(29,8,w,n,b,"L");
  fm="%.20f"; n=sprintf(b,fm,x); ck(29,8,w,n,b,"R");
  /* id 30 bits 0000000000000001 */
  x=1; q=p2e(-1074); x=x*q;
  strcpy(w,"0.000000");
  printf("P 30 0|%s|",w); printf("|%f|\n",x);
  n=sprintf(b,"%f",x); ck(30,0,w,n,b,"L");
  fm="%f"; n=sprintf(b,fm,x); ck(30,0,w,n,b,"R");
  strcpy(w,"0");
  printf("P 30 1|%s|",w); printf("|%.0f|\n",x);
  n=sprintf(b,"%.0f",x); ck(30,1,w,n,b,"L");
  fm="%.0f"; n=sprintf(b,fm,x); ck(30,1,w,n,b,"R");
  strcpy(w,"0.0");
  printf("P 30 2|%s|",w); printf("|%.1f|\n",x);
  n=sprintf(b,"%.1f",x); ck(30,2,w,n,b,"L");
  fm="%.1f"; n=sprintf(b,fm,x); ck(30,2,w,n,b,"R");
  strcpy(w,"0.000");
  printf("P 30 3|%s|",w); printf("|%.3f|\n",x);
  n=sprintf(b,"%.3f",x); ck(30,3,w,n,b,"L");
  fm="%.3f"; n=sprintf(b,fm,x); ck(30,3,w,n,b,"R");
  strcpy(w,"0.000000");
  printf("P 30 4|%s|",w); printf("|%.6f|\n",x);
  n=sprintf(b,"%.6f",x); ck(30,4,w,n,b,"L");
  fm="%.6f"; n=sprintf(b,fm,x); ck(30,4,w,n,b,"R");
  strcpy(w,"0.000000000000000");
  printf("P 30 5|%s|",w); printf("|%.15f|\n",x);
  n=sprintf(b,"%.15f",x); ck(30,5,w,n,b,"L");
  fm="%.15f"; n=sprintf(b,fm,x); ck(30,5,w,n,b,"R");
  strcpy(w,"0.0000000000000000");
  printf("P 30 6|%s|",w); printf("|%.16f|\n",x);
  n=sprintf(b,"%.16f",x); ck(30,6,w,n,b,"L");
  fm="%.16f"; n=sprintf(b,fm,x); ck(30,6,w,n,b,"R");
  strcpy(w,"     0.000");
  printf("P 30 7|%s|",w); printf("|%10.3f|\n",x);
  n=sprintf(b,"%10.3f",x); ck(30,7,w,n,b,"L");
  fm="%10.3f"; n=sprintf(b,fm,x); ck(30,7,w,n,b,"R");
  strcpy(w,"0.00000000000000000000");
  printf("P 30 8|%s|",w); printf("|%.20f|\n",x);
  n=sprintf(b,"%.20f",x); ck(30,8,w,n,b,"L");
  fm="%.20f"; n=sprintf(b,fm,x); ck(30,8,w,n,b,"R");
  /* id 31 bits 3eb0c6f7a0b5ed8d */
  x=4722366482869645; q=p2e(-72); x=x*q;
  strcpy(w,"0.000001");
  printf("P 31 0|%s|",w); printf("|%f|\n",x);
  n=sprintf(b,"%f",x); ck(31,0,w,n,b,"L");
  fm="%f"; n=sprintf(b,fm,x); ck(31,0,w,n,b,"R");
  strcpy(w,"0");
  printf("P 31 1|%s|",w); printf("|%.0f|\n",x);
  n=sprintf(b,"%.0f",x); ck(31,1,w,n,b,"L");
  fm="%.0f"; n=sprintf(b,fm,x); ck(31,1,w,n,b,"R");
  strcpy(w,"0.0");
  printf("P 31 2|%s|",w); printf("|%.1f|\n",x);
  n=sprintf(b,"%.1f",x); ck(31,2,w,n,b,"L");
  fm="%.1f"; n=sprintf(b,fm,x); ck(31,2,w,n,b,"R");
  strcpy(w,"0.000");
  printf("P 31 3|%s|",w); printf("|%.3f|\n",x);
  n=sprintf(b,"%.3f",x); ck(31,3,w,n,b,"L");
  fm="%.3f"; n=sprintf(b,fm,x); ck(31,3,w,n,b,"R");
  strcpy(w,"0.000001");
  printf("P 31 4|%s|",w); printf("|%.6f|\n",x);
  n=sprintf(b,"%.6f",x); ck(31,4,w,n,b,"L");
  fm="%.6f"; n=sprintf(b,fm,x); ck(31,4,w,n,b,"R");
  strcpy(w,"0.000001000000000");
  printf("P 31 5|%s|",w); printf("|%.15f|\n",x);
  n=sprintf(b,"%.15f",x); ck(31,5,w,n,b,"L");
  fm="%.15f"; n=sprintf(b,fm,x); ck(31,5,w,n,b,"R");
  strcpy(w,"0.0000010000000000");
  printf("P 31 6|%s|",w); printf("|%.16f|\n",x);
  n=sprintf(b,"%.16f",x); ck(31,6,w,n,b,"L");
  fm="%.16f"; n=sprintf(b,fm,x); ck(31,6,w,n,b,"R");
  strcpy(w,"     0.000");
  printf("P 31 7|%s|",w); printf("|%10.3f|\n",x);
  n=sprintf(b,"%10.3f",x); ck(31,7,w,n,b,"L");
  fm="%10.3f"; n=sprintf(b,fm,x); ck(31,7,w,n,b,"R");
  strcpy(w,"0.00000099999999999999");
  printf("P 31 8|%s|",w); printf("|%.20f|\n",x);
  n=sprintf(b,"%.20f",x); ck(31,8,w,n,b,"L");
  fm="%.20f"; n=sprintf(b,fm,x); ck(31,8,w,n,b,"R");
  printf("TOT T:/CC/GATES/gs %d cells %d fails\n",cells,fails);
  return fails;
}
