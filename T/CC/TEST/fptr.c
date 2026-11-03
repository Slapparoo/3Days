/* M18 function pointers: declarations (local/global/struct-member/
 * typedef), assignment via name or &name, direct and indirect call
 * forms, forward function addresses patched at definition, anonymous
 * (*param) declarators, NULL sentinels, address equality, sizeof, and
 * saved objects: stored code addresses are CC_B_CODE relocations, so
 * a reloaded module rebases every one of them. Expect 27. */
struct node { int (*fn)(int,int); int v; };
typedef int (*binop)(int,int);
int (*cmpa)(void);
int gticks;
int add(int a,int b)
{
  return a+b;
}
int mul(int a,int b)
{
  return a*b;
}
int tally(void)
{
  gticks=gticks+1;
  return gticks;
}
int apply(int x,int y,binop op)
{
  return op(x,y);
}
int direct(int (*f)(int,int),int x,int y)
{
  return f(x,y)*2;
}
int (*gf)(int,int);
int later(int a)
{
  return a*11;
}
int fwd(int a);
int twice(int (*f)(int))
{
  return f(3)+f(4);
}
int anonrun(int (*)(int),int z);
int main()
{
  struct node n;
  int (*fp)(int,int);
  int (*ga)(int);
  binop bp;
  int tot;
  tot=0;
  n.fn=add;
  n.v=7;
  tot=tot+(n.fn(7,3)==10);                     /*  1 member fp: name */
  tot=tot+((*n.fn)(1,1)==2);                   /*  2 member fp: deref form */
  n.fn=mul;
  tot=tot+(n.fn(3,4)==12);                     /*  3 retarget member slot */
  gf=add;
  tot=tot+(gf(4,5)==9);                        /*  4 global fp */
  gf=mul;
  tot=tot+(gf(4,5)==20);                       /*  5 */
  bp=add;
  tot=tot+(apply(6,7,bp)==13);                 /*  6 typedef fp arg */
  tot=tot+(apply(6,7,mul)==42);                /*  7 function name arg */
  tot=tot+(direct(add,3,4)==14);               /*  8 (*f)(int,int) param */
  ga=fwd;
  tot=tot+(ga(2)==22);                         /*  9 forward address */
  cmpa=tally;
  tot=tot+((cmpa()==1)&&(cmpa()==2));          /* 10 zero-arg fp, side effects */
  tot=tot+(twice(later)==77);                  /* 11 33+44 through one-param fp */
  tot=tot+((fp=add)(1,2)==3);                  /* 12 assignment IS the value */
  tot=tot+((*fp)(3,4)==7);                     /* 13 deref-group call */
  tot=tot+((fp)(5,6)==11);                     /* 14 paren-group call */
  gf=add;
  tot=tot+(fp==gf);                            /* 15 same target via two routes */
  tot=tot+(gf!=mul);                           /* 16 fp var vs function name */
  tot=tot+(add!=mul);                          /* 17 function name vs name */
  tot=tot+(cmpa!=0);                           /* 18 fp truthy when set */
  cmpa=0;
  tot=tot+(cmpa==0);                           /* 19 NULL sentinel */
  cmpa=tally;
  tot=tot+(cmpa!=0);                           /* 20 retarget after NULL */
  tot=tot+(sizeof(fp)==8);                     /* 21 fp value is a quad */
  tot=tot+(sizeof(binop)==8);                  /* 22 typedef fp size */
  tot=tot+((long)add!=(long)mul);              /* 23 addresses as longs */
  bp=mul;
  tot=tot+(bp==mul);                           /* 24 typedef fp equality */
  tot=tot+(anonrun(add,10)==10);               /* 25 anonymous (*param) form */
  tot=tot+(gticks==2);                         /* 26 tally ran exactly twice */
  return tot+1;
}
int anonrun(int (*)(int),int z)
{
  return z;
}
int fwd(int a)
{
  return a*11;
}
