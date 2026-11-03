int sum3(int *a)
{
  return a[0]+a[1]+a[2];
}
int main()
{
  int v[3];
  v[0]=1;
  v[1]=2;
  v[2]=4;
  return sum3(v);
}
