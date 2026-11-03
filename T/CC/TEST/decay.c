/* M25 element type through array decay: subscripting a decayed array
 * yields the ELEMENT's own type, so `char *tab[i][j]` strides and loads
 * bytes, `struct P *pa[i]->m` resolves (and pa[i][1] steps a whole
 * struct), `int *ap[i][j]` keeps its slot semantics, pointer arithmetic
 * through an element scales by the pointee's size, and 2-D pointer
 * arrays chain three levels. Pre-M25 every pointer element was reported
 * as int: tab[0][0] loaded a dword, pa[0]->a was refused outright, and a
 * store through tab[0][i] wrote eight bytes. Expect 27. */
struct P { int a; char b; int c; };
struct P arr[3]={{1,2,3},{4,5,6},{7,8,9}};
struct P *pa[2];
char *tab[3];
char w1[4]="ab";
char w2[4]="cde";
int gv;
int gv2;
int *ap[2];
char *m2[2][2];
int sum2(char *t[],int n)
{
  int i;
  int j;
  int t2;
  t2=0;
  for(i=0;i!=n;i++)
  {
    j=0;
    while(t[i][j])
    {
      t2=t2+t[i][j];
      j=j+1;
    }
  }
  return t2;
}
int memsum(struct P *q[],int n)
{
  int i;
  int t2;
  t2=0;
  for(i=0;i!=n;i++)
    t2=t2+q[i]->a+q[i]->c;
  return t2;
}
int rowsum(char *r[][2])
{
  return r[0][0][0]+r[1][1][0];
}
int main()
{
  struct P st1={10,11,12};
  struct P st2={20,21,22};
  struct P *lpa[1];
  char *ltab[2];
  int tot;
  tot=0;
  gv=12;
  gv2=34;
  pa[0]=&arr[0];
  pa[1]=&arr[2];
  tab[0]=w1;
  tab[1]=w2;
  tab[2]=0;
  ap[0]=&gv;
  ap[1]=&gv2;
  m2[0][0]=w1;
  m2[0][1]=w2;
  m2[1][0]=w2;
  m2[1][1]=w1;
  ltab[0]="hi";
  ltab[1]="z";
  lpa[0]=&st1;
  tot=tot+(tab[0][0]=='a');                      /*  1 byte at index 0 */
  tot=tot+(tab[0][1]=='b');                      /*  2 byte at index 1 */
  tot=tot+(tab[1][2]=='e');                      /*  3 far element, far byte */
  tot=tot+(tab[2]==0);                          /*  4 NULL element */
  tot=tot+(sizeof(tab[0])==8);                  /*  5 a pointer element */
  tot=tot+(sizeof(tab[0][0])==1);               /*  6 a byte */
  tot=tot+(strlen(tab[1])==3);                  /*  7 single subscript */
  tot=tot+(sum2(tab,2)==('a'+'b'+'c'+'d'+'e')); /*  8 param array walked */
  tot=tot+((pa[0]->a==1)&&(pa[0]->c==3));       /*  9 struct member through */
  tot=tot+((pa[1]->a==7)&&(pa[1]->b==8));       /* 10 second element */
  tot=tot+(((*pa[1]).c==9)&&(pa[1][0].c==9));   /* 11 deref and [0] forms */
  tot=tot+(pa[0][1].a==arr[1].a);               /* 12 [1] steps a whole struct */
  tot=tot+(memsum(pa,2)==(1+3+7+9));            /* 13 struct-pointer param */
  tot=tot+(sizeof(pa[0])==8);                   /* 14 a struct POINTER */
  tot=tot+(ap[0][0]==12);                       /* 15 int element unchanged */
  tot=tot+((ap[0]!=0)&&(ap[1]!=0));             /* 16 pointer values intact */
  ap[0]=ap[0]+1;
  tot=tot+(ap[0][0]==34);                       /* 17 += scales by a slot */
  ap[0]=ap[0]-1;
  tot=tot+(ap[0][0]==12);                       /* 18 and -= walks back */
  tab[0]=tab[0]+1;
  tot=tot+(tab[0][0]=='b');                     /* 19 char* scales by 1 */
  tot=tot+(tab[0][0]==w1[1]);                   /* 20 same byte either way */
  tab[0]=tab[0]-1;
  tot=tot+(tab[0][0]=='a');                     /* 21 walked back */
  tab[1][0]='C';
  tot=tot+((tab[1][0]=='C')&&(w2[0]=='C'));     /* 22 BYTE store through it */
  tot=tot+(w2[1]=='d');                         /* 23 the neighbour survived */
  pa[0]->a=77;
  tot=tot+((pa[0]->a==77)&&(arr[0].a==77));     /* 24 quad store through */
  tot=tot+((m2[0][1][1]=='d')&&(m2[1][0][2]=='e'));  /* 25 2-D, three levels */
  tot=tot+(rowsum(m2)==(w1[0]+w1[0]));          /* 26 param rows of pointers */
  tot=tot+((lpa[0]->a==10)&&(ltab[0][1]=='i')); /* 27 frame arrays too */
  return tot+1;
}
