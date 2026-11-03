int main()
{
  int s=0;
  int i;
  for (i=0;i<20;i++)
  {
    if (i%2)
      continue;
    if (i>6)
      break;
    s=s+i;
  }
  return s;
}
