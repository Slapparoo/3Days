/* M30 initializer designators. `[0]=x` and `.a=x` say WHICH part of an
 * aggregate a value belongs to: index designators at one and two levels,
 * member designators, chains that cross from an element into its members,
 * designated and sequential items mixed in either order, size deduction from
 * the highest designated index, designated ROWS of a 2-D char array taking a
 * string literal, designated elements of pointer and function-pointer arrays,
 * a designated `static` block object, and the brace-elision cursor C uses when
 * an enclosure opens or closes partway through an element — the rule M30 had
 * to fix so that a designation moves the SAME cursor a group does. Every
 * check is C-true; this fixture needs no model adjustment.
 * Expect 43 (42 checks + closer). */
enum { SLOTL=2, SLOTR=3 };
struct P { int a; char b; int c; };
union U { int i; char ch; };
struct W { union U v; int n; };
struct L { char *p; int k; };
int inc(int x)
{
  return x+1;
}
int dbl(int x)
{
  return x*2;
}
int gv[8] = { [0]=1, [3]=5 };
int gd[] = { [3]=7 };
int gm[2][3] = { [0][1]=5, [1]={1,2} };
char gr[2][3] = { "ab", "cd" };
char gr2[2][3] = { [1]="hi" };
char gw[6] = { [1]='b', [4]='e' };
char *gt[4] = { [2]="two", [0]="zero" };
int (*fpt[3])(int) = { [2]=dbl, [0]=inc };
struct L gl = { .k=7, .p="seven" };
struct P gp[2] = { [1].a=1, [0]={.a=9,.c=2} };
struct P gp2[] = { [2].a=3 };
struct W gwv = { .v.i=5 };
union U gu = { .ch='x' };
int tick30()
{
  static int sc[4] = { [2]=1 };

  sc[2]=sc[2]+1;
  return sc[2];
}
int main()
{
  union U u2;
  int tot;
  tot=0;
  tot=tot+((gv[0]==1)&&(gv[3]==5));               /*  1 first and last slots */
  tot=tot+((gv[1]==0)&&(gv[6]==0));               /*  2 untouched slots zero */
  tot=tot+((gd[3]==7)&&(gd[0]==0));               /*  3 deduced by index     */
  tot=tot+(((int)(sizeof(gd)/sizeof(int)))==4);   /*  4 size = highest + 1   */
  tot=tot+(gm[0][1]==5);                          /*  5 [i][j] picks inside  */
  tot=tot+((gm[0][0]==0)&&(gm[0][2]==0));         /*  6 the row's other slots*/
  tot=tot+((gm[1][0]==1)&&(gm[1][1]==2));         /*  7 a designated group   */
  tot=tot+(gm[1][2]==0);                          /*  8 fills from its start */
  tot=tot+((gr[0][0]=='a')&&(gr[1][0]=='c'));     /*  9 a literal IS one row */
  tot=tot+((gr[0][2]==0)&&(gr[1][2]==0));         /* 10 the NUL fits, rows   */
  tot=tot+((gr2[1][0]=='h')&&(gr2[1][2]==0));     /* 11 designated literal   */
  tot=tot+(gr2[0][0]==0);                         /* 12 and the other row    */
  tot=tot+((gw[1]=='b')&&(gw[4]=='e'));            /* 13 byte elements too    */
  tot=tot+((gw[0]==0)&&(gw[2]==0));               /* 14 gaps stay zero       */
  tot=tot+((gt[2][0]=='t')&&(gt[0][0]=='z'));     /* 15 pointer elements     */
  tot=tot+((gt[1]==0)&&(gt[3]==0));               /* 16 holes are NULL       */
  tot=tot+((fpt[2](5)==10)&&(fpt[0](1)==2));      /* 17 function pointers    */
  tot=tot+(fpt[1]==0);                            /* 18 a hole in the table  */
  fpt[1]=inc;                                     /* 19 and a designated     */
  tot=tot+(fpt[1](4)==5);                         /* 20 table is retargetable */
  tot=tot+((gl.k==7)&&(gl.p[0]=='s'));            /* 21 members, one a ptr   */
  tot=tot+((gp[1].a==1)&&(gp[0].a==9));           /* 22 [i].m and [i]={...}  */
  tot=tot+((gp[0].c==2)&&(gp[1].b==0));           /* 23 others stay zero     */
  tot=tot+((gp2[2].a==3)&&(gp2[0].a==0));         /* 24 tag array deduced    */
  tot=tot+(((int)(sizeof(gp2)/sizeof(struct P)))==3);	/* 25 by designator */
  tot=tot+((gwv.v.i==5)&&(gwv.n==0));             /* 26 .v.i descends a tag  */
  tot=tot+((gu.i & 0xFF)==120);                   /* 27 .ch IS the union slot */
  u2.i=5;
  tot=tot+((u2.ch & 0xFF)==5);                    /* 28 and .i its low byte  */
  tot=tot+((tick30()==2)&&(tick30()==3));         /* 29 designated static    */
  { int r[3] = { [0] 9 };                         /* 30 the '=' is OPTIONAL  */
    tot=tot+((r[0]==9)&&(r[1]==0)); }
  { int r[5] = { [2]=1, 7 };                      /* 31 a flat value follows */
    tot=tot+((r[2]==1)&&(r[3]==7)); }		/*    the designated one   */
  { int r[5] = { [3]=1, [0]=2 };                  /* 32 designators can run  */
    tot=tot+((r[0]==2)&&(r[3]==1)&&(r[1]==0)); }	/*    backwards            */
  { int r[2][3] = { [1][2]=9, [0][0]=4 };         /* 33 in either order      */
    tot=tot+((r[1][2]==9)&&(r[0][0]==4)&&(r[0][1]==0)); }
  { int r[2][2] = { {[1]=3}, {[0]=4} };           /* 34 and inside groups    */
    tot=tot+((r[0][1]==3)&&(r[1][0]==4)); }
  { int r[2][3] = { 1, {5} };                     /* 35 an enclosure partway */
    tot=tot+((r[0][1]==5)&&(r[1][0]==0)); }	/*    through an element     */
  { int r[2][3] = { {5}, 9 };                     /* 36 and a flat value     */
    tot=tot+((r[1][0]==9)&&(r[0][1]==0)); }	/*    resumes AFTER the row  */
  { char r[2][4] = { "ab", [1]="cd" };            /* 37 literal, then a      */
    tot=tot+((r[0][0]=='a')&&(r[1][0]=='c')); }	/*    designated literal   */
  { struct P w = { .c=7 };                        /* 38 partial members      */
    tot=tot+((w.a==0)&&(w.c==7)); }
  { struct P w = { .b='k', .a=3 };                /* 39 members out of order */
    tot=tot+((w.a==3)&&(w.b=='k')); }
  { struct P w = { .a=1, 5 };                     /* 40 a flat value moves on */
    tot=tot+((w.b==5)&&(w.c==0)); }		/*    to the NEXT member   */
  { struct P w = { {1}, 5 };                      /* 41 a group takes ONE    */
    tot=tot+((w.a==1)&&(w.b==5)); }		/*    member of a scalar   */
  { struct P w[2] = { {1}, 5 };                   /* 42 but a WHOLE element  */
    tot=tot+((w[1].a==5)&&(w[0].b==0)); }	/*    of an array of tags  */
  { int w[5] = { [SLOTL]=7, [SLOTR]=8 };         /* 43 a designator index is */
    tot=tot+((w[2]==7)&&(w[3]==8)&&(w[4]==0)); }	/*    a CONSTANT EXPRESSION  */
  return tot+1;
}
