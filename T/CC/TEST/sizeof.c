/* M15 sizeof + casts. C-truth sizes (char=1, everything else the
 * 8-byte quad, struct=byte total), arrays measured via the symbol
 * (sizeof(sbuf)=128 for char sbuf[128]; a pointer stays 8), casts
 * truncate chars, keep pointer tags, and fold void/long to the model.
 * The size_t line doubles as an M13 header+typedef integration check.
 * Each tot=tot+ line is one numbered check; main returns that count
 * plus the always-one closer. Expect 26. */
#include "stddef.h"
struct node { int v; struct node *next; };
int arr[10];
char sbuf[128];
int main()
{
  char *cp;
  int *ip;
  void *vp;
  struct node n1;
  size_t sl;
  int tot;
  tot=0;
  tot=tot+(sizeof(int)==8);                      /*  1 */
  tot=tot+(sizeof(char)==1);                     /*  2 */
  tot=tot+(sizeof(long)==8);                     /*  3 */
  tot=tot+(sizeof(unsigned long)==8);            /*  4 */
  tot=tot+(sizeof(int*)==8);                     /*  5 */
  tot=tot+(sizeof(char*)==8);                    /*  6 */
  tot=tot+(sizeof(sbuf)==128);                   /*  7 bare ident array */
  tot=tot+(sizeof(arr)==80);                     /*  8 */
  tot=tot+(sizeof(struct node)==16);             /*  9 */
  tot=tot+(sizeof(*(&arr[0]))==8);               /* 10 expr: & then deref */
  ip=arr;
  tot=tot+(sizeof(*ip)==8);                      /* 11 deref of int* */
  cp=(char *)ip;
  tot=tot+(sizeof(*cp)==1);                      /* 12 deref of char* */
  cp[0]=65;
  tot=tot+(arr[0]==65);                          /* 13 byte alias via cast */
  tot=tot+((char *)ip==(char *)arr);             /* 14 cast vs cast */
  tot=tot+((char)321==65);                       /* 15 C truncation */
  tot=tot+((char)-1==255);                       /* 16 */
  tot=tot+((unsigned long)7==7);                 /* 17 */
  tot=tot+(sizeof(int)+1==9);                    /* 18 sizeof in expression */
  n1.v=5;
  tot=tot+(sizeof(n1)==16);                      /* 19 struct symbol */
  tot=tot+((int)(long)(char)300==44);            /* 20 chained casts */
  vp=(void *)&n1;
  tot=tot+(((struct node *)vp)->v==5);           /* 21 cast keeps struct tag */
  tot=tot+(sizeof(*(struct node *)vp)==16);      /* 22 deref-of-cast size */
  sl=sizeof(size_t);
  tot=tot+(sl==8);                               /* 23 typedef size from header */
  tot=tot+(sizeof(char *[4])==32);               /* 24 pointer-array type */
  tot=tot+(sizeof(char[64])==64);                /* 25 char array type */
  return tot+1;
}
