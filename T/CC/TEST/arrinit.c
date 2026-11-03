/* M22 aggregate initializers: brace lists (flat and nested), size
 * deduction for `int a[]` / `int m[][3]` / `char s[]`, string-literal
 * byte init with C's "NUL only if it fits" rule, zero fill of the tail
 * (mandatory for frame objects), globals and `static` block arrays,
 * braced scalars, enum-constant elements. Expect 26. */
enum { TWO=2, SIX=6 };
int gt[3]={10,20,30};
int gp[]={1,2,3,4};
int gz[4]={};
int garr[TWO*2];
int gq[2][3]={{1,2,3},{4,5,6}};
char gs[]="hi";
char gs2[2]="ab";
int counter2()
{
  static int hist[]={100,200};
  hist[0]=hist[0]+1;
  return hist[0]*1000+hist[1];
}
int main()
{
  int a[4]={1,2,3,4};
  int b[]={5,6,7};
  int c[6]={1,2};
  int zero[3]={};
  int e[]={TWO,SIX,TWO+SIX};
  int mw[2][3]={{1,2,3},{4,5,6}};
  int m[2][3]={1,2,3,4,5,6};
  int n[][2]={1,2,3,4,5,6};
  int x={5};
  char s[]="abc";
  char s2[8]="xy";
  int *ip;
  int tot;
  static int sy={9};
  tot=0;
  tot=tot+((a[0]==1)&&(a[2]==3)&&(a[3]==4));   /*  1 local brace list */
  tot=tot+((b[0]==5)&&(b[2]==7)&&(sizeof(b)==24));  /*  2 deduced size */
  tot=tot+((c[0]==1)&&(c[1]==2)&&(c[2]==0)&&(c[5]==0));  /*  3 tail zeros */
  tot=tot+((zero[0]==0)&&(zero[2]==0));        /*  4 {} is all zeros */
  tot=tot+((e[0]==2)&&(e[1]==6)&&(e[2]==8));   /*  5 enum-folded elements */
  tot=tot+((mw[0][2]==3)&&(mw[1][0]==4)&&(mw[1][2]==6));  /*  6 row groups */
  tot=tot+((m[0][0]==1)&&(m[1][1]==5)&&(m[1][2]==6));  /*  7 2-D flat */
  tot=tot+((n[0][1]==2)&&(n[2][1]==6));        /*  8 int m[][2] deduction */
  tot=tot+(x==5);                              /*  9 braced scalar */
  tot=tot+(sy==9);                             /* 10 braced scalar static */
  tot=tot+((s[0]=='a')&&(s[2]=='c')&&(s[3]==0));  /* 11 char[] from a string */
  tot=tot+(sizeof(s)==4);                      /* 12 length INCLUDING the NUL */
  tot=tot+((s2[0]=='x')&&(s2[1]=='y')&&(s2[2]==0)&&(s2[7]==0));  /* 13 padded */
  tot=tot+((gt[0]==10)&&(gt[2]==30));          /* 14 global brace list */
  tot=tot+((gp[0]==1)&&(gp[3]==4));            /* 15 deduced global */
  tot=tot+((gz[0]==0)&&(gz[3]==0));            /* 16 global {} */
  tot=tot+((garr[0]==0)&&(garr[3]==0));        /* 17 zeroed by default */
  tot=tot+((gq[0][0]==1)&&(gq[1][1]==5)&&(gq[1][2]==6));  /* 18 global row groups */
  tot=tot+((gs[0]=='h')&&(gs[1]=='i')&&(gs[2]==0));  /* 19 global char[] string */
  tot=tot+((gs2[0]=='a')&&(gs2[1]=='b'));      /* 20 no room for the NUL */
  tot=tot+((e[1]+e[2])==14);                   /* 21 elements are values */
  gt[0]=77;
  tot=tot+(gt[0]==77);                         /* 22 arenas stay writable */
  ip=a;
  tot=tot+((ip[1]==2)&&(ip[3]==4));            /* 23 through a pointer */
  s[0]='z';
  tot=tot+((s[0]=='z')&&(s[1]=='b'));          /* 24 byte stores in a frame array */
  tot=tot+((counter2()==101200)&&(counter2()==102200));  /* 25 static array persists */
  tot=tot+(sizeof(a)==32);                     /* 26 slot-sized elements */
  return tot+1;
}
