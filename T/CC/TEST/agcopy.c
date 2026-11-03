/* M27 whole-object assignment: `v = w;` copies a struct or union member
 * slot by slot — objects, array elements, dereferences, chained
 * assignment, frame<->arena in both directions, and `static` objects that
 * keep what they were given across calls. The copy is shallow (C's rule),
 * it fully overwrites the destination, and it reads each source slot
 * before writing it, so aliases and self-copies are safe. Expect 27. */
struct P { int a; char b; int c; };
struct Q { int *p; int n; };
union U { int i; char ch; int *p; };
int gv;
int gi;
struct P gs;
int giveback()
{
  static struct P keep;
  struct P fresh;
  fresh.a=7;
  fresh.b=8;
  fresh.c=9;
  keep=fresh;
  return keep.a+keep.b+keep.c;
}
int stepstatic()
{
  static struct P s;
  struct P t;
  t.a=s.a+1;
  t.b=2;
  t.c=0;
  s=t;
  return s.a;
}
int main()
{
  struct P x;
  struct P y;
  struct P z;
  struct P arr[3];
  struct Q qa;
  struct Q qb;
  union U u;
  union U v;
  struct P *p;
  struct P *q;
  int tot;
  int i;
  int sum;
  tot=0;
  gv=55;
  gi=12;
  y.a=1;
  y.b=2;
  y.c=3;
  z.a=0;
  z.b=0;
  z.c=0;
  tot=tot+((x=y).a==1);                       /*  1 the value is the lhs */
  tot=tot+((x.a==1)&&(x.b==2)&&(x.c==3));     /*  2 every member copied */
  x.a=99;
  x=z;
  tot=tot+((x.a==0)&&(x.c==0));               /*  3 destination fully overwritten */
  x=y;
  y.a=77;
  tot=tot+((x.a==1)&&(y.a==77));              /*  4 value semantics both ways */
  y.a=1;
  x.c=-7;
  y.c=x.c;
  tot=tot+(y.c==-7);                          /*  5 a negative member travels */
  x=y;
  tot=tot+(x.b==2);                           /*  6 the char member travels */
  tot=tot+(sizeof(x)==24);                    /*  7 model slots (C says 12) */
  x=y;
  z=x;
  tot=tot+((z.a==1)&&(z.c==-7));              /*  8 copy of a copy */
  y.c=3;
  arr[0].a=10;
  arr[0].b=11;
  arr[0].c=12;
  arr[1]=arr[0];
  tot=tot+((arr[1].a==10)&&(arr[1].c==12));   /*  9 array element copy */
  arr[2]=arr[1];
  tot=tot+((arr[2].a==10)&&(arr[2].b==11));   /* 10 element to element */
  arr[1].c=0;
  tot=tot+((arr[1].c==0)&&(arr[2].c==12)&&(arr[0].c==12));  /* 11 separate objects */
  arr[1]=arr[1];
  tot=tot+((arr[1].a==10)&&(arr[1].c==0));    /* 12 self-copy is harmless */
  p=&x;
  *p=y;
  tot=tot+((*p).a==1);                        /* 13 copy through a dereference */
  x.a=5;
  x=y;
  p=&y;
  q=&x;
  *p=*q;
  tot=tot+((y.a==1)&&(x.a==1));               /* 14 dereference to dereference */
  p=&arr[0];
  *p=arr[1];
  tot=tot+((arr[0].a==10)&&(arr[1].a==10));   /* 15 distinct objects, same value */
  qa.p=&gi;
  qa.n=4;
  qb=qa;
  tot=tot+((qb.p==&gi)&&(qb.n==4));           /* 16 a pointer member is COPIED */
  *qb.p=*qb.p+100;
  tot=tot+((gi==112)&&(qa.p[0]==112));        /* 17 shallow: one target, two views */
  qb.p=&gv;
  tot=tot+((qa.p==&gi)&&(qb.p==&gv));         /* 18 retargeting one leaves the other */
  v.i=66;
  u=v;
  tot=tot+((u.i==66)&&(u.ch==66));            /* 19 union copy aliases the same way */
  v.p=&gv;
  u=v;
  tot=tot+((*u.p==55)&&(u.p==v.p));           /* 20 union pointer member copied */
  u.ch='Z';
  tot=tot+((u.ch=='Z')&&(v.p!=0));           /* 21 writing through the alias */
  y.a=1;
  y.b=2;
  y.c=3;
  gs=y;
  tot=tot+((gs.a==1)&&(gs.c==3));             /* 22 frame into a global */
  gs.c=31;
  x=gs;
  tot=tot+((x.c==31)&&(gs.c==31));            /* 23 global into a frame */
  tot=tot+(giveback()==24);                   /* 24 static object gets the copy */
  tot=tot+(giveback()==24);                   /* 25 reassigned on the next call */
  tot=tot+(stepstatic()==1);                  /* 26 a static chain starts zeroed */
  sum=0;
  i=0;
  for(i=0;i!=3;i++)
    arr[i]=y;
  for(i=0;i!=3;i++)
    sum=sum+arr[i].a;
  tot=tot+((sum==3)&&(arr[2].c==3));          /* 27 copying inside a loop */
  return tot+1;
}
