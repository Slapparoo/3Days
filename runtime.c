#include "3d.h"
#include <math.h>
#include <stdio.h>
#include <stdarg.h>
#include <ctype.h>
#include <string.h>
#include <math.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
extern int64_t HCSetJmp(void *ptr);
extern void HCLongJmp(void *ptr);
#ifndef MACOS
#include <SDL2/SDL.h>
#include <SDL2/SDL_video.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_clipboard.h>
#include <SDL2/SDL_events.h>
#else
#include <SDL.h>
#include <SDL_video.h>
#include <SDL_render.h>
#include <SDL_rect.h>
#include <SDL_clipboard.h>
#include <SDL_events.h>
#endif
#include <stddef.h>
#include <stdalign.h>
#ifdef TARGET_WIN32
#include <windows.h>
#include <fileapi.h>
#include <shlwapi.h>
#include <memoryapi.h>
#else
#include <sys/mman.h>
#endif
typedef struct CType CType;
static int64_t BFFS(int64_t v) {
	if(!v) return -1;
    return __builtin_ffsl(v)-1;
}
static int64_t BCLZ(int64_t v) {
	if(!v) return -1;
    return 63-__builtin_clzl(v);
}
static void *MemNCpy(void *d,void *s,long sz) {
    return memcpy(d,s,sz);
}
static int64_t __Move(char *old,char *new) {
	int ret=0;
	old=VFsFileNameAbs(old);
	new=VFsFileNameAbs(new);
	if(old&&new)
		ret=0==rename(old,new);
	PoopFree(old);
	PoopFree(new);
    return ret;
}
static int64_t STK_StrNew(int64_t *stk) {
    return PoopAllocSetTask(strdup(stk[0]),stk[1]);
}
static int64_t STK_MAllocIdent(int64_t *stk) {
    return PoopAllocSetTask(memcpy(PoopMAlloc(MSize(*stk)),*stk,MSize(*stk)),stk[1]);
}
static int64_t IsDir(char *fn) {
    fn=__VFsFileNameAbs(fn);
    if(!fn) return 0;
    struct stat buf;
    stat(fn, &buf);
    PoopFree(fn);
    return S_ISDIR(buf.st_mode);
}
int64_t FileWrite(char *fn,void *data,int64_t sz) {
    FILE *f=fopen(fn,"wb");
    if(!f) return 0;
    fwrite(data, 1, sz, f);
    fclose(f);
    return 1;
}
void* FileRead(char *fn,int64_t *sz) {
    FILE *f=fopen(fn,"rb");
    if(!f) return NULL;
    fseek(f,0,SEEK_END);
    size_t len=ftell(f);
    fseek(f,0, SEEK_SET);
    len-=ftell(f);
    void *data=TD_MALLOC(len+1);
    fread(data, 1, len, f);
    fclose(f);
    if(sz) *sz=len;
    return data;
}
static char **__Dir(char *fn) {
    fn=__VFsFileNameAbs(fn);
    DIR *dir=opendir(fn);
    if(!dir) {
        PoopFree(fn);
        return NULL;
    }
    struct dirent *ent;
    vec_str_t items;
    vec_init(&items);
    while(ent=readdir(dir))
        vec_push(&items,strdup(ent->d_name));
    vec_push(&items,NULL);
    PoopFree(fn);
    return items.data;
}
static int64_t IsWindows() {
#ifdef TARGET_WIN32
    return 1;
#else
    return 0;
#endif
}
static int64_t IsMac() {
#ifdef MACOS
    return 1;
#else
    return 0;
#endif
}
#ifdef TARGET_WIN32
static void EscapePathCat(char *buffer,char *path,DWORD  buf_sz) {
#else
static void EscapePathCat(char *buffer,char *path,size_t buf_sz) {
#endif
#ifdef TARGET_WIN32
  char spaced[2048];
  strcpy(spaced,path);
  PathQuoteSpaces(spaced);
  strcpy(buffer+strlen(buffer),spaced);
  #else
  //TODO escape for unix paths
  strcpy(buffer+strlen(buffer),path);
  #endif
}
void CreateTagsAndErrorsFiles(char *tags,char *errs,char *root) {
  char buffer[2048];
  buffer[0]=0;
  #ifdef TARGET_WIN32
  #else
  EscapePathCat(buffer,CompilerPath,sizeof(buffer));
  #endif
  if(tags) {
        sprintf(buffer+strlen(buffer)," -t ");
        EscapePathCat(buffer,tags,sizeof(buffer));
  }
  if(errs) {
        sprintf(buffer+strlen(buffer)," -e ");
        EscapePathCat(buffer,errs,sizeof(buffer));
  }
  if(root) {
      sprintf(buffer+strlen(buffer)," ");
      EscapePathCat(buffer,root,sizeof(buffer));
  }
  #ifdef TARGET_WIN32
  //https://www.codeproject.com/Articles/1842/A-newbie-s-elementary-guide-to-spawning-processes
  SHELLEXECUTEINFO ShExecInfo = {0};
  ShExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
  ShExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
  ShExecInfo.hwnd = NULL;
  ShExecInfo.lpVerb = NULL;
  ShExecInfo.lpFile = CompilerPath;
  ShExecInfo.lpParameters = buffer;
  ShExecInfo.lpDirectory = NULL;
  ShExecInfo.nShow = SW_HIDE;
  ShExecInfo.hInstApp = NULL;
  ShellExecuteEx(&ShExecInfo);
  WaitForSingleObject(ShExecInfo.hProcess, INFINITE);
  CloseHandle(ShExecInfo.hProcess);
  #else
  system(buffer);
  #endif
}
static char *hc_SDL_GetWindowTitle(SDL_Window *win) {
    return strdup(SDL_GetWindowTitle(win));
}
static void ForeachFunc(void(*func)(const char *name,void *ptr,long sz)) {
  map_iter_t iter;
  const char *key;
  CSymbol *sym;
  iter=map_iter(&Loader.symbols);
  CHash *h;
  while(key=map_next(&Loader.symbols,&iter)) {
      if(!map_get(&TOSLoader,key)) {
          FFI_CALL_TOS_3(func,key,map_get(&Loader.symbols,key)->value_ptr,HTT_FUN);
      }
  }
  iter=map_iter(&TOSLoader);
  while(key=map_next(&TOSLoader,&iter)) {
    int64_t iter;
    vec_CHash_t *var=map_get(&TOSLoader, key);
    vec_foreach_ptr(var,h,iter) {
        if(h->type==HTT_EXPORT_SYS_SYM)
            FFI_CALL_TOS_3(func,key,h->val,HTT_FUN);
        else
            FFI_CALL_TOS_3(func,key,h->val,h->type);
    }
  }
}
static void STK_RegisterFunctionPtr(vec_char_t *blob,char *name,void *fptr,int64_t arity) {
	int64_t blob_off=blob->length,i;
    #ifndef TARGET_WIN32
	/*
	PUSH RBP
	MOV RBP,RSP
	AND RSP,-0x10
	PUSH RSI
	PUSH RDI
	PUSH R10
	PUSH R11
	LEA RDI,[RBP+8+8]
	 */
	char *atxt="\x55\x48\x89\xE5\x48\x83\xE4\xF0\x56\x57\x41\x52\x41\x53\x48\x8D\x7D\x10";
	vec_pusharr(blob,atxt,0x12);
	//MOV RAX,fptr
	atxt="\x48\xb8";
	vec_pusharr(blob,atxt,0x2);
	for(i=0;i!=8;i++)
		vec_push(blob,(((int64_t)fptr)>>(i*8))&0xff);
	/*
	CALL RAX
	POP R11
	POP R10
	POP RDI
	POP RSI
	LEAVE
	*/
	atxt="\xFF\xD0\x41\x5B\x41\x5A\x5F\x5E\xC9";
	vec_pusharr(blob,atxt,0x9);
    #else
    /*
    PUSH RBP
    MOV RBP,RSP
    AND RSP,-0x10
    PUSH R10
    PUSH R11
    SUB RSP,0x20 //Manditory 4 stack arguments must be "pushed"
    LEA RCX,[RBP+8+8]
    PUSH R9
    PUSH R8
    PUSH RDX
    PUSH RCX
     */
    char *atxt="\x55\x48\x89\xE5\x48\x83\xE4\xF0\x41\x52\x41\x53\x48\x83\xEC\x20\x48\x8D\x4D\x10\x41\x51\x41\x50\x52\x51";
    vec_pusharr(blob,atxt,0x1a);
    //MOV RAX,fptr
	atxt="\x48\xb8";
	vec_pusharr(blob,atxt,0x2);
	for(i=0;i!=8;i++)
		vec_push(blob,(((int64_t)fptr)>>(i*8))&0xff);
    /*
    CALL RAX
    ADD RSP,0x40
    POP R11
    POP R10
    LEAVE
    */
    atxt="\xFF\xD0\x48\x83\xC4\x40\x41\x5B\x41\x5A\xC9";
    vec_pusharr(blob,atxt,0xb);
    #endif
	//RET1 ARITY*8
	atxt="\xc2";
	vec_pusharr(blob,atxt,0x1);
	arity*=8;
	vec_push(blob,arity&0xff);
    vec_push(blob,arity>>8);
    CSymbol sym;
    memset(&sym,0,sizeof(sym));
    sym.type=SYM_FUNC;
    sym.add_to_rt_blob=1;
    sym.value_ptr=blob_off;
    sym.is_importable=1;
    map_set(&Loader.symbols, name, sym);
}
static double Log2(double c) {
    return log(c)/log(2);
}
void UnblockSignals() {
	#ifndef TARGET_WIN32
    sigset_t set;
    sigfillset(&set);
    sigprocmask(SIG_UNBLOCK,&set,NULL);
    #endif
}
int64_t STK_FileRead(int64_t *stk) {
    void *r=VFsFileRead(stk[0],stk[1]);
    if(r) PoopAllocSetTask(r,stk[2]);
    return r;
}
int64_t STK_FileWrite(int64_t *stk) {
    return VFsFileWrite(stk[0],stk[1],stk[2]);
}
int64_t STK_UnblockSignals(int64_t *stk) {
    UnblockSignals();
}
int64_t STK_Signal(int64_t *stk) {
    signal(stk[0],stk[1]);
}
int64_t STK_ForeachFunc(int64_t *stk) {
    ForeachFunc(stk[0]);
}
int64_t STK_TOSPrint(int64_t *stk) {
    TOSPrint(stk[0],stk[1],stk+2);
}
 int64_t STK_Del(int64_t *stk) {
	return VFsDel(stk[0]);
}
int64_t STK_pow(double *stk) {
    union {int64_t i;double_t d;} val;
    val.d=pow(stk[0],stk[1]);
    return val.i;
}
int64_t STK_CreateTagsAndErrorsFiles(int64_t *stk) {
    CreateTagsAndErrorsFiles(stk[0],stk[1],stk[2]);
}
int64_t STK_MSize(int64_t *stk) {
    return MSize(stk[0]);
}
int64_t STK_BFFS(int64_t *stk) {
    return BFFS(stk[0]);
}
int64_t STK_BCLZ(int64_t *stk) {
    return BCLZ(stk[0]);
}
void *STK_PoopMAlloc(int64_t *stk) {
    return PoopMallocTask(stk[0],stk[1]);
}
void *STK_PoopMAlloc32(int64_t *stk) {
    return PoopMalloc32Task(stk[0],stk[1]);
}
int64_t STK_FOpen(int64_t *stk) {
    return fopen(stk[1],stk[2]);
}
int64_t STK_FClose(int64_t *stk) {
    return fclose(stk[1]);
}
#define NEXT_BLK 0x7FFFFFFFFFFFFFFFll
int64_t STK_FBlkRead(int64_t *stk) {
    if(NEXT_BLK!=stk[2]) {
        fseek(stk[0],stk[2]*(1<<9),SEEK_SET);
    }
    return fread(stk[1],1<<9,stk[3],stk[0]);
}
int64_t STK_FBlkWrite(int64_t *stk) {
    if(NEXT_BLK!=stk[2]) {
        fseek(stk[0],stk[2]*(1<<9),SEEK_SET);
    }
    return fwrite(stk[1],1<<9,stk[3],stk[0]);
}
int64_t STK_PoopFree(int64_t *stk) {
    PoopFree(stk[0]);
}
int64_t STK_MemNCpy(int64_t *stk) {
    return memcpy(stk[0],stk[1],stk[2]);
}
int64_t STK_strlen(int64_t *stk) {
    return strlen(stk[0]);
}
int64_t STK_strcmp(int64_t *stk) {
    return strcmp(stk[0],stk[1]);
}
int64_t STK_strncmp(int64_t *stk) {
    return strncmp(stk[0],stk[1],stk[2]);
}
int64_t STK_strcpy(int64_t *stk) {
    return strcpy(stk[0],stk[1]);
}
int64_t STK_strncpy(int64_t *stk) {
    return strncpy(stk[0],stk[1],stk[2]);
}
int64_t STK_strstr(int64_t *stk) {
    return strstr(stk[0],stk[1]);
}
int64_t STK_atan(double *stk) {
    union {int64_t i;double_t d;} val;
    val.d=atan(stk[0]);
    return val.i;
}
int64_t STK_abs(double *stk) {
    union {int64_t i;double_t d;} val;
    val.d=abs(stk[0]);
    return val.i;
}
int64_t STK_cos(double *stk) {
    union {int64_t i;double_t d;} val;
    val.d=cos(stk[0]);
    return val.i;
}
int64_t STK_sin(double *stk) {
    union {int64_t i;double_t d;} val;
    val.d=sin(stk[0]);
    return val.i;
}
int64_t STK_sqrt(double *stk) {
    union {int64_t i;double_t d;} val;
    val.d=sin(stk[0]);
    return val.i;
}
int64_t STK_tan(double *stk) {
    union {int64_t i;double_t d;} val;
    val.d=tan(stk[0]);
    return val.i;
}
int64_t STK_ceil(double *stk) {
    union {int64_t i;double_t d;} val;
    val.d=ceil(stk[0]);
    return val.i;
}
int64_t STK_floor(double *stk) {
    union {int64_t i;double_t d;} val;
    val.d=floor(stk[0]);
    return val.i;
}
int64_t STK___Move(int64_t *stk) {
	return __Move(stk[0],stk[1]);
}
int64_t STK_log(double *stk) {
    union {int64_t i;double_t d;} val;
    val.d=log(stk[0]);
    return val.i;
}
int64_t STK_log10(double *stk) {
    union {int64_t i;double_t d;} val;
    val.d=log10(stk[0]);
    return val.i;
}
int64_t STK_log2(double *stk) {
    union {int64_t i;double_t d;} val;
    val.d=log2(stk[0]);
    return val.i;
}
int64_t STK_round(double *stk) {
    union {int64_t i;double_t d;} val;
    val.d=round(stk[0]);
    return val.i;
}
int64_t STK_trunc(double *stk) {
    union {int64_t i;double_t d;} val;
    val.d=trunc(stk[0]);
    return val.i;
}
int64_t STK_Kill(int64_t *stk) {
    __KillThread(stk[0]);
}
int64_t STK_exit(int64_t *stk) {
    exit(stk[0]);
}
int64_t STK_Cd(int64_t *stk) {
    return VFsCd(stk[0],0);
}
int64_t STK_DirCur(int64_t *stk) {
    return VFsDirCur();
}
int64_t STK_DirMk(int64_t *stk) {
    return VFsCd(stk[0],VFS_CDF_MAKE);
}
int64_t STK_FileNameAbs(int64_t *stk) {
    return VFsFileNameAbs(stk[0]);
}
int64_t STK_IsMac(int64_t *stk) {
    return IsMac();
}
int64_t STK_IsWindows(int64_t *stk) {
    return IsWindows();
}
int64_t STK___Dir(int64_t *stk) {
    return __Dir(stk[0]);
}
int64_t STK_memset(int64_t *stk) {
    return memset(stk[0],stk[1],stk[2]);
}
int64_t STK_IsDir(int64_t *stk) {
    return IsDir(stk[0]);
}
int64_t STK_NewDrawWindow(int64_t *stk) {
    return NewDrawWindow();
}
int64_t STK_DrawWindowUpdate(int64_t *stk) {
    DrawWindowUpdate(stk[0],stk[1],stk[2],stk[3]);
}
int64_t STK_DrawWindowDel(int64_t *stk) {
    DrawWindowDel(stk[0]);
}
int64_t STK___AddTimer(int64_t *stk) {
    __AddTimer(stk[0],stk[1]);
}
int64_t STK___GetTicks() {
    return SDL_GetTicks();
}
int64_t STK___Spawn(int64_t *stk) {
    __Spawn(stk[0],stk[1],stk[2],stk[3]);
}
int64_t STK___AwakeThread(int64_t *stk) {
    __AwakeThread(stk[0]);
}
int64_t STK___FreeThread(int64_t *stk) {
    __FreeThread(stk[0]);
}
int64_t STK_SetKBCallback(int64_t *stk) {
    SetKBCallback(stk[0],stk[1]);
}
int64_t STK___Suspend(int64_t *stk) {
    __Suspend(stk[0]);
}
int64_t STK___AwaitThread(int64_t *stk) {
    __AwaitThread(stk[0]);
}
int64_t STK___KillThread(int64_t *stk) {
    __KillThread(stk[0]);
}
int64_t STK_SetMSCallback(int64_t *stk) {
    SetMSCallback(stk[0]);    
}
int64_t STK_Sleep(int64_t *stk) {
    __Sleep(stk[0]);
}
int64_t STK_GetFs(int64_t *stk) {
    return GetFs();
}
int64_t STK_Yield(int64_t *stk) {
    __Yield();
}
int64_t STK_SndFreq(int64_t *stk) {
    SndFreq(stk[0]);
}
    int64_t STK_SetClipboardText(int64_t *stk) {
    SDL_SetClipboardText(stk[0]);
}
int64_t STK_GetClipboardText(int64_t *stk) {
    char *find,*ret;
    if(!SDL_HasClipboardText())
        return NULL;
    find=SDL_GetClipboardText();
    ret=strdup(find);
    SDL_free(find);
    return ret;
}
int64_t STK___SetThreadPtr(int64_t *stk) {
    __SetThreadPtr(stk[0],stk[1]);
}
int64_t STK___SleepUntilChange(int64_t *stk) {
    __SleepUntilChange(stk[0],stk[1]);
}
int64_t STK_FSize(int64_t *stk) {
	return VFsFSize(stk[0]);
}
int64_t STK_FUnixTime(int64_t *stk) {
	return VFsUnixTime(stk[0]);
}
int64_t STK_SetPtrCallers(int64_t *stk) {
	assert(stk[1]==5);
	PoopAllocSetCallers(stk[0],stk[1],stk+2);
}
/*
 * TODO give a better name
 * Returns NULL if in bounds
 * Retusn ptr to nearest item if slightly out of bounds
 * Returns 0x7FFFFFFFFFFFFFFFll if really out of bounds
 */
int64_t STK_InBounds(int64_t *stk) {
	void *near=NULL;
	if(InBounds(stk[0],stk[1],&near)) {
		return NULL;
	}
	if(!near) return 0x7FFFFFFFFFFFFFFFll;
	return near;
}
void TOS_RegisterFuncPtrs() {
	map_iter_t miter;
	const char *key;
	CSymbol *s;
	vec_char_t ffi_blob;
	vec_init(&ffi_blob);
	STK_RegisterFunctionPtr(&ffi_blob,"__InBounds",STK_InBounds,2);
	STK_RegisterFunctionPtr(&ffi_blob,"SetPtrCallers",STK_SetPtrCallers,2+5);
	STK_RegisterFunctionPtr(&ffi_blob,"__MAlloc32",STK_PoopMAlloc32,2);
	STK_RegisterFunctionPtr(&ffi_blob,"__MAlloc",STK_PoopMAlloc,2);
	STK_RegisterFunctionPtr(&ffi_blob,"__Free",STK_PoopFree,1);
	STK_RegisterFunctionPtr(&ffi_blob,"FUnixTime",STK_FUnixTime,1);
	STK_RegisterFunctionPtr(&ffi_blob,"FSize",STK_FSize,1);
    STK_RegisterFunctionPtr(&ffi_blob,"__SleepUntilChange",STK___SleepUntilChange,2);
    STK_RegisterFunctionPtr(&ffi_blob,"__SetThreadPtr",STK___SetThreadPtr,2);
    STK_RegisterFunctionPtr(&ffi_blob,"SetClipboardText",STK_SetClipboardText,1);
    STK_RegisterFunctionPtr(&ffi_blob,"GetClipboardText",STK_GetClipboardText,0);
    STK_RegisterFunctionPtr(&ffi_blob,"FOpen",STK_FOpen,3);
    STK_RegisterFunctionPtr(&ffi_blob,"FClose",STK_FClose,1);
    STK_RegisterFunctionPtr(&ffi_blob,"FBlkRead",STK_FBlkRead,4);
    STK_RegisterFunctionPtr(&ffi_blob,"FBlkWrite",STK_FBlkWrite,4);
    STK_RegisterFunctionPtr(&ffi_blob,"SndFreq",STK_SndFreq,1);
    STK_RegisterFunctionPtr(&ffi_blob,"Sleep",&STK_Sleep,1);
    STK_RegisterFunctionPtr(&ffi_blob,"Fs",STK_GetFs,0);
    STK_RegisterFunctionPtr(&ffi_blob,"SetKBCallback",STK_SetKBCallback,2);
    STK_RegisterFunctionPtr(&ffi_blob,"SetMSCallback",STK_SetMSCallback,1);
    STK_RegisterFunctionPtr(&ffi_blob,"__AddTimer",STK___AddTimer,2);
    STK_RegisterFunctionPtr(&ffi_blob,"__GetTicks",STK___GetTicks,0);
    STK_RegisterFunctionPtr(&ffi_blob,"__Exit",__Exit,0);
    STK_RegisterFunctionPtr(&ffi_blob,"__KillThread",STK___KillThread,1);
    STK_RegisterFunctionPtr(&ffi_blob,"__AwaitThread",STK___AwaitThread,1);
    STK_RegisterFunctionPtr(&ffi_blob,"__AwakeThread",STK___AwakeThread,1);
    STK_RegisterFunctionPtr(&ffi_blob,"__FreeThread",STK___FreeThread,1);
    STK_RegisterFunctionPtr(&ffi_blob,"__Spawn",STK___Spawn,4);
    STK_RegisterFunctionPtr(&ffi_blob,"__Suspend",STK___Suspend,1);
    STK_RegisterFunctionPtr(&ffi_blob,"Yield",STK_Yield,0);
    STK_RegisterFunctionPtr(&ffi_blob,"UnblockSignals",STK_UnblockSignals,0);
    STK_RegisterFunctionPtr(&ffi_blob,"signal",STK_Signal,2);
    STK_RegisterFunctionPtr(&ffi_blob,"__BootstrapForeachSymbol",STK_ForeachFunc,1);
    STK_RegisterFunctionPtr(&ffi_blob,"__FileRead",STK_FileRead,3);
    STK_RegisterFunctionPtr(&ffi_blob,"__FileWrite",STK_FileWrite,3);
    STK_RegisterFunctionPtr(&ffi_blob,"IsDir",STK_IsDir,1);
    STK_RegisterFunctionPtr(&ffi_blob,"DrawWindowDel",STK_DrawWindowDel,1);
    STK_RegisterFunctionPtr(&ffi_blob,"DrawWindowUpdate",STK_DrawWindowUpdate,4);
    STK_RegisterFunctionPtr(&ffi_blob,"DrawWindowNew",STK_NewDrawWindow,0);
    //SPECIAL
    STK_RegisterFunctionPtr(&ffi_blob,"TOSPrint",STK_TOSPrint,0);
    STK_RegisterFunctionPtr(&ffi_blob,"Pow",STK_pow,2);
    STK_RegisterFunctionPtr(&ffi_blob,"MSize",STK_MSize,1);
    STK_RegisterFunctionPtr(&ffi_blob,"MSize2",STK_MSize,1);
    STK_RegisterFunctionPtr(&ffi_blob,"Bsf",STK_BFFS,1);
    STK_RegisterFunctionPtr(&ffi_blob,"Bsr",STK_BCLZ,1);
    STK_RegisterFunctionPtr(&ffi_blob,"MemCpy",STK_MemNCpy,3);
    STK_RegisterFunctionPtr(&ffi_blob,"MemNCpy",STK_MemNCpy,3);
    STK_RegisterFunctionPtr(&ffi_blob,"StrLen",STK_strlen,1);
    STK_RegisterFunctionPtr(&ffi_blob,"StrCmp",STK_strcmp,2);
    STK_RegisterFunctionPtr(&ffi_blob,"StrNCmp",STK_strncmp,3);
    STK_RegisterFunctionPtr(&ffi_blob,"StrCpy",STK_strcpy,2);
    STK_RegisterFunctionPtr(&ffi_blob,"StrNCpy",STK_strncpy,3);
    STK_RegisterFunctionPtr(&ffi_blob,"StrMatch",STK_strstr,2);
    STK_RegisterFunctionPtr(&ffi_blob,"ATan",STK_atan,1);
    STK_RegisterFunctionPtr(&ffi_blob,"Abs",STK_abs,1);
    STK_RegisterFunctionPtr(&ffi_blob,"Cos",STK_cos,1);
    STK_RegisterFunctionPtr(&ffi_blob,"Sin",STK_sin,1);
    STK_RegisterFunctionPtr(&ffi_blob,"Sqrt",STK_sqrt,1);
    STK_RegisterFunctionPtr(&ffi_blob,"Tan",STK_tan,1);
    STK_RegisterFunctionPtr(&ffi_blob,"Ceil",STK_ceil,1);
    STK_RegisterFunctionPtr(&ffi_blob,"Floor",STK_floor,1);
    STK_RegisterFunctionPtr(&ffi_blob,"Ln",STK_log,1);
    STK_RegisterFunctionPtr(&ffi_blob,"Log10",STK_log10,1);
    STK_RegisterFunctionPtr(&ffi_blob,"Log2",STK_log2,1);
    STK_RegisterFunctionPtr(&ffi_blob,"Round",STK_round,1);
    STK_RegisterFunctionPtr(&ffi_blob,"Trunc",STK_trunc,1);
    STK_RegisterFunctionPtr(&ffi_blob,"Exit",STK_exit,1);
    STK_RegisterFunctionPtr(&ffi_blob,"Cd",STK_Cd,1);
    STK_RegisterFunctionPtr(&ffi_blob,"DirCur",STK_DirCur,0);
    STK_RegisterFunctionPtr(&ffi_blob,"DirMk",STK_DirMk,1);
    STK_RegisterFunctionPtr(&ffi_blob,"__Del",STK_Del,1);
    STK_RegisterFunctionPtr(&ffi_blob,"FileNameAbs",STK_FileNameAbs,1);
    STK_RegisterFunctionPtr(&ffi_blob,"DirNameAbs",STK_FileNameAbs,1);
    STK_RegisterFunctionPtr(&ffi_blob,"__Dir",STK___Dir,1);
    STK_RegisterFunctionPtr(&ffi_blob,"IsWindows",STK_IsWindows,0);
    STK_RegisterFunctionPtr(&ffi_blob,"IsMac",STK_IsMac,0);
    STK_RegisterFunctionPtr(&ffi_blob,"MemSet",STK_memset,3);
    char *blob=PoopMAlloc32(ffi_blob.length);
    memcpy(blob,ffi_blob.data,ffi_blob.length);
    vec_deinit(&ffi_blob);
    miter=map_iter(&Loader.symbols);
    while(key=map_next(&Loader.symbols,&miter)) {
		if((s=map_get(&Loader.symbols,key))->add_to_rt_blob) {
			s->value_ptr=blob+(int64_t)s->value_ptr;
			s->add_to_rt_blob=0;
		}
	}
}
