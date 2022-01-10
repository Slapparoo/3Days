#include <assert.h>
#include "rl.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "alloc.h"
#include "3d.h"
#include "ext/linenoise/linenoise.h"
#ifdef TARGET_WIN32
#else
#include <dirent.h>
#endif
CCompletion* (*rlACGen)(const char *buffer,long srcoff,const char* text,long *length);
static int IsEscaped(const char *start,const char *at) {
  if(at-1>=start)
    if(at[-1]=='\\')
      if(!IsEscaped(start, at-1))
        return 1;
  return 0;
}
static void completion(const char *buf,linenoiseCompletions *lc) {
    long count,count2;
    char buffer[1024];
    int instr=0;
    const char *ptr=buf;
    sloop:;
    if(strchr(ptr, '\"')) {
      instr=1;
      ptr++;
      sloop2:;
      ptr=strchr(ptr, '\"');
      if(ptr)
        if(IsEscaped(buf,ptr)) {
          ptr++;
          goto sloop2;
        }
      if(ptr) {
        ptr++;
        instr=0;
        goto sloop;
      }
    }
    if(rlACGen&&!instr) {
        count=strlen(buf);
        while(--count>=0) {
          if(isalnum(buf[count])||buf[count]=='_')
            continue;
          break;
        }
       CCompletion *comps=rlACGen(buf,count,buf+count+1,&count2);
       while(--count2>=0) {
            strncpy(buffer, buf,count+1);
            buffer[count+1]=0;
            strcat(buffer,comps[count2].completion);
            linenoiseAddCompletion(lc,buffer);
            TD_FREE(comps[count2].displayText);
            TD_FREE(comps[count2].completion);
        }
        TD_FREE(comps);
    }
    #ifndef TARGET_WIN32
    DIR *dir=opendir(".");
    vec_char_t path;
    vec_init(&path);
    vec_pusharr(&path,"",1);
    if(ptr=strrchr(buf, '\"')) {
        ptr++;
        count=ptr-buf;
        struct dirent *de;
        floop:
        while(de=readdir(dir)) {
          long min=(strlen(de->d_name)>strlen(ptr))?strlen(ptr):strlen(de->d_name);
          if(!strncmp(de->d_name,ptr,min)) {
              if(strlen(de->d_name)==min)
                if(ptr[min]=='/') {
                  printf("%s\n", ptr);
                  ptr+=min+1;
                  vec_pop(&path);
                  vec_pusharr(&path,de->d_name,strlen(de->d_name));
                  vec_pusharr(&path,"/",2);
                  closedir(dir);
                  dir=opendir(path.data);
                  goto floop;
                }
              strncpy(buffer, buf,count);
              buffer[count]=0;
              sprintf(buffer+strlen(buffer), "%s%s", path.data,de->d_name);
              linenoiseAddCompletion(lc,buffer);
          }
        }
    }
    vec_deinit(&path);
    closedir(dir);
    #endif
}
char* rl(char* prompt) {
  char *fn=linenoise(prompt);
  linenoiseHistoryAdd(fn);
  char *r=strdup(fn);
  linenoiseFree(fn);
  return r;
}
void InitRL() {
    linenoiseSetCompletionCallback(completion);
}
