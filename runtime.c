#include "3d.h"
#include <stdio.h>
#include <stdarg.h>
#include <ctype.h>
#include <string.h>
#include <math.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#ifdef TARGET_WIN32
#include <windows.h>
#include <fileapi.h>
#endif
#include <curses.h>
typedef struct TOS_Fs {
    uint64_t except_ch;
    uint64_t catch_except;
    int64_t rand_seed;
} TOS_Fs;
static __thread TOS_Fs Fs;
void *GetFs() {
    return &Fs;
}
typedef struct ExceptFrame {
    ExceptBuf pad;
    struct ExceptFrame *parent;
    long callStackSize;
} ExceptFrame;
static __thread ExceptFrame *curframe;
static ExceptFrame *EnterTry() {
    ExceptFrame *new=TD_MALLOC(sizeof(ExceptFrame));
    new->parent=curframe;
    curframe=new;
    new->callStackSize=Debugger.callStack.length;
    return new;
}
static void PopTryFrame() {
    ExceptFrame *c=curframe;
    ExceptFrame *par=curframe->parent;
    TD_FREE(curframe);
    curframe=par;
}
static void GCollect() {
  GC_Enable();
  GC_Collect();
  GC_Disable();
}
static void throw(uint64_t val) {
    assert(curframe); //TODO
    Fs.except_ch=val;
    Fs.catch_except=0;
    ExceptFrame old=*curframe;
    vec_truncate(&Debugger.callStack,curframe->callStackSize);
    PopTryFrame();
    HCLongJmp(old.pad);
}
/**
 * If Fs.catch_except is not set,it will throw again
 */
