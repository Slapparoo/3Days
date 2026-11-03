int slen(char *s)
{
  int n=0;
  while (s[n])
    n++;
  return n;
}
int main()
{
  return slen("TempleOS");
}
