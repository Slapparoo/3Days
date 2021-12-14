#include <assert.h>
#include "rl.h"
#if _WIN64
#include <windows.h>
#else
#endif
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "alloc.h"
#ifdef BSD
#include <readline/readline.h>
#include <readline/history.h>
#else
#include <readline.h>
#ifndef TARGET_WIN32
#include <history.h>
#endif
#endif
#define ENABLE_VIRTUAL_TERMINAL_PROCESSING  0x4
#define ENABLE_VIRTUAL_TERMINAL_INPUT 0x200

char* rl(char* prompt) {
  char *fn=readline(prompt);
  char *r=strdup(fn);
  free(fn);
  #ifdef TARGET_WIN32
  //Set terminal attributes to our wanted ones as readline chnages the attrs
  DWORD omode, origOmode;
  GetConsoleMode(GetStdHandle(STD_OUTPUT_HANDLE), &omode);
  origOmode = omode;
  omode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
  SetConsoleMode(GetStdHandle(STD_OUTPUT_HANDLE), omode);
  #endif
  if(strlen(r)) add_history(r);
  return r;
}
CCompletion* (*rlACGen)(const char *buffer,long srcoff,const char* text,long *length);
static CCompletion *tmp;
static long tmp_length;
static char *gen(const char *text,int iter) {
  if(iter>=tmp_length) return NULL;
  return strcpy(calloc(1,strlen(tmp[iter].completion)+1),tmp[iter].completion);
}
static char *rlEntry(const char *text,int start,int end) {
  long cnt,ocnt;
  char buffer[end-start+1];
  buffer[end-start]=0;
  strncpy(buffer, text,start);
  tmp=rlACGen(text,0,text,&tmp_length);
  char **ret=rl_completion_matches(buffer,gen);
  while(--tmp_length>=0) {
    TD_FREE(tmp[cnt].displayText);
    TD_FREE(tmp[cnt].completion);
  }
  TD_FREE(tmp);
  return ret;
}
void InitRL() {
  #ifndef TARGET_WIN32
  rl_catch_signals=0;
  rl_completion_append_character='\x00';
  rl_initialize();
  #endif
  rl_attempted_completion_function=&rlEntry;
}
