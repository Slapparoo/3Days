/* M13 preprocessor-lite smoke test: guards, chained includes, object
 * macros (incl. expansion inside array sizes and macro bodies),
 * literal opacity, #ifdef/#else, #if 0 skipping garbage vs #if 1,
 * #undef/redefine, #pragma. Each tot=tot+ line is one numbered check;
 * main returns that count plus the always-one closer. Expect 12. */
#include "T:/CC/TEST/pp1.h"
#include "T:/CC/TEST/pp1.h"
#include "T:/CC/TEST/pp2.h"
#define CAP 60
#define SZ 4
#define NAME "cap"
int main()
{
  char buf[SZ];
  char namebuf[8];
  int tot;
  int x;
  tot=0;
  tot=tot+(helper1()==7);                          /* 1 fn defined in header */
  tot=tot+(SEVEN==7);                              /* 2 */
  tot=tot+(TWELVE==12);                            /* 3 body rescan */
  tot=tot+(strcmp("SEVEN","SEVEN")==0);            /* 4 literals are opaque */
  tot=tot+(helper1()+SEVEN==14);                   /* 5 */
  x=0;
#ifdef UNDEFINED_THING
  x=100;
#else
  x=2;
#endif
  tot=tot+(x==2);                                  /* 6 #else arm */
#if 0
  int broken_syntax(@@!!)
#else
#define NINE 9
#endif
  tot=tot+(NINE==9);                               /* 7 skipped garbage, #else defines */
#if 1
  tot=tot+(CAP==60);                               /* 8 #if 1 arm emits */
#endif
#define EIGHT 8
#undef EIGHT
#define EIGHT 3
  tot=tot+(EIGHT==3);                              /* 9 #undef then redefine */
  strcpy(namebuf,NAME);
  tot=tot+(strcmp(namebuf,"cap")==0);              /* 10 macro -> string literal */
  memset(buf,'a',SZ);
  tot=tot+((buf[0]=='a')&&((buf[3]=='a')&&(buf[SZ-1]=='a')));
                                                   /* 11 macro as array size */
#pragma pin_no_thing
  puts("pp ok");
  tot=tot+1;
  return tot;
}
