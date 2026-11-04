/* M35: C doubles, delegated to the host's F64 operators.
 *
 * A double is ONE 8-byte slot holding the IEEE-754 binary64 pattern, so storage
 * (globals, locals, statics, params, returns, struct/union members, arrays and
 * their pointer-free subscripting) needs nothing new and every check below is
 * about COMPUTATION: the arithmetic, the conversions at the five boundaries a
 * bit pattern can cross silently (cast, assignment, argument, return,
 * initializer), comparison, and the operator rules lite refuses.
 *
 * Values are observable only through (int)/(long) truncation and %d: printf has
 * no %f yet.  A check that wraps its float in (int) cannot see a mixed-type
 * comparison, which is why 50-53 are left unwrapped.  Every literal used here is
 * an exact binary fraction (1.5, 2.5,
 * 0.25, 7.0) so that the kernel's decimal parser (a few ULP off on values like
 * pi) cannot move an answer; the single-precision, -0.0-truthiness, sizeof(float)
 * and hex-float divergences are recorded in PLAN rather than asserted here.
 * Expect 53 — the value gcc -std=gnu99 gives this same file. */
double gi = 3;                    /* int initializer -> 3.0 */
double gf = 1.5 + 2.5;            /* folded by the delegated add */
int    gd = 2.5;                  /* float initializer -> 2 */
double ga[3] = {1, 2.5, 3};       /* per-element conversion */
double gm[2][2] = {1, 2, 3, 4};   /* element type survives two dimensions */
struct P { double x; int i; };
struct P gp = {2, 3};             /* the member's own type decides */
union  U { double d; int i; };
union  U gu = {5};

double fdd(double a){ return a * 2.0; }
double fdi(void){ return 3; }
int    fid(double a){ return (int)a; }
int    fdi2(void){ return 2.5; }
double rec(double x){ if (x <= 0.0) return 0.0; return x + rec(x - 1.0); }
double fsq(double t[]){ return t[0] + t[1]; }
int    step(void){ static double s = 3; s = s + 1.0; return (int)s; }

int main()
{
  int tot,i,n;
  double a,b,c,x;
  double arr[4];
  double one,half;
  float  fl;
  struct P p;

  tot=0;
  a=1.0;
  b=2.0;
  x=0.0;
  /* 1-6  initializers convert to the object's type (6.7.9/19) */
  tot=tot+((int)gi==3);
  tot=tot+((int)gf==4);
  tot=tot+(gd==2);
  tot=tot+((int)(ga[0]+ga[1]+ga[2])==6);
  tot=tot+((int)(gm[0][1]+gm[1][0])==5);
  tot=tot+(((int)gp.x==2)&&(gp.i==3));
  /* 7  union: member 0's type */
  tot=tot+((int)gu.d==5);
  /* 8-16  arithmetic on the delegated F64 operators */
  tot=tot+((int)(a+b)==3);
  tot=tot+((int)(a-b)==-1);
  tot=tot+((int)(a*b)==2);
  tot=tot+((int)(b/a)==2);
  tot=tot+((int)(7.0/2.0)==3);
  tot=tot+((int)-2.5==-2);
  tot=tot+((int)(a+1)==2);            /* int operand converts */
  tot=tot+((int)(1+a)==2);
  tot=tot+((int)(2*(a+b))==6);
  /* 17  unary minus is the sign bit, not two's complement of the quad */
  c=-a;
  tot=tot+(((int)c==-1)&&((int)(c*c)==1));
  /* 18-21  comparison, including the two-negative pair a quad compare flips */
  tot=tot+((a<b)&&!(a>b));
  tot=tot+((a<=a)&&(a>=a)&&(a<=b)&&!(a>=b));
  one=1.0;
  half=0.5;
  tot=tot+((one+half==1.5)&&!(one+half!=1.5));
  c=-2.5;
  tot=tot+((c<-1.0)&&(c>-3.0)&&!(c>-1.0));
  /* 22-25  casts are the only implicit crossing of the two models */
  tot=tot+(((int)2.5==2)&&((int)-2.5==-2));
  tot=tot+((int)(double)7==7);
  tot=tot+((int)(double)(int)2.5==2);
  tot=tot+((int)(char)65.5==65);
  /* 26-31  assignment converts to the LEFT type */
  x=1.0;
  x=x+2.0;
  tot=tot+((int)x==3);
  gi=gi+1.0;
  tot=tot+((int)gi==4);
  x=3;
  tot=tot+((int)x==3);
  n=0;
  n=2.5;
  tot=tot+(n==2);
  x=1.0;
  x+=2.5;
  tot=tot+((int)x==3);
  x=2.0;
  x*=2.5;
  tot=tot+((int)x==5);
  /* 32  ++/-- step 1.0, not the quad 1 */
  x=1.5;
  x++;
  i=(int)x;
  x--;
  tot=tot+((i==2)&&((int)x==1));
  /* 33  a subscripted float element through the same path */
  arr[0]=1.5;
  arr[0]++;
  tot=tot+((int)arr[0]==2);
  /* 34-39  the boundaries a call has: argument, parameter, return */
  tot=tot+((int)fdd(1.5)==3);
  tot=tot+((int)fdd(3)==6);          /* int argument converts to 3.0 */
  tot=tot+((int)fdi()==3);
  tot=tot+(fid(2.5)==2);
  tot=tot+(fdi2()==2);
  tot=tot+((int)rec(3.0)==6);        /* recursion through the prototype */
  /* 40  array parameter: the element type arrives with the array */
  arr[0]=1.0;
  arr[1]=2.5;
  tot=tot+((int)fsq(arr)==3);
  /* 41  a static local double keeps its converted initial value */
  step();
  tot=tot+(step()==5);
  /* 42-44  control flow reads the comparison, which is an integer 0/1 */
  if (a<b)
    tot=tot+1;
  x=0.0;
  while (x<3.0)
    x=x+1.0;
  tot=tot+((int)x==3);
  tot=tot+((a>0.0)&&(b>0.0)&&!(a>b));
  /* 45  both arms floating: the type stands */
  tot=tot+((int)((a<b)?1.0:2.0)==1);
  /* 46-48  sizes the slot model answers the same way C does */
  tot=tot+((sizeof(double)==8)&&(sizeof(x)==8));
  tot=tot+(sizeof(ga)==24);
  tot=tot+(sizeof(gm)==32);
  /* 49  float is the same slot and the same value (see PLAN for sizeof) */
  fl=1.5;
  tot=tot+(((int)fl==1)&&((int)(fl+fl)==3));
  /* 50-53  C converts BOTH operands of a comparison (6.5.8/6.5.9), so a mixed
   * pair is not the F64 bits of one side against the number on the other: with
   * x holding exactly 2.0 the bit pattern is 4613937818241073152, and `x==2` is
   * true here only because the integer 2 was converted first.  Every operator
   * appears once, both operand orders appear, `n` is a VARIABLE so the
   * conversion cannot come from folding, and the last check is two constants,
   * which is the folder's route rather than the emitted code's. */
  x=2.0;
  tot=tot+((x==2)&&!(x==3)&&(x!=3));
  tot=tot+((x<3)&&!(x<2)&&((x<=2)&&((x>=2)&&!(x>2))));
  n=1;
  tot=tot+((n<x)&&!(2<x));
  tot=tot+(1.5<2);
  return tot;
}
