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
TOS_Fs Fs;
void *GetFs() {
    return &Fs;
}
ExceptFrame *curframe;
ExceptFrame *EnterTry() {
    ExceptFrame *new=TD_MALLOC(sizeof(ExceptFrame));
    new->parent=curframe;
    curframe=new;
    return new;
}
void PopTryFrame() {
    ExceptFrame *c=curframe;
    ExceptFrame *par=curframe->parent;
    TD_FREE(curframe);
    curframe=par;
}
void throw(uint64_t val) {
    if(!curframe)
      exit(0);
    Fs.except_ch=val;
    Fs.catch_except=0;
    ExceptFrame old=*curframe;
    PopTryFrame();
    if(!curframe) {
      fail:;
      /*
      #ifdef BOOTSTRAPED
      char code[9];
      *(uint64_t*)code=Fs.except_ch;
      code[8]=0;
      fprintf(stderr,"Uncaught exception('%s'):\n",code);
      Backtrace(-1,1);
      #endif
      */
    }
    HCLongJmp(old.pad);
}
/**
 * If Fs.catch_except is not set,it will throw again
 */
static void ExitCatch() {
    if(!Fs.catch_except) {
        if(!curframe) {
          #ifdef BOOTSTRAPED
          char code[9];
          *(uint64_t*)code=Fs.except_ch;
          code[8]=0;
          fprintf(stderr,"Uncaught exception('%s'):\n",code);
          Backtrace(-1,1);
          #endif
        }
        ExceptFrame old=*curframe;
        PopTryFrame();
        HCLongJmp(old.pad);
    }
}
static int64_t BFFS(int64_t v) {
	if(!v) return -1;
    return __builtin_ffsl(v)-1;
}
static int64_t BCLZ(int64_t v) {
	if(!v) return -1;
    return 63-__builtin_clzl(v);
}
static uint64_t PowU64(uint64_t x,uint64_t n) {
    if(n==0) return 1;
    uint64_t y=1;
    while(n>1) {
        if(!(n%2)) {
            x=x*x;
            n=n/2;
        } else {
            y=x*y;
            x=x*x;
            n=(n-1)/2;
        }
    }
    return x*y;
}
static char *__GetStr(char*txt) {
    return rl(txt);
}
static void *MemNCpy(void *d,void *s,long sz) {
    return memcpy(d,s,sz);
}
static int64_t __Move(char *old,char *new) {
    return 0==rename(old, new);
}
static int64_t Cd(char *dir) {
    return 0==chdir(dir);
}

static char *DirCur(char *dir) {
    char buffer[1024];
    getcwd(buffer,1024);
    return strdup(buffer);
}

