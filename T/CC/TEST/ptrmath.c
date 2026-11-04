/* M29 pointer arithmetic. `p+n` scales by the element step and `n+p` is the
 * same sum; `p-q` is an ELEMENT COUNT (not a byte count, and not a pointer);
 * a decayed struct array steps whole tags, so `st+1 == &st[1]` comes out
 * true the way C reports it; a 2-D array steps whole ROWS; a struct pointer
 * steps a tag. Byte pointers were already right (their step is 1), which is
 * why the byte-count bug hid: strlen-shaped code worked and int-shaped code
 * did not. `p+q`, `n-p`, `p+=q` and arithmetic on a struct OBJECT are C
 * constraint violations and diagnose. Expect 32. */
struct P { int x; char c; };
int a[5];
int m[2][3];
struct P st[4];
char w[8];
int f(int x)
{
  return x+1;
}
int g2(int x)
{
  return x+2;
}
int walkcount(int t[],int n)
{
  int *p;
  int c;
  c=0;
  for(p=t;p!=t+n;++p)
    c=c+1;
  return c;
}
int slen(char *s)
{
  char *q;
  q=s;
  while(*q)
    q++;
  return (int)(q-s);
}
int main()
{
  int *p;
  int *q;
  char *cp;
  struct P *sp;
  struct P *sq;
  int (*fp[2])(int);
  int (*fq[2])(int);
  int i;
  int tot;
  tot=0;
  a[0]=11;
  a[1]=22;
  a[2]=33;
  a[3]=44;
  a[4]=55;
  m[0][0]=1;
  m[0][1]=2;
  m[0][2]=3;
  m[1][0]=4;
  m[1][1]=5;
  m[1][2]=6;
  st[0].x=1;
  st[1].x=2;
  st[2].x=3;
  st[3].x=4;
  w[0]='h';
  w[1]='e';
  w[2]='l';
  w[3]='l';
  w[4]='o';
  w[5]=0;
  p=a+2;
  tot=tot+(p[0]==33);                           /*  1 array + int scales a slot */
  p=2+a;
  tot=tot+(p[0]==33);                           /*  2 int + array: SAME sum */
  p=a+1;
  q=a+4;
  tot=tot+((q-p)==3);                           /*  3 the difference is a COUNT */
  tot=tot+((p-q)==-3);                          /*  4 and negative counts work */
  p=a;
  tot=tot+((a+3-p)==3);                         /*  5 array minus pointer */
  tot=tot+((p-p)==0);                           /*  6 zero difference */
  i=(int)((a+4)-a);
  tot=tot+(a[i]==55);                           /*  7 a count indexes correctly */
  p=a+3;
  tot=tot+(((p-a)*2)==6);                       /*  8 a count is an int: no second */
  p=a;                                          /*     scaling by the element size */
  p+=2;
  tot=tot+(p[0]==33);                           /*  9 p+=n scales */
  p-=1;
  tot=tot+(p[0]==22);                           /* 10 p-=n scales back */
  q=p+3;
  tot=tot+((q-p)==3);                           /* 11 pointer from a compound */
  cp=w+2;
  tot=tot+((cp-w)==2);                          /* 12 byte pointers: bytes ARE */
  tot=tot+(*cp=='l');                           /* 13 elements for a char step */
  cp=w;
  cp+=4;
  tot=tot+(*cp=='o');                           /* 14 compound on a byte pointer */
  tot=tot+(slen(w)==5);                         /* 15 the idiom that hid the bug */
  sp=st+2;
  tot=tot+(sp->x==3);                           /* 16 struct array steps a WHOLE */
  tot=tot+((st+1)==(&st[1]));                   /* 17 st+1 and &st[1] AGREE (C) */
  tot=tot+((st+3-st)==3);                       /* 18 a struct-array difference */
  sp=st;                                        /*     two tags apart */
  sq=st+2;
  tot=tot+((sq-sp)==2);                         /* 19 counts tags, not bytes */
  sp=&st[1];
  tot=tot+((sp-st)==1);                         /* 20 address-of minus array */
  sp=st+1;
  sp++;
  tot=tot+(sp->x==3);                           /* 21 ++ steps a WHOLE TAG, as C */
  sp=st+2;                                      /*     does for `struct P *sp` */
  sp+=1;
  tot=tot+(sp->x==4);                           /* 22 += steps a tag */
  sp=st+3;
  sp-=2;
  tot=tot+(sp->x==2);                           /* 23 -= steps back a tag */
  p=m+1;
  tot=tot+(p[0]==4);                            /* 24 a 2-D array steps a ROW, so */
  tot=tot+((m+1)==(&m[1]));                     /* 25 m+1 and &m[1] are the same */
  tot=tot+(((m+1)[0][0])==4);                   /* 26 and (m+1)[0][0] indexes it */
  tot=tot+(((m+1)-m)==1);                       /* 27 and the difference counts 1 */
  tot=tot+(walkcount(a,4)==4);                  /* 28 p!=t+n loop, ++ scaling */
  i=0;
  for(p=a;p!=a+5;++p)
    i=i+1;
  tot=tot+(i==5);                               /* 29 five slots, five steps */
  fp[0]=f;
  fq[0]=f;
  tot=tot+((fp[0]==fq[0])==1);                  /* 30 same function, same address */
  fp[1]=g2;                                     /*     (the residue regression) */
  tot=tot+((fp[0]!=fq[0])==0);                  /* 31 != stays 0 for equal ones */
  tot=tot+((fp[1]==fq[0])==0);                  /* 32 and 1 for different ones */
  return tot+1;
}
