int main()
{
  int a[3];
  int *p;
  p=a;
  *p=1;
  p++;
  *p=2;
  p++;
  *p=3;
  return a[0]+a[1]+a[2];
}
