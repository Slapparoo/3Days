#include <stdio.h>
#include <string.h>
/* M37 delegation sweep (T:/CC/GATES/gs5.c).  Expectations are the kernel's own
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
  /* id 20 bits 444b1ae4d6e2ef50 */
  x=7629394531250000; q=p2e(17); x=x*q;
  strcpy(w,"1000000000000000480000.000000");
  printf("P 20 0|%s|",w); printf("|%f|\n",x);
  n=sprintf(b,"%f",x); ck(20,0,w,n,b,"L");
  fm="%f"; n=sprintf(b,fm,x); ck(20,0,w,n,b,"R");
  strcpy(w,"1000000000000000480000");
  printf("P 20 1|%s|",w); printf("|%.0f|\n",x);
  n=sprintf(b,"%.0f",x); ck(20,1,w,n,b,"L");
  fm="%.0f"; n=sprintf(b,fm,x); ck(20,1,w,n,b,"R");
  strcpy(w,"1000000000000000480000.0");
  printf("P 20 2|%s|",w); printf("|%.1f|\n",x);
  n=sprintf(b,"%.1f",x); ck(20,2,w,n,b,"L");
  fm="%.1f"; n=sprintf(b,fm,x); ck(20,2,w,n,b,"R");
  strcpy(w,"1000000000000000480000.000");
  printf("P 20 3|%s|",w); printf("|%.3f|\n",x);
  n=sprintf(b,"%.3f",x); ck(20,3,w,n,b,"L");
  fm="%.3f"; n=sprintf(b,fm,x); ck(20,3,w,n,b,"R");
  strcpy(w,"1000000000000000480000.000000");
  printf("P 20 4|%s|",w); printf("|%.6f|\n",x);
  n=sprintf(b,"%.6f",x); ck(20,4,w,n,b,"L");
  fm="%.6f"; n=sprintf(b,fm,x); ck(20,4,w,n,b,"R");
  strcpy(w,"1000000000000000480000.000000000000000");
  printf("P 20 5|%s|",w); printf("|%.15f|\n",x);
  n=sprintf(b,"%.15f",x); ck(20,5,w,n,b,"L");
  fm="%.15f"; n=sprintf(b,fm,x); ck(20,5,w,n,b,"R");
  strcpy(w,"1000000000000000480000.0000000000000000");
  printf("P 20 6|%s|",w); printf("|%.16f|\n",x);
  n=sprintf(b,"%.16f",x); ck(20,6,w,n,b,"L");
  fm="%.16f"; n=sprintf(b,fm,x); ck(20,6,w,n,b,"R");
  strcpy(w,"1000000000000000480000.000");
  printf("P 20 7|%s|",w); printf("|%10.3f|\n",x);
  n=sprintf(b,"%10.3f",x); ck(20,7,w,n,b,"L");
  fm="%10.3f"; n=sprintf(b,fm,x); ck(20,7,w,n,b,"R");
  strcpy(w,"1000000000000000480000.00000000000000000000");
  printf("P 20 8|%s|",w); printf("|%.20f|\n",x);
  n=sprintf(b,"%.20f",x); ck(20,8,w,n,b,"L");
  fm="%.20f"; n=sprintf(b,fm,x); ck(20,8,w,n,b,"R");
  /* id 21 bits 4132d687e4189375 */
  x=5302428716536693; q=p2e(-32); x=x*q;
  strcpy(w,"1234567.891000");
  printf("P 21 0|%s|",w); printf("|%f|\n",x);
  n=sprintf(b,"%f",x); ck(21,0,w,n,b,"L");
  fm="%f"; n=sprintf(b,fm,x); ck(21,0,w,n,b,"R");
  strcpy(w,"1234568");
  printf("P 21 1|%s|",w); printf("|%.0f|\n",x);
  n=sprintf(b,"%.0f",x); ck(21,1,w,n,b,"L");
  fm="%.0f"; n=sprintf(b,fm,x); ck(21,1,w,n,b,"R");
  strcpy(w,"1234567.9");
  printf("P 21 2|%s|",w); printf("|%.1f|\n",x);
  n=sprintf(b,"%.1f",x); ck(21,2,w,n,b,"L");
  fm="%.1f"; n=sprintf(b,fm,x); ck(21,2,w,n,b,"R");
  strcpy(w,"1234567.891");
  printf("P 21 3|%s|",w); printf("|%.3f|\n",x);
  n=sprintf(b,"%.3f",x); ck(21,3,w,n,b,"L");
  fm="%.3f"; n=sprintf(b,fm,x); ck(21,3,w,n,b,"R");
  strcpy(w,"1234567.891000");
  printf("P 21 4|%s|",w); printf("|%.6f|\n",x);
  n=sprintf(b,"%.6f",x); ck(21,4,w,n,b,"L");
  fm="%.6f"; n=sprintf(b,fm,x); ck(21,4,w,n,b,"R");
  strcpy(w,"1234567.890999994240000");
  printf("P 21 5|%s|",w); printf("|%.15f|\n",x);
  n=sprintf(b,"%.15f",x); ck(21,5,w,n,b,"L");
  fm="%.15f"; n=sprintf(b,fm,x); ck(21,5,w,n,b,"R");
  strcpy(w,"1234567.8909999942400000");
  printf("P 21 6|%s|",w); printf("|%.16f|\n",x);
  n=sprintf(b,"%.16f",x); ck(21,6,w,n,b,"L");
  fm="%.16f"; n=sprintf(b,fm,x); ck(21,6,w,n,b,"R");
  strcpy(w,"1234567.891");
  printf("P 21 7|%s|",w); printf("|%10.3f|\n",x);
  n=sprintf(b,"%10.3f",x); ck(21,7,w,n,b,"L");
  fm="%10.3f"; n=sprintf(b,fm,x); ck(21,7,w,n,b,"R");
  strcpy(w,"1234567.89099999424000000000");
  printf("P 21 8|%s|",w); printf("|%.20f|\n",x);
  n=sprintf(b,"%.20f",x); ck(21,8,w,n,b,"L");
  fm="%.20f"; n=sprintf(b,fm,x); ck(21,8,w,n,b,"R");
  /* id 22 bits 4345ee2a2eb5a5c4 */
  x=6172839450617284; q=p2e(1); x=x*q;
  strcpy(w,"12345678901234566.400000");
  printf("P 22 0|%s|",w); printf("|%f|\n",x);
  n=sprintf(b,"%f",x); ck(22,0,w,n,b,"L");
  fm="%f"; n=sprintf(b,fm,x); ck(22,0,w,n,b,"R");
  strcpy(w,"12345678901234568");
  printf("P 22 1|%s|",w); printf("|%.0f|\n",x);
  n=sprintf(b,"%.0f",x); ck(22,1,w,n,b,"L");
  fm="%.0f"; n=sprintf(b,fm,x); ck(22,1,w,n,b,"R");
  strcpy(w,"12345678901234566.4");
  printf("P 22 2|%s|",w); printf("|%.1f|\n",x);
  n=sprintf(b,"%.1f",x); ck(22,2,w,n,b,"L");
  fm="%.1f"; n=sprintf(b,fm,x); ck(22,2,w,n,b,"R");
  strcpy(w,"12345678901234566.400");
  printf("P 22 3|%s|",w); printf("|%.3f|\n",x);
  n=sprintf(b,"%.3f",x); ck(22,3,w,n,b,"L");
  fm="%.3f"; n=sprintf(b,fm,x); ck(22,3,w,n,b,"R");
  strcpy(w,"12345678901234566.400000");
  printf("P 22 4|%s|",w); printf("|%.6f|\n",x);
  n=sprintf(b,"%.6f",x); ck(22,4,w,n,b,"L");
  fm="%.6f"; n=sprintf(b,fm,x); ck(22,4,w,n,b,"R");
  strcpy(w,"12345678901234566.400000000000000");
  printf("P 22 5|%s|",w); printf("|%.15f|\n",x);
  n=sprintf(b,"%.15f",x); ck(22,5,w,n,b,"L");
  fm="%.15f"; n=sprintf(b,fm,x); ck(22,5,w,n,b,"R");
  strcpy(w,"12345678901234566.4000000000000000");
  printf("P 22 6|%s|",w); printf("|%.16f|\n",x);
  n=sprintf(b,"%.16f",x); ck(22,6,w,n,b,"L");
  fm="%.16f"; n=sprintf(b,fm,x); ck(22,6,w,n,b,"R");
  strcpy(w,"12345678901234566.400");
  printf("P 22 7|%s|",w); printf("|%10.3f|\n",x);
  n=sprintf(b,"%10.3f",x); ck(22,7,w,n,b,"L");
  fm="%10.3f"; n=sprintf(b,fm,x); ck(22,7,w,n,b,"R");
  strcpy(w,"12345678901234566.40000000000000000000");
  printf("P 22 8|%s|",w); printf("|%.20f|\n",x);
  n=sprintf(b,"%.20f",x); ck(22,8,w,n,b,"L");
  fm="%.20f"; n=sprintf(b,fm,x); ck(22,8,w,n,b,"R");
  /* id 23 bits 43e56a95319d63e1 */
  x=6028163525993441; q=p2e(11); x=x*q;
  strcpy(w,"12345678901234569600.000000");
  printf("P 23 0|%s|",w); printf("|%f|\n",x);
  n=sprintf(b,"%f",x); ck(23,0,w,n,b,"L");
  fm="%f"; n=sprintf(b,fm,x); ck(23,0,w,n,b,"R");
  strcpy(w,"12345678901234569600");
  printf("P 23 1|%s|",w); printf("|%.0f|\n",x);
  n=sprintf(b,"%.0f",x); ck(23,1,w,n,b,"L");
  fm="%.0f"; n=sprintf(b,fm,x); ck(23,1,w,n,b,"R");
  strcpy(w,"12345678901234569600.0");
  printf("P 23 2|%s|",w); printf("|%.1f|\n",x);
  n=sprintf(b,"%.1f",x); ck(23,2,w,n,b,"L");
  fm="%.1f"; n=sprintf(b,fm,x); ck(23,2,w,n,b,"R");
  strcpy(w,"12345678901234569600.000");
  printf("P 23 3|%s|",w); printf("|%.3f|\n",x);
  n=sprintf(b,"%.3f",x); ck(23,3,w,n,b,"L");
  fm="%.3f"; n=sprintf(b,fm,x); ck(23,3,w,n,b,"R");
  strcpy(w,"12345678901234569600.000000");
  printf("P 23 4|%s|",w); printf("|%.6f|\n",x);
  n=sprintf(b,"%.6f",x); ck(23,4,w,n,b,"L");
  fm="%.6f"; n=sprintf(b,fm,x); ck(23,4,w,n,b,"R");
  strcpy(w,"12345678901234569600.000000000000000");
  printf("P 23 5|%s|",w); printf("|%.15f|\n",x);
  n=sprintf(b,"%.15f",x); ck(23,5,w,n,b,"L");
  fm="%.15f"; n=sprintf(b,fm,x); ck(23,5,w,n,b,"R");
  strcpy(w,"12345678901234569600.0000000000000000");
  printf("P 23 6|%s|",w); printf("|%.16f|\n",x);
  n=sprintf(b,"%.16f",x); ck(23,6,w,n,b,"L");
  fm="%.16f"; n=sprintf(b,fm,x); ck(23,6,w,n,b,"R");
  strcpy(w,"12345678901234569600.000");
  printf("P 23 7|%s|",w); printf("|%10.3f|\n",x);
  n=sprintf(b,"%10.3f",x); ck(23,7,w,n,b,"L");
  fm="%10.3f"; n=sprintf(b,fm,x); ck(23,7,w,n,b,"R");
  strcpy(w,"12345678901234569600.00000000000000000000");
  printf("P 23 8|%s|",w); printf("|%.20f|\n",x);
  n=sprintf(b,"%.20f",x); ck(23,8,w,n,b,"L");
  fm="%.20f"; n=sprintf(b,fm,x); ck(23,8,w,n,b,"R");
  printf("TOT T:/CC/GATES/gs %d cells %d fails\n",cells,fails);
  return fails;
}
