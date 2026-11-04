/* M34: C's file-scope redeclaration rules — a name may be declared many times
 * and still be ONE object with ONE address and ONE type, and only ONE of those
 * declarations may be a definition with an initializer.
 *
 * A declaration WITHOUT `extern` is a TENTATIVE definition: C completes it with
 * a zero initializer, which is exactly what the zeroed arena already is, so
 * `int g;` twice is one zeroed object, not two. A declaration WITH `extern` is
 * not a definition at all: it names an object whose storage lives elsewhere, and
 * here "elsewhere" can only be a later declaration in this file, so
 * `extern int g;` then `int g;` — and `int g;` then `extern int g;` — both bind
 * the same slot. That is what the identity checks below are for: a function
 * compiled BETWEEN the two declarations bakes the address it was given at the
 * time it was compiled, and `p()==&g` says the definition did not move it.
 * `sizeof` of the tag object says the extern declaration already knew the tag.
 *
 * The length of an array is the one thing a redeclaration may still ADD:
 * `extern int a[];` states no length, so the object is incomplete until a
 * definition says how long it is — by brackets, or by an initializer — and only
 * then can it be measured. A definition may not restate a DIFFERENT length, and
 * two lengths that agree cost one allocation, not two.
 *
 * What still refuses, all of it C's own rule rather than this compiler's
 * invention: an initializer on a second definition (`redefinition`), a
 * redeclaration that changes the type, a scalar redeclared as an array or an
 * array as a scalar, and an object declared `extern` that nothing in this file
 * ever defines — reported only when something actually LOOKED at it, which is
 * what a linker does. Expect 25 (24 checks + the closer). */
struct PT { int x; int y; };

/* extern first, definition later, and a reader and a writer compiled in
 * between: the address they baked has to be the one the definition keeps. */
extern int ga;
int *pa(void)
{
  return &ga;
}
int wb(void)
{
  ga=19;
  return 0;
}
int ga;

/* definition first, declaration after it — and the value it started with
 * survives the declaration, because there is only ever one object. */
int gb=7;
extern int gb;
int *pb(void)
{
  return &gb;
}
int wc(void)
{
  gb=gb+3;
  return 0;
}

/* two tentative definitions. */
int gc;
int *pc(void)
{
  return &gc;
}
int gc;

/* a tentative definition, then a definition WITH an initializer, with a reader
 * and a writer compiled in between: the initializer lands in the same slot. */
int gd;
int wd(void)
{
  gd=gd+1;
  return 0;
}
int *pd(void)
{
  return &gd;
}
int gd=20;

/* the other way round: an initialized definition, then a tentative one, which
 * must neither clear it nor replace it. */
int ge=5;
int *pe(void)
{
  return &ge;
}
int ge;

/* an incomplete array declared `extern`, read and measured by functions
 * compiled before its length is known, then completed by an initializer that
 * supplies that length. */
extern int gf[];
int *pf(void)
{
  return gf;
}
int wf(void)
{
  return gf[0]+gf[1]+gf[2];
}
int gf[]={1,2,3};

/* the same with a byte array completed by a sized definition. */
extern char gg[];
char gg[4]="ab";

/* a tag object: the `extern` declaration has to know the tag already (or the
 * member writes below would not resolve), and the tentative after it may not
 * claim a second object. */
extern struct PT gn;
int wgn(void)
{
  gn.y=41;
  return 0;
}
struct PT gn;

/* a tag object declared tentatively twice, with a writer in between. */
struct PT gh;
int wh(void)
{
  gh.x=17;
  return 0;
}
struct PT gh;

/* a function pointer: tentative first, defined by an initializer, and a CALL
 * compiled in between both. */
int (*gi)(int);
int wi(int v)
{
  return gi(v);
}
int five(int v)
{
  return v+7;
}
int (*gi)(int)=five;

/* file-scope `static`, declared twice. */
static int gj;
int wj(void)
{
  gj=33;
  return 0;
}
static int gj;

/* two array declarations that both state the same length: one allocation. */
int gk[3];
int wk(void)
{
  gk[1]=9;
  return 0;
}
int gk[3];

/* ONE statement that declares a new name AND completes an `extern` one. */
extern int gl;
int gm;
int gl=3, gm=4;

int main()
{
  int tot;

  tot=0;

  tot=tot+(pa()==&ga);                       /* 1 address survives the def */
  wb();
  tot=tot+(ga==19);                          /* 2 written through it */
  tot=tot+(pb()==&gb);                       /* 3 def then extern */
  wc();
  wc();
  tot=tot+(gb==13);                          /* 4 the init is the start value */
  tot=tot+(pc()==&gc);                       /* 5 two tentatives, one slot */
  tot=tot+(gc==0);                           /* 6 and it starts zeroed */
  tot=tot+(pd()==&gd);                       /* 7 tentative then def */
  wd();
  tot=tot+(gd==21);                          /* 8 init then increment */
  tot=tot+(pe()==&ge);                       /* 9 def-with-init then tentative */
  tot=tot+(ge==5);                           /* 10 the tentative kept 5 */
  tot=tot+(pf()==&gf[0]);                    /* 11 length arrives later, no move */
  tot=tot+(wf()==6);                         /* 12 read while the length was open */
  tot=tot+(sizeof(gf)/sizeof(int)==3);       /* 13 and measurable once it is not */
  tot=tot+(gg[1]==98);                       /* 14 char array completed by a def */
  tot=tot+((gg[2]==0)&&(gg[3]==0));          /* 15 its tail is still zeros */
  tot=tot+(sizeof(gg)/sizeof(char)==4);      /* 16 the length it stated */
  wgn();
  tot=tot+(gn.y==41);                        /* 17 extern tag, then tentative */
  tot=tot+(sizeof(gn)==sizeof(struct PT));   /* 18 and the extern knew the tag */
  wh();
  tot=tot+(gh.x==17);                        /* 19 tag object declared twice */
  tot=tot+(wi(4)==11);                       /* 20 fp tentative then =init */
  wj();
  tot=tot+(gj==33);                          /* 21 file-scope static twice */
  wk();
  tot=tot+(gk[1]==9);                        /* 22 two matching array lengths */
  tot=tot+(sizeof(gk)/sizeof(int)==3);       /* 23 one allocation for them */
  tot=tot+((gl==3)&&(gm==4));                /* 24 two declarators, one new name */

  return tot+1;
}
