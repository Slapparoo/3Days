struct Node
{
  int v;
  struct Node *next;
};
struct Node a;
struct Node b;
int main()
{
  struct Node *p;
  int s;
  a.v=1;
  a.next=&b;
  b.v=41;
  b.next=0;
  s=0;
  p=&a;
  while (p)
  {
    s+=p->v;
    p=p->next;
  }
  if (a.next->v==41)
    s=s-1;
  return s;
}
