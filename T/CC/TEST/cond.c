/* M32 conditionals. What is pinned here is the DECISION each directive makes:
 * every check is a branch the two compilers must agree on, so a wrong decision
 * arrives as a wrong value rather than as a message. The conditions cover
 * `defined` in its four written forms (parenthesised, spaced, bare, negated),
 * the arithmetic, bitwise, relational, equality and logical operators at C's
 * precedence, `?:`, character constants decoded through the SAME escape table
 * the literals below them use, hex and binary literals and an integer suffix,
 * the zero C gives a name that is still a name after expansion (6.10.1/3), a
 * function-like macro in a condition, `defined` reaching a condition through a
 * macro body, short-circuiting that keeps `1/0` in a dead arm from being a
 * diagnostic, #elif chains where only the branches up to the first taken one
 * are evaluated, nesting, and text that must never be read at all: a broken
 * declaration, an #error and an #include of a file that does not exist, all
 * inside `#if 0`. `#pragma` is deliberately not exercised here: our prep
 * CONSUMES the directive while gcc -E forwards it to the compiler proper, so
 * the two outputs cannot match token for token on a file that has one; its
 * verdict is pinned in the diagnostic matrix instead.
 * Literal rules are THIS compiler's (PLAN: no octal, no
 * 128-bit), so no condition here uses a leading-zero or an over-large literal.
 * Expect 43 (42 checks + closer). */
#define FL 1
#define ZE 0
#define SEVEN 7
#define BIG (1 << 3)
#define F(x) ((x)*2)
#define ISDEF(x) defined(x)
#define ALIAS NOTAD3
#define OBJD defined(NOTAD)
#define V 3

#if FL
int pickA(void)
{
  return 5;
}
#else
int pickA(void)
{
  return 9;
}
#endif

#if defined(NOTAD)
int pickB(void)
{
  return 5;
}
#else
int pickB(void)
{
  return 9;
}
#endif

int pickC(void)
{
  int out;

  out=0;
  #if V == 1
  out=1;
  #elif V == 3
  out=2;                                        /* the chain picks this arm */
  #else
  out=3;
  #endif
  return out;
}

#if FL
int szA[3];
#else
int szA[7];
#endif

