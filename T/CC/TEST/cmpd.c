int main()
{
  int x=2;
  int *p;
  int arr[4];
  char buf[4];
  int i;
  x+=3;
  x*=4;
  x-=6;
  x/=5;
  x%=7;
  for (i=0;i<4;i++)
    arr[i]=i+1;
  p=arr;
  p+=2;
  x+=*p;
  buf[0]='a';
  buf[0]+=1;
  buf[0]^=2;
  x+=buf[0];
  return x;
}
