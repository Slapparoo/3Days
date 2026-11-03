/* M21 block scoping + `static` block objects: shadowing through scope
 * waterlines, per-function static twins, static arrays/structs/pointers/
 * fps on arena storage, for-init/switch/nested-block scopes, params
 * shadowing globals and their own function names, and a function stub
 * created inside a block (the scope eraser keeps function symbols, so
 * the queued call patches still land at definition). Expect 48. */
struct P { int a; int b; };
int gv;
int inc(int x)
{
  return x+1;
}
int dbl(int x)
{
  return x*2;
}
int tw1()
{
  static int n;
  n=n+1;
  return n;
}
int tw2()
{
  static int n=10;
  n=n*2;
  return n;
}
int hist()
{
  static int buf[3];
  static int fills;
  buf[fills]=fills*7+1;
  fills=fills+1;
  return buf[0]+buf[1]+buf[2];
}
int shape()
{
  static struct P p;
  p.a=p.a+3;
  p.b=p.b+2;
  return p.a*100+p.b;
}
int loopy()
{
  int i;
  int tot;
  static int seen;
  tot=0;
  for(i=0;i!=3;i++)
  {
    static int tick;
    tick=tick+1;
    tot=tot+tick;
  }
  seen=seen+1;
  return tot+1000*seen;
}
int deepstat()
{
  int r;
  {
    static int dn;
    dn=dn+11;
    r=dn;
  }
  return r;
}
int statvsshadow()
{
  static int gv;
  gv=gv+3;
  return gv;
}
int pstat()
{
  static int acc;
  int *pp;
  pp=&acc;
  *pp=*pp+4;
  return acc;
}
int staticsizes()
{
  static int sav[5];
  int *pp;
  int tot;
  pp=&sav[2];
  *pp=6;
  tot=0;
  tot=tot+(sav[2]==6);
  tot=tot+(sizeof(sav)==40);
  tot=tot+(sizeof(sav[0])==8);
  return tot;
}
int usefp(int which)
{
  static int (*fp)(int);
  if (which==1)
    fp=inc;
  if (which==2)
    fp=dbl;
  if (fp==0)
    return -1;
  return fp(which);
}
int blockstub()
{
  int t;
  t=0;
  {
    t=later1();
  }
  return t+later1();
}
int paramshadow(int gv)
{
  return gv+1;
}
int selfname(int selfname)
{
  return selfname*2;
}
int later1()
{
  return 7;
}
int main()
{
  int x;
  int acc;
  int j;
  int tot;
  int *ip;
  tot=0;
  gv=5;
  x=1;
  {
    int x;
    x=2;
    tot=tot+(x==2);                            /*  1 the inner name wins */
    {
      int x;
      x=7;
      tot=tot+(x==7);                          /*  2 double nesting */
    }
    tot=tot+(x==2);                            /*  3 middle restored */
  }
  tot=tot+(x==1);                              /*  4 outer sees its own */
  x=9;
  tot=tot+(x==9);                              /*  5 outer slot still live */
  tot=tot+(tw1()==1);                          /*  6 static starts zeroed */
  tot=tot+(tw1()==2);                          /*  7 value survives the call */
  tot=tot+(tw2()==20);                         /*  8 twin, folded initializer */
  tot=tot+(tw1()==3);                          /*  9 */
  tot=tot+(tw2()==40);                         /* 10 twins are independent */
  tot=tot+(hist()==1);                         /* 11 static array: first fill */
  tot=tot+(hist()==9);                         /* 12 second call appends */
  tot=tot+(hist()==24);                        /* 13 third: 1+8+15 */
  tot=tot+(shape()==302);                      /* 14 static struct accumulates */
  tot=tot+(shape()==604);                      /* 15 */
  tot=tot+(loopy()==1006);                     /* 16 static in a loop body */
  tot=tot+(loopy()==2015);                     /* 17 tick 4+5+6, seen 2 */
  tot=tot+(staticsizes()==3);                  /* 18 subscript + sizes */
  tot=tot+(deepstat()==11);                    /* 19 nested-block static */
  tot=tot+(deepstat()==22);                    /* 20 name died, value did not */
  tot=tot+(usefp(1)==2);                       /* 21 static fp assigned */
  tot=tot+(usefp(2)==4);                       /* 22 retargeted */
  tot=tot+(usefp(9)==18);                      /* 23 target survives calls */
  tot=tot+(pstat()==4);                        /* 24 pointer to a static */
  tot=tot+(pstat()==8);                        /* 25 */
  tot=tot+(paramshadow(10)==11);               /* 26 param shadows the global */
  tot=tot+(gv==5);                             /* 27 global untouched */
  tot=tot+(selfname(4)==8);                    /* 28 param shadows its function */
  tot=tot+(blockstub()==14);                   /* 29 in-block stub patched later */
  {
    int gv;
    gv=77;
    tot=tot+((gv==77)&&(statvsshadow()==3));   /* 30 block local vs block static */
    tot=tot+(gv==77);                          /* 31 */
  }
  tot=tot+(statvsshadow()==6);                 /* 32 the static advanced alone */
  tot=tot+(gv==5);                             /* 33 global gv still 5 */
  acc=1;
  for(j=0;j!=3;j++)
    acc=acc+j;
  tot=tot+((acc==4)&&(j==3));                  /* 34 for with an outer j in play */
  for(int j=0;j!=3;j++)
    acc=acc+1;
  tot=tot+((acc==7)&&(j==3));                  /* 35 the inner for-decl shadowed */
  j=2;
  switch (j)
  {
    int hit;
    case 2:
      hit=4;
      tot=tot+(hit==4);                        /* 36 switch-body decl in a case */
      {
        int hit;
        hit=9;
        tot=tot+((hit+2)==11);                 /* 37 shadowing inside the switch */
      }
      tot=tot+(hit==4);                        /* 38 restored at the inner brace */
      break;
    default:
      tot=tot+0;
  }
  tot=tot+(j==2);                              /* 39 switch expression var intact */
  ip=&x;
  *ip=*ip+1000;
  tot=tot+(x==1009);                           /* 40 pointers see the frame slot */
  tot=tot+(tw1()==4);                          /* 41 counter untouched by it all */
  tot=tot+(staticsizes()==3);                  /* 42 static array not re-zeroed */
  tot=tot+(paramshadow(gv)==6);                /* 43 global through a shadowing param */
  tot=tot+(usefp(2)==4);                       /* 44 retarget after persistence */
  tot=tot+(tw2()==80);                         /* 45 20 -> 40 -> 80 */
  {
    int x[4];
    x[0]=1;
    tot=tot+(sizeof(x)==32);                   /* 46 inner array: slot sizes */
    tot=tot+((sizeof(x[0])==8)&&(x[0]==1));    /* 47 element size + the object */
  }
  tot=tot+(sizeof(x)==8);                      /* 48 the outer scalar is back */
  return tot+1;
}
