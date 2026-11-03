int main()
{
  int a[4];
  int *p;
  p=a;
  *p=1;
  p[1]=2;
  p=p+2;
  *p=6;
  return a[0]+a[1]+a[2];
}
