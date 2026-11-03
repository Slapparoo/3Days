/* M12.1 abort(): exit-shaped return to the harness with 134, from
 * inside a callee (the CCFunRet path every exit takes). */
int helper()
{
  abort();
  return 3;
}
int main()
{
  int x;
  x=helper();
  if (x==3)
    return 200;
  return 1;
}
