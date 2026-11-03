/* M28 ++/-- for real: PREFIX ++v and --v now parse (they never did —
 * `++v` died at the operator), and POSTFIX moved into the postfix chain,
 * so `(*p)++` and `(v)++` work too. One emitter serves both spellings:
 * prefix yields the NEW value, postfix the OLD one, and scaling, byte-vs-
 * quad width and what survives in RAX cannot differ between them. Covers
 * scalars, chars, array elements (1-D, 2-D, byte elements through a char
 * pointer), pointer steps, dereferenced targets, struct and union members,
 * globals, statics, while/for conditions and the `%` interplay that shares
 * RCX and RDX. Expect 42 (41 checks + closer). */
struct P { int a; char b; int c; };
union U { int i; char ch; };
int gv;
char w1[4]="ab";
char w2[4]="cde";
int bump(int x)
{
  return x+1;
}
int tick()
{
  static int k;
  ++k;
  return k;
}
int loopcount(int n)
{
  int i;
  int c;
  c=0;
  for(i=0;i!=n;++i)
    c=c+i;
  return c;
}
int walksum(int a[],int n)
{
  int *p;
  int c;
  c=0;
  for(p=a;p!=a+n;++p)
    c=c+*p;
  return c;
}
int memberstep(struct P *p)
{
  p->a++;
  ++p->c;
  p->b++;
  return p->a+p->c+p->b;
}
int main()
{
  int a[4];
  int m[2][3];
  int v;
  int n;
  int w;
  int *p;
  char ch;
  char *tab[2];
  struct P s;
  struct P *sp;
  union U u;
  int tot;
  int i;
  tot=0;
  a[0]=11;
  a[1]=22;
  a[2]=33;
  a[3]=44;
  v=1;
  ++v;
  tot=tot+(v==2);                               /*  1 prefix bumps the object */
  --v;
  tot=tot+(v==1);                               /*  2 prefix decrement too */
  v=1;
  tot=tot+((++v)==2);                           /*  3 prefix value IS the new */
  tot=tot+((v--)==2);                           /*  4 postfix value IS the old */
  tot=tot+(v==1);                               /*  5 and the object moved anyway */
  v=4;
  tot=tot+((--v)==3);                           /*  6 prefix decrement value */
  tot=tot+((v++)==3);                           /*  7 postfix increment value */
  tot=tot+(v==4);                               /*  8 */
  p=a;
  ++p;
  tot=tot+(*p==22);                             /*  9 pointer steps a SLOT */
  tot=tot+((*(p++))==22);                       /* 10 postfix deref uses the old */
  tot=tot+(*p==33);                             /* 11 then the pointer has moved */
  p=a;
  tot=tot+((*++p)==22);                         /* 12 pre-increment then deref */
  --p;
  tot=tot+(*p==11);                             /* 13 and back down */
  v=1;
  (v)++;
  tot=tot+(v==2);                               /* 14 (v)++ — new: parens */
  v=1;
  p=&v;
  (*p)++;
  tot=tot+(v==2);                               /* 15 (*p)++ — new: deref base */
  v=1;
  p=&v;
  ++*p;
  tot=tot+(v==2);                               /* 16 ++*p reaches the target */
  ch='a';
  ++ch;
  tot=tot+(ch=='b');                           /* 17 char scalar bumps by ONE */
  ch='b';
  tot=tot+((ch--)=='b');                        /* 18 old value, byte-wide */
  tot=tot+(ch=='a');                            /* 19 */
  tot=tot+((w1[0]++)=='a');                     /* 20 char array element, old value */
  tot=tot+(w1[0]=='b');                         /* 21 BYTE store */
  ++w2[0];
  tot=tot+(w2[0]=='d');                         /* 22 prefix on a char element */
  tab[0]=w1;
  tab[1]=w2;
  tot=tot+((tab[0][0]++)=='b');                 /* 23 byte element of a byte pointer */
  tot=tot+(w1[0]=='c');                         /* 24 through the table */
  ++tab[1][1];
  tot=tot+(w2[1]=='e');                         /* 25 prefix two subscripts deep */
  m[0][0]=1;
  m[0][1]=2;
  m[0][2]=3;
  m[1][0]=4;
  m[1][1]=5;
  m[1][2]=6;
  ++m[1][1];
  tot=tot+(m[1][1]==6);                         /* 26 2-D element, row scale intact */
  s.a=10;
  s.b='k';
  s.c=20;
  sp=&s;
  tot=tot+(memberstep(sp)==140);                /* 27 member ++, -- and byte width */
  tot=tot+((s.a==11)&&(s.c==21)&&(s.b=='l'));   /* 28 the object saw all three */
  gv=5;
  ++gv;
  tot=tot+(gv==6);                              /* 29 a global bumps */
  tot=tot+((tick()==1)&&(tick()==2));           /* 30 static, prefix, across calls */
  tot=tot+(loopcount(4)==(0+1+2+3));            /* 31 ++i as the for update */
  tot=tot+(walksum(a,4)==(11+22+33+44));        /* 32 ++p walking a pointer param */
  n=3;
  w=0;
  while(n--)
    w=w+n;
  tot=tot+((w==3)&&(n==-1));                    /* 33 while(n--) tests the OLD value */
  n=3;
  w=0;
  while(--n)
    w=w+n;
  tot=tot+((w==3)&&(n==0));                     /* 34 while(--n) tests the NEW one */
  i=5;
  tot=tot+(a[++i % 4]==33);                     /* 35 ++ inside a modulo index */
  v=9;
  tot=tot+(bump(++v)==11);                      /* 36 ++ as a call argument */
  tot=tot+(v==10);                              /* 37 the caller saw the bump */
  v=1;
  tot=tot+(((++v)==2)?1:0);                     /* 38 prefix in a condition */
  v=1;
  tot=tot+(sizeof(v++)==8);                     /* 39 sizeof does not evaluate */
  tot=tot+(v==1);                               /* 40 and so nothing was stored */
  for(i=0;i!=3;++i)
    a[i]=1;
  for(i=0;i!=3;++i)
    a[i]++;
  tot=tot+((a[0]==2)&&(a[2]==2));               /* 41 loops with prefix updates */
  return tot+1;
}
