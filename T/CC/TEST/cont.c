/* M33 line continuations, and #include relative to the file that contains
 * the directive.
 *
 * A backslash that ends a line splices the next line into it BEFORE anything
 * is recognised — before a directive, before a comment, before a literal —
 * and that ordering is what every check below pins: a macro BODY, its
 * PARAMETER list, its NAME, a `##` right operand, an `#if` condition, an
 * `#ifdef` name, an `#include` name, a call's ARGUMENT LIST, one argument of
 * it, a string literal, a character constant, an identifier in a declarator,
 * an initializer list and a `for` header all cross the splice. The same
 * ordering makes a `//` claim the line after it, makes a block comment pick
 * up its terminator from the next line, and makes a body swallow the WHOLE
 * LINE below it even when that line was a directive: INSIDE2 below is never
 * defined, because the `#define` that would have defined it is the tail of
 * EATS.
 *
 * Whitespace decides which side of the splice survives, and lite follows gcc
 * exactly here, warnings aside: a space BEFORE the backslash stays, so
 * `STR(1 ` + the backslash + a newline + `2)` stringizes `1 2` as two
 * tokens; every space AFTER the backslash goes with the newline, so `12` +
 * backslash + two spaces + a newline + `34` is the one number 1234. A file
 * whose last byte is a backslash with no newline after it keeps the
 * backslash (pinned in the corpus, since a fixture cannot end mid-line and
 * still hold checks).
 *
 * The path rule pinned here: a QUOTED name is searched for in the directory
 * of the file containing the directive first — the main file's directory,
 * then the directory of each header in turn — and only then in the header
 * home, while `<name>` goes straight to the home. contaux.h therefore
 * reaches `sub/leafh.h` from the main file's directory, leafh.h reaches
 * `leafb.h` from ITS own, and the `leafb.h` lying beside this file — 60,
 * same macro name — is never opened. The include that follows contaux.h is
 * the other half of the rule: contaux3.h sits beside this file and nowhere
 * else, so it only resolves if the directory stack unwound.
 *
 * Expect 30 (29 checks + the closer). */
#include "contaux.h"
#include "contau\
x3.h"

#define FLAG 1
#define STR(x) #x

/* a function-like macro whose parameter list is split across a splice */
#define ADD2(a, \
           b) ((a)+(b))
/* an object body spanning three physical lines */
#define SUM3 1 + \
2 + \
3
#define TAIL 10 \
+ 1
#define CAP \
8
/* a statement macro: every line of the body continues onto the next */
#define STEP(a,b) do { \
  a=a+1; \
  b=b+1; \
} while (0)
/* the continued line is a DIRECTIVE, and joining happens first, so it becomes
 * the tail of this body instead of running: INSIDE2 is never defined */
#define EATS \
#define INSIDE2 1
#if defined(INSIDE2)
#define ATE 100
#else
#define ATE 101
#endif
/* a condition split at its end, and one split mid-expression */
#if SUM3 == \
6
#define CHN 11
#else
#define CHN 13
#endif
#if 1 && \
0
#define CHN2 21
#else
#define CHN2 22
#endif
/* an #ifdef name and a #define name, each joined by a splice */
#ifdef FL\
AG
#define IFNAME 8
#else
#define IFNAME 9
#endif
#define JOINED_NA\
ME 5
/* a `##` right operand arriving on the continued line: PAST(7) */
#define PAST(x) x ## \
1
/* a body whose continuation lands on an EMPTY line: the directive took two
 * physical lines and its body is still just `6` */
#define EMPTYSIX 6 \

#define GOT STR(1 \
2)

int joined1\
2[4];

char slicetxt[12]="split\
across";

char onech='a\
';

int addup(int a,int b)
{
  return a+b;
}

int main()
{
  int tot,r,x,y,z,q,n,i,j;
  int decla\
red=5;
  int vec[3]={1, \
2, 3};
  char buf[CAP];

  tot=0;

  tot=tot+(CAUX==7);    /* 1 a plain name found the header beside this file */
  tot=tot+(LEAF==6);  /* 2 a header included its own sibling, not the decoy */
  tot=tot+(CAUX3==4);  /* 3 the directory stack unwound before this include */

  r=0;
  r=ADD2(9, \
10);
  tot=tot+(r==19);      /* 4 a macro call's argument list split by a splice */
  x=ADD2(1 \
+ 2, 3);
  tot=tot+(x==6);             /* 5 one argument of a call split by a splice */
  x=addup(40, \
1);
  tot=tot+(x==41);           /* 6 a real call's arguments split by a splice */

  tot=tot+(SUM3==6);      /* 7 an object body spanning three physical lines */
  tot=tot+(TAIL==11);       /* 8 an object body spanning two physical lines */
  tot=tot+(EMPTYSIX==6);      /* 9 a directive continued onto an EMPTY line */
  tot=tot+(sizeof(buf)/sizeof(char)==8);   /* 10 a continued body sized buf */

  r=0;
  q=0;
  STEP(r,q);
  tot=tot+((r==1)&&(q==1));                                           /* 11 */

  tot=tot+(ATE==101);         /* 12 a body swallowed the directive below it */
  tot=tot+(CHN==11);                /* 13 an #if condition split at its end */
  tot=tot+(CHN2==22);           /* 14 an #if condition split mid-expression */
  tot=tot+(IFNAME==8);        /* 15 an #ifdef name joined across the splice */
  tot=tot+(JOINED_NAME==5);   /* 16 a #define name joined across the splice */
  tot=tot+(PAST(7)==71);        /* 17 a ## right operand from the next line */
  tot=tot+(strcmp(GOT,"1 2")==0); /* 18 space before the backslash survives */

  tot=tot+(strcmp(slicetxt,"splitacross")==0);                        /* 19 */
  tot=tot+(sizeof(slicetxt)/sizeof(char)==12);   /* 20 size proves the join */
  tot=tot+(onech==97);                    /* 21 a character constant joined */

  joined12[0]=3;
  joined12[1]=4;
  tot=tot+(joined12[0]+joined12[1]==7); /* 22 joined identifier, file scope */
  tot=tot+(declared==5);              /* 23 an identifier joined in a block */
  tot=tot+(vec[0]+vec[1]+vec[2]==6);        /* 24 an initializer list split */

  i=0;
  j=0;
  for (i=0;i!= \
3;i++)
    j=j+1;
  tot=tot+(j==3);                      /* 25 a for header split by a splice */

  y=1;
  // this comment claims the line after it \
  y=100;
  tot=tot+(y==1);                  /* 26 a // comment claimed the next line */

  z=2;
  /* the terminator arrives from the line below \
  z=200; */
  z=z+1;
  tot=tot+(z==3);             /* 27 a block comment took its end from below */

  n=0;
  n=12\  
34;
  tot=tot+(n==1234);         /* 28 the space after the backslash is deleted */

  z=0;
#if 0
  z=1 \
100;
#endif
  z=z+1;
  tot=tot+(z==1);          /* 29 a splice inside a skipped region stays put */

  return tot+1;
}
