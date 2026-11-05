#include <stdio.h>
#include <string.h>
/* M37 delegation sweep (T:/CC/GATES/gs8.c).  Expectations are the kernel's own
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
  /* id 32 bits 3ddb7cdfd9d7bdbb */
  x=7737125245533627; q=p2e(-86); x=x*q;
  strcpy(w,"0.000000");
  printf("P 32 0|%s|",w); printf("|%f|\n",x);
  n=sprintf(b,"%f",x); ck(32,0,w,n,b,"L");
  fm="%f"; n=sprintf(b,fm,x); ck(32,0,w,n,b,"R");
  strcpy(w,"0");
  printf("P 32 1|%s|",w); printf("|%.0f|\n",x);
  n=sprintf(b,"%.0f",x); ck(32,1,w,n,b,"L");
  fm="%.0f"; n=sprintf(b,fm,x); ck(32,1,w,n,b,"R");
  strcpy(w,"0.0");
  printf("P 32 2|%s|",w); printf("|%.1f|\n",x);
  n=sprintf(b,"%.1f",x); ck(32,2,w,n,b,"L");
  fm="%.1f"; n=sprintf(b,fm,x); ck(32,2,w,n,b,"R");
  strcpy(w,"0.000");
  printf("P 32 3|%s|",w); printf("|%.3f|\n",x);
  n=sprintf(b,"%.3f",x); ck(32,3,w,n,b,"L");
  fm="%.3f"; n=sprintf(b,fm,x); ck(32,3,w,n,b,"R");
  strcpy(w,"0.000000");
  printf("P 32 4|%s|",w); printf("|%.6f|\n",x);
  n=sprintf(b,"%.6f",x); ck(32,4,w,n,b,"L");
  fm="%.6f"; n=sprintf(b,fm,x); ck(32,4,w,n,b,"R");
  strcpy(w,"0.000000000100000");
  printf("P 32 5|%s|",w); printf("|%.15f|\n",x);
  n=sprintf(b,"%.15f",x); ck(32,5,w,n,b,"L");
  fm="%.15f"; n=sprintf(b,fm,x); ck(32,5,w,n,b,"R");
  strcpy(w,"0.0000000001000000");
  printf("P 32 6|%s|",w); printf("|%.16f|\n",x);
  n=sprintf(b,"%.16f",x); ck(32,6,w,n,b,"L");
  fm="%.16f"; n=sprintf(b,fm,x); ck(32,6,w,n,b,"R");
  strcpy(w,"     0.000");
  printf("P 32 7|%s|",w); printf("|%10.3f|\n",x);
  n=sprintf(b,"%10.3f",x); ck(32,7,w,n,b,"L");
  fm="%10.3f"; n=sprintf(b,fm,x); ck(32,7,w,n,b,"R");
  strcpy(w,"0.00000000010000000000");
  printf("P 32 8|%s|",w); printf("|%.20f|\n",x);
  n=sprintf(b,"%.20f",x); ck(32,8,w,n,b,"L");
  fm="%.20f"; n=sprintf(b,fm,x); ck(32,8,w,n,b,"R");
  /* id 33 bits 3f202e7ef70994dd */
  x=4554722472727773; q=p2e(-65); x=x*q;
  strcpy(w,"0.000123");
  printf("P 33 0|%s|",w); printf("|%f|\n",x);
  n=sprintf(b,"%f",x); ck(33,0,w,n,b,"L");
  fm="%f"; n=sprintf(b,fm,x); ck(33,0,w,n,b,"R");
  strcpy(w,"0");
  printf("P 33 1|%s|",w); printf("|%.0f|\n",x);
  n=sprintf(b,"%.0f",x); ck(33,1,w,n,b,"L");
  fm="%.0f"; n=sprintf(b,fm,x); ck(33,1,w,n,b,"R");
  strcpy(w,"0.0");
  printf("P 33 2|%s|",w); printf("|%.1f|\n",x);
  n=sprintf(b,"%.1f",x); ck(33,2,w,n,b,"L");
  fm="%.1f"; n=sprintf(b,fm,x); ck(33,2,w,n,b,"R");
  strcpy(w,"0.000");
  printf("P 33 3|%s|",w); printf("|%.3f|\n",x);
  n=sprintf(b,"%.3f",x); ck(33,3,w,n,b,"L");
  fm="%.3f"; n=sprintf(b,fm,x); ck(33,3,w,n,b,"R");
  strcpy(w,"0.000123");
  printf("P 33 4|%s|",w); printf("|%.6f|\n",x);
  n=sprintf(b,"%.6f",x); ck(33,4,w,n,b,"L");
  fm="%.6f"; n=sprintf(b,fm,x); ck(33,4,w,n,b,"R");
  strcpy(w,"0.000123456000000");
  printf("P 33 5|%s|",w); printf("|%.15f|\n",x);
  n=sprintf(b,"%.15f",x); ck(33,5,w,n,b,"L");
  fm="%.15f"; n=sprintf(b,fm,x); ck(33,5,w,n,b,"R");
  strcpy(w,"0.0001234560000000");
  printf("P 33 6|%s|",w); printf("|%.16f|\n",x);
  n=sprintf(b,"%.16f",x); ck(33,6,w,n,b,"L");
  fm="%.16f"; n=sprintf(b,fm,x); ck(33,6,w,n,b,"R");
  strcpy(w,"     0.000");
  printf("P 33 7|%s|",w); printf("|%10.3f|\n",x);
  n=sprintf(b,"%10.3f",x); ck(33,7,w,n,b,"L");
  fm="%10.3f"; n=sprintf(b,fm,x); ck(33,7,w,n,b,"R");
  strcpy(w,"0.00012345599999999896");
  printf("P 33 8|%s|",w); printf("|%.20f|\n",x);
  n=sprintf(b,"%.20f",x); ck(33,8,w,n,b,"L");
  fm="%.20f"; n=sprintf(b,fm,x); ck(33,8,w,n,b,"R");
  /* id 34 bits 44dfde9f10a8d361 */
  x=8970499038696289; q=p2e(26); x=x*q;
  strcpy(w,"602000000000000000000000.000000");
  printf("P 34 0|%s|",w); printf("|%f|\n",x);
  n=sprintf(b,"%f",x); ck(34,0,w,n,b,"L");
  fm="%f"; n=sprintf(b,fm,x); ck(34,0,w,n,b,"R");
  strcpy(w,"602000000000000000000000");
  printf("P 34 1|%s|",w); printf("|%.0f|\n",x);
  n=sprintf(b,"%.0f",x); ck(34,1,w,n,b,"L");
  fm="%.0f"; n=sprintf(b,fm,x); ck(34,1,w,n,b,"R");
  strcpy(w,"602000000000000000000000.0");
  printf("P 34 2|%s|",w); printf("|%.1f|\n",x);
  n=sprintf(b,"%.1f",x); ck(34,2,w,n,b,"L");
  fm="%.1f"; n=sprintf(b,fm,x); ck(34,2,w,n,b,"R");
  strcpy(w,"602000000000000000000000.000");
  printf("P 34 3|%s|",w); printf("|%.3f|\n",x);
  n=sprintf(b,"%.3f",x); ck(34,3,w,n,b,"L");
  fm="%.3f"; n=sprintf(b,fm,x); ck(34,3,w,n,b,"R");
  strcpy(w,"602000000000000000000000.000000");
  printf("P 34 4|%s|",w); printf("|%.6f|\n",x);
  n=sprintf(b,"%.6f",x); ck(34,4,w,n,b,"L");
  fm="%.6f"; n=sprintf(b,fm,x); ck(34,4,w,n,b,"R");
  strcpy(w,"602000000000000000000000.000000000000000");
  printf("P 34 5|%s|",w); printf("|%.15f|\n",x);
  n=sprintf(b,"%.15f",x); ck(34,5,w,n,b,"L");
  fm="%.15f"; n=sprintf(b,fm,x); ck(34,5,w,n,b,"R");
  strcpy(w,"602000000000000000000000.0000000000000000");
  printf("P 34 6|%s|",w); printf("|%.16f|\n",x);
  n=sprintf(b,"%.16f",x); ck(34,6,w,n,b,"L");
  fm="%.16f"; n=sprintf(b,fm,x); ck(34,6,w,n,b,"R");
  strcpy(w,"602000000000000000000000.000");
  printf("P 34 7|%s|",w); printf("|%10.3f|\n",x);
  n=sprintf(b,"%10.3f",x); ck(34,7,w,n,b,"L");
  fm="%10.3f"; n=sprintf(b,fm,x); ck(34,7,w,n,b,"R");
  strcpy(w,"602000000000000000000000.00000000000000000000");
  printf("P 34 8|%s|",w); printf("|%.20f|\n",x);
  n=sprintf(b,"%.20f",x); ck(34,8,w,n,b,"L");
  fm="%.20f"; n=sprintf(b,fm,x); ck(34,8,w,n,b,"R");
  /* id 35 bits 4004000000000000 */
  x=5629499534213120; q=p2e(-51); x=x*q;
  strcpy(w,"2.500000");
  printf("P 35 0|%s|",w); printf("|%f|\n",x);
  n=sprintf(b,"%f",x); ck(35,0,w,n,b,"L");
  fm="%f"; n=sprintf(b,fm,x); ck(35,0,w,n,b,"R");
  strcpy(w,"2");
  printf("P 35 1|%s|",w); printf("|%.0f|\n",x);
  n=sprintf(b,"%.0f",x); ck(35,1,w,n,b,"L");
  fm="%.0f"; n=sprintf(b,fm,x); ck(35,1,w,n,b,"R");
  strcpy(w,"2.5");
  printf("P 35 2|%s|",w); printf("|%.1f|\n",x);
  n=sprintf(b,"%.1f",x); ck(35,2,w,n,b,"L");
  fm="%.1f"; n=sprintf(b,fm,x); ck(35,2,w,n,b,"R");
  strcpy(w,"2.500");
  printf("P 35 3|%s|",w); printf("|%.3f|\n",x);
  n=sprintf(b,"%.3f",x); ck(35,3,w,n,b,"L");
  fm="%.3f"; n=sprintf(b,fm,x); ck(35,3,w,n,b,"R");
  strcpy(w,"2.500000");
  printf("P 35 4|%s|",w); printf("|%.6f|\n",x);
  n=sprintf(b,"%.6f",x); ck(35,4,w,n,b,"L");
  fm="%.6f"; n=sprintf(b,fm,x); ck(35,4,w,n,b,"R");
  strcpy(w,"2.499999999999996");
  printf("P 35 5|%s|",w); printf("|%.15f|\n",x);
  n=sprintf(b,"%.15f",x); ck(35,5,w,n,b,"L");
  fm="%.15f"; n=sprintf(b,fm,x); ck(35,5,w,n,b,"R");
  strcpy(w,"2.4999999999999952");
  printf("P 35 6|%s|",w); printf("|%.16f|\n",x);
  n=sprintf(b,"%.16f",x); ck(35,6,w,n,b,"L");
  fm="%.16f"; n=sprintf(b,fm,x); ck(35,6,w,n,b,"R");
  strcpy(w,"     2.500");
  printf("P 35 7|%s|",w); printf("|%10.3f|\n",x);
  n=sprintf(b,"%10.3f",x); ck(35,7,w,n,b,"L");
  fm="%10.3f"; n=sprintf(b,fm,x); ck(35,7,w,n,b,"R");
  strcpy(w,"2.49999999999999392000");
  printf("P 35 8|%s|",w); printf("|%.20f|\n",x);
  n=sprintf(b,"%.20f",x); ck(35,8,w,n,b,"L");
  fm="%.20f"; n=sprintf(b,fm,x); ck(35,8,w,n,b,"R");
  printf("TOT T:/CC/GATES/gs %d cells %d fails\n",cells,fails);
  return fails;
}
