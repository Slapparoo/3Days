int main()
{
  int i;
  int s=0,a=2;
  int *p,*q;
  int arr[4];
  for (i=0;i<4;i++)
    arr[i]=i;
  p=arr;
  q=&s;
  *q=5;
  return p[3]+*q+a+s;
}
