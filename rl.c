#include <assert.h>
#include "rl.h"
#if _WIN64
#include <windows.h>
#else
#endif
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "gc.h"
#include "alloc.h"
#include <readline.h>
char* rl(char* prompt) {
  char *fn=readline(prompt);
  char *r=strdup(fn);
  free(fn);
  if(strlen(r)) add_history(r);
  return r;
}
CCompletion* (*rlACGen)(const char *buffer,long srcoff,const char* text,long *length);
static char *rlEntry(const char *text,int iter) {
  long cnt,ocnt;;
  CCompletion *comps=rlACGen(text,0,text,&cnt);
  ocnt=cnt;
  char *ret=NULL;
  while(--cnt>=0) {
    if(cnt==iter) return ret=strcpy(calloc(1,strlen(comps[cnt].completion)+1),comps[cnt].completion);
    TD_FREE(comps[cnt].displayText);
    TD_FREE(comps[cnt].completion);
  }
  TD_FREE(comps);
  return ret;
}
void InitRL() {
  #ifndef TARGET_WIN32
  rl_catch_signals=0;
  rl_completion_append_character='\x00';
  rl_initialize();
  #endif
  rl_completion_entry_function=&rlEntry;
}
