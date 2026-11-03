/* M13 adapted-musl headers: the declared surface must PARSE against
 * the compiler (typedefs, const/void/unsigned long, prototypes over
 * builtins and prelude functions) and calls must still dispatch
 * correctly. FILE is an ignored stream token. Expect 16. */
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
char *msg="Hello World";
void doit(void)
{
  return;
}
int main()
{
  char buf[64];
  char *p;
  void *vp;
  size_t i;
  size_t ln;
  int tot;
  tot=0;
  doit();
  ln=strlen(msg);
  tot=tot+(ln==11);                                /*  1 size_t via typedef */
  p=strcpy(buf,msg);
  tot=tot+(strcmp(buf,"Hello World")==0);          /*  2 */
  tot=tot+(p==buf);                                /*  3 */
  p=strchr(buf,'W');
  tot=tot+(p==buf+6);                              /*  4 */
  vp=memcpy(buf+40,msg,11);
  tot=tot+(strcmp(buf+40,"Hello World")==0);       /*  5 */
  tot=tot+(memcmp(buf+40,msg,11)==0);              /*  6 void* params from header */
  i=0;
  while (i<3)
  {
    buf[i]='q';
    i=i+1;
  }
  tot=tot+((buf[0]=='q')&&(buf[2]=='q'));          /*  7 */
  tot=tot+(atoi("88")==88);                        /*  8 */
  tot=tot+(abs(-4)==4);                            /*  9 */
  p=calloc(3,3);
  tot=tot+(p!=NULL);                               /* 10 NULL macro */
  tot=tot+(EXIT_SUCCESS==0);                       /* 11 macro constant */
  sprintf(buf,"%d-%s",42,"z");
  tot=tot+(strcmp(buf,"42-z")==0);                 /* 12 */
  tot=tot+(puts("h ok")==0);                       /* 13 */
  tot=tot+(fputs("fp",1)==0);                      /* 14 */
  tot=tot+(fputc('Z',1)==90);                      /* 15 */
  puts("!fin");
  tot=tot+1;
  return tot;
}
