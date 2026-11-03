int main()
{
  int a=6,b=10;
  int x,y,z;
  x=a&b;
  y=a|b;
  z=a^b;
  x=x+(1<<10);
  y=y+(20>>2);
  z=z+~5;
  if ((1|2&4)==1)
    x=x+1;
  if ((3==3&1==1)==1)
    y=y+1;
  if ((1^2<<1)==5)
    z=z+2;
  return x+y+z;
}
