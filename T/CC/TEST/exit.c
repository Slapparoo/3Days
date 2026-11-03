int emit(int c)
{
  if (c>0)
  {
    putchar(c+48);
    return emit(c-1);
  }
  return 0;
}
int main()
{
  emit(3);
  putchar(10);
  exit(7);
  return 0;
}
