int main()
{
  char buf[4];
  char c;
  buf[0]=65;
  buf[1]=7;
  c=10;
  buf[0]++;
  buf[1]++;
  c++;
  return buf[0]+buf[1]+c;
}
