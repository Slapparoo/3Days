#include <stdio.h>
int main()
{
  double da,db,x;
  char b[1200];
  int n;
  da=3;db=2;x=da/db;
  n=sprintf(b,"%20.15f",x);
  printf("A01|%s|%d|\n",b,n);
  n=sprintf(b,"%010.4f",x);
  printf("A02|%s|%d|\n",b,n);
  n=sprintf(b,"%-10.4f",x);
  printf("A03|%s|%d|\n",b,n);
  n=sprintf(b,"%#10.2f",x);
  printf("A04|%s|%d|\n",b,n);
  n=sprintf(b,"% 10.2f",x);
  printf("A05|%s|%d|\n",b,n);
  n=sprintf(b,"%+10.2f",x);
  printf("A06|%s|%d|\n",b,n);
  n=sprintf(b,"%.600f",x);
  printf("A07|%s|%d|\n",b,n);
  n=sprintf(b,"%999.999f",x);
  printf("A08|%s|%d|\n",b,n);
  n=sprintf(b,"%01000.3f",x);
  printf("A09|%s|%d|\n",b,n);
  n=sprintf(b,"%3.1f",x);
  printf("A10|%s|%d|\n",b,n);
  n=sprintf(b,"%-3.1f",x);
  printf("A11|%s|%d|\n",b,n);
  printf("B01|%20.15f|\n",x);
  printf("B02|%010.4f|\n",x);
  printf("B03|%-10.4f|\n",x);
  printf("B04|%#10.2f|\n",x);
  printf("B05|% 10.2f|\n",x);
  printf("B06|%+10.2f|\n",x);
  printf("B07|%.600f|\n",x);
  printf("B08|%999.999f|\n",x);
  printf("B09|%01000.3f|\n",x);
  printf("B10|%3.1f|\n",x);
  printf("B11|%-3.1f|\n",x);
  n=sprintf(b,"%5d %d",5,7);
  printf("ARITY1|%s|%d|\n",b,n);
  n=sprintf(b,"%-2d",7);
  printf("ARITY2|%s|%d|\n",b,n);
  n=sprintf(b,"%*d",10,7);
  printf("ARITY3|%s|%d|\n",b,n);
  return 0;
}
