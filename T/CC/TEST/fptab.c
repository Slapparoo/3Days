/* M26 arrays of function pointers: `int (*tab[2])(int)` declared at file
 * scope, block scope and `static`, sized or deduced, passed as a
 * parameter, and called through `tab[i](x)` / `(*tab[i])(x)` / `(tab[i])(x)`;
 * elements are typed as pointers (M25) and their code addresses carry
 * arena relocations (M24), so a saved object re-derives every entry after
 * the code buffer moves. Expect 30. */
typedef int (*binop)(int);
int inc(int x)
{
  return x+1;
}
int dbl(int x)
{
  return x*2;
}
int neg(int x)
{
  return -x;
}
int (*gt[3])(int)={inc,dbl,neg};
int (*gd[])(int)={neg,neg};
struct F { int (*fn)(int); int tag; };
int apply(binop f,int x)
{
  return f(x);
}
int through(int (*t[])(int),int i,int x)
{
  return t[i](x);
}
int fixed(int (*t[2])(int))
{
  return t[0](10)+t[1](10);
}
int sumall(int (*t[])(int),int n,int x)
{
  int i;
  int c;
  c=0;
  i=0;
  while(i!=n)
  {
    c=c+t[i](x);
    i=i+1;
  }
  return c;
}
int pick(int which)
{
  static int (*tbl[3])(int)={inc,dbl,neg};
  if (which==3)
    tbl[0]=neg;
  return tbl[0](40);
}
int main()
{
  int (*a[2])(int);
  int (*b[3])(int)={dbl,neg,inc};
  int (*u[])(int)={inc,inc};
  int (*tt[3])(int)={inc};
  binop tab[3];
  struct F f;
  binop got;
  int tot;
  int i;
  int c;
  a[0]=inc;
  a[1]=dbl;
  tab[0]=gt[0];
  tab[1]=gt[1];
  tab[2]=gt[2];
  tot=0;
  tot=tot+(a[0](2)==3);                       /*  1 element call */
  tot=tot+(a[1](2)==4);                      /*  2 second element */
  tot=tot+((*a[1])(2)==4);                   /*  3 deref call form */
  tot=tot+((a[0])(2)==3);                    /*  4 parenthesized form */
  a[0]=dbl;
  tot=tot+(a[0](2)==4);                      /*  5 retargeted */
  a[0]=inc;
  tot=tot+(a[0](2)==3);                      /*  6 retargeted back */
  tot=tot+(b[0](5)==10);                     /*  7 frame table from init */
  tot=tot+(b[2](5)==6);                      /*  8 third element */
  tot=tot+(sizeof(a)==16);                   /*  9 array is slots */
  tot=tot+(sizeof(a[0])==8);                 /* 10 an element is a pointer */
  tot=tot+(tt[1]==0);                        /* 11 tail slots zero-filled */
  tot=tot+((a[0]!=0)&&(a[1]!=0));            /* 12 assigned slots live */
  i=0;
  c=0;
  for(i=0;i!=2;i++)
    c=c+a[i](7);
  tot=tot+(c==22);                           /* 13 loop dispatch: 8+14 */
  tot=tot+(through(b,1,9)==-9);              /* 14 table through a param */
  tot=tot+(through(u,1,1)==2);               /* 15 deduced table as arg */
  tot=tot+(fixed(a)==31);                    /* 16 param with a written count */
  tot=tot+(sumall(b,3,3)==(6+(-3)+4));       /* 17 all three elements */
  tot=tot+(apply(a[1],6)==12);               /* 18 element into a scalar fp */
  f.fn=b[0];
  f.tag=7;
  tot=tot+((f.fn(3)==6)&&(f.tag==7));        /* 19 element into a struct member */
  got=gt[1];
  tot=tot+(got(8)==16);                      /* 20 global table element copied */
  tot=tot+(gt[0](0)==1);                     /* 21 global sized table */
  tot=tot+(gt[2](5)==-5);                    /* 22 neg */
  tot=tot+(gd[0](3)==-3);                    /* 23 global deduced table */
  tot=tot+(gd[1]==gt[2]);                    /* 24 same function, same address */
  tot=tot+(tab[0]==gt[0]);                   /* 25 typedef'd fp array works too */
  tot=tot+((tab[1](2)==4)&&(tab[2](2)==-2)); /* 26 through the typedef table */
  tot=tot+(pick(1)==41);                      /* 27 static table: first call */
  tot=tot+(pick(2)==41);                      /* 28 the table persists */
  tot=tot+(pick(3)==-40);                     /* 29 element retargeted */
  tot=tot+(pick(9)==-40);                     /* 30 and the change persists */
  return tot+1;
}
