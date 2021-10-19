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
static char **rlAttempted(const char *text,int start,int end) {
  if(!rlACGen) return NULL;
  char buffer[end-start+1];
  strncpy(buffer,text+start,end-start);
  buffer[end-start]=0;
  long cnt;
  CCompletion *comps=rlACGen(text,start,buffer,&cnt);
  char **ret=calloc(sizeof(char*),cnt+1);
  while(--cnt>=0) {
    ret[cnt]=strcpy(calloc(strlen(comps[cnt].completion)+1,1),comps[cnt].completion);
    TD_FREE(comps[cnt].displayText);
    TD_FREE(comps[cnt].completion);
  }
  TD_FREE(comps);
  return ret;
}
void InitRL() {
  //initialize_readline ();
  rl_attempted_completion_function = &rlAttempted;
}
