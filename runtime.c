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
#include <sys/types.h>
#include <time.h>
extern int64_t HCSetJmp(void *ptr);
extern void HCLongJmp(void *ptr);
#include <stddef.h>
#include <stdalign.h>
#ifdef TARGET_WIN32
#include <windows.h>
#include <fileapi.h>
#include <shlwapi.h>
#include <memoryapi.h>
#include "ext/wineditline-2.206/include/editline/readline.h"
#else
#include <sys/mman.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <errno.h>
#endif
void HolyFree(void *ptr) {
	static void *fptr;
	if(!fptr) {
		fptr=map_get(&TOSLoader,"_FREE")->data->val;
	}
	FFI_CALL_TOS_1(fptr,ptr);
}
void *HolyMAlloc(int64_t sz) {
	static void *fptr;
	if(!fptr) {
		fptr=map_get(&TOSLoader,"_MALLOC")->data->val;
	}
	return FFI_CALL_TOS_2(fptr,sz,NULL);
}
char *HolyStrDup(char *str) {
	return strcpy(HolyMAlloc(strlen(str)+1),str);
}
#ifdef USE_NETWORKING
#include "ext/dyad/src/dyad.h"
static void STK_DyadInit() {
		dyad_init();
		dyad_setUpdateTimeout(0.);
}
static void STK_DyadUpdate() {
	dyad_update();
}
static void STK_DyadShutdown() {
	dyad_shutdown();
}
static void *STK_DyadNewStream() {
	return dyad_newStream();
}
static void *STK_DyadListen(int64_t *stk) {
	return dyad_listen(stk[0],stk[1]);
}
static void *STK_DyadConnect(int64_t *stk) {
	return dyad_connect(stk[0],stk[1],stk[2]);
}
static void STK_DyadWrite(int64_t *stk) {
	dyad_write(stk[0],stk[1],stk[2]);
}
static void STK_DyadEnd(int64_t *stk) {
	dyad_end(stk[0]);
}
static void STK_DyadClose(int64_t *stk) {
	dyad_close(stk[0]);
}
static char *STK_DyadGetAddress(int64_t *stk) {
	char *ret=dyad_getAddress(stk[0]);
	return HolyStrDup(ret);
}
static void DyadReadCB(dyad_Event *e) {
	FFI_CALL_TOS_4(e->udata,e->stream,e->data,e->size,e->udata2);
}
static void STK_DyadSetReadCallback(int64_t *stk) {
	dyad_addListener(stk[0],DYAD_EVENT_LINE,&DyadReadCB,stk[1],stk[2]);
}
static void DyadListenCB(dyad_Event *e) {
	FFI_CALL_TOS_2(e->udata,e->remote,e->udata2);
}
static STK_DyadSetOnListenCallback(int64_t *stk) {
	dyad_addListener(stk[0],DYAD_EVENT_ACCEPT,&DyadListenCB,stk[1],stk[2]);
}
#endif
static void UnblockSignals() {
	#ifndef TARGET_WIN32
	sigset_t all;
	sigfillset(&all);
	sigprocmask(SIG_UNBLOCK,&all,NULL);
	#endif
}
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
	TD_FREE(old);
	TD_FREE(new);
    return ret;
}
static int64_t IsDir(char *fn) {
    fn=__VFsFileNameAbs(fn);
    if(!fn) return 0;
    struct stat buf;
    stat(fn, &buf);
    TD_FREE(fn);
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
    void *data=HolyMAlloc(len+1);
    fread(data, 1, len, f);
    ((char*)data)[len]=0;
    fclose(f);
    if(sz) *sz=len;
    return data;
}
static char **__Dir(char *fn) {
	int64_t sz;
	char **ret;
    fn=__VFsFileNameAbs(fn);
    DIR *dir=opendir(fn);
    if(!dir) {
        TD_FREE(fn);
        return NULL;
    }
    struct dirent *ent;
    vec_str_t items;
    vec_init(&items);
    while(ent=readdir(dir))
        vec_push(&items,HolyStrDup(ent->d_name));
    vec_push(&items,NULL);
    TD_FREE(fn);
    sz=items.length*sizeof(char*);
    ret=memcpy(HolyMAlloc(sz),items.data,sz);
    vec_deinit(&items);
    closedir(dir);
    return ret;
}
static void STK_InteruptCore(int64_t *stk) {
	InteruptCore(stk[0]);
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
static char *GetCipherPasswd() {
	if(!cipher_passwd)
		return NULL;
	return HolyStrDup(cipher_passwd);
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
int64_t STK_FileRead(int64_t *stk) {
	int64_t sz;
    char *r=VFsFileRead(stk[0],&sz);
    if(stk[1]) ((int64_t*)stk[1])[0]=sz;
    return r;
}
int64_t STK_FileWrite(int64_t *stk) {
    return VFsFileWrite(stk[0],stk[1],stk[2]);
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
int64_t STK_FOpen(int64_t *stk) {
	char *fn=__VFsFileNameAbs(stk[0]);
    FILE *f=fopen(fn,stk[1]);
    TD_FREE(fn);
    return f;
}
int64_t STK_FClose(int64_t *stk) {
    return fclose(stk[0]);
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
int64_t STK___Move(int64_t *stk) {
	return __Move(stk[0],stk[1]);
}
int64_t STK_Cd(int64_t *stk) {
    return VFsCd(stk[0],0);
}
int64_t STK_DirCur(int64_t *stk) {
    char *d=VFsDirCur(),*r;
    r=HolyStrDup(d);
    TD_FREE(d);
    return r;
}
int64_t STK_DirMk(int64_t *stk) {
	char *d=VFsDirCur();
    int r=VFsCd(stk[0],VFS_CDF_MAKE);
    VFsCd(d,0);
    TD_FREE(d);
    return r;
}
int64_t STK_FileNameAbs(int64_t *stk) {
    char *a=VFsFileNameAbs(stk[0]),*r;
    r=HolyStrDup(a);
    TD_FREE(a);
    return r;
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
int64_t STK___GetTicks() {
	#ifndef TARGET_WIN32
	//https://stackoverflow.com/questions/2958291/equivalent-to-gettickcount-on-linux
    struct timespec ts;
    int64_t theTick = 0U;
    clock_gettime( CLOCK_REALTIME, &ts );
    theTick  = ts.tv_nsec / 1000000;
    theTick += ts.tv_sec * 1000;
    return theTick;
    #else
    static int64_t freq;
    int64_t cur;
    if(!freq) {
		QueryPerformanceFrequency(&freq);
		freq/=1000;
	}
	QueryPerformanceCounter(&cur);
    return cur/freq;
    #endif
}
int64_t STK_SetKBCallback(int64_t *stk) {
    SetKBCallback(stk[0],stk[1]);
}
int64_t STK_SetMSCallback(int64_t *stk) {
    SetMSCallback(stk[0]);    
}
#ifdef TARGET_WIN32
//https://stackoverflow.com/questions/85122/how-to-make-thread-sleep-less-than-a-millisecond-on-windows
static void SleepShort(int64_t ms) {
	static int once;
	static NTSTATUS (*ntdelay)(int,int64_t *);
	static NTSTATUS (*zwtimer)(int,int,int64_t *);
	if(!once) {
		ULONG reso;
		zwtimer=GetProcAddress(GetModuleHandle("ntdll.dll"), "ZwSetTimerResolution");
		ntdelay=GetProcAddress(GetModuleHandle("ntdll.dll"), "NtDelayExecution");
		once=1;
		zwtimer(10000,1,&reso);
	}
    int64_t interval=-1*ms;
    ntdelay(0,&interval);
}

#endif
int64_t STK_Sleep(int64_t *stk) {
	#ifndef TARGET_WIN32
	usleep(1000*stk[0]);
	#else
	SleepShort(stk[0]);
	#endif
}
int64_t STK_GetFs(int64_t *stk) {
    return GetFs();
}
int64_t STK_SetFs(int64_t *stk) {
    SetFs(stk[0]);
}
int64_t STK_SndFreq(int64_t *stk) {
    SndFreq(stk[0]);
}
int64_t STK_SetClipboardText(int64_t *stk) {
    //SDL_SetClipboardText(stk[0]);
    SetClipboard(stk[0]);
}
int64_t STK___GetStr(int64_t *stk) {
	#ifndef TARGET_WIN32
	char *s=linenoise(stk[0]);
	if(!s) return s;
	linenoiseHistoryAdd(s);
	char *r=HolyStrDup(s);
	free(s);
	return r;
	#else
	char *s=readline(stk[0]),*r;
	if(!s) return s;
	r=HolyStrDup(s);
	add_history(r);
	rl_free(s);
	#endif
	return r;
}
int64_t STK_GetClipboardText(int64_t *stk) {
    char *r=ClipboardText();
    char *r2=HolyStrDup(r);
    TD_FREE(r);
    return r2;
}
int64_t STK___SleepUntilValue(int64_t *stk) {
}
int64_t STK_FSize(int64_t *stk) {
	return VFsFSize(stk[0]);
}
int64_t STK_FUnixTime(int64_t *stk) {
	return VFsUnixTime(stk[0]);
}
int64_t STK___FExists(int64_t *stk) {
	return VFsFileExists(stk[0]);
}
int64_t STK_ChDrv(int64_t *stk) {
	return VFsChDrv(stk[0]);
}
#ifndef TARGET_WIN32
#include <time.h>
int64_t STK_Now(int64_t *stk) {
	int64_t t;
	t=time(NULL);
	return t;
}
#else
static int64_t FILETIME2Unix(FILETIME *t) {
	//https://www.frenk.com/2009/12/convert-filetime-to-unix-timestamp/	
	int64_t time=t->dwLowDateTime|((int64_t)t->dwHighDateTime<<32),adj;
	adj=10000*(int64_t)11644473600000ll;
	time-=adj;
	return time/10000000ll;
}
int64_t STK_Now(int64_t *stk) {
	int64_t r;
	FILETIME ft;
	GetSystemTimeAsFileTime(&ft);
	return FILETIME2Unix(&ft);
}
#endif
int64_t mp_cnt(int64_t *stk) {
	#ifndef TARGET_WIN32
	return sysconf(_SC_NPROCESSORS_ONLN);
	#else
	SYSTEM_INFO info;
	GetSystemInfo(&info);
	return info.dwNumberOfProcessors;
	#endif
} 
void SpawnCore(int64_t *stk) {
	CreateCore(stk[0],stk[1]);
}
void STK_GrPalleteSet(int64_t *stk) {
	GrPalleteSet(stk[0],stk[1]);
}
int64_t STK_GrPalleteGet(int64_t *stk) {
	return GrPalleteGet(stk[0]);
}
int64_t STK_NewVirtualChunk(int64_t *stk) {
	return NewVirtualChunk(stk[0],stk[1]);
}
int64_t STK_FreeVirtualChunk(int64_t *stk) {
	FreeVirtualChunk(stk[0],stk[1]);
}
void TOS_RegisterFuncPtrs() {
	map_iter_t miter;
	const char *key;
	CSymbol *s;
	vec_char_t ffi_blob;
	vec_init(&ffi_blob);
	STK_RegisterFunctionPtr(&ffi_blob,"UnixNow",STK_Now,0);
	STK_RegisterFunctionPtr(&ffi_blob,"InteruptCore",STK_InteruptCore,1);
	STK_RegisterFunctionPtr(&ffi_blob,"NewVirtualChunk",STK_NewVirtualChunk,2);
	STK_RegisterFunctionPtr(&ffi_blob,"FreeVirtualChunk",STK_FreeVirtualChunk,2);
	STK_RegisterFunctionPtr(&ffi_blob,"__CmdLineBootText",CmdLineBootText,0);
	STK_RegisterFunctionPtr(&ffi_blob,"Exit3Days",__Shutdown,0);
	STK_RegisterFunctionPtr(&ffi_blob,"ChDrv",STK_ChDrv,1);
	STK_RegisterFunctionPtr(&ffi_blob,"__GetStr",STK___GetStr,1);
	STK_RegisterFunctionPtr(&ffi_blob,"__IsCmdLine",IsCmdLine,0);
	STK_RegisterFunctionPtr(&ffi_blob,"__FExists",STK___FExists,1);
	STK_RegisterFunctionPtr(&ffi_blob,"mp_cnt",mp_cnt,0);
	STK_RegisterFunctionPtr(&ffi_blob,"GetGs",GetGs,0);
	STK_RegisterFunctionPtr(&ffi_blob,"__SpawnCore",SpawnCore,2);
	STK_RegisterFunctionPtr(&ffi_blob,"__CoreNum",CoreNum,0);
	STK_RegisterFunctionPtr(&ffi_blob,"FUnixTime",STK_FUnixTime,1);
	STK_RegisterFunctionPtr(&ffi_blob,"FSize",STK_FSize,1);
    STK_RegisterFunctionPtr(&ffi_blob,"__SleepUntilValue",STK___SleepUntilValue,3);
    STK_RegisterFunctionPtr(&ffi_blob,"SetClipboardText",STK_SetClipboardText,1);
    STK_RegisterFunctionPtr(&ffi_blob,"GetClipboardText",STK_GetClipboardText,0);
    STK_RegisterFunctionPtr(&ffi_blob,"FOpen",STK_FOpen,3);
    STK_RegisterFunctionPtr(&ffi_blob,"FClose",STK_FClose,1);
    STK_RegisterFunctionPtr(&ffi_blob,"FBlkRead",STK_FBlkRead,4);
    STK_RegisterFunctionPtr(&ffi_blob,"FBlkWrite",STK_FBlkWrite,4);
    STK_RegisterFunctionPtr(&ffi_blob,"SndFreq",STK_SndFreq,1);
    STK_RegisterFunctionPtr(&ffi_blob,"__Sleep",&STK_Sleep,1);
    STK_RegisterFunctionPtr(&ffi_blob,"GetFs",STK_GetFs,0);
    STK_RegisterFunctionPtr(&ffi_blob,"SetFs",STK_SetFs,1);
    STK_RegisterFunctionPtr(&ffi_blob,"SetKBCallback",STK_SetKBCallback,2);
    STK_RegisterFunctionPtr(&ffi_blob,"SetMSCallback",STK_SetMSCallback,1);
    STK_RegisterFunctionPtr(&ffi_blob,"__GetTicks",STK___GetTicks,0);
    STK_RegisterFunctionPtr(&ffi_blob,"__BootstrapForeachSymbol",STK_ForeachFunc,1);
    STK_RegisterFunctionPtr(&ffi_blob,"__FileRead",STK_FileRead,3);
    STK_RegisterFunctionPtr(&ffi_blob,"__FileWrite",STK_FileWrite,3);
    STK_RegisterFunctionPtr(&ffi_blob,"IsDir",STK_IsDir,1);
    STK_RegisterFunctionPtr(&ffi_blob,"DrawWindowDel",STK_DrawWindowDel,1);
    STK_RegisterFunctionPtr(&ffi_blob,"DrawWindowUpdate",STK_DrawWindowUpdate,4);
    STK_RegisterFunctionPtr(&ffi_blob,"DrawWindowNew",STK_NewDrawWindow,0);
    STK_RegisterFunctionPtr(&ffi_blob,"UnblockSignals",UnblockSignals,0);
    //SPECIAL
    STK_RegisterFunctionPtr(&ffi_blob,"TOSPrint",STK_TOSPrint,0);
    STK_RegisterFunctionPtr(&ffi_blob,"FileNameAbs",STK_FileNameAbs,1);
    STK_RegisterFunctionPtr(&ffi_blob,"DirNameAbs",STK_FileNameAbs,1);
    STK_RegisterFunctionPtr(&ffi_blob,"__Dir",STK___Dir,1);
    STK_RegisterFunctionPtr(&ffi_blob,"Cd",STK_Cd,1);
    STK_RegisterFunctionPtr(&ffi_blob,"DirCur",STK_DirCur,0);
    STK_RegisterFunctionPtr(&ffi_blob,"DirMk",STK_DirMk,1);
    STK_RegisterFunctionPtr(&ffi_blob,"__Del",STK_Del,1);
    #ifdef USE_NETWORKING
    STK_RegisterFunctionPtr(&ffi_blob,"DyadInit",&STK_DyadInit,0);
    STK_RegisterFunctionPtr(&ffi_blob,"DyadUpdate",&STK_DyadUpdate,0);
    STK_RegisterFunctionPtr(&ffi_blob,"DyadShutdown",&STK_DyadShutdown,0);
    STK_RegisterFunctionPtr(&ffi_blob,"DyadNewStream",&STK_DyadNewStream,0);
    STK_RegisterFunctionPtr(&ffi_blob,"DyadListen",&STK_DyadListen,2);
    STK_RegisterFunctionPtr(&ffi_blob,"DyadConnect",&STK_DyadConnect,3);
    STK_RegisterFunctionPtr(&ffi_blob,"DyadWrite",&STK_DyadWrite,3);
    STK_RegisterFunctionPtr(&ffi_blob,"DyadEnd",&STK_DyadEnd,1);
    STK_RegisterFunctionPtr(&ffi_blob,"DyadClose",&STK_DyadClose,1);
    STK_RegisterFunctionPtr(&ffi_blob,"DyadGetAddress",STK_DyadGetAddress,1);
    STK_RegisterFunctionPtr(&ffi_blob,"DyadSetReadCallback",STK_DyadSetReadCallback,3);
    STK_RegisterFunctionPtr(&ffi_blob,"DyadSetOnListenCallback",STK_DyadSetOnListenCallback,3);
    #endif
    STK_RegisterFunctionPtr(&ffi_blob,"GrPaletteColorSet",STK_GrPalleteSet,2);
    STK_RegisterFunctionPtr(&ffi_blob,"GrPaletteColorGet",STK_GrPalleteGet,1);
    STK_RegisterFunctionPtr(&ffi_blob,"GetCipherPasswd",GetCipherPasswd,0);
    char *blob=NewVirtualChunk(ffi_blob.length,1);
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
