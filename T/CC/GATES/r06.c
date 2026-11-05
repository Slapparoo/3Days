#include <stdio.h>
int main()
{
  double da,db,x;
  char s[8];
  int n;
  da=3;db=2;x=da/db;
  s[0]='a';
  n=printf("%010d",x,10,s);
  printf("OK|%d|",n);
  return 0;
}