static int64_t DirMk(char *dir) {
#ifndef TARGET_WIN32
    return 0==mkdir(dir, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
#else
    return 0==CreateDirectoryA(dir,NULL);
#endif
}
static char *FileNameAbs(char *fn) {

#ifndef TARGET_WIN32
    char *d=realpath(fn,NULL);
    if(!d) return NULL;
    char *ret=strdup(d);
    free(d);
    return ret;
#else
    char buf[1024];
    if(GetFileAttributesA(fn)==INVALID_FILE_ATTRIBUTES)
      return NULL;
    GetFullPathNameA(fn,1024,buf,NULL);
    return strdup(buf);
#endif
}
static int64_t IsDir(char *fn) {
    struct stat buf;
    stat(fn, &buf);
    return S_ISDIR(buf.st_mode);
}
static int64_t FileWrite(char *fn,void *data,int64_t sz) {
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
    DIR *dir=opendir(fn);
    if(!dir) return NULL;
    struct dirent *ent;
    vec_str_t items;
    vec_init(&items);
    while(ent=readdir(dir))
        vec_push(&items,strdup(ent->d_name));
    vec_push(&items,NULL);
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
static char *hc_SDL_GetClipboardText() {
    return strdup(SDL_GetClipboardText());
}
static int64_t GetSurfaceW(SDL_Surface *s) {
    return s->w;
}
static int64_t GetSurfaceH(SDL_Surface *s) {
    return s->h;
}
static void ForeachFunc(void(*func)(const char *name,void *ptr,long sz)) {
  map_iter_t iter=map_iter(&Loader.symbols);
  const char *key;
  while(key=map_next(&Loader.symbols,&iter)) {
    CSymbol *var=map_get(&Loader.symbols, key);
    if(!var->is_importable)
        continue;
    FFI_CALL_TOS_3(func,key,var->value_ptr,var->size);
  }
}
static void STK_RegisterFunctionPtr(char *name,void *fptr) {
    CSymbol sym;
    sym.type=SYM_FUNC;
    sym.value_ptr=fptr;
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
char *GetBuiltinMacrosText();
int64_t STK_RegisterRuntimeClasses(int64_t *stk) {
    RegisterRuntimeClasses(stk[0],stk[1],stk[2],stk[3]);
}
int64_t STK_FileRead(int64_t *stk) {
    return FileRead(stk[0],stk[1]);
}
int64_t STK_FileWrite(int64_t *stk) {
    return FileWrite(stk[0],stk[1],stk[2]);
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
int64_t STK_PopTryFrame(int64_t *stk) {
    PopTryFrame();
}
int64_t STK_EnterTry(int64_t *stk) {
    return EnterTry();
}
int64_t STK_ExitCatch(int64_t *stk) {
    ExitCatch();
}
int64_t STK_TOSPrint(int64_t *stk) {
    TOSPrint(stk[0],stk[1],stk+2);
}
int64_t STK_BoundsCheck(int64_t *stk) {
    BoundsCheck(stk[0],stk[1]);
}
 int64_t STK_Del(int64_t *stk) {
#ifdef TARGET_WIN32
    return DeleteFileA(stk[0]);
#else
    return remove(stk[0]);
#endif
	
}
int64_t STK_pow(double *stk) {
    union {int64_t i;double_t d;} val;
    val.d=pow(stk[0],stk[1]);
    return val.i;
}
int64_t STK_PoopSetGCEnable(int64_t *stk) {
    return PoopSetGCEnable(stk[0]);
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
int64_t STK_throw(int64_t *stk) {
    throw(stk[0]);
}
int64_t STK_PoopMAlloc(int64_t *stk) {
    return PoopMAlloc(stk[0]);
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
int64_t STK_GetFs(int64_t *stk) {
    return GetFs();
}
int64_t STK___GetStr(int64_t *stk) {
    return __GetStr(stk[0]);
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
int64_t STK_exit(int64_t *stk) {
    exit(stk[0]);
}
int64_t STK_Cd(int64_t *stk) {
    return Cd(stk[0]);
}
int64_t STK_DirCur(int64_t *stk) {
    return DirCur(stk[0]);
}
int64_t STK_DirMk(int64_t *stk) {
    return DirMk(stk[0]);
}
int64_t STK___Move(int64_t *stk) {
    return __Move(stk[0],stk[1]);
}
int64_t STK_FileNameAbs(int64_t *stk) {
    return FileNameAbs(stk[0]);
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
int64_t STK_SDL_Init(int64_t *stk) {
    return SDL_Init(stk[0]);
}
int64_t STK_SDL_WasInit(int64_t *stk) {
    return SDL_WasInit(stk[0]);
}
int64_t STK_SDL_CreateWindow(int64_t *stk) {
    return (int64_t)SDL_CreateWindow((char*)stk[0],stk[1],stk[2],stk[3],stk[4],stk[5]);
}
int64_t STK_SDL_SetTextureBlendMode(int64_t *stk) {
    SDL_SetTextureBlendMode((SDL_Texture*)stk[0],stk[1]);
}
int64_t STK_SDL_CreateWindowAndRenderer(int64_t *stk) {
    SDL_CreateWindowAndRenderer(stk[0],stk[1],stk[2],(SDL_Window**)stk[3],(SDL_Renderer**)stk[4]);
}
int64_t STK_SDL_DestroyWindow(int64_t *stk) {
    SDL_DestroyWindow(stk[0]);
}
int64_t STK_SDL_GetGrabbedWindow(int64_t *stk) {
    return (int64_t)SDL_GetGrabbedWindow();
}
int64_t STK_SDL_GetWindowPosition(int64_t *stk) {
    SDL_GetWindowPosition((SDL_Window*)stk[0],(int*)stk[1],(int*)stk[2]);
}
int64_t STK_SDL_GetWindowMaximumSize(int64_t *stk) {
    SDL_GetWindowMaximumSize((SDL_Window*)stk[0],(int*)stk[1],(int*)stk[2]);
}
int64_t STK_SDL_GetWindowMinimumSize(int64_t *stk) {
    SDL_GetWindowMinimumSize((SDL_Window*)stk[0],(int*)stk[1],(int*)stk[2]);
}
int64_t STK_SDL_GetWindowSize(int64_t *stk) {
    SDL_GetWindowSize(stk[0],stk[1],stk[2]);
}
int64_t STK_hc_SDL_GetWindowTitle(int64_t *stk) {
    return (int64_t)hc_SDL_GetWindowTitle((SDL_Window*)stk[0]);
}
int64_t STK_SDL_MaximizeWindow(int64_t *stk) {
    SDL_MaximizeWindow((SDL_Window*)stk[0]);
}
int64_t STK_SDL_MinimizeWindow(int64_t *stk) {
    SDL_MinimizeWindow((SDL_Window*)stk[0]);
}
int64_t STK_SDL_SetWindowBordered(int64_t *stk) {
    SDL_SetWindowBordered((SDL_Window*)stk[0],stk[1]);
}
int64_t STK_SDL_SetWindowFullscreen(int64_t *stk) {
    SDL_SetWindowFullscreen((SDL_Window*)stk[0],stk[1]);
}
int64_t STK_SDL_SetWindowMaximumSize(int64_t *stk) {
    SDL_SetWindowMaximumSize((SDL_Window*)stk[0],stk[1],stk[2]);
}
int64_t STK_SDL_SetWindowMinimumSize(int64_t *stk) {
    SDL_SetWindowMinimumSize((SDL_Window*)stk[0],stk[1],stk[2]);
}
int64_t STK_SDL_SetWindowResizable(int64_t *stk) {
    SDL_SetWindowResizable((SDL_Window*)stk[0],stk[1]);
}
int64_t STK_SDL_SetWindowSize(int64_t *stk) {
    SDL_SetWindowSize((SDL_Window*)stk[0],stk[1],stk[2]);
}
int64_t STK_SDL_SetWindowTitle(int64_t *stk) {
    SDL_SetWindowTitle((SDL_Window*)stk[0],stk[1]);
}
int64_t STK_SDL_ShowWindow(int64_t *stk) {
    SDL_ShowWindow((SDL_Window*)stk[0]);
}
int64_t STK_SDL_HideWindow(int64_t *stk) {
    SDL_HideWindow((SDL_Window*)stk[0]);
}
int64_t STK_SDL_CreateRenderer(int64_t *stk) {
    return (int64_t)SDL_CreateRenderer((SDL_Renderer*)stk[0],stk[1],stk[2]);
}
int64_t STK_SDL_CreateTexture(int64_t *stk) {
    return (int64_t)SDL_CreateTexture((SDL_Renderer*)stk[0],stk[1],stk[2],stk[3],stk[4]);
}
int64_t STK_SDL_DestroyRenderer(int64_t *stk) {
    SDL_DestroyRenderer((SDL_Renderer*)stk[0]);
}
int64_t STK_SDL_GetRenderer(int64_t *stk) {
    return (int64_t)SDL_GetRenderer((SDL_Window*)stk[0]);
}
int64_t STK_SDL_GetRendererOutputSize(int64_t *stk) {
    return SDL_GetRendererOutputSize((SDL_Renderer*)stk[0],(int*)stk[1],(int*)stk[2]);
}
int64_t STK_SDL_GetRenderTarget(int64_t *stk) {
    return (int64_t)SDL_GetRenderTarget((SDL_Renderer*)stk[0]);
}
int64_t STK_SDL_GetTextureAlphaMod(int64_t *stk) {
    return SDL_GetTextureAlphaMod((SDL_Texture*)stk[0],(Uint8*)stk[1]);
}
int64_t STK_SDL_GetTextureColorMod(int64_t *stk) {
    return SDL_GetTextureColorMod((SDL_Texture*)stk[0],(Uint8*)stk[1],(Uint8*)stk[2],(Uint8*)stk[3]);
}
int64_t STK_SDL_RenderClear(int64_t *stk) {
    return SDL_RenderClear((SDL_Renderer*)stk[0]);
}
int64_t STK_SDL_RenderCopy(int64_t *stk) {
    return SDL_RenderCopy((SDL_Renderer*)stk[0],(SDL_Texture*)stk[1],(SDL_Rect*)stk[2],(SDL_Rect*)stk[3]);
}
int64_t STK_SDL_RenderDrawLine(int64_t *stk) {
    return SDL_RenderDrawLine((SDL_Renderer*)stk[0],stk[1],stk[2],stk[3],stk[4]);
}
int64_t STK_SDL_RenderDrawPoint(int64_t *stk) {
    return SDL_RenderDrawPoint((SDL_Renderer*)stk[0],stk[1],stk[2]);
}
int64_t STK_SDL_RenderDrawPoints(int64_t *stk) {
    return SDL_RenderDrawPoints((SDL_Renderer*)stk[0],(SDL_Point*)stk[1],stk[2]);
}
int64_t STK_SDL_RenderDrawRect(int64_t *stk) {
    return SDL_RenderDrawRect((SDL_Renderer*)stk[0],(SDL_Rect*)stk[1]);
}
int64_t STK_SDL_RenderDrawRects(int64_t *stk) {
    return SDL_RenderDrawRects((SDL_Renderer*)stk[0],(SDL_Rect*)stk[1],stk[2]);
}
int64_t STK_SDL_RenderFillRects(int64_t *stk) {
    return SDL_RenderDrawRects((SDL_Renderer*)stk[0],(SDL_Rect*)stk[1],stk[2]);
}
int64_t STK_SDL_RenderGetClipRect(int64_t *stk) {
    SDL_RenderGetClipRect((SDL_Renderer*)stk[0],(SDL_Rect*)stk[1]);
}
int64_t STK_SDL_RenderPresent(int64_t *stk) {
    SDL_RenderPresent((SDL_Renderer*)stk[0]);
}
int64_t STK_SDL_RenderSetClipRect(int64_t *stk) {
    return SDL_RenderSetClipRect((SDL_Renderer*)stk[0],(SDL_Rect*)stk[1]);
}
int64_t STK_SDL_SetRenderTarget(int64_t *stk) {
    return SDL_SetRenderTarget((SDL_Renderer*)stk[0],(SDL_Texture*)stk[1]);
}
int64_t STK_SDL_SetTextureAlphaMod(int64_t *stk) {
    return SDL_SetTextureAlphaMod((SDL_Texture*)stk[0],stk[1]);
}
int64_t STK_SDL_SetTextureColorMod(int64_t *stk) {
    return SDL_SetTextureColorMod((SDL_Texture*)stk[0],stk[1],stk[2],stk[3]);
}
int64_t STK_SDL_UpdateTexture(int64_t *stk) {
    return SDL_UpdateTexture((SDL_Texture*)stk[0],(SDL_Rect*)stk[1],(void*)stk[2],stk[3]);
}
int64_t STK_SDL_QueryTexture(int64_t *stk) {
    return SDL_QueryTexture((SDL_Texture*)stk[0],(Uint32*)stk[1],(int*)stk[2],(int*)stk[3],(int*)stk[4]);
}
int64_t STK_SDL_GetClipboardText(int64_t *stk) {
    return (int64_t)hc_SDL_GetClipboardText();
}
int64_t STK_SDL_SetClipboardText(int64_t *stk) {
    return SDL_SetClipboardText((char*)stk[0]);
}
int64_t STK_SDL_PollEvent(int64_t *stk) {
    return SDL_PollEvent((SDL_Event*)stk[0]);
}
int64_t STK_SDL_WaitEvent(int64_t *stk) {
    return SDL_WaitEvent((SDL_Event*)stk[0]);
}
int64_t STK_SDL_DestroyTexture(int64_t *stk) {
    SDL_DestroyTexture((SDL_Texture*)stk[0]);
}
int64_t STK_SDL_StartTextInput(int64_t *stk) {
    SDL_StartTextInput();
}
int64_t STK_SDL_StopTextInput(int64_t *stk) {
    SDL_StopTextInput();
}
int64_t STK_SDL_GetError(int64_t *stk) {
    SDL_GetError();
}
int64_t STK_SDL_ClearError(int64_t *stk) {
    SDL_ClearError();
}
int64_t STK_SDL_FlushEvent(int64_t *stk) {
    SDL_FlushEvent(stk[0]);
}
int64_t STK_GetSurfaceW(int64_t *stk) {
    return GetSurfaceW((SDL_Surface*)stk[0]);
}
int64_t STK_GetSurfaceH(int64_t *stk) {
    return GetSurfaceH((SDL_Surface*)stk[0]);
}
int64_t STK_SDL_CreateRGBSurface(int64_t *stk) {
    return (int64_t)SDL_CreateRGBSurface(stk[0],stk[1],stk[2],stk[3],stk[4],stk[5],stk[6],stk[7]);
}
int64_t STK_SDL_CreateRGBSurfaceFrom(int64_t *stk) {
    return (int64_t)SDL_CreateRGBSurfaceFrom((void*)stk[0],stk[1],stk[2],stk[3],stk[4],stk[5],stk[6],stk[7],stk[8]);
}
int64_t STK_SDL_UpperBlit(int64_t *stk) {
    return SDL_UpperBlit((SDL_Surface*)stk[0],(SDL_Rect*)stk[1],(SDL_Surface*)stk[2],(SDL_Rect*)stk[3]);
}
int64_t STK_SDL_FillRect(int64_t *stk) {
    return SDL_FillRect((SDL_Surface*)stk[0],(SDL_Rect*)stk[1],stk[2]);
}
int64_t STK_SDL_FillRects(int64_t *stk) {
    return SDL_FillRects((SDL_Surface*)stk[0],(SDL_Rect*)stk[1],stk[2],stk[3]);
}
int64_t STK_SDL_GetClipRect(int64_t *stk) {
     SDL_GetClipRect((SDL_Surface*)stk[0],(SDL_Rect*)stk[1]);
}
int64_t STK_SDL_GetColorKey(int64_t *stk) {
    return SDL_GetColorKey((SDL_Surface*)stk[0],(Uint32*)stk[1]);
}
int64_t STK_SDL_GetSurfaceAlphaMod(int64_t *stk) {
    return SDL_GetSurfaceAlphaMod((SDL_Surface*)stk[0],(Uint8*)stk[1]);
}
int64_t STK_SDL_GetSurfaceColorMod(int64_t *stk) {
    return SDL_GetSurfaceColorMod((SDL_Surface*)stk[0],(Uint8*)stk[1],(Uint8*)stk[2],(Uint8*)stk[3]);
}
int64_t STK_SDL_LockSurface(int64_t *stk) {
    return SDL_LockSurface((SDL_Surface*)stk[0]);
}
int64_t STK_SDL_UnlockSurface(int64_t *stk) {
    SDL_UnlockSurface((SDL_Surface*)stk[0]);
}
int64_t STK_SDL_SetClipRect(int64_t *stk) {
    SDL_SetClipRect((SDL_Surface*)stk[0],(SDL_Rect*)stk[1]);
}
int64_t STK_SDL_SetColorKey(int64_t *stk) {
    SDL_SetColorKey((SDL_Surface*)stk[0],stk[1],stk[2]);
}
int64_t STK_SDL_SetSurfaceAlphaMod(int64_t *stk) {
    return SDL_SetSurfaceAlphaMod((SDL_Surface*)stk[0],stk[1]);
}
int64_t STK_SDL_SetSurfaceColorMod(int64_t *stk) {
    return SDL_SetSurfaceColorMod((SDL_Surface*)stk[0],stk[1],stk[2],stk[3]);
}
int64_t STK_SDL_SetSurfaceRLE(int64_t *stk) {
    return SDL_SetSurfaceRLE((SDL_Surface*)stk[0],stk[1]);
}
int64_t STK_SDL_BlitSurface(int64_t *stk) {
    return SDL_BlitSurface((SDL_Surface*)stk[0],(SDL_Rect*)stk[1],(SDL_Surface*)stk[2],(SDL_Rect*)stk[3]);
}
int64_t STK_SDL_BlitScaled(int64_t *stk) {
    return SDL_LowerBlitScaled((SDL_Surface*)stk[0],(SDL_Rect*)stk[1],(SDL_Surface*)stk[2],(SDL_Rect*)stk[3]);
}
int64_t STK_SDL_FreeSurface(int64_t *stk) {
    SDL_FreeSurface((SDL_Surface*)stk[0]);
}
int64_t STK_SDL_UpdateWindowSurface(int64_t *stk) {
    return (int64_t)SDL_UpdateWindowSurface((SDL_Window*)stk[0]);
}
int64_t STK_SDL_GetWindowSurface(int64_t *stk) {
    return (int64_t)SDL_GetWindowSurface((SDL_Window*)stk[0]);
}
int64_t STK_GetBuiltinMacrosText(int64_t *stk) {
    return (int64_t)GetBuiltinMacrosText();
}
int64_t STK_SDL_DisableScreenSaver(int64_t *stk) {
    SDL_DisableScreenSaver();
}
int64_t STK_GCollect(int64_t *stk) {
    PoopCollect();
} 
int64_t STK_IsDir(int64_t *stk) {
    return IsDir(stk[0]);
}
int64_t STK___GetBuiltinMacrosText(int64_t *stk) {
    return GetBuiltinMacrosText();
}
//
//This was built for TOS abi
//
void JIT_DebuggerHit();
void STK_HCSetJmp();
int64_t FFI_RegisterRuntimeClasses();
int64_t FFI_UnblockSignals();
int64_t FFI_Signal();
int64_t FFI_ForeachFunc();
int64_t TOS_HCSetJmp();
int64_t FFI_PopTryFrame();
int64_t FFI_EnterTry();
int64_t FFI_ExitCatch();
int64_t FFI_TOSPrint();
int64_t FFI_BoundsCheck();
int64_t FFI_pow();
int64_t FFI_PoopSetGCEnable();
int64_t FFI_GCollect();
int64_t FFI_CreateTagsAndErrorsFiles();
int64_t FFI_MSize();
int64_t FFI_BFFS();
int64_t FFI_BCLZ();
int64_t FFI_throw();
int64_t FFI_PoopMAlloc();
int64_t FFI_PoopFree();
int64_t FFI_MemNCpy();
int64_t FFI_MemNCpy();
int64_t FFI_strlen();
int64_t FFI_strcmp();
int64_t FFI_strncmp();
int64_t FFI_strcpy();
int64_t FFI_strncpy();
int64_t FFI_strstr();
int64_t FFI_GetFs();
int64_t FFI___GetStr();
int64_t FFI_atan();
int64_t FFI_abs();
int64_t FFI_cos();
int64_t FFI_sin();
int64_t FFI_sqrt();
int64_t FFI_tan();
int64_t FFI_ceil();
int64_t FFI_floor();
int64_t FFI_log();
int64_t FFI_log10();
int64_t FFI_log2();
int64_t FFI_round();
int64_t FFI_trunc();
int64_t FFI_exit();
int64_t FFI_Cd();
int64_t FFI_DirCur();
int64_t FFI_DirMk();
int64_t FFI___Move();
int64_t FFI_FileNameAbs();
int64_t FFI_FileNameAbs();
int64_t FFI___Dir();
int64_t FFI_IsWindows();
int64_t FFI_IsMac();
int64_t FFI_memset();
int64_t FFI_SDL_Init();
int64_t FFI_SDL_WasInit();
int64_t FFI_SDL_CreateWindow();
int64_t FFI_SDL_SetTextureBlendMode();
int64_t FFI_SDL_CreateWindowAndRenderer();
int64_t FFI_SDL_DestroyWindow();
int64_t FFI_SDL_DestroyRenderer();
int64_t FFI_SDL_DisableScreenSaver();
int64_t FFI_SDL_GetGrabbedWindow();
int64_t FFI_SDL_GetWindowPosition();
int64_t FFI_SDL_GetWindowMinimumSize();
int64_t FFI_SDL_GetWindowMaximumSize();
int64_t FFI_SDL_GetWindowSize();
int64_t FFI_hc_SDL_GetWindowTitle();
int64_t FFI_SDL_HideWindow();
int64_t FFI_SDL_MaximizeWindow();
int64_t FFI_SDL_MinimizeWindow();
int64_t FFI_SDL_SetWindowBordered();
int64_t FFI_SDL_SetWindowFullscreen();
int64_t FFI_SDL_SetWindowMaximumSize();
int64_t FFI_SDL_SetWindowMinimumSize();
int64_t FFI_SDL_SetWindowResizable();
int64_t FFI_SDL_SetWindowSize();
int64_t FFI_SDL_SetWindowTitle();
int64_t FFI_SDL_ShowWindow();
int64_t FFI_SDL_CreateRenderer();
int64_t FFI_SDL_CreateTexture();
int64_t FFI_SDL_DestroyRenderer();
int64_t FFI_SDL_GetRenderDrawColor();
int64_t FFI_SDL_GetRenderer();
int64_t FFI_SDL_GetRendererOutputSize();
int64_t FFI_SDL_GetRenderTarget();
int64_t FFI_SDL_GetTextureAlphaMod();
int64_t FFI_SDL_GetTextureColorMod();
int64_t FFI_SDL_RenderClear();
int64_t FFI_SDL_RenderCopy();
int64_t FFI_SDL_RenderDrawLine();
int64_t FFI_SDL_RenderDrawLines();
int64_t FFI_SDL_RenderDrawPoint();
int64_t FFI_SDL_RenderDrawPoints();
int64_t FFI_SDL_RenderDrawRect();
int64_t FFI_SDL_RenderDrawRects();
int64_t FFI_SDL_RenderFillRects();
int64_t FFI_SDL_RenderGetClipRect();
int64_t FFI_SDL_RenderPresent();
int64_t FFI_SDL_RenderSetClipRect();
int64_t FFI_SDL_SetRenderDrawColor();
int64_t FFI_SDL_SetRenderTarget();
int64_t FFI_SDL_SetTextureAlphaMod();
int64_t FFI_SDL_SetTextureColorMod();
int64_t FFI_SDL_UpdateTexture();
int64_t FFI_SDL_QueryTexture();
int64_t FFI_SDL_GetClipboardText();
int64_t FFI_SDL_SetClipboardText();
int64_t FFI_SDL_PollEvent();
int64_t FFI_SDL_WaitEvent();
int64_t FFI_SDL_DestroyRenderer();
int64_t FFI_SDL_DestroyTexture();
int64_t FFI_SDL_StartTextInput();
int64_t FFI_SDL_StopTextInput();
int64_t FFI_SDL_GetError();
int64_t FFI_SDL_ClearError();
int64_t FFI_SDL_FlushEvent();
int64_t FFI_GetSurfaceW();
int64_t FFI_GetSurfaceH();
int64_t FFI_SDL_CreateRGBSurface();
int64_t FFI_SDL_CreateRGBSurfaceFrom();
int64_t FFI_SDL_UpperBlit();
int64_t FFI_SDL_FillRect();
int64_t FFI_SDL_FillRects();
int64_t FFI_SDL_GetClipRect();
int64_t FFI_SDL_GetColorKey();
int64_t FFI_SDL_GetSurfaceAlphaMod();
int64_t FFI_SDL_GetSurfaceColorMod();
int64_t FFI_SDL_LockSurface();
int64_t FFI_SDL_UnlockSurface();
int64_t FFI_SDL_SetClipRect();
int64_t FFI_SDL_SetColorKey();
int64_t FFI_SDL_SetSurfaceAlphaMod();
int64_t FFI_SDL_SetSurfaceColorMod();
int64_t FFI_SDL_SetSurfaceRLE();
int64_t FFI_SDL_BlitSurface();
int64_t FFI_SDL_BlitScaled();
int64_t FFI_SDL_FreeSurface();
int64_t FFI_SDL_UpdateWindowSurface();
int64_t FFI_SDL_GetWindowSurface();
int64_t FFI_GetBuiltinMacrosText();
int64_t FFI_IsDir();
int64_t FFI_FileRead();
int64_t FFI_FileWrite();
int64_t FFI_Del();
void TOS_RegisterFuncPtrs() {
    STK_RegisterFunctionPtr("RegisterRuntimeClasses",FFI_RegisterRuntimeClasses);
    STK_RegisterFunctionPtr("UnblockSignals",FFI_UnblockSignals);
    STK_RegisterFunctionPtr("signal",FFI_Signal);
    STK_RegisterFunctionPtr("ForeachFuncInTable",FFI_ForeachFunc);
    STK_RegisterFunctionPtr("FileRead",FFI_FileRead);
    STK_RegisterFunctionPtr("FileWrite",FFI_FileWrite);
    STK_RegisterFunctionPtr("IsDir",FFI_IsDir);
    //SPECIAL
    STK_RegisterFunctionPtr("HCSetJmp",TOS_HCSetJmp);
    //SPECIAL
    STK_RegisterFunctionPtr("JIT_DebuggerHit",JIT_DebuggerHit);
    STK_RegisterFunctionPtr("PopTryFrame",FFI_PopTryFrame);
    STK_RegisterFunctionPtr("EnterTry",FFI_EnterTry);
    STK_RegisterFunctionPtr("ExitCatch",FFI_ExitCatch);
    STK_RegisterFunctionPtr("TOSPrint",FFI_TOSPrint);
    STK_RegisterFunctionPtr("WhineOnOutOfBounds",FFI_BoundsCheck);
    STK_RegisterFunctionPtr("Pow",FFI_pow);
    STK_RegisterFunctionPtr("PoopSetGCEnable",FFI_PoopSetGCEnable);
    STK_RegisterFunctionPtr("GC_Collect",FFI_GCollect);
    STK_RegisterFunctionPtr("CreateTagsAndErrorsFiles",FFI_CreateTagsAndErrorsFiles);
    STK_RegisterFunctionPtr("MSize",FFI_MSize);
    STK_RegisterFunctionPtr("Bsf",FFI_BFFS);
    STK_RegisterFunctionPtr("Bsr",FFI_BCLZ);
    STK_RegisterFunctionPtr("throw",FFI_throw);
    STK_RegisterFunctionPtr("MAlloc",FFI_PoopMAlloc);
    STK_RegisterFunctionPtr("Free",FFI_PoopFree);
    STK_RegisterFunctionPtr("MemCpy",FFI_MemNCpy);
    STK_RegisterFunctionPtr("MemNCpy",FFI_MemNCpy);
    STK_RegisterFunctionPtr("StrLen",FFI_strlen);
    STK_RegisterFunctionPtr("StrCmp",FFI_strcmp);
    STK_RegisterFunctionPtr("StrNCmp",FFI_strncmp);
    STK_RegisterFunctionPtr("StrCpy",FFI_strcpy);
    STK_RegisterFunctionPtr("StrNCpy",FFI_strncpy);
    STK_RegisterFunctionPtr("StrMatch",FFI_strstr);
    STK_RegisterFunctionPtr("Fs",FFI_GetFs);
    STK_RegisterFunctionPtr("__GetStr",FFI___GetStr);
    STK_RegisterFunctionPtr("ATan",FFI_atan);
    STK_RegisterFunctionPtr("Abs",FFI_abs);
    STK_RegisterFunctionPtr("Cos",FFI_cos);
    STK_RegisterFunctionPtr("Sin",FFI_sin);
    STK_RegisterFunctionPtr("Sqrt",FFI_sqrt);
    STK_RegisterFunctionPtr("Tan",FFI_tan);
    STK_RegisterFunctionPtr("Ceil",FFI_ceil);
    STK_RegisterFunctionPtr("Floor",FFI_floor);
    STK_RegisterFunctionPtr("Ln",FFI_log);
    STK_RegisterFunctionPtr("Log10",FFI_log10);
    STK_RegisterFunctionPtr("Log2",FFI_log2);
    STK_RegisterFunctionPtr("Round",FFI_round);
    STK_RegisterFunctionPtr("Trunc",FFI_trunc);
    STK_RegisterFunctionPtr("Exit",FFI_exit);
    STK_RegisterFunctionPtr("Cd",FFI_Cd);
    STK_RegisterFunctionPtr("DirCur",FFI_DirCur);
    STK_RegisterFunctionPtr("DirMk",FFI_DirMk);
    STK_RegisterFunctionPtr("__Move",FFI___Move);
    STK_RegisterFunctionPtr("Del",FFI_Del);
    STK_RegisterFunctionPtr("FileNameAbs",FFI_FileNameAbs);
    STK_RegisterFunctionPtr("DirNameAbs",FFI_FileNameAbs);
    STK_RegisterFunctionPtr("__Dir",FFI___Dir);
    STK_RegisterFunctionPtr("IsWindows",FFI_IsWindows);
    STK_RegisterFunctionPtr("IsMac",FFI_IsMac);
    STK_RegisterFunctionPtr("MemSet",FFI_memset);
    STK_RegisterFunctionPtr("SDL_Init",FFI_SDL_Init);
    STK_RegisterFunctionPtr("SDL_WasInit",FFI_SDL_WasInit);
    STK_RegisterFunctionPtr("SDL_CreateWindow",FFI_SDL_CreateWindow);
    STK_RegisterFunctionPtr("SDL_SetTextureBlendMode",FFI_SDL_SetTextureBlendMode);
    STK_RegisterFunctionPtr("SDL_CreateWindowAndRenderer",FFI_SDL_CreateWindowAndRenderer);
    STK_RegisterFunctionPtr("SDL_DestroyWindow",FFI_SDL_DestroyWindow);
    STK_RegisterFunctionPtr("SDL_DestroyRenderer",FFI_SDL_DestroyRenderer);
    STK_RegisterFunctionPtr("SDL_DisableScreenSaver",FFI_SDL_DisableScreenSaver);
    STK_RegisterFunctionPtr("SDL_GetGrabbedWindow",FFI_SDL_GetGrabbedWindow);
    STK_RegisterFunctionPtr("SDL_GetWindowPosition",FFI_SDL_GetWindowPosition);
    STK_RegisterFunctionPtr("SDL_GetWindowMinimumSize",FFI_SDL_GetWindowMinimumSize);
    STK_RegisterFunctionPtr("SDL_GetWindowMaximumSize",FFI_SDL_GetWindowMaximumSize);
    STK_RegisterFunctionPtr("SDL_GetWindowSize",FFI_SDL_GetWindowSize);
    STK_RegisterFunctionPtr("SDL_GetWindowTitle",FFI_hc_SDL_GetWindowTitle);
    STK_RegisterFunctionPtr("SDL_HideWindow",FFI_SDL_HideWindow);
    STK_RegisterFunctionPtr("SDL_MaximizeWindow",FFI_SDL_MaximizeWindow);
    STK_RegisterFunctionPtr("SDL_MinimizeWindow",FFI_SDL_MinimizeWindow);
    STK_RegisterFunctionPtr("SDL_SetWindowBordered",FFI_SDL_SetWindowBordered);
    STK_RegisterFunctionPtr("SDL_SetWindowFullscreen",FFI_SDL_SetWindowFullscreen);
    STK_RegisterFunctionPtr("SDL_SetWindowMaximumSize",FFI_SDL_SetWindowMaximumSize);
    STK_RegisterFunctionPtr("SDL_SetWindowMinimumSize",FFI_SDL_SetWindowMinimumSize);
    STK_RegisterFunctionPtr("SDL_SetWindowResizable",FFI_SDL_SetWindowResizable);
    STK_RegisterFunctionPtr("SDL_SetWindowSize",FFI_SDL_SetWindowSize);
    STK_RegisterFunctionPtr("SDL_SetWindowTitle",FFI_SDL_SetWindowTitle);
    STK_RegisterFunctionPtr("SDL_ShowWindow",FFI_SDL_ShowWindow);
    STK_RegisterFunctionPtr("SDL_CreateRenderer",FFI_SDL_CreateRenderer);
    STK_RegisterFunctionPtr("SDL_CreateTexture",FFI_SDL_CreateTexture);
    STK_RegisterFunctionPtr("SDL_DestroyRenderer",FFI_SDL_DestroyRenderer);
    STK_RegisterFunctionPtr("SDL_GetRenderer",FFI_SDL_GetRenderer);
    STK_RegisterFunctionPtr("SDL_GetRendererOutputSize",FFI_SDL_GetRendererOutputSize);
    STK_RegisterFunctionPtr("SDL_GetTextureAlphaMod",FFI_SDL_GetTextureAlphaMod);
    STK_RegisterFunctionPtr("SDL_GetTextureColorMod",FFI_SDL_GetTextureColorMod);
    STK_RegisterFunctionPtr("SDL_RenderClear",FFI_SDL_RenderClear);
    STK_RegisterFunctionPtr("SDL_RenderCopy",FFI_SDL_RenderCopy);
    STK_RegisterFunctionPtr("SDL_RenderDrawLine",FFI_SDL_RenderDrawLine);
    STK_RegisterFunctionPtr("SDL_RenderDrawPoint",FFI_SDL_RenderDrawPoint);
    STK_RegisterFunctionPtr("SDL_RenderDrawPoints",FFI_SDL_RenderDrawPoints);
    STK_RegisterFunctionPtr("SDL_RenderDrawRect",FFI_SDL_RenderDrawRect);
    STK_RegisterFunctionPtr("SDL_RenderDrawRects",FFI_SDL_RenderDrawRects);
    STK_RegisterFunctionPtr("SDL_RenderFillRects",FFI_SDL_RenderFillRects);
    STK_RegisterFunctionPtr("SDL_RenderGetClipRect",FFI_SDL_RenderGetClipRect);
    STK_RegisterFunctionPtr("SDL_RenderPresent",FFI_SDL_RenderPresent);
    STK_RegisterFunctionPtr("SDL_RenderSetClipRect",FFI_SDL_RenderSetClipRect);
    STK_RegisterFunctionPtr("SDL_SetTextureAlphaMod",FFI_SDL_SetTextureAlphaMod);
    STK_RegisterFunctionPtr("SDL_SetTextureColorMod",FFI_SDL_SetTextureColorMod);
    STK_RegisterFunctionPtr("SDL_UpdateTexture",FFI_SDL_UpdateTexture);
    STK_RegisterFunctionPtr("SDL_QueryTexture",FFI_SDL_QueryTexture);
    STK_RegisterFunctionPtr("SDL_GetClipboardText",FFI_SDL_GetClipboardText);
    STK_RegisterFunctionPtr("SDL_SetClipboardText",FFI_SDL_SetClipboardText);
    STK_RegisterFunctionPtr("SDL_PollEvent",FFI_SDL_PollEvent);
    STK_RegisterFunctionPtr("SDL_WaitEvent",FFI_SDL_WaitEvent);
    STK_RegisterFunctionPtr("SDL_DestroyRenderer",FFI_SDL_DestroyRenderer);
    STK_RegisterFunctionPtr("SDL_DestroyTexture",FFI_SDL_DestroyTexture);
    STK_RegisterFunctionPtr("SDL_StartTextInput",FFI_SDL_StartTextInput);
    STK_RegisterFunctionPtr("SDL_StopTextInput",FFI_SDL_StopTextInput);
    STK_RegisterFunctionPtr("SDL_GetError",FFI_SDL_GetError);
    STK_RegisterFunctionPtr("SDL_ClearError",FFI_SDL_ClearError);
    STK_RegisterFunctionPtr("SDL_FlushEvent",FFI_SDL_FlushEvent);
    STK_RegisterFunctionPtr("SDL_GetSurfaceWidth",FFI_GetSurfaceW);
    STK_RegisterFunctionPtr("SDL_GetSurfaceHeight",FFI_GetSurfaceH);
    STK_RegisterFunctionPtr("SDL_CreateRGBSurface",FFI_SDL_CreateRGBSurface);
    STK_RegisterFunctionPtr("SDL_CreateRGBSurfaceFrom",FFI_SDL_CreateRGBSurfaceFrom);
    STK_RegisterFunctionPtr("SDL_UpperBlit",FFI_SDL_UpperBlit);
    STK_RegisterFunctionPtr("SDL_FillRect",FFI_SDL_FillRect);
    STK_RegisterFunctionPtr("SDL_FillRects",FFI_SDL_FillRects);
    STK_RegisterFunctionPtr("SDL_GetClipRect",FFI_SDL_GetClipRect);
    STK_RegisterFunctionPtr("SDL_GetColorKey",FFI_SDL_GetColorKey);
    STK_RegisterFunctionPtr("SDL_GetSurfaceAlphaMod",FFI_SDL_GetSurfaceAlphaMod);
    STK_RegisterFunctionPtr("SDL_GetSurfaceColorMod",FFI_SDL_GetSurfaceColorMod);
    STK_RegisterFunctionPtr("SDL_LockSurface",FFI_SDL_LockSurface);
    STK_RegisterFunctionPtr("SDL_UnlockSurface",FFI_SDL_UnlockSurface);
    STK_RegisterFunctionPtr("SDL_SetClipRect",FFI_SDL_SetClipRect);
    STK_RegisterFunctionPtr("SDL_SetColorKey",FFI_SDL_SetColorKey);
    STK_RegisterFunctionPtr("SDL_SetSurfaceAlphaMod",FFI_SDL_SetSurfaceAlphaMod);
    STK_RegisterFunctionPtr("SDL_SetSurfaceColorMod",FFI_SDL_SetSurfaceColorMod);
    STK_RegisterFunctionPtr("SDL_SetSurfaceRLE",FFI_SDL_SetSurfaceRLE);
    STK_RegisterFunctionPtr("SDL_BlitSurface",FFI_SDL_BlitSurface);
    STK_RegisterFunctionPtr("SDL_BlitScaled",FFI_SDL_BlitScaled);
    STK_RegisterFunctionPtr("SDL_FreeSurface",FFI_SDL_FreeSurface);
    STK_RegisterFunctionPtr("SDL_UpdateWindowSurface",FFI_SDL_UpdateWindowSurface);
    STK_RegisterFunctionPtr("SDL_GetWindowSurface",FFI_SDL_GetWindowSurface);
    STK_RegisterFunctionPtr("__GetBuiltinMacrosText",FFI_GetBuiltinMacrosText);
}
static void *GetType(void *fp,char *name,long ptr_level) {
    CSymbol *gt;
	gt=map_get(&Loader.symbols,"GetType");
	if(!gt) return NULL;
	fp=gt->value_ptr;
	return FFI_CALL_TOS_2(gt->value_ptr,name,ptr_level);
}
static void *__LoaderCreateTypeFwd(void *fp,char *name,long sz,long align) {
    CSymbol *gt;
	gt=map_get(&Loader.symbols,"LoaderCreateTypeFwd");
	if(!gt) return NULL;
	return FFI_CALL_TOS_3(gt->value_ptr,name,sz,align);
}
#define LoaderCreateTypeFwd(fp,t) __LoaderCreateTypeFwd(fp,#t,sizeof(t),8);
static void __LoaderAddMember(void *fp,void *dst,void *t,char *name,long off) {
    CSymbol *gt;
	gt=map_get(&Loader.symbols,"LoaderAddMember");
	if(!gt) return NULL;
    return FFI_CALL_TOS_4(gt->value_ptr,dst,t,name,off);
}
static CType *PrimType(void *fp,int64_t sz) {
    switch(sz) {
    case 1:
        return GetType(fp,"I8i",0);
    case 2:
        return GetType(fp,"I16i",0);
    case 4:
        return GetType(fp,"I32i",0);
    case 8:
        return GetType(fp,"I64i",0);
    default: abort();
    }
}
static CType *LoaderCreateArrayType(void *fp,char *name,int64_t dim) {
    CSymbol *gt;
	gt=map_get(&Loader.symbols,"LoaderCreateArrayType");
	if(!gt) return NULL;
	fp=gt->value_ptr;
    return FFI_CALL_TOS_2(fp,name,dim);
}
#define LoaderAddMember(fp,d,t,ct,mem) __LoaderAddMember(fp,d,t,#mem,offsetof(ct,mem));
#define LoaderAddPrimMember(fp,gt,d,ct,mem) __LoaderAddMember(fp,d,PrimType(gt,sizeof(((ct*)NULL)->mem)),#mem,offsetof(ct,mem));
static void CreateMacroInt(vec_char_t *to,char *name,int64_t value) {
    char buffer[1024];
    sprintf(buffer,"#define %s %ld\n",name,value);
    vec_pusharr(to,buffer,strlen(buffer));
}
vec_char_t CreateMacros() {
    vec_char_t macros;
    vec_init(&macros);
    //SIGNALS
    CreateMacroInt(&macros,"SIGILL",SIGILL);
    CreateMacroInt(&macros,"SIGABRT",SIGABRT);
    CreateMacroInt(&macros,"SIGFPE",SIGFPE);
    #ifndef TARGET_WIN32
    CreateMacroInt(&macros,"SIGKILL",SIGKILL);
    CreateMacroInt(&macros,"SIGBUS",SIGBUS);
    CreateMacroInt(&macros,"SIGSTOP",SIGSTOP);
    #endif
    CreateMacroInt(&macros,"SIGSEGV",SIGSEGV);
    CreateMacroInt(&macros,"SIGTERM",SIGTERM);
    CreateMacroInt(&macros,"SDL_INIT_TIMER",SDL_INIT_TIMER);
    CreateMacroInt(&macros,"SDL_INIT_AUDIO",SDL_INIT_AUDIO);
    CreateMacroInt(&macros,"SDL_INIT_VIDEO",SDL_INIT_VIDEO);
    CreateMacroInt(&macros,"SDL_INIT_EVENTS",SDL_INIT_EVENTS);
    CreateMacroInt(&macros,"SDL_INIT_EVERYTHING",SDL_INIT_EVERYTHING);
    CreateMacroInt(&macros,"SDL_BLENDMODE_NONE",SDL_BLENDMODE_NONE);
    CreateMacroInt(&macros,"SDL_BLENDMODE_BLEND",SDL_BLENDMODE_BLEND);
    CreateMacroInt(&macros,"SDL_BLENDMODE_ADD",SDL_BLENDMODE_ADD);
    CreateMacroInt(&macros,"SDL_BLENDMODE_MOD",SDL_BLENDMODE_MOD);
    CreateMacroInt(&macros,"SDL_WINDOWPOS_UNDEFINED",SDL_WINDOWPOS_UNDEFINED);
    CreateMacroInt(&macros,"SDL_WINDOWPOS_CENTERED",SDL_WINDOWPOS_CENTERED);
    CreateMacroInt(&macros,"SDL_WINDOW_FULLSCREEN",SDL_WINDOW_FULLSCREEN);
    CreateMacroInt(&macros,"SDL_WINDOW_FULLSCREEN_DESKTOP",SDL_WINDOW_FULLSCREEN_DESKTOP);
    CreateMacroInt(&macros,"SDL_WINDOW_RESIZABLE",SDL_WINDOW_RESIZABLE);
    CreateMacroInt(&macros,"SDL_WINDOW_MINIMIZED",SDL_WINDOW_MINIMIZED);
    CreateMacroInt(&macros,"SDL_WINDOW_MAXIMIZED",SDL_WINDOW_MAXIMIZED);
    CreateMacroInt(&macros,"SDL_RENDERER_SOFTWARE",SDL_RENDERER_SOFTWARE);
    CreateMacroInt(&macros,"SDL_RENDERER_ACCELERATED",SDL_RENDERER_ACCELERATED);
    CreateMacroInt(&macros,"SDL_RENDERER_PRESENTVSYNC",SDL_RENDERER_PRESENTVSYNC);
    CreateMacroInt(&macros,"SDL_RENDERER_TARGETTEXTURE",SDL_RENDERER_TARGETTEXTURE);
    CreateMacroInt(&macros,"SDL_TEXTUREACCESS_TARGET",SDL_TEXTUREACCESS_TARGET);
    CreateMacroInt(&macros,"SDL_TEXTUREACCESS_STREAMING",SDL_TEXTUREACCESS_STREAMING);
    CreateMacroInt(&macros,"SDL_TEXTUREACCESS_STATIC",SDL_TEXTUREACCESS_STATIC);
    CreateMacroInt(&macros,"SDL_PIXELFORMAT_RGBA8888",SDL_PIXELFORMAT_RGBA8888);
    CreateMacroInt(&macros,"SDL_WINDOWEVENT",SDL_WINDOWEVENT);
    CreateMacroInt(&macros,"SDL_WINDOWEVENT_RESIZED",SDL_WINDOWEVENT_RESIZED);
    CreateMacroInt(&macros,"SDL_KEYUP",SDL_KEYUP);
    CreateMacroInt(&macros,"SDL_KEYDOWN",SDL_KEYDOWN);
    CreateMacroInt(&macros,"KMOD_LSHIFT",KMOD_LSHIFT);
    CreateMacroInt(&macros,"KMOD_RSHIFT",KMOD_RSHIFT);
    CreateMacroInt(&macros,"KMOD_SHIFT",KMOD_SHIFT);
    CreateMacroInt(&macros,"KMOD_LCTRL",KMOD_LCTRL);
    CreateMacroInt(&macros,"KMOD_RCTRL",KMOD_RCTRL);
    CreateMacroInt(&macros,"KMOD_CTRL",KMOD_CTRL);
    CreateMacroInt(&macros,"KMOD_LALT",KMOD_LALT);
    CreateMacroInt(&macros,"KMOD_RALT",KMOD_RALT);
    CreateMacroInt(&macros,"KMOD_ALT",KMOD_ALT);
    CreateMacroInt(&macros,"KMOD_LGUI",KMOD_LGUI);
    CreateMacroInt(&macros,"KMOD_RGUI",KMOD_RGUI);
    CreateMacroInt(&macros,"KMOD_GUI",KMOD_GUI);
    CreateMacroInt(&macros,"KMOD_CAPS",KMOD_CAPS);
    CreateMacroInt(&macros,"SDL_TEXTEDITING",SDL_TEXTEDITING);
    CreateMacroInt(&macros,"SDL_TEXTINPUT",SDL_TEXTINPUT);
    CreateMacroInt(&macros,"SDL_MOUSEMOTION",SDL_MOUSEMOTION);
    CreateMacroInt(&macros,"SDL_BUTTON_LMASK",SDL_BUTTON_LMASK);
    CreateMacroInt(&macros,"SDL_BUTTON_RMASK",SDL_BUTTON_RMASK);
    CreateMacroInt(&macros,"SDL_BUTTON_MMASK",SDL_BUTTON_MMASK);
    CreateMacroInt(&macros,"SDL_MOUSEBUTTONUP",SDL_MOUSEBUTTONUP);
    CreateMacroInt(&macros,"SDL_MOUSEBUTTONDOWN",SDL_MOUSEBUTTONDOWN);
    CreateMacroInt(&macros,"SDL_PRESSED",SDL_PRESSED);
    CreateMacroInt(&macros,"SDL_RELEASED",SDL_RELEASED);
    CreateMacroInt(&macros,"SDL_QUIT",SDL_QUIT);
    return macros;
}
char *GetBuiltinMacrosText() {
    vec_char_t macros=CreateMacros();
    vec_push(&macros,0);
    return macros.data;
}
void RegisterRuntimeClasses(void *gt,void *ctf,void *add_mem,void *c_arr_t) {
    //SDL2 bindings
    __LoaderCreateTypeFwd(ctf,"SDL_Window",0,1);
    __LoaderCreateTypeFwd(ctf,"SDL_Renderer",0,1);
    __LoaderCreateTypeFwd(ctf,"SDL_Texture",0,1);
    LoaderCreateTypeFwd(ctf,SDL_Surface);
    CType *sdlr_t=LoaderCreateTypeFwd(ctf,SDL_Rect);
    LoaderAddPrimMember(add_mem,gt,sdlr_t,SDL_Rect,x);
    LoaderAddPrimMember(add_mem,gt,sdlr_t,SDL_Rect,y);
    LoaderAddPrimMember(add_mem,gt,sdlr_t,SDL_Rect,w);
    LoaderAddPrimMember(add_mem,gt,sdlr_t,SDL_Rect,h);
    CType *sdlp_t=LoaderCreateTypeFwd(ctf,SDL_Point);
    LoaderAddPrimMember(add_mem,gt,sdlp_t,SDL_Point,x);
    LoaderAddPrimMember(add_mem,gt,sdlp_t,SDL_Point,y);

    CType *sdlevent_t=LoaderCreateTypeFwd(ctf,SDL_Event);
    {
        LoaderAddPrimMember(add_mem,gt,sdlevent_t,SDL_Event,type);
        {
            CType *sdlevent_wind_t=LoaderCreateTypeFwd(ctf,SDL_WindowEvent);
            LoaderAddPrimMember(add_mem,gt,sdlevent_wind_t,SDL_WindowEvent,type);
            LoaderAddPrimMember(add_mem,gt,sdlevent_wind_t,SDL_WindowEvent,timestamp);
            LoaderAddPrimMember(add_mem,gt,sdlevent_wind_t,SDL_WindowEvent,event);
            LoaderAddPrimMember(add_mem,gt,sdlevent_wind_t,SDL_WindowEvent,data1);
            LoaderAddPrimMember(add_mem,gt,sdlevent_wind_t,SDL_WindowEvent,data2);
            LoaderAddMember(add_mem,sdlevent_t,sdlevent_wind_t,SDL_Event,window);
        }
        {
            CType *sdlevent_key_t=LoaderCreateTypeFwd(ctf,SDL_KeyboardEvent);
            LoaderAddPrimMember(add_mem,gt,sdlevent_key_t,SDL_KeyboardEvent,type);
            LoaderAddPrimMember(add_mem,gt,sdlevent_key_t,SDL_KeyboardEvent,timestamp);
            LoaderAddPrimMember(add_mem,gt,sdlevent_key_t,SDL_KeyboardEvent,state);
            LoaderAddPrimMember(add_mem,gt,sdlevent_key_t,SDL_KeyboardEvent,repeat);
            //
            CType *sdlkeysym_t=LoaderCreateTypeFwd(ctf,SDL_Keysym);
            LoaderAddPrimMember(add_mem,gt,sdlkeysym_t,SDL_Keysym,scancode);
            LoaderAddPrimMember(add_mem,gt,sdlkeysym_t,SDL_Keysym,sym);
            LoaderAddPrimMember(add_mem,gt,sdlkeysym_t,SDL_Keysym,mod);
            //
            LoaderAddMember(add_mem,sdlevent_key_t,sdlkeysym_t,SDL_KeyboardEvent,keysym);
            LoaderAddMember(add_mem,sdlevent_t,sdlevent_key_t,SDL_Event,key);
        }
        {
            CType *sdlevent_edit_t=LoaderCreateTypeFwd(ctf,SDL_TextEditingEvent);
            LoaderAddPrimMember(add_mem,gt,sdlevent_edit_t,SDL_TextEditingEvent,type);
            LoaderAddPrimMember(add_mem,gt,sdlevent_edit_t,SDL_TextEditingEvent,timestamp);
            LoaderAddMember(add_mem,sdlevent_edit_t,LoaderCreateArrayType(c_arr_t,"U8i",32),SDL_TextEditingEvent,text);
            LoaderAddPrimMember(add_mem,gt,sdlevent_edit_t,SDL_TextEditingEvent,start);
            LoaderAddPrimMember(add_mem,gt,sdlevent_edit_t,SDL_TextEditingEvent,length);
            LoaderAddMember(add_mem,sdlevent_t,sdlevent_edit_t,SDL_Event,edit);
        }
        {
            CType *sdlevent_text_t=LoaderCreateTypeFwd(ctf,SDL_TextInputEvent);
            LoaderAddPrimMember(add_mem,gt,sdlevent_text_t,SDL_TextInputEvent,type);
            LoaderAddPrimMember(add_mem,gt,sdlevent_text_t,SDL_TextInputEvent,timestamp);
            LoaderAddMember(add_mem,sdlevent_text_t,LoaderCreateArrayType(c_arr_t,"U8i",32),SDL_TextInputEvent,text);
            LoaderAddMember(add_mem,sdlevent_t,sdlevent_text_t,SDL_Event,text);
        }
        {
            CType *sdlevent_mousemot_t=LoaderCreateTypeFwd(ctf,SDL_MouseMotionEvent);
            LoaderAddPrimMember(add_mem,gt,sdlevent_mousemot_t,SDL_MouseMotionEvent,type);
            LoaderAddPrimMember(add_mem,gt,sdlevent_mousemot_t,SDL_MouseMotionEvent,timestamp);
            LoaderAddPrimMember(add_mem,gt,sdlevent_mousemot_t,SDL_MouseMotionEvent,which);
            LoaderAddPrimMember(add_mem,gt,sdlevent_mousemot_t,SDL_MouseMotionEvent,state);
            LoaderAddPrimMember(add_mem,gt,sdlevent_mousemot_t,SDL_MouseMotionEvent,x);
            LoaderAddPrimMember(add_mem,gt,sdlevent_mousemot_t,SDL_MouseMotionEvent,y);
            LoaderAddPrimMember(add_mem,gt,sdlevent_mousemot_t,SDL_MouseMotionEvent,xrel);
            LoaderAddPrimMember(add_mem,gt,sdlevent_mousemot_t,SDL_MouseMotionEvent,yrel);
            LoaderAddMember(add_mem,sdlevent_t,sdlevent_mousemot_t,SDL_Event,motion);
            LoaderAddMember(add_mem,sdlevent_t,sdlevent_mousemot_t,SDL_Event,wheel);
        }
        {
            CType *sdlevent_mouse_t=LoaderCreateTypeFwd(ctf,SDL_MouseButtonEvent);
            LoaderAddPrimMember(add_mem,gt,sdlevent_mouse_t,SDL_MouseButtonEvent,type);
            LoaderAddPrimMember(add_mem,gt,sdlevent_mouse_t,SDL_MouseButtonEvent,timestamp);
            LoaderAddPrimMember(add_mem,gt,sdlevent_mouse_t,SDL_MouseButtonEvent,which);
            LoaderAddPrimMember(add_mem,gt,sdlevent_mouse_t,SDL_MouseButtonEvent,button);
            LoaderAddPrimMember(add_mem,gt,sdlevent_mouse_t,SDL_MouseButtonEvent,state);
            LoaderAddPrimMember(add_mem,gt,sdlevent_mouse_t,SDL_MouseButtonEvent,clicks);;
            LoaderAddMember(add_mem,sdlevent_t,sdlevent_mouse_t,SDL_Event,button);
        }
        {
            CType *sdlevent_quit_t=LoaderCreateTypeFwd(ctf,SDL_QuitEvent);
            LoaderAddPrimMember(add_mem,gt,sdlevent_quit_t,SDL_QuitEvent,type);
            LoaderAddPrimMember(add_mem,gt,sdlevent_quit_t,SDL_QuitEvent,timestamp);
            LoaderAddMember(add_mem,sdlevent_t,sdlevent_quit_t,SDL_Event,quit);
        }
    }
}
void RegisterMacrosAndREPL(char *includes,int flags,char *body_code) {
    vec_char_t macros=CreateMacros();
    const char *rtc="#define RUNTIME_C\n";
    vec_pusharr(&macros,rtc,strlen(rtc));
    if(includes)
      vec_pusharr(&macros,includes,strlen(includes));
    vec_push(&macros,0);
    CSymbol *repl=map_get(&Loader.symbols,"REPL");
    assert(repl);
    for(;;) {
        if(!HCSetJmp(EnterTry())) {
            FFI_CALL_TOS_3(repl->value_ptr,macros.data,flags,body_code);
            break;
        } else {
            fprintf(stderr,"Caught exception,re-entering.\n");
        }
        if(macros.data)
            vec_deinit(&macros);
        macros.data=NULL;
    }
}
void RegisterMacrosAndCompile(char *includes,char *to_file,char *embed_header) {
    vec_char_t macros=CreateMacros();
    const char *rtc="#define RUNTIME_C\n";
    vec_pusharr(&macros,rtc,strlen(rtc));
    if(includes)
      vec_pusharr(&macros,includes,strlen(includes));
    vec_push(&macros,0);
    CSymbol *comp=map_get(&Loader.symbols,"CompileBinModule");
    FFI_CALL_TOS_3(comp->value_ptr,macros.data,to_file,embed_header);
}
