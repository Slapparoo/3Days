/* M23 struct and union initializers: members filled in declaration
 * order, partial initializers zero-filled, brace groups mapped by the
 * same C 6.7.9 rules that arrays use (a union's arity is ONE member),
 * struct arrays (deduced and sized), globals, `static` block objects,
 * by-value one-slot members, and enum-folded elements. Expect 21. */
enum { TWO=2, THREE=3, FOUR=4 };
struct P { int a; char b; int c; };
union U { int i; char ch; int *p; };
struct Q { int x; int y; };
struct Box { union U v; int t; };
struct L { int *p; int n; };
struct P gp={7,'x',9};
struct P gpa[]={{1,2,3},{4,5,6}};
union U gu={65};
union U gua[2]={1,2};
struct Box gb={{77},8};
struct L gl={0,THREE};
int counter3()
{
  static struct P sp={10,20,30};
  sp.a=sp.a+1;
  return sp.a*1000+sp.b*10+sp.c;
}
int main()
{
  struct P p={1,'a',3};
  struct P p2={5};
  struct P pa[3]={{1,2,3},{4,5,6}};
  struct P pf[4]={1,2,3,4,5,6,7,8};
  struct P ep={TWO,THREE,FOUR};
  union U u={66};
  struct Q q={100,200};
  struct Box bx={{9},4};
  struct L ln={0,5};
  int tot;
  static struct P so={1,2,3};
  tot=0;
  tot=tot+((p.a==1)&&(p.b=='a')&&(p.c==3));      /*  1 members in order */
  tot=tot+((p2.a==5)&&(p2.b==0)&&(p2.c==0));     /*  2 partial init zero-fills */
  tot=tot+((pa[0].a==1)&&(pa[1].b==5)&&(pa[2].c==0));  /*  3 array + tail element */
  tot=tot+((pf[0].a==1)&&(pf[2].a==7)&&(pf[2].b==8)&&(pf[3].a==0));  /*  4 flat fills members in order */
  tot=tot+((ep.a==2)&&(ep.b==3)&&(ep.c==4));     /*  5 enum-folded elements */
  tot=tot+((u.i==66)&&(u.ch==66));               /*  6 union: one member */
  tot=tot+((q.x==100)&&(q.y==200));              /*  7 second tag */
  tot=tot+((bx.v.i==9)&&(bx.t==4));              /*  8 by-value one-slot member */
  tot=tot+((ln.p==0)&&(ln.n==5));                /*  9 NULL pointer member */
  tot=tot+((gp.a==7)&&(gp.b=='x')&&(gp.c==9));   /* 10 global struct */
  tot=tot+((gpa[0].a==1)&&(gpa[1].b==5)&&(gpa[1].c==6));  /* 11 global array, deduced */
  tot=tot+(sizeof(gpa)==48);                     /* 12 two elements of three slots */
  tot=tot+((gu.i==65)&&(gu.ch=='A'));            /* 13 global union */
  tot=tot+((gua[0].i==1)&&(gua[1].i==2));        /* 14 union array: one value each */
  tot=tot+((gb.v.i==77)&&(gb.t==8));             /* 15 global by-value member group */
  tot=tot+(gl.n==THREE);                         /* 16 global enum element */
  tot=tot+((gl.p==0));                           /* 17 */
  tot=tot+(counter3()==11230);                   /* 18 static struct persists */
  tot=tot+(counter3()==12230);                   /* 19 */
  tot=tot+((so.a==1)&&(so.c==3));                /* 20 static block struct */
  tot=tot+(sizeof(u)==8);                        /* 21 union stays one slot */
  return tot+1;
}
