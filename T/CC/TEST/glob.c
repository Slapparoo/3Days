int g;
int bump()
{
  g=g+2;
  return g;
}
int main()
{
  g=1;
  bump();
  return g;
}
