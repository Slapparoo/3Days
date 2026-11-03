struct Point
{
  int x;
  int y;
};
struct Point g;
int getx(struct Point *p)
{
  return p->x;
}
int main()
{
  struct Point v;
  struct Point *p;
  struct Point vs[3];
  int i;
  int s;
  v.x=7;
  v.y=9;
  p=&v;
  p->x=p->x+1;
  g.x=100;
  s=v.x+getx(&v)+g.x;
  for (i=0;i<3;i++)
  {
    vs[i].x=i*10;
    vs[i].y=i;
  }
  s=s+vs[2].x+vs[1].y;
  if (&v==p)
    s=s+1;
  return s;
}