int main()
{
  int tot,r,a,i;

  tot=0;

  r=0;
  #ifdef FL
  r=1;
  #else
  r=2;
  #endif
  tot=tot+(r==1);                       /* 1 #ifdef of a defined name takes */
  r=0;
  #ifndef FL
  r=1;
  #else
  r=2;
  #endif
  tot=tot+(r==2);                       /* 2 #ifndef declines the same name */
  r=0;
  #ifdef NOTAD
  r=1;
  #else
  r=2;
  #endif
  tot=tot+(r==2);            /* 3 #ifdef of an undefined name does not take */
  r=0;
  #ifndef NOTAD
  r=1;
  #else
  r=2;
  #endif
  tot=tot+(r==1);                        /* 4 #ifndef takes it in its place */

  r=0;
  #if defined(FL)
  r=r+1;
  #endif
  #if defined (FL)
  r=r+2;
  #endif
  #if defined FL
  r=r+4;
  #endif
  #if !defined(NOTAD)
  r=r+8;
  #endif
  #if defined(SEVEN) == 1
  r=r+16;
  #endif
  tot=tot+(r==31);                   /* 5 the four written forms of defined */

  r=0;
  #if FL
  r=r+1;
  #endif
  #if !ZE
  r=r+2;
  #endif
  #if NOTAD == 0
  r=r+4;
  #endif
  #if FL == SEVEN - 6
  r=r+8;
  #endif
  tot=tot+(r==15);          /* 6 a body of 1, a ! of 0, == under arithmetic */

  r=0;
  #if (2+3)*4 == 20
  r=r+1;
  #endif
  #if 1 + 2 * 3 == 7
  r=r+2;
  #endif
  #if (8 - 2) / 3 == 2
  r=r+4;
  #endif
  #if 7 % 4 == 3
  r=r+8;
  #endif
  tot=tot+(r==15);                         /* 7 * / % bind tighter than + - */

  r=0;
  #if 0x10 == 16
  r=r+1;
  #endif
  #if 0X1F == 31
  r=r+2;
  #endif
  #if 0b11 == 3
  r=r+4;
  #endif
  #if 1ul > 0
  r=r+8;
  #endif
  tot=tot+(r==15);                /* 8 hex, capital X, binary, and a suffix */

  r=0;
  #if 'a' == 97
  r=r+1;
  #endif
  #if '\x41' == 65
  r=r+2;
  #endif
  #if '\101' == 65
  r=r+4;
  #endif
  #if '\n' == 10
  r=r+8;
  #endif
  tot=tot+(r==15);                     /* 9 char constants through CCEscape */

  r=0;
  #if 'ab' == 0x6162
  r=r+1;
  #endif
  #if ~0 == -1
  r=r+2;
  #endif
  #if -(-3) == 3
  r=r+4;
  #endif
  #if !(0)
  r=r+8;
  #endif
  tot=tot+(r==15);                 /* 10 'ab' packs left to right; ~0 is -1 */

  r=0;
  #if (5 ^ 3) == 6
  r=r+1;
  #endif
  #if (6 & 3) == 2
  r=r+2;
  #endif
  #if (6 | 1) == 7
  r=r+4;
  #endif
  #if (1 << 4) | 2 == 16
  r=r+8;
  #endif
  tot=tot+(r==15);         /* 11 ^ & | are bitwise; == binds tighter than | */

  r=0;
  #if (2 >> 1) == 1
  r=r+1;
  #endif
  #if 1 <= 1 && 2 >= 1 && 1 < 2 && 2 > 1
  r=r+2;
  #endif
  #if 1 != 2
  r=r+4;
  #endif
  #if 1 == 1 != 0
  r=r+8;
  #endif
  tot=tot+(r==15);       /* 12 >>, the relational forms, != and == chaining */

  r=0;
  #if 1 && 1
  r=r+1;
  #endif
  #if 0 || 1
  r=r+2;
  #endif
  #if (1 && 0) || (0 || 1)
  r=r+4;
  #endif
  #if (1 || 0) == 1
  r=r+8;
  #endif
  tot=tot+(r==15);                     /* 13 && and || yield exactly 0 or 1 */

  r=0;
  #if 1 ? 2 : 3
  r=2;
  #else
  r=3;
  #endif
  tot=tot+(r==2);                /* 14 the whole ?: value is what is tested */
  r=0;
  #if 0 ? 2 : 3
  r=2;
  #else
  r=3;
  #endif
  r=r+1;
  #if 0 ? 3 : 0
  r=r+100;
  #else
  r=r+1;
  #endif
  tot=tot+(r==4);            /* 15 a ?: whose chosen arm is 0 goes to #else */

  r=0;
  #if 1 ? 0 : 1/0
  r=1;
  #else
  r=2;
  #endif
  tot=tot+(r==2);           /* 16 a dead ?: arm is parsed but never faulted */
  r=0;
  #if 1 || 1/0
  r=1;
  #else
  r=2;
  #endif
  tot=tot+(r==1);                        /* 17 || short-circuits before 1/0 */
  r=0;
  #if 0 && 1/0
  r=1;
  #else
  r=2;
  #endif
  tot=tot+(r==2);                        /* 18 && short-circuits before 1/0 */

  r=0;
  #if F(3) == 6
  r=1;
  #else
  r=2;
  #endif
  tot=tot+(r==1);                /* 19 a function-like macro in a condition */
  r=0;
  #if F(F(1)) == 4
  r=1;
  #else
  r=2;
  #endif
  tot=tot+(r==1);                  /* 20 a nested invocation in a condition */
  r=0;
  #if BIG == 8
  r=1;
  #else
  r=2;
  #endif
  tot=tot+(r==1);           /* 21 an object-like body that is an expression */

  r=0;
  #if ISDEF(NOTAD) == 0
  r=1;
  #else
  r=2;
  #endif
  tot=tot+(r==1);             /* 22 defined() in a body sees the name given */
  r=0;
  #if ISDEF(ALIAS) == 0
  r=1;
  #else
  r=2;
  #endif
  tot=tot+(r==1);                /* 23 the argument is expanded, THEN named */
  r=0;
  #if defined(ALIAS)
  r=r+1;
  #endif
  #if defined(NOTAD3)
  r=r+2;
  #endif
  tot=tot+(r==1);      /* 24 a macro is a name to defined; its value is not */
  #if OBJD == 0
  r=1;
  #else
  r=2;
  #endif
  tot=tot+(r==1);        /* 25 an object-like body whose text is defined(x) */
  r=0;
  #if defined(F)
  r=1;
  #else
  r=2;
  #endif
  tot=tot+(r==1);           /* 26 a function-like name is a name to defined */

  r=0;
  #if V == 1
  r=1;
  #elif V == 3
  r=2;
  #elif V == 5
  r=3;
  #else
  r=4;
  #endif
  tot=tot+(r==2);        /* 27 the first #elif that answers takes the chain */
  r=0;
  #if V == 3
  r=1;
  #elif V == 1/0
  r=2;
  #elif ))
  r=3;
  #else
  r=4;
  #endif
  tot=tot+(r==1);            /* 28 a chain that is decided stops evaluating */
  r=0;
  #if 0
  r=1;
  #elif defined(NOTAD)
  r=2;
  #elif defined(FL)
  r=3;
  #else
  r=4;
  #endif
  tot=tot+(r==3);                  /* 29 #elif reaches the arm that answers */
  r=0;
  #if 0
  r=1;
  #elif 0
  r=2;
  #else
  r=3;
  #endif
  tot=tot+(r==3);           /* 30 an all-false chain falls through to #else */

  r=0;
  #if FL
  #if defined(FL)
  r=r+1;
  #else
  r=r+2;
  #endif
  #if ZE
  r=r+4;
  #else
  r=r+8;
  #endif
  #else
  r=9;
  #endif
  tot=tot+(r==9);            /* 31 nesting: an #else belongs to its own #if */
  r=0;
  #if ZE
  #if defined(FL)
  r=r+1;
  #else
  r=r+2;
  #endif
  #else
  #if defined(NOTAD)
  r=r+4;
  #else
  r=r+8;
  #endif
  #endif
  tot=tot+(r==8);      /* 32 an inner region in a skipped one stays skipped */

  r=0;
  #if 0
  int ))) this text is never read (;
  #error it is not read, either
  #include "no-such-file.h"
  #endif
  r=1;
  tot=tot+(r==1);       /* 33 skipped text is not parsed, expanded, or read */
  r=0;
  #ifdef NOTAD
  #error would fail the compile
  #endif
  r=1;
  tot=tot+(r==1);         /* 34 an #error in a branch not taken cannot fail */

  r=0;
  #define MYP 1
  #ifdef MYP
  r=1;
  #endif
  #undef MYP
  #ifdef MYP
  r=r+2;
  #else
  r=r+4;
  #endif
  #define MYP 2
  #if MYP == 2
  r=r+8;
  #endif
  tot=tot+(r==13);         /* 35 #define and #undef in a block, seen by #if */

  a=0;
  #if FL
  a=a+1;
  #endif
  #if ZE
  a=a+100;
  #endif
  tot=tot+(a==1);             /* 36 a whole statement comes with its branch */
  i=0;
  #if FL
  for (i=0;i!=4;i++)
    a=a+i;
  #endif
  tot=tot+(a==7);                      /* 37 and so does a whole for header */

  tot=tot+(pickA()==5);                 /* 38 #if chose which pickA returns */
  tot=tot+(pickB()==9);       /* 39 an untaken defined() branch chose pickB */
  tot=tot+(pickC()==2);             /* 40 a chain inside a body chose pickC */
  tot=tot+(sizeof(szA)/sizeof(int)==3);   /* 41 a declaration is chosen too */
  r=0;
  #if ZE
  #if 1
  #error never
  #elif 1
  #error never either
  #endif
  #endif
  r=1;
  tot=tot+(r==1);                            /* 42 a skipped chain's #elif */
  return tot+1;
}
