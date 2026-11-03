struct Item
{
  char flag;
  char *name;
  int qty;
};
struct Item it;
int sumqty(struct Item *items,int n)
{
  int i;
  int t=0;
  for (i=0;i<n;i++)
    t+=items[i].qty;
  return t;
}
int main()
{
  struct Item items[2];
  it.flag='A';
  it.name="widget";
  it.qty=5;
  items[0].qty=10;
  items[1].qty=32;
  if (it.flag=='A')
    printf("%s qty=%d c=%c\n",it.name,it.qty,it.flag);
  return sumqty(items,2)+it.qty;
}
