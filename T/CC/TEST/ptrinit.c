/* M24 pointer-valued initializers + arena relocations: string literals,
 * DEFINED function names and &global as initializer values, in globals,
 * `static` block objects, structs, and frame arrays; pointer slots in the
 * ARENA now carry relocations, so a saved object re-derives them instead
 * of pointing at the arena the module was compiled against. Expect 24. */
enum { BLUE=4, SIX=6 };
int inc(int x)
{
  return x+1;
}
int dbl(int x)
{
  return x*2;
}
int gv;
int gi=BLUE+SIX;	/* 10: folded enum arithmetic */
char wbuf[4]="abc";
char *wmut=&wbuf;
char *gtab[3]={"ab","cd","ef"};
int *gptr=&gv;
int zed;
int *zap[2]={0,&zed};
int (*gfp)(int)=inc;
int (*gfp2)(int)=0;
struct L { int *p; int n; };
struct L gl={&gi,BLUE+1};	/* n = 5 */
int counter4()
{
  static int (*sf)(int)=dbl;
  static int sc=BLUE;
  sc=sc+1;
  return sf(sc);
}
int callit(int (*f)(int),int x)
{
  return f(x);
}
int lensum(char *tab[],int n)
{
  int i;
  int t;
  t=0;
  for(i=0;i!=n;i++)
    t=t+strlen(tab[i]);
  return t;
}
int main()
{
  char *ltab[2]={"mn","op"};
  int *lip[2]={&gv,&zed};
  struct L ll={&zed,9};
  int tot;
  int i;
  tot=0;
  gv=55;
  zed=77;
  tot=tot+(wmut[0]=='a');                       /*  1 &array slot address */
  tot=tot+(strlen(wmut)==3);                    /*  2 */
  tot=tot+((gtab[0]!=0)&&(gtab[2]!=0));         /*  3 pointer table elements */
  tot=tot+((strlen(gtab[0])==2)&&(strlen(gtab[2])==2));  /*  4 through the table */
  tot=tot+(lensum(gtab,3)==6);                  /*  5 table to a char*[] param */
  tot=tot+(gptr[0]==55);                        /*  6 &global initializer */
  tot=tot+(zap[0]==0);                          /*  7 NULL element */
  tot=tot+(zap[1][0]==77);                      /*  8 second element live */
  tot=tot+(gfp(20)==21);                        /*  9 function-name initializer */
  tot=tot+(gfp2==0);                            /* 10 NULL fp global */
  tot=tot+(callit(gfp,1)==2);                   /* 11 fp passed around */
  tot=tot+(callit(dbl,3)==6);                   /* 12 name as an argument too */
  tot=tot+(gl.p[0]==10);                        /* 13 struct member &global */
  tot=tot+(gl.n==5);                            /* 14 enum-folded member */
  tot=tot+((ll.p[0]==77)&&(ll.n==9));           /* 15 frame struct member init */
  tot=tot+((strlen(ltab[0])==2)&&(strlen(ltab[1])==2));  /* 16 frame pointer array */
  tot=tot+((lip[0][0]==55)&&(lip[1][0]==77));    /* 17 frame &global array */
  tot=tot+(counter4()==10);                     /* 18 static fp + static int init */
  tot=tot+(counter4()==12);                     /* 19 both persist */
  wmut[0]='Z';
  tot=tot+(wmut[0]=='Z');                       /* 20 writable through the slot */
  gtab[0]="zy";
  tot=tot+(strlen(gtab[0])==2);                 /* 21 table entry retargeted */
  tot=tot+(gptr==&gv);                          /* 22 slot identity holds */
  tot=tot+(zap[1]==&zed);                       /* 23 */
  i=0;
  for(i=0;i!=3;i++)
    if (gtab[i]==0)
      tot=tot+100;
  tot=tot+(i==3);                               /* 24 table loop completes */
  return tot+1;
}
