int main()
{
  int i=0;
  int n=0;
  while (i<3)
  {
    int j=0;
    while (j<4)
    {
      j++;
      if (j==2)
        break;
      n=n+1;
    }
    i++;
  }
  return n;
}
