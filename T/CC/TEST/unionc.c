/* M19 qualifier keywords (extern static register auto volatile inline
 * fold to no-ops with C's default-int promise), unsigned/signed char
 * into the byte model, union types as slot-aliased one-slot tags,
 * block-scope tag definitions, the full escape set for both literal
 * kinds, and string-literal indexing. Expect 40. */
static int sg=7;
extern int eg;
register int rg;
volatile int vg;
auto int ag;
extern int helper(int x);
union Value
{
  int i;
  char b;
  int *p;
};
struct Box { int w; union Value val; };
int bumpn(union Value cells[],int i);
int bytenum(char tab[][8],int r);
static int sfunc(int x)
{
  return x*3;
}
inline int ifunc(int x)
{
  return x-1;
}
extern int helper(int x)
{
  return x+100;
}
int main()
{
  struct Inner { int x; int y; };
  unsigned char ub[4];
  signed char sb;
  char unsigned cu;
  unsigned u;
  union Value cs[3];
  union Value v;
  struct Box bx;
  struct Inner in;
  int *ip;
  int side;
  int tot;
  int i;
  static int sl;
  char tab[2][8];
  tot=0;
  eg=5; rg=6; vg=7; ag=8; sl=9;
  in.x=1; in.y=2;
  tot=tot+((sg+eg+rg+vg+ag+sl)==42);           /*  1 every qualifier form declares */
  tot=tot+((helper(1)==101)&&(sfunc(3)==9)&&(ifunc(5)==4));  /*  2 qualified functions */
  tot=tot+((in.x+in.y)==3);                    /*  3 block-scope struct def + declarator */
  ub[0]=300;
  tot=tot+(ub[0]==44);                         /*  4 unsigned char store + byte load */
  tot=tot+(sizeof(ub)==4);                     /*  5 byte-element array: byte size */
  tot=tot+(sizeof(unsigned char)==1);          /*  6 the type itself */
  tot=tot+(sizeof(char unsigned)==1);          /*  7 reversed order too */
  sb=-3;
  tot=tot+(sb==253);                           /*  8 signed char = byte model (documented) */
  cu=65;
  tot=tot+(cu=='A');                           /*  9 */
  u=1;
  tot=tot+((u==1)&&(sizeof(unsigned)==8));     /* 10 bare unsigned is the int model */
  tot=tot+(sizeof(v)==8);                      /* 11 union IS one slot */
  tot=tot+(sizeof(union Value)==8);            /* 12 tag form in sizeof */
  v.i=0x41;
  tot=tot+(v.b=='A');                          /* 13 int -> char alias */
  v.b='Z';
  tot=tot+(v.i==90);                           /* 14 char -> int (zeroed slot: defined) */
  side=40;
  ip=&side;
  v.p=ip;
  *v.p=*v.p+2;
  tot=tot+((side==42)&&(v.i==(long)ip));       /* 15 pointer member live; int member IS the quad */
  bx.w=3;
  bx.val.i=11;
  tot=tot+((bx.w*100+bx.val.i)==311);          /* 16 by-value union member chains */
  tot=tot+(sizeof(struct Box)==16);            /* 17 struct with union member */
  for(i=0;i!=3;i++)
    cs[i].i=i*100;
  tot=tot+((cs[0].i==0)&&(cs[1].i==100)&&(cs[2].i==200));  /* 18 union array */
  tot=tot+(sizeof(cs)==24);                    /* 19 one slot per element */
  tot=tot+(bumpn(cs,1)==101);                  /* 20 union[] param member write */
  tot=tot+(cs[1].i==101);                      /* 21 visible through the param */
  cs[2].b='q';
  tot=tot+((cs[2].b=='q')&&(cs[2].i==113));    /* 22 byte alias inside an array element */
  strcpy(tab[0],"42");
  tot=tot+(bytenum(tab,0)==42);                /* 23 char[][8] param byte stride */
  {
    struct Deep { union Value first; int tail; };
    struct Deep d;
    d.first.i=77;
    d.tail=8;
    tot=tot+((d.first.i==77)&&(d.tail==8));    /* 24 block def whose member is a union */
    tot=tot+(sizeof(struct Deep)==16);         /* 25 */
  }
  {
    int acc;
    acc=0;
    for(int j=0;j!=4;j++)
      acc=acc+1;                                /*  26 for-init declaration, */
    tot=tot+(acc==4);                           /*  26 j lives only inside the loop */
  }
  tot=tot+("hello"[1]=='e');                    /* 27 string literal indexed */
  tot=tot+(("ab"+1)[0]=='b');                   /* 28 offset then index */
  tot=tot+(sizeof("ab")==8);                   /* 29 literal is pointer-sized (documented) */
  tot=tot+(strlen("a\tb")==3);                  /* 30 tab = one byte */
  tot=tot+("\x41"[0]=='A');                     /* 31 hex escape */
  tot=tot+("\101"[0]=='A');                     /* 32 octal escape */
  tot=tot+("\17"[0]==15);                      /* 33 octal stops at a non-octal digit */
  tot=tot+("\a"[0]==7);                        /* 34 bell */
  tot=tot+('\x41'=='A');                        /* 35 char-literal hex */
  tot=tot+('\101'=='A');                        /* 36 char-literal octal */
  tot=tot+(strlen("a\0b")==1);                  /* 37 \0 ends the string like C does */
  tot=tot+("\x41\x42"[1]=='B');                 /* 38 two hex escapes in a row */
  tot=tot+(sg*100+(helper(0)-100)==700);       /* 39 static global + qualified call */
  return tot+1;
}
int bumpn(union Value cells[],int i)
{
  cells[i].i=cells[i].i+1;
  return cells[i].i;
}
int bytenum(char tab[][8],int r)
{
  return atoi(tab[r]);
}
