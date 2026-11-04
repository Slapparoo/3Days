/* M31 function-like macros. The rules being driven here are: an argument is
 * expanded ONCE, in the context of its invocation, and then substituted
 * without further expansion; `#param` stringizes and `param ## x` pastes with
 * BOTH operands substituted unexpanded; a variadic parameter list makes
 * `__VA_ARGS__` an operand, its stringized form is the tail as written, and
 * `, ## __VA_ARGS__` deletes its comma only when the tail is empty; and a
 * paste with one empty operand yields the OTHER operand. Unlike every earlier
 * prep fixture these run through ordinary C code paths — array sizes,
 * initializers, case labels, a for header, a static's initializer, string
 * compares — so the answers come from executing the compiled code.
 * Self-reference and blue paint are pinned by the prep oracle against
 * `gcc -E` instead: a self-referential object macro cannot even be declared as
 * a variable once both engines have expanded it in the declaration.
 * Expect 45 (44 checks + closer). */
#define TWO(x) ((x)*2)
#define ADD(a,b) ((a)+(b))
#define DUP(x) ((x)+(x))
#define SMALL(a,b) (((a)<(b))?(a):(b))
#define LUCKY() 7
#define HEAD(a,b) (a)
#define CAT(a,b) a##b
#define ONE(x) x##1
#define STR(x) #x
#define STR2(x) # x
#define KEEP(h, ...) h
#define LIST(...) __VA_ARGS__
#define STRI(...) #__VA_ARGS__
#define OPT(h, ...) (h , ## __VA_ARGS__)
#define JOIN(h, ...) h##__VA_ARGS__
#define APPLY(f,a) f(a)
#define PASS(...) INNER(__VA_ARGS__)
#define INNER(a,b) ((a)*(b))
#define OUTR(x) INNER2(x,2)
#define INNER2(a,b) ((a)*(b))
#define FLD(s) s.a
#define AD2(a,b) ((a)+(b))
#define NUM 1
#define THREE 1

int val1=11;
int n1=7;
int xxyy=3;
struct P { int a; char b; int c; };
struct P gp = { 5, 'x', 9 };

int low2(int x,int y)
{
  if (x<y)
    return x;
  return y;
}

int inc(int v)
{
  return v+1;
}

int main()
{
  int w3[3] = { LIST(1,2,3) };
  int q[TWO(2)];
  int tot;
  int i,r;
  int AD2;
  char t[16];
  char c8[8];
  struct P p;
  static int gs = ADD(1,2);

  tot=0;
  for (i=0;i!=4;i++)
    q[i]=i+1;
  p.a=4;
  p.b=0;
  p.c=6;
  AD2=8;

  tot=tot+(TWO(3)==6);                                /* 1 one argument       */
  tot=tot+(ADD(TWO(1),3)==5);                         /* 2 an argument is     */
  tot=tot+(ADD(1,TWO(2))==5);                         /*   expanded first     */
  tot=tot+(DUP(3)==6);                                /* 3 a parameter used   */
  tot=tot+(DUP(1+2)==6);                              /*   twice, as TEXT     */
  tot=tot+(SMALL(3,5)==3);                            /* 4 a ternary body     */
  tot=tot+(SMALL(1+1,4)==2);                          /* 5 expression args    */
  tot=tot+(LUCKY()==7);                               /* 6 zero parameters    */
  tot=tot+(HEAD(9,1)==9);                             /* 7 two args, one used */
  tot=tot+(CAT(,2)==2);                               /* 8 empty LEFT operand */
  tot=tot+(CAT(1,)==1);                               /* 9 empty RIGHT one    */
  tot=tot+(CAT(val,1)==11);                           /* 10 a paste forms a   */
  tot=tot+(ONE(n)==7);                                /*    name out of the   */
  tot=tot+(CAT(xx,yy)==3);                            /*    pieces            */
  tot=tot+(low2(LIST(3,9))==3);                       /* 11 a variadic tail   */
  tot=tot+(low2(9,LIST(4))==4);                       /*    IS an argument    */
  tot=tot+((w3[0]+w3[1]+w3[2])==6);                   /* 12 list, as an       */
  tot=tot+(PASS(3,4)==12);                            /*    initializer: the  */
  tot=tot+(OUTR(3)==6);                               /*    tail is split on  */
  tot=tot+(KEEP(9,1,2)==9);                           /*    its OWN commas    */
  tot=tot+(APPLY(inc,4)==5);                          /* 13 a macro builds a  */
  tot=tot+(strcmp(HEAD("a,b","c"),"a,b")==0);         /*    call; a comma in  */
  tot=tot+(sizeof(q)/sizeof(int)==4);                 /*    a literal is not  */
  tot=tot+(FLD(gp)==5);                               /* 14 member access     */
  tot=tot+(FLD(p)==4);                                /*    through a macro   */
  tot=tot+(AD2==8);                                   /* 15 a function-like   */
                                                      /*    name on its own   */
                                                      /*    is an identifier  */
  {
    int sw;
    sw=2;
    r=0;
    switch (sw)
    {
      case TWO(1):
        r=1;
        break;                                        /* 16 a case label is a */
      default:
        r=2;
        break;                                        /*    folded constant   */
    }
    tot=tot+(r==1);
  }
  r=0;
  for (i=0;i!=TWO(2);i++)                             /* 17 a for header uses */
    r=r+q[i];                                         /*    a macro twice     */
  tot=tot+(r==10);
  tot=tot+(gs==3);                                    /* 18 a static inits    */
                                                      /*    through a macro   */
  strcpy(t,STR(1+  2));
  tot=tot+(strcmp(t,"1+ 2")==0);                      /* 19 stringize: runs   */
  strcpy(t,STR2(2+2));                                /* 20 of whitespace     */
  tot=tot+(strcmp(t,"2+2")==0);                       /*    collapse to one,  */
  strcpy(t,STR("hi"));                                /*    `#` and its       */
  tot=tot+(strcmp(t,"\"hi\"")==0);                    /*    operand may be    */
  strcpy(c8,STR('z'));                                /*    spaced            */
  tot=tot+(strcmp(c8,"'z'")==0);                      /* 21 a char literal    */
  strcpy(t,STR(a /**/ b));                            /*    operand           */
  tot=tot+(strcmp(t,"a b")==0);                       /* 22 a comment inside  */
  strcpy(t,STRI(1, 2));                               /*    an argument is    */
  tot=tot+(strcmp(t,"1, 2")==0);                      /*    whitespace, and   */
  strcpy(t,STRI(7));                                  /*    the tail keeps    */
  tot=tot+(strcmp(t,"7")==0);                         /*    its OWN spacing   */
  tot=tot+(OPT(5)==5);                                /* 23 the comma before  */
  tot=tot+(OPT(5,6)==6);                              /*    ## goes away with */
  tot=tot+(JOIN(5)==5);                               /* 24 nothing after it, */
  tot=tot+((JOIN(5,6))==56);                          /*    but a name does   */
  tot=tot+(NUM==1);                                   /* 25 object-like use   */
  #undef NUM
  #define NUM(x) ((x)*3)
  tot=tot+(NUM(2)==6);                                /* 26 #undef then the   */
  #define THREE(y) ((y)-1)
  tot=tot+(THREE(9)==8);                              /* 27 same name becomes */
  #define LATE 4
  tot=tot+(LATE==4);                                  /*    function-like,    */
                                                      /*    inside a block    */
  return tot+1;
}