static void ExitCatch() {
    if(!Fs.catch_except) {
        assert(curframe); //TODO
        ExceptFrame old=*curframe;
        PopTryFrame();
        HCLongJmp(old.pad);
    }
}
static struct jit_op *CompileEnterTry() {
    jit_prepare(Compiler.JIT);
    jit_call(Compiler.JIT,EnterTry);
    jit_retval(Compiler.JIT, R(0));
    jit_prepare(Compiler.JIT);
    jit_putargr(Compiler.JIT, R(0));
    jit_putargi(Compiler.JIT, 1);
#ifndef TARGET_WIN32
    jit_call(Compiler.JIT,&HCSetJmp);
#else
    jit_call(Compiler.JIT,&HCSetJmp);
#endif
    jit_retval(Compiler.JIT, R(0));
    return jit_bnei(Compiler.JIT, (jit_value)NULL,R(0),0);
}
static void CompileCatch(AST *catch,struct jit_op *patch) {
    /**
     * try {
     *    ...
     */
    //Pop the exception frame,we shouldnt have thrown if we are here
    jit_prepare(Compiler.JIT);
    jit_call(Compiler.JIT, PopTryFrame);
    struct jit_op *jmp=jit_jmpi(Compiler.JIT, (jit_value)NULL);
    /**
     * catch {...}
     */
    jit_patch(Compiler.JIT, patch);
    __CompileAST(catch);
    //Throw again
    ReleaseValue(&vec_pop(&Compiler.valueStack));
    jit_prepare(Compiler.JIT);
    jit_call(Compiler.JIT, ExitCatch);
    jit_patch(Compiler.JIT,jmp);
}
void CompileTry(AST *t) {
    struct jit_op *cat=CompileEnterTry();
    __CompileAST(t->try.try);
    ReleaseValue(&vec_pop(&Compiler.valueStack));
    CompileCatch(t->try.catch,cat);
}
static int64_t BFFS(int64_t v) {
    return __builtin_ffsl(v);
}
static int64_t BCLZ(int64_t v) {
    return __builtin_clzl(v);
}
static void CreateBuiltin(void *fptr,CType *rtype,char *name,int hasvargs,...) {
    va_list list;
    va_start(list,hasvargs);
    CType *ftype=TD_MALLOC(sizeof(CType));
    ftype->refCnt=1;
    ftype->func.ret=rtype;
    ftype->func.hasvargs=hasvargs;
    ftype->type=TYPE_FUNC;
    CType *argtype;
    while(argtype=va_arg(list,CType*)) {
        vec_push(&ftype->func.arguments,argtype);
        vec_push(&ftype->func.dftArgs,NULL);
        vec_push(&ftype->func.names,NULL);
    }
    va_end(list);
    CFunction *func=TD_MALLOC(sizeof(CType));
    func->type=ftype;
    func->funcptr=fptr;
    CVariable *var=TD_MALLOC(sizeof(CType));
    var->isGlobal=1,var->isFunc=1;
    var->func=func;
    var->type=ftype;
    map_set(&Compiler.globals, name, var);
}
static AST *CreateDummyName(char *text) {
    AST *t=TD_MALLOC(sizeof(AST));
    t->refCnt=1;
    t->type=AST_NAME;
    t->name=strdup(text);
    return t;
}
static char *__GetStr() {
    return rl("");
}
static FILE *FOpen(char *fn,char *flags,long cnt) {
    return fopen(fn,flags);
}
static void FClose(FILE *f) {
    fclose(f);
}
static long FSize(FILE *f) {
    long r=ftell(f);
    fseek(f, 0, SEEK_END);
    long end=ftell(f);
    fseek(f, 0, SEEK_SET);
    long ret=end-ftell(f);
    fseek(f,r, SEEK_SET);
    return ret;
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
    char *ret=strdup(d);
    free(d);
    return ret;
#else
    char buf[1024];
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
static void* FileRead(char *fn,int64_t *sz) {
    FILE *f=fopen(fn,"rb");
    if(!f) return NULL;
    long len=FSize(f);
    fseek(f,  0, SEEK_SET);
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
static int64_t ColorPair(int64_t i) {
    return COLOR_PAIR(i);
}
void CreateMacroInt(char *name,int64_t i) {
    char buffer[128];
    sprintf(buffer,"%ld",i);
    CMacro macro= {strdup(name),strdup(buffer)};
    map_set(&Lexer.macros,name,macro);
}
static void WMove(void *w,int64_t y,int64_t x) {
    wmove(w, y, x);
}
static int64_t Lines() {
    return LINES;
}
static int64_t Cols() {
    return COLS;
}
static void WPrint(WINDOW *wind,char *fmt,int64_t argc,void *argv) {
    char *s=MStrPrint(fmt,argc,argv);
    wprintw(wind,"%s",s);
    TD_FREE(s);
}
//Sign extend to 64bits
static int64_t GetCH() {
    return getch();
}
//Sign extend to 64bits
static int64_t WGetCH(void *w) {
    return wgetch(w);
}
void GetParYX(WINDOW *w,int64_t *y,int64_t *x) {
    int y2,x2;
    getparyx(w, y2, x2);
    if(y) *y=y2;
    if(x) *x=x2;
}
WINDOW *StdScr() {
    return stdscr;
}
void CreateTagsAndErrorFiles(char *dest,char *errs,char *root) {
  if(!root) {
    char buffer[2048];
    sprintf(buffer,"%s -s -t %s -e %s",CompilerPath,dest,errs);
    //printf("%s\n",buffer);
    system(buffer);
  } else {
    char buffer[2048];
    sprintf(buffer,"%s -s -t %s -e %s %s",CompilerPath,dest,errs,root);
    //printf("%s\n",buffer);
    system(buffer);
  }
}
static void Test(int64_t a,int64_t b,int64_t c,int64_t d,int64_t e,int64_t f,int64_t  g) {
  printf("%lld,%lld,%lld,%lld,%lld,%lld,%lld\n",a,b,c,d,e,f,g);
}
//https://docs.microsoft.com/en-us/cpp/build/x64-calling-convention?view=msvc-160
static void TestMixed(int a, double b, int c, double d, int e, double f) {
  printf("%d,%lf,%d,%lf,%d,%lf\n",a,b,c,d,e,f);
}
static void MVWCHGAT(WINDOW *w,int64_t y,int64_t x,int64_t n,int64_t a,int64_t c,void *opts) {
  register long rsp asm ("rsp");
  assert(rsp%16==0);
  FILE *f=fopen("TAODS","w");
  double toads=a+1.2;
  fprintf(f,"%lld,%lld,%lld,%lld,%lld,%lld,%lld,%p\n",w,y,x,n,A_REVERSE,c,opts,rsp);
  fclose(f);
  mvwchgat(w,y,x,n,a,c,opts);
}
void RegisterBuiltins() {
    CType *u8 =CreatePrimType(TYPE_U8);
    CType *u8p =CreatePtrType(u8);
    CType *u8pp =CreatePtrType(u8p);
    CType *u0 =CreatePrimType(TYPE_U0);
    CType *u0p =CreatePtrType(u0);
    CType *i64 =CreatePrimType(TYPE_I64);
    CType *f64 =CreatePrimType(TYPE_F64);
    CType *i64p =CreatePtrType(i64);
    CType *cfs =CreateClassForwardDecl(NULL, CreateDummyName("CFs"));
    CType *cfsptr =CreatePtrType(cfs);
    CType *cfile =CreateClassForwardDecl(NULL, CreateDummyName("CFile"));
    CType *cfileptr =CreatePtrType(cfile);
    CType *wind =CreateClassForwardDecl(NULL, CreateDummyName("WINDOW"));
    CType *windp =CreatePtrType(wind);
    CreateBuiltin(&GCollect,u0,"GC_Collect",0,NULL);
    CreateBuiltin(&CreateTagsAndErrorFiles,u0,"CreateTagsAndErrorFiles",0,u8p,u8p,u8p,NULL);
    CreateBuiltin(&MSize, i64, "MSize",0, u0p,NULL);
    CreateBuiltin(&BFFS, i64, "Bsf",0, i64,NULL);
    CreateBuiltin(&BCLZ, i64, "Bsr",0, i64,NULL);
    CreateBuiltin(&nonl,u0,"nonl",0,NULL);
    CreateBuiltin(&throw, u0, "throw",0, i64,NULL);
    CreateBuiltin(&GC_Malloc, u0p, "MAlloc",0, i64,NULL);
    CreateBuiltin(&GC_Free, u0, "Free",0, u0p,NULL);
    CreateBuiltin(&MemNCpy, u0p, "MemCpy",0, u0p,u0p,i64,NULL);
    CreateBuiltin(&MemNCpy, u0p, "MemNCpy",0, u0p,u0p,i64,NULL);
    CreateBuiltin(&strlen, i64, "StrLen",0, u8p,NULL);
    CreateBuiltin(&strcmp, i64, "StrCmp",0, u8p,u8p,NULL);
    CreateBuiltin(&EnterDebugger, u0, "Debugger",0,NULL);
    CreateBuiltin(&strncmp, i64, "StrNCmp",0,u8p,u8p,i64,NULL);
    CreateBuiltin(&strcpy, u8p, "StrCpy",0,u8p,u8p,NULL);
    CreateBuiltin(&strncpy, u8p, "StrNCpy",0,u8p,u8p,i64,NULL);
    CreateBuiltin(&strstr, u8p, "StrMatch",0,u8p,u8p,NULL);
    CreateBuiltin(&GetFs, cfsptr, "Fs",0,NULL);
    CreateBuiltin(&__GetStr, u8p, "__GetStr",0,NULL);
    CreateBuiltin(&atan, f64,"ATan", 0,f64,NULL);
    CreateBuiltin(&fabs, f64, "Abs",0,f64,NULL);
    CreateBuiltin(&cos, f64, "Cos", 0,f64,NULL);
    CreateBuiltin(&sin, f64, "Sin", 0,f64,NULL);
    CreateBuiltin(&sqrt, f64, "Sqrt", 0,f64,NULL);
    CreateBuiltin(&tan, f64, "Tan", 0,f64,NULL);
    CreateBuiltin(&ceil, f64, "Ceil", 0,f64,NULL);
    CreateBuiltin(&log, f64, "Ln", 0,f64,NULL);
    CreateBuiltin(&log10, f64, "Log10", 0,f64,NULL);
    CreateBuiltin(&log2, f64, "Log2", 0,f64,NULL);
    CreateBuiltin(&round, f64, "Round", 0,f64,NULL);
    CreateBuiltin(&trunc, f64, "Trunc", 0,f64,NULL);
    CreateBuiltin(&exit, u0, "Exit", 0,i64,NULL);
    CreateBuiltin(&FOpen, cfileptr, "FOpen", 0,u8p,u8p,i64,NULL);
    CreateBuiltin(&FClose, u0, "FClose",0, cfileptr,NULL);
    CreateBuiltin(&FSize, i64, "FSize", 0,cfileptr,NULL);
    CreateBuiltin(&Cd, i64, "Cd", 0,u8p,NULL);
    CreateBuiltin(&DirCur, u8p, "DirCur",0,NULL);
    CreateBuiltin(&DirMk, i64, "DirMk",0,u8p,NULL);
    CreateBuiltin(&__Move, i64, "__Move",0,u8p,u8p,NULL);
    CreateBuiltin(&FileNameAbs, u8p, "FileNameAbs",0,u8p,NULL);
    CreateBuiltin(&FileNameAbs, u8p, "DirNameAbs",0,u8p,NULL);
    CreateBuiltin(&IsDir, i64, "IsDir",0,u8p,NULL);
    CreateBuiltin(&FileWrite, i64, "FileWrite",0,u8p,u0p,i64,NULL);
    CreateBuiltin(&FileRead, u8p, "FileRead",0,u8p,i64p,NULL);
    CreateBuiltin(&abort, u0, "abort",0,NULL);
    CreateBuiltin(&__Dir, u8pp, "__Dir",0,u8p,NULL);
    CreateBuiltin(&IsWindows, i64, "IsWindows",0,NULL);
    CreateBuiltin(&MStrPrint, u8p, "MStrPrint",1,u8p,NULL);
    CreateBuiltin(&StreamPrint, u0, "StreamPrint",1,u8p,NULL);
    CreateBuiltin(&memset, u0p, "MemSet",0,u8p,u8,i64,NULL);
    CreateBuiltin(&GetCH,u0,"getch",0,NULL);
    CreateBuiltin(&redrawwin,u0,"redrawwin",0,windp,NULL);
    CreateBuiltin(&MVWCHGAT,u0,"mvwchgat",0,windp,i64,i64,i64,i64,i64,u0p,NULL);
    CreateBuiltin(&Test,u0,"Test",0,i64,i64,i64,i64,i64,i64,i64,NULL);
    CreateBuiltin(&TestMixed,u0,"TestMixed",0,i64,f64,i64,f64,i64,f64,NULL);
    CreateBuiltin(&wclear,u0,"wclear",0,windp,NULL);
    CreateBuiltin(&flushinp,u0,"flushinp",0,NULL);
    //curses
    CreateMacroInt("COLOR_RED", COLOR_RED);
    CreateMacroInt("COLOR_BLACK",COLOR_BLACK);
    CreateMacroInt("COLOR_BLUE",COLOR_BLUE);
    CreateMacroInt("COLOR_GREEN",COLOR_GREEN);
    CreateMacroInt("COLOR_YELLOW",COLOR_YELLOW);
    CreateMacroInt("COLOR_MAGENTA",COLOR_MAGENTA);
    CreateMacroInt("COLOR_CYAN",COLOR_CYAN);
    CreateMacroInt("COLOR_WHITE",COLOR_WHITE);
    CreateMacroInt("ERR",ERR);
    CreateMacroInt("A_NORMAL",A_NORMAL);
    CreateMacroInt("A_STANDOUT",A_STANDOUT);
    CreateMacroInt("A_UNDERLINE",A_UNDERLINE);
    CreateMacroInt("A_REVERSE",A_REVERSE);
    CreateMacroInt("A_DIM",A_DIM);
    CreateMacroInt("A_BOLD",A_BOLD);
    CreateMacroInt("KEY_LEFT",KEY_LEFT);
    CreateMacroInt("KEY_RIGHT",KEY_RIGHT);
    CreateMacroInt("KEY_UP",KEY_UP);
    CreateMacroInt("KEY_DOWN",KEY_DOWN);
    CreateMacroInt("KEY_RESIZE",KEY_RESIZE);
    CreateBuiltin(&resize_term, u0, "resize_term", 0, i64, i64, NULL);
    CreateBuiltin(&clear, u0, "clear", 0, NULL);
#ifdef TARGET_WIN32
    CreateBuiltin(&is_termresized, u8, "is_termresized", 0, NULL);
    CreateMacroInt("KEY_BACKSPACE",KEY_BACKSPACE);
    CreateMacroInt("KEY_F1",KEY_F0+1);
    CreateMacroInt("KEY_F2",KEY_F0+2);
    CreateMacroInt("KEY_F3",KEY_F0+3);
    CreateMacroInt("KEY_F4",KEY_F0+4);
    CreateMacroInt("KEY_F5",KEY_F0+5);
    CreateMacroInt("KEY_F6",KEY_F0+6);
    CreateMacroInt("KEY_F7",KEY_F0+7);
    CreateMacroInt("KEY_F8",KEY_F0+8);
    CreateMacroInt("KEY_F9",KEY_F0+9);
    CreateMacroInt("KEY_F11",KEY_F0+10);
    CreateMacroInt("KEY_F10",KEY_F0+11);
    CreateMacroInt("KEY_F12",KEY_F0+12);
    CreateMacroInt("KEY_NPAGE",KEY_NPAGE);
    CreateMacroInt("KEY_PPAGE",KEY_PPAGE);
    CreateMacroInt("KEY_ENTER",KEY_ENTER);
    CreateMacroInt("KEY_HOME",KEY_HOME);
    CreateMacroInt("KEY_END",KEY_END);
    CreateMacroInt("KEY_BTAB",KEY_BTAB);
    CreateMacroInt("KEY_TAB",KEY_STAB);
    CreateMacroInt("ALT_A",ALT_A);
CreateMacroInt("ALT_B",ALT_B);
CreateMacroInt("ALT_C",ALT_C);
CreateMacroInt("ALT_D",ALT_D);
CreateMacroInt("ALT_E",ALT_E);
CreateMacroInt("ALT_F",ALT_F);
CreateMacroInt("ALT_G",ALT_G);
CreateMacroInt("ALT_H",ALT_H);
CreateMacroInt("ALT_I",ALT_I);
CreateMacroInt("ALT_J",ALT_J);
CreateMacroInt("ALT_K",ALT_K);
CreateMacroInt("ALT_L",ALT_L);
CreateMacroInt("ALT_M",ALT_M);
CreateMacroInt("ALT_N",ALT_N);
CreateMacroInt("ALT_O",ALT_O);
CreateMacroInt("ALT_P",ALT_P);
CreateMacroInt("ALT_Q",ALT_Q);
CreateMacroInt("ALT_R",ALT_R);
CreateMacroInt("ALT_S",ALT_S);
CreateMacroInt("ALT_T",ALT_T);
CreateMacroInt("ALT_U",ALT_U);
CreateMacroInt("ALT_V",ALT_V);
CreateMacroInt("ALT_W",ALT_W);
CreateMacroInt("ALT_X",ALT_X);
CreateMacroInt("ALT_Y",ALT_Y);
CreateMacroInt("ALT_Z",ALT_Z);
    #endif
    CreateBuiltin(&standend,u0,"standend",0,NULL);
    CreateBuiltin(&newwin, windp, "newwin",0,i64,i64,i64,i64,NULL);
    CreateBuiltin(&delwin, u0, "delwin",0,windp,NULL);
    CreateBuiltin(&init_pair, u0, "init_pair", 0,i64, i64,i64,NULL);
    CreateBuiltin(&WMove, u0, "wmove", 0, windp,i64,i64,NULL);
    CreateBuiltin(&wattron, u0, "wattron", 0, windp,i64,NULL);
    CreateBuiltin(&wattroff, u0, "wattroff", 0,windp, i64,NULL);
    CreateBuiltin(&ColorPair,i64,"COLOR_PAIR",0,i64,NULL);
    CreateBuiltin(&initscr,windp,"initscr",0,NULL);
    CreateBuiltin(&raw,u0,"raw",0,NULL);
    CreateBuiltin(&cbreak,u0,"cbreak",0,NULL);
    CreateBuiltin(&echo,u0,"echo",0,NULL);
    CreateBuiltin(&noecho,u0,"noecho",0,NULL);
    CreateBuiltin(&keypad,u0,"keypad",0,windp,i64,NULL);
    CreateBuiltin(&WPrint,u0,"wprint",1,windp,u8p,NULL);
    CreateBuiltin(&refresh,u0,"refresh",0,NULL);
    CreateBuiltin(&wrefresh,u0,"wrefresh",0,windp,NULL);
    CreateBuiltin(&wresize,u0,"wresize",0,windp,i64,i64,NULL);
    CreateBuiltin(&getcurx,i64,"getcurx",0,windp,NULL);
    CreateBuiltin(&getcury,i64,"getcury",0,windp,NULL);
    CreateBuiltin(&endwin,i64,"endwin",0,NULL);
    CreateBuiltin(&mvwin,i64,"mvwin",0,windp,i64,i64,NULL);
    CreateBuiltin(&WGetCH,i64,"wgetch",0,windp,NULL);
    CreateBuiltin(&box,u0,"box",0,windp,i64,i64,NULL);
    CreateBuiltin(&start_color,u0,"start_color",0,NULL);
    CreateBuiltin(&getmaxx,i64,"getmaxx",0,windp,NULL);
    CreateBuiltin(&getmaxy,i64,"getmaxy",0,windp,NULL);
    CreateBuiltin(&wtimeout,u0,"wtimeout",0,windp,i64,NULL);
    CreateBuiltin(&timeout,u0,"timeout",0,i64,NULL);
    CreateBuiltin(&wscrl,u0,"wscrl",0,windp,i64,NULL);
    CreateBuiltin(&waddch,u0,"waddch",0,windp,u8,NULL);
    CreateBuiltin(&wclrtoeol,u0,"wclrtoeol",0,windp,NULL);
    CreateBuiltin(&Lines,i64,"LINES",0,NULL);
    CreateBuiltin(&Cols,i64,"COLS",0,NULL);
    CreateBuiltin(&wbkgdset,i64,"wbkgdset",0,windp,i64,NULL);
    CreateBuiltin(&curs_set,u0,"curs_set",0,i64,NULL);
    CreateBuiltin(&getbegx,i64,"getbegx",0,windp,NULL);
    CreateBuiltin(&getbegy,i64,"getbegy",0,windp,NULL);
    CreateBuiltin(&StdScr,windp,"stdscr",0,NULL);
}
