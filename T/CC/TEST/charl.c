int main()
{
  char c;
  int n=0;
  c='A';
  if (c==65)
    n+=1;
  if ('\n'==10)
    n+=2;
  if ('0'=='A'-17)
    n+=4;
  if ('\\'==92)
    n+=8;
  c='\t';
  c+=1;
  if (c==10)
    n+=16;
  return n;
}
