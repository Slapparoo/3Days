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
#include <SDL2/SDL.h>
#include <SDL2/SDL_video.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_clipboard.h>
#include <SDL2/SDL_events.h>
#include <stddef.h>
#include <stdalign.h>
#ifdef TARGET_WIN32
#include <windows.h>
#include <fileapi.h>
#endif
#include <curses.h>
typedef struct TOS_Fs {
    uint64_t except_ch;
    uint64_t catch_except;
    int64_t rand_seed;
    void *last_cc;
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
ExceptFrame *EnterTry() {
    ExceptFrame *new=TD_MALLOC(sizeof(ExceptFrame));
    new->parent=curframe;
    curframe=new;
    #ifdef BOOTSTRAPED
    new->callStackSize=Debugger.callStack.length;
    #endif
    return new;
}
static AST *HC_CreateI64(int64_t i) {
    AST *r=TD_MALLOC(sizeof(AST));
    r->type=AST_INT;
    r->integer=i;
    return r;
}
static AST *HC_CreateF64(double f) {
    AST *r=TD_MALLOC(sizeof(AST));
    r->type=AST_FLOAT;
    r->floating=f;
    return r;
}
static AST *HC_CreateStr(char *str) {
    AST *r=TD_MALLOC(sizeof(AST));
    r->type=AST_STRING;
    r->string=strdup(str);
    return r;
}
static AST *HC_SetPosFromLexer(AST *node,char *fn,long line) {
    node->fn=fn;
    node->ln=line;
    return node;
}
#include "LEXER.HH"
#include "HolyC.tab.h"
static AST *HC_CreateToken(int64_t tok) {
    AST *r=TD_MALLOC(sizeof(AST));
    r->type=AST_TOKEN;
    switch(tok) {
      case TK_EOF: r->tokenAtom=0;  break;
      case TK_PLUS_PLUS:  r->tokenAtom=HC_INC; break;
      case TK_MINUS_MINUS: r->tokenAtom=HC_DEC; break;
      case TK_DEREFERENCE: r->tokenAtom=HC_ARROW; break;
      case TK_DBL_COLON: r->tokenAtom=HC_DOUBLE_COLON; break;
      case TK_SHL: r->tokenAtom=HC_SHL; break;
      case TK_SHR: r->tokenAtom=HC_SHR; break;
      case TK_EQU_EQU: r->tokenAtom=HC_EQ; break;
      case TK_NOT_EQU: r->tokenAtom=HC_NE; break;
      case TK_LESS_EQU: r->tokenAtom=HC_LE; break;
      case TK_GREATER_EQU: r->tokenAtom=HC_GE; break;
      case TK_AND_AND: r->tokenAtom=HC_LAND; break;
      case TK_OR_OR: r->tokenAtom=HC_LOR; break;
      case TK_XOR_XOR: r->tokenAtom=HC_LXOR; break;
      case TK_SHL_EQU	: r->tokenAtom=HC_EQ_SHL; break;
      case TK_SHR_EQU:	r->tokenAtom=HC_EQ_SHR; break;
      case TK_MUL_EQU: r->tokenAtom=HC_EQ_MUL; break;
      case TK_DIV_EQU:	r->tokenAtom=HC_EQ_DIV; break;
      case TK_AND_EQU: r->tokenAtom=HC_EQ_BAND; break;
      case TK_OR_EQU: r->tokenAtom=HC_EQ_BOR; break;
      case TK_XOR_EQU: r->tokenAtom=HC_EQ_BXOR; break;
      case TK_ADD_EQU: r->tokenAtom=HC_EQ_ADD; break;
      case TK_SUB_EQU: r->tokenAtom=HC_EQ_SUB; break;
      case TK_IF: r->tokenAtom=HC_IF; break;
      case TK_ELSE: r->tokenAtom=HC_ELSE; break;
      case TK_MOD_EQU: r->tokenAtom=HC_EQ_MOD; break;
      case TK_ELLIPSIS:r->tokenAtom=HC_DOT_DOT_DOT; break;
      case '.': r->tokenAtom=HC_DOT; break;
      case '!': r->tokenAtom=HC_LNOT; break;
      case '~': r->tokenAtom=HC_BNOT; break;
      case '>': r->tokenAtom=HC_GT; break;
      case '<': r->tokenAtom=HC_LT; break;
      case '`': r->tokenAtom=HC_POW; break;
      case '*': r->tokenAtom=HC_MUL; break;
      case '/': r->tokenAtom=HC_DIV; break;
      case '%': r->tokenAtom=HC_MOD; break;
      case '+': r->tokenAtom=HC_ADD; break;
      case '-': r->tokenAtom=HC_SUB; break;
      case '=': r->tokenAtom=HC_ASSIGN; break;
      case ',': r->tokenAtom=HC_COMMA; break;
      case '{': r->tokenAtom=HC_LEFT_CURLY; break;
      case '[': r->tokenAtom=HC_LEFT_SQAURE; break;
      case '(': r->tokenAtom=HC_LEFT_PAREN; break;
      case '}': r->tokenAtom=HC_RIGHT_CURLY; break;
      case ']': r->tokenAtom=HC_RIGHT_SQAURE; break;
      case ')': r->tokenAtom=HC_RIGHT_PAREN; break;
      case ';': r->tokenAtom=HC_SEMI; break;
      case ':': r->tokenAtom=HC_COLON; break;
      case '&': r->tokenAtom=HC_BAND; break;
      case '|': r->tokenAtom=HC_BOR; break;
      case '^': r->tokenAtom=HC_BXOR; break;
      default: printf("Unkown token %d\n",tok); abort();
    }
    return r;
}
static AST *HC_CreateIdent(char *id) {
    AST *r=TD_MALLOC(sizeof(AST));
    const struct {
        const char *name;
        int code;
    } kws[]={{"union",HC_UNION},
        {"catch",HC_CATCH},
        {"class",HC_CLASS},
        {"try",HC_TRY},
        {"if",HC_IF},
        {"else",HC_ELSE},
        {"for",HC_FOR},
        {"while",HC_WHILE},
        {"extern",HC_EXTERN},
        {"_extern",HC_EXTERN2},
        {"return",HC_RET},
        {"sizeof",HC_SIZEOF},
        {"intern",HC_INTERN},
        {"do",HC_DO},
        {"goto",HC_GOTO},
        {"break",HC_BREAK},
        {"switch",HC_SWITCH},
        {"start",HC_START},
        {"end",HC_END},
        {"case",HC_CASE},
        {"default",HC_DEFAULT},
        {"public",HC_PUBLIC},
        {"import",HC_IMPORT},
        {"_import",HC_IMPORT2},
        {"lastclass",HC_LASTCLASS},
        {"static",HC_STATIC},
        {"DU8",HC_DU8},
        {"DU16",HC_DU16},
        {"DU32",HC_DU32},
        {"DU64",HC_DU64},
        {"ALIGN",HC_ALIGN},
        {"BINFILE",HC_BINFILE},
        {"asm",HC_ASM},
        {"IMPORT",HC_ASM_IMPORT},
    };
    long count=sizeof(kws)/sizeof(*kws);
    while(--count>=0) {
        if(!strcmp(kws[count].name,id)) {
            r->type=AST_TOKEN ;
            r->tokenAtom=kws[count].code;
            return r;
        }
    }
    r->type=AST_NAME;
    r->name=strdup(id);
    return r;
}
void PopTryFrame() {
    ExceptFrame *c=curframe;
    ExceptFrame *par=curframe->parent;
    TD_FREE(curframe);
    curframe=par;
}
static void GC_Free(void *ptr) {
    tgc_free(&gc,ptr);
}
static void *GC_Malloc(size_t sz) {
    return TD_MALLOC(sz);
}
static size_t MSize2(void *ptr) {
    return tgc_get_size(&gc,ptr);
}
static void GCollect() {
  tgc_resume(&gc);
  tgc_run(&gc);
  tgc_pause(&gc);
}
void throw(uint64_t val) {
    assert(curframe); //TODO
    Fs.except_ch=val;
    Fs.catch_except=0;
    ExceptFrame old=*curframe;
    #ifdef BOOTSTRAPED
    vec_truncate(&Debugger.callStack,curframe->callStackSize);
    #endif
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
    jit_value fr=MoveGlobalPtrToReg(GetVariable("EnterTry"), 2);
    jit_prepare(Compiler.JIT);
    jit_callr(Compiler.JIT,fr);
    jit_retval(Compiler.JIT, R(0));
    fr=MoveGlobalPtrToReg(GetVariable("HCSetJmp"), 2);
    jit_prepare(Compiler.JIT);
    jit_putargr(Compiler.JIT, R(0));
    jit_putargi(Compiler.JIT, 1);
#ifndef TARGET_WIN32
    jit_callr(Compiler.JIT,fr);
#else
    jit_callr(Compiler.JIT,fr);
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
    jit_value fr=MoveGlobalPtrToReg(GetVariable("PopTryFrame"), 2);
    jit_prepare(Compiler.JIT);
    jit_callr(Compiler.JIT, fr);
    struct jit_op *jmp=jit_jmpi(Compiler.JIT, (jit_value)NULL);
    /**
     * catch {...}
     */
    jit_patch(Compiler.JIT, patch);
    __CompileAST(catch);
    //Throw again
    ReleaseValue(&vec_pop(&Compiler.valueStack));
    fr=MoveGlobalPtrToReg(GetVariable("ExitCatch"), 2);
    jit_prepare(Compiler.JIT);
    jit_callr(Compiler.JIT, fr);
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
static int64_t PowI64(int64_t x,int64_t n) {
    if(n<0) return 0;
    if(n==0) return 1;
    int64_t y=1;
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
static void CreateBuiltin(void *fptr,CType *rtype,char *name,int hasvargs,...) {
    if(GetVariable(name)) return ;
    va_list list;
    va_start(list,hasvargs);
    CType *ftype=TD_MALLOC(sizeof(CType));
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
    CFunction *func=TD_MALLOC(sizeof(CFunction));
    func->type=ftype;
    func->funcptr=fptr;
    CVariable *var=TD_MALLOC(sizeof(CVariable));
    var->isGlobal=1,var->isFunc=1,var->isBuiltin=1;
    var->func=func;
    var->type=ftype;
    var->name=strdup(name);
    map_set(&Compiler.globals, name, var);
}
static AST *CreateDummyName(char *text) {
    AST *t=TD_MALLOC(sizeof(AST));
    t->refCnt=1;
    t->type=AST_NAME;
    t->name=strdup(text);
    return t;
}
static char *__GetStr(char*txt) {
    return rl(txt);
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
#ifndef BOOTSTRAPED
void CreateMacroInt(char *name,int64_t i) {
    char buffer[128];
    sprintf(buffer,"%ld",i);
    CMacro macro= {strdup(name),strdup(buffer)};
    map_set(&Lexer.macros,name,macro);
}
#else
void CreateMacroInt(char *name,int64_t i) {
    if(!GetVariable("LexIncludeStr")) return;
    char buffer[128];
    sprintf(buffer,"#define %s %ld\n",name,i);
    void(*inc)(void*cc,char *fn,char *src,int64_t act_f)=(void*)GetVariable("LexIncludeStr")->func->funcptr;
    inc(Lexer.HCLexer,"(nofile)",strdup(buffer),0);
}
#endif
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
static void LoadBinFile(char *name) {
  FILE *f=fopen(name,"rb");
  LoadAOTBin(f,0);
  fclose(f);
}
void CreateBinFile(char *bin,char *root) {
  char buffer[2048];
  strcpy(buffer,CompilerPath);
  strcat(buffer," -s -c ");
  strcat(buffer,bin);
  if(root) {
      strcat(buffer," ");
      strcat(buffer,root);
  }
  system(buffer);
}
void CreateTagsAndErrorsFiles(char *tags,char *errs,char *root) {
  char buffer[2048];
  strcpy(buffer,CompilerPath);
  strcat(buffer," -s");
  if(tags) {
        sprintf(buffer+strlen(buffer)," -t %s ",tags);
  }
  if(errs) {
        sprintf(buffer+strlen(buffer)," -e %s ",errs);
  }
  if(root) {
      strcat(buffer,root);
  }
  system(buffer);
}
static void Test(int64_t a,int64_t b,int64_t c,int64_t d,int64_t e,int64_t f,int64_t  g) {
  printf("%lld,%lld,%lld,%lld,%lld,%lld,%lld\n",a,b,c,d,e,f,g);
}
//https://docs.microsoft.com/en-us/cpp/build/x64-calling-convention?view=msvc-160
static void TestMixed(int a, double b, int c, double d, int e, double f) {
  printf("%d,%lf,%d,%lf,%d,%lf\n",a,b,c,d,e,f);
}
static void MVWCHGAT(WINDOW *w,int64_t y,int64_t x,int64_t n,int64_t a,int64_t c,void *opts) {
  mvwchgat(w,y,x,n,a,c,opts);
}
void AddMemberToClass(CType *cls,CType *t,char* name,long offset) {
    if(!cls) return;
    CMember mem;
    memset(&mem,0,sizeof(mem));
    mem.name=strdup(name);
    mem.offset=offset;
    mem.type=t;
    vec_push(&cls->cls.members,mem);
}
void AddMemberToClassBySize(CType *cls,long size,char* name,long offset) {
    if(!cls) return;
    CMember mem;
    memset(&mem,0,sizeof(mem));
    mem.name=strdup(name);
    mem.offset=offset;
    switch(size) {
    case 1: mem.type=CreatePrimType(TYPE_I8); break;
    case 2: mem.type=CreatePrimType(TYPE_I16); break;
    case 4: mem.type=CreatePrimType(TYPE_I32); break;
    case 8: mem.type=CreatePrimType(TYPE_I64); break;
    }
    vec_push(&cls->cls.members,mem);
}
void UAddMemberToClassBySize(CType *cls,long size,char* name,long offset) {
    if(!cls) return;
    CMember mem;
    memset(&mem,0,sizeof(mem));
    mem.name=strdup(name);
    mem.offset=offset;
    switch(size) {
    case 1: mem.type=CreatePrimType(TYPE_U8); break;
    case 2: mem.type=CreatePrimType(TYPE_U16); break;
    case 4: mem.type=CreatePrimType(TYPE_U32); break;
    case 8: mem.type=CreatePrimType(TYPE_U64); break;
    }
    vec_push(&cls->cls.members,mem);
}
static CType *CreateEmptyClass(char *name,long size,long align) {
    if(map_get(&Compiler.types,name)) return NULL;
    CType *t=TD_MALLOC(sizeof(CType));
    t->isBuiltin=1;
    t->type=TYPE_CLASS;
    t->cls.name=strdup(name);
    t->cls.size=size;
    t->cls.align=align;
    map_set(&Compiler.types,name,t);
    return t;
}
#define ADD_TYPED_MEMBER(hc,hct,t,mem) AddMemberToClass(hc,hct,#mem,offsetof(t,mem));
#define ADD_PRIM_MEMBER(hc,t,mem) AddMemberToClassBySize(hc,sizeof(((t*)NULL)->mem),#mem,offsetof(t,mem));
#define ADD_UPRIM_MEMBER(hc,t,mem) UAddMemberToClassBySize(hc,sizeof(((t*)NULL)->mem),#mem,offsetof(t,mem));
#define IMPORT_CLASS_WO_MEMBERS(type) CreateEmptyClass(#type,sizeof(type),alignof(type))
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
int64_t JIT_EvalExpr(char *t,char **end) {
    return EvalExprNoComma(t,end);
}
static double Bit4BitU64ToF64(uint64_t b) {
    union {double f;int64_t i;} val;
    val.i=b;
    return val.f;
}
static uint64_t Bit4BitF64ToU64(double b) {
    union {double f;int64_t i;} val;
    val.f=b;
    return val.i;
}
static double F64And(double a,double b) {
    return (*(uint64_t*)&a)&(*(uint64_t*)&b);
}
static double F64Or(double a,double b) {
    return (*(uint64_t*)&a)&(*(uint64_t*)&b);
}
static double F64Xor(double a,double b) {
    return (*(uint64_t*)&a)&(*(uint64_t*)&b);
}
static double F64Shl(double a,int64_t b) {
    return (*(uint64_t*)&a)<<b;
}
static double F64Shr(double a,int64_t b) {
    return (*(uint64_t*)&a)>>b;
}
void RegisterBuiltins() {
    //Primitive types
  CType *u0 =CreatePrimType(TYPE_U0);
  map_set(&Compiler.types, "U0", u0);
  CType *bl=CreatePrimType(TYPE_BOOL);
  map_set(&Compiler.types, "Bool", bl);
  CType *i8 =CreatePrimType(TYPE_I8);
  map_set(&Compiler.types, "I8i", i8);
  CType *u8 =CreatePrimType(TYPE_U8);
  map_set(&Compiler.types, "U8i", u8);
  CType *i16 =CreatePrimType(TYPE_I16);
  map_set(&Compiler.types, "I16i", i16);
  CType *u16 =CreatePrimType(TYPE_U16);
  map_set(&Compiler.types, "U16i", u16);
  CType *i32 =CreatePrimType(TYPE_I32);
  map_set(&Compiler.types, "I32i", i32);
  CType *u32 =CreatePrimType(TYPE_U32);
  map_set(&Compiler.types, "U32i", u32);
  CType *i64 =CreatePrimType(TYPE_I64);
  map_set(&Compiler.types, "I64i", i64);
  CType *u64 =CreatePrimType(TYPE_U64);
  map_set(&Compiler.types, "U64i", u64);
  CType *f64=CreatePrimType(TYPE_F64);
  map_set(&Compiler.types, "F64", f64);
    //
    CType *u8p =CreatePtrType(u8);
    CType *u8pp =CreatePtrType(u8p);
    CType *u0p =CreatePtrType(u0);
    CType *i32p =CreatePtrType(i32);
    CType *i64p =CreatePtrType(i64);
    CType *cfs =CreateClassForwardDecl(NULL, CreateDummyName("CFs"));
    CType *cfsptr =CreatePtrType(cfs);
    CType *cfile =CreateClassForwardDecl(NULL, CreateDummyName("CFile"));
    CType *cfileptr =CreatePtrType(cfile);
    CType *wind =CreateClassForwardDecl(NULL, CreateDummyName("WINDOW"));
    CType *windp =CreatePtrType(wind);
    //
    CreateBuiltin(&HC_CreateF64,u0p,"HC_CreateF64",0,f64,NULL);
    CreateBuiltin(&HC_CreateI64,u0p,"HC_CreateI64",0,i64,NULL);
    CreateBuiltin(&HC_CreateToken,u0p,"HC_CreateToken",0,i64,NULL);
    CreateBuiltin(&HC_CreateStr,u0p,"HC_CreateStr",0,u8p,NULL);
    CreateBuiltin(&HC_CreateIdent,u0p,"HC_CreateIdent",0,u8p,NULL);
    CreateBuiltin(&HC_SetPosFromLexer,u0p,"HC_SetPosFromLexer",0,u0p,u8p,i64,NULL);
    //
    CreateBuiltin(&HCSetJmp,u0,"HCSetJmp",0,NULL);
    CreateBuiltin(&PopTryFrame,u0,"PopTryFrame",0,NULL);
    CreateBuiltin(&EnterTry,u0,"EnterTry",0,NULL);
    CreateBuiltin(&ExitCatch,u0,"ExitCatch",0,NULL);
    CreateBuiltin(&TOSPrint,u0,"TOSPrint",1,u8p,NULL);
    CreateBuiltin(&PowU64,u64,"PowU64",0,u64,u64,NULL);
    CreateBuiltin(&PowI64,i64,"PowI64",0,i64,i64,NULL);
    CreateBuiltin(&fmod,f64,"FMod",0,f64,f64,NULL);
    #ifdef BOOTSTRAPED
    CreateBuiltin(&WhineOnOutOfBounds,u0,"WhineOnOutOfBounds",0,u0p,i64,NULL);
    CreateBuiltin(&DbgLeaveFunction,u0,"DbgLeaveFunction",0,NULL);
    CreateBuiltin(&VisitBreakpoint,u0,"VisitBreakpoint",0,u0p,NULL);
    CreateBuiltin(&DbgEnterFunction,u0,"DbgEnterFunction",0,u0p,u0p,NULL);
    CreateBuiltin(&EnterDebugger, u0, "Debugger",0,NULL);
    CreateBuiltin(&__LexExe,u0,"__LexExe",0,NULL);
    #else
    CreateBuiltin(NULL,u0,"__LexExe",0,NULL);
    #endif
    CreateBuiltin(&Bit4BitU64ToF64,f64,"Bit4BitU64ToF64",0,u64,NULL);
    CreateBuiltin(&Bit4BitF64ToU64,u64,"Bit4BitF64ToU64",0,f64,NULL);
    CreateBuiltin(&F64And,f64,"F64And",0,f64,f64,NULL);
    CreateBuiltin(&F64Xor,f64,"F64Xor",0,f64,f64,NULL);
    CreateBuiltin(&F64Or,f64,"F64Or",0,f64,f64,NULL);
    CreateBuiltin(&F64Shr,f64,"F64Shr",0,f64,i64,NULL);
    CreateBuiltin(&F64Shl,f64,"F64Shl",0,f64,i64,NULL);
    CreateBuiltin(&pow,f64,"Pow",0,f64,f64,NULL);
    CreateBuiltin(&CreateBinFile,u0,"CreateBinFile",0,u8p,u8p,NULL);
    CreateBuiltin(&LoadBinFile,u0,"LoadBinFile",0,u8p,NULL);
    CreateBuiltin(&EvalExprNoComma,i64,"JIT_Eval",0,u8p,u8pp,NULL);
    CreateBuiltin(&GCollect,u0,"GC_Collect",0,NULL);
    CreateBuiltin(&CreateTagsAndErrorsFiles,u0,"CreateTagsAndErrorsFiles",0,u8p,u8p,u8p,NULL);
    CreateBuiltin(&MSize2, i64, "MSize",0, u0p,NULL);
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
    CreateBuiltin(&strncmp, i64, "StrNCmp",0,u8p,u8p,i64,NULL);
    CreateBuiltin(&strcpy, u8p, "StrCpy",0,u8p,u8p,NULL);
    CreateBuiltin(&strncpy, u8p, "StrNCpy",0,u8p,u8p,i64,NULL);
    CreateBuiltin(&strstr, u8p, "StrMatch",0,u8p,u8p,NULL);
    CreateBuiltin(&GetFs, cfsptr, "Fs",0,NULL);
    CreateBuiltin(&__GetStr, u8p, "__GetStr",0,u8p,NULL);
    CreateBuiltin(&atan, f64,"ATan", 0,f64,NULL);
    CreateBuiltin(&fabs, f64, "Abs",0,f64,NULL);
    CreateBuiltin(&cos, f64, "Cos", 0,f64,NULL);
    CreateBuiltin(&sin, f64, "Sin", 0,f64,NULL);
    CreateBuiltin(&sqrt, f64, "Sqrt", 0,f64,NULL);
    CreateBuiltin(&tan, f64, "Tan", 0,f64,NULL);
    CreateBuiltin(&ceil, f64, "Ceil", 0,f64,NULL);
    CreateBuiltin(&floor, f64, "Floor", 0,f64,NULL);
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
    //SDL2 bindings
    CType *sdlr_t=IMPORT_CLASS_WO_MEMBERS(SDL_Rect);
    ADD_PRIM_MEMBER(sdlr_t,SDL_Rect,x);
    ADD_PRIM_MEMBER(sdlr_t,SDL_Rect,y);
    ADD_PRIM_MEMBER(sdlr_t,SDL_Rect,w);
    ADD_PRIM_MEMBER(sdlr_t,SDL_Rect,h);
    CType *sdlp_t=IMPORT_CLASS_WO_MEMBERS(SDL_Point);
    ADD_PRIM_MEMBER(sdlp_t,SDL_Point,x);
    ADD_PRIM_MEMBER(sdlp_t,SDL_Point,y);
    //Init section
    CreateMacroInt("SDL_INIT_TIMER",SDL_INIT_TIMER);
    CreateMacroInt("SDL_INIT_AUDIO",SDL_INIT_AUDIO);
    CreateMacroInt("SDL_INIT_VIDEO",SDL_INIT_VIDEO);
    CreateMacroInt("SDL_INIT_EVENTS",SDL_INIT_EVENTS);
    CreateMacroInt("SDL_INIT_EVERYTHING",SDL_INIT_EVERYTHING);
    CreateBuiltin(&SDL_Init,i64,"SDL_Init",0,i64,NULL);
    CreateBuiltin(&SDL_WasInit,i64,"SDL_WasInit",0,i64,NULL);
    //Video section
    CreateMacroInt("SDL_BLENDMODE_NONE",SDL_BLENDMODE_NONE);
    CreateMacroInt("SDL_BLENDMODE_BLEND",SDL_BLENDMODE_BLEND);
    CreateMacroInt("SDL_BLENDMODE_ADD",SDL_BLENDMODE_ADD);
    CreateMacroInt("SDL_BLENDMODE_MOD",SDL_BLENDMODE_MOD);
    CType *sdlwindp_t=CreatePtrType(CreateClassForwardDecl(NULL,CreateDummyName("SDL_Window")));
    CType *sdlrendp_t=CreatePtrType(CreateClassForwardDecl(NULL,CreateDummyName("SDL_Renderer")));
    CType *sdltextp_t=CreatePtrType(CreateClassForwardDecl(NULL,CreateDummyName("SDL_Texture")));
    CreateMacroInt("SDL_WINDOWPOS_UNDEFINED",SDL_WINDOWPOS_UNDEFINED);
    CreateMacroInt("SDL_WINDOWPOS_CENTERED",SDL_WINDOWPOS_CENTERED);
    CreateMacroInt("SDL_WINDOW_FULLSCREEN",SDL_WINDOW_FULLSCREEN);
    CreateMacroInt("SDL_WINDOW_FULLSCREEN_DESKTOP",SDL_WINDOW_FULLSCREEN_DESKTOP);
    CreateMacroInt("SDL_WINDOW_RESIZABLE",SDL_WINDOW_RESIZABLE);
    CreateMacroInt("SDL_WINDOW_MINIMIZED",SDL_WINDOW_MINIMIZED);
    CreateMacroInt("SDL_WINDOW_MAXIMIZED",SDL_WINDOW_MAXIMIZED);
    CreateBuiltin(&SDL_CreateWindow,sdlwindp_t,"SDL_CreateWindow",0,u8p,i64,i64,i64,i64,i64,NULL);
    CreateBuiltin(&SDL_SetTextureBlendMode,i64,"SDL_SetTextureBlendMode",0,sdltextp_t,i64,NULL);
    CreateBuiltin(&SDL_CreateWindowAndRenderer,sdlrendp_t,"SDL_CreateWindowAndRenderer",0,i64,i64,i64,CreatePtrType(sdlwindp_t),CreatePtrType(sdlrendp_t),NULL);
    CreateBuiltin(&SDL_DestroyWindow,u0,"SDL_DestroyWindow",0,sdlwindp_t,NULL);
    CreateBuiltin(&SDL_DestroyRenderer,u0,"SDL_DestroyRenderer",0,sdlrendp_t,NULL);
    CreateBuiltin(&SDL_DisableScreenSaver,u0,"SDL_DisableScreenSaver",0,NULL);
    CreateBuiltin(&SDL_GetGrabbedWindow,sdlwindp_t,"SDL_GetGrabbedWindow",0,NULL);
    CreateBuiltin(&SDL_GetWindowPosition,u0,"SDL_GetWindowPosition",0,sdlwindp_t,i32p,i32p,NULL);
    CreateBuiltin(&SDL_GetWindowMinimumSize,u0,"SDL_GetWindowMinimumSize",0,sdlwindp_t,i32p,i32p,NULL);
    CreateBuiltin(&SDL_GetWindowMaximumSize,u0,"SDL_GetWindowMaximumSize",0,sdlwindp_t,i32p,i32p,NULL);
    CreateBuiltin(&SDL_GetWindowSize,u0,"SDL_GetWindowSize",0,sdlwindp_t,i32p,i32p,NULL);
    CreateBuiltin(&hc_SDL_GetWindowTitle,u8p,"SDL_GetWindowTitle",0,sdlwindp_t,NULL);
    CreateBuiltin(&SDL_HideWindow,u0,"SDL_HideWindow",0,sdlwindp_t,NULL);
    CreateBuiltin(&SDL_MaximizeWindow,u0,"SDL_MaximizeWindow",0,sdlwindp_t,NULL);
    CreateBuiltin(&SDL_MinimizeWindow,u0,"SDL_MinimizeWindow",0,sdlwindp_t,NULL);
    CreateBuiltin(&SDL_SetWindowBordered,u0,"SDL_SetWindowBordered",0,sdlwindp_t,i64,NULL);
    CreateBuiltin(&SDL_SetWindowFullscreen,u0,"SDL_SetWindowFullscreen",0,sdlwindp_t,i64,NULL);
    CreateBuiltin(&SDL_SetWindowMaximumSize,u0,"SDL_SetWindowMaximumSize",0,sdlwindp_t,i64,i64,NULL);
    CreateBuiltin(&SDL_SetWindowMinimumSize,u0,"SDL_SetWindowMinimumSize",0,sdlwindp_t,i64,i64,NULL);
    CreateBuiltin(&SDL_SetWindowResizable,u0,"SDL_SetWindowResizable",0,sdlwindp_t,i64,NULL);
    CreateBuiltin(&SDL_SetWindowSize,u0,"SDL_SetWindowSize",0,sdlwindp_t,i64,i64,NULL);
    CreateBuiltin(&SDL_SetWindowTitle,u0,"SDL_SetWindowTitle",0,sdlwindp_t,u8p,NULL);
    CreateBuiltin(&SDL_ShowWindow,u0,"SDL_ShowWindow",0,sdlwindp_t,NULL);
    CreateMacroInt("SDL_RENDERER_SOFTWARE",SDL_RENDERER_SOFTWARE);
    CreateMacroInt("SDL_RENDERER_ACCELERATED",SDL_RENDERER_ACCELERATED);
    CreateMacroInt("SDL_RENDERER_PRESENTVSYNC",SDL_RENDERER_PRESENTVSYNC);
    CreateMacroInt("SDL_RENDERER_TARGETTEXTURE",SDL_RENDERER_TARGETTEXTURE);
    CreateBuiltin(&SDL_CreateRenderer,sdlrendp_t,"SDL_CreateRenderer",0,sdlwindp_t,i64,i64,0);
    CreateMacroInt("SDL_TEXTUREACCESS_TARGET",SDL_TEXTUREACCESS_TARGET);
    CreateMacroInt("SDL_TEXTUREACCESS_STREAMING",SDL_TEXTUREACCESS_STREAMING);
    CreateMacroInt("SDL_TEXTUREACCESS_STATIC",SDL_TEXTUREACCESS_STATIC);
    CreateMacroInt("SDL_PIXELFORMAT_RGBA8888",SDL_PIXELFORMAT_RGBA8888);
    CreateBuiltin(&SDL_CreateTexture,sdltextp_t,"SDL_CreateTexture",0,sdlrendp_t,i64,i64,i64,i64,NULL);
    CreateBuiltin(&SDL_DestroyRenderer,sdlrendp_t,"SDL_DestroyRenderer",0,sdlrendp_t,NULL);
    CreateBuiltin(&SDL_GetRenderDrawColor,i64,"SDL_GetRenderDrawColor",0,sdlrendp_t,u8p,u8p,u8p,u8p,NULL);
    CreateBuiltin(&SDL_GetRenderer,sdlrendp_t,"SDL_GetRenderer",0,sdlwindp_t,NULL);
    CreateBuiltin(&SDL_GetRendererOutputSize,i64,"SDL_GetRendererOutputSize",0,sdlrendp_t,i32p,i32p,NULL);
    CreateBuiltin(&SDL_GetRenderTarget,sdltextp_t,"SDL_GetRenderTarget",0,sdlrendp_t,NULL);
    CreateBuiltin(&SDL_GetTextureAlphaMod,i64,"SDL_GetTextureAlphaMod",0,sdltextp_t,u8p,NULL);
    CreateBuiltin(&SDL_GetTextureColorMod,i64,"SDL_GetTextureColorMod",0,sdltextp_t,u8p,u8p,u8p,NULL);
    CreateBuiltin(&SDL_RenderClear,i64,"SDL_RenderClear",0,sdlrendp_t,NULL);
    CType *sdlrp_t=CreatePtrType(sdlr_t);
    CType *sdlpp_t=CreatePtrType(sdlp_t);
    CreateBuiltin(&SDL_RenderCopy,i64,"SDL_RenderCopy",0,sdlrendp_t,sdltextp_t,sdlrp_t,sdlrp_t,NULL);
    CreateBuiltin(&SDL_RenderDrawLine,i64,"SDL_RenderDrawLine",0,sdlrendp_t,i64,i64,i64,i64,NULL);
    CreateBuiltin(&SDL_RenderDrawLines,i64,"SDL_RenderDrawLines",0,sdlrendp_t,sdlp_t,i64,NULL);
    CreateBuiltin(&SDL_RenderDrawPoint,i64,"SDL_RenderDrawPoint",0,sdlrendp_t,i64,i64,NULL);
    CreateBuiltin(&SDL_RenderDrawPoints,i64,"SDL_RenderDrawPoints",0,sdlrendp_t,sdlpp_t,i64,NULL);
    CreateBuiltin(&SDL_RenderDrawRect,i64,"SDL_RenderDrawRect",0,sdlrendp_t,sdlrp_t,NULL);
    CreateBuiltin(&SDL_RenderDrawRects,i64,"SDL_RenderDrawRects",0,sdlrendp_t,sdlrp_t,i64,NULL);
    CreateBuiltin(&SDL_RenderFillRect,i64,"SDL_RenderFillRect",0,sdlrendp_t,sdlrp_t,NULL);
    CreateBuiltin(&SDL_RenderFillRects,i64,"SDL_RenderFillRects",0,sdlrendp_t,sdlrp_t,i64,NULL);
    CreateBuiltin(&SDL_RenderGetClipRect,i64,"SDL_RenderGetClipRect",0,sdlrendp_t,sdlrp_t,NULL);
    CreateBuiltin(&SDL_RenderPresent,i64,"SDL_RenderPresent",0,sdlrendp_t,NULL);
    CreateBuiltin(&SDL_RenderSetClipRect,i64,"SDL_RenderSetClipRect",0,sdlrendp_t,sdlrp_t,NULL);
    CreateBuiltin(&SDL_SetRenderDrawColor,i64,"SDL_SetRenderDrawColor",0,sdlrendp_t,u8,u8,u8,u8,NULL);
    CreateBuiltin(&SDL_SetRenderTarget,i64,"SDL_SetRenderTarget",0,sdlrendp_t,sdltextp_t,NULL);
    CreateBuiltin(&SDL_SetTextureAlphaMod,i64,"SDL_SetTextureAlphaMod",0,sdltextp_t,u8,NULL);
    CreateBuiltin(&SDL_SetTextureColorMod,i64,"SDL_SetTextureColorMod",0,sdltextp_t,u8,u8,u8,NULL);
    CreateBuiltin(&SDL_UpdateTexture,i64,"SDL_UpdateTexture",0,sdltextp_t,sdlrp_t,u0p,i64,NULL);
    CreateBuiltin(&SDL_QueryTexture,i64,"SDL_QueryTexture",0,sdltextp_t,i32p,i32p,i32p,i32p,NULL);
    //Clipboard
    CreateBuiltin(&hc_SDL_GetClipboardText,u8p,"SDL_GetClipboardText",0,NULL);
    CreateBuiltin(&SDL_SetClipboardText,u0,"SDL_SetClipboardText",0,u8p,NULL);
    //Events
    CType *sdlevent_t=IMPORT_CLASS_WO_MEMBERS(SDL_Event);
    {
        ADD_PRIM_MEMBER(sdlevent_t,SDL_Event,type);
        {
            CreateMacroInt("SDL_WINDOWEVENT",SDL_WINDOWEVENT);
            CType *sdlevent_wind_t=IMPORT_CLASS_WO_MEMBERS(SDL_WindowEvent);
            ADD_UPRIM_MEMBER(sdlevent_wind_t,SDL_WindowEvent,type);
            ADD_UPRIM_MEMBER(sdlevent_wind_t,SDL_WindowEvent,timestamp);
            ADD_UPRIM_MEMBER(sdlevent_wind_t,SDL_WindowEvent,event);
            ADD_PRIM_MEMBER(sdlevent_wind_t,SDL_WindowEvent,data1);
            ADD_PRIM_MEMBER(sdlevent_wind_t,SDL_WindowEvent,data2);
            ADD_TYPED_MEMBER(sdlevent_t,sdlevent_wind_t,SDL_Event,window);
        }
        {
            CreateMacroInt("SDL_KEYUP",SDL_KEYUP);
            CreateMacroInt("SDL_KEYDOWN",SDL_KEYDOWN);
            CreateMacroInt("KMOD_LSHIFT",KMOD_LSHIFT);
            CreateMacroInt("KMOD_RSHIFT",KMOD_RSHIFT);
            CreateMacroInt("KMOD_SHIFT",KMOD_SHIFT);
            CreateMacroInt("KMOD_LCTRL",KMOD_LCTRL);
            CreateMacroInt("KMOD_RCTRL",KMOD_RCTRL);
            CreateMacroInt("KMOD_CTRL",KMOD_CTRL);
            CreateMacroInt("KMOD_LALT",KMOD_LALT);
            CreateMacroInt("KMOD_RALT",KMOD_RALT);
            CreateMacroInt("KMOD_ALT",KMOD_ALT);
            CreateMacroInt("KMOD_LGUI",KMOD_LGUI);
            CreateMacroInt("KMOD_RGUI",KMOD_RGUI);
            CreateMacroInt("KMOD_GUI",KMOD_GUI);
            CreateMacroInt("KMOD_CAPS",KMOD_CAPS);
            CType *sdlevent_key_t=IMPORT_CLASS_WO_MEMBERS(SDL_KeyboardEvent);
            ADD_UPRIM_MEMBER(sdlevent_key_t,SDL_KeyboardEvent,type);
            ADD_UPRIM_MEMBER(sdlevent_key_t,SDL_KeyboardEvent,timestamp);
            ADD_UPRIM_MEMBER(sdlevent_key_t,SDL_KeyboardEvent,state);
            ADD_UPRIM_MEMBER(sdlevent_key_t,SDL_KeyboardEvent,repeat);
            //
            CType *sdlkeysym_t=IMPORT_CLASS_WO_MEMBERS(SDL_Keysym);
            ADD_UPRIM_MEMBER(sdlkeysym_t,SDL_Keysym,scancode);
            ADD_UPRIM_MEMBER(sdlkeysym_t,SDL_Keysym,sym);
            ADD_UPRIM_MEMBER(sdlkeysym_t,SDL_Keysym,mod);
            //
            ADD_TYPED_MEMBER(sdlevent_key_t,sdlkeysym_t,SDL_KeyboardEvent,keysym);
            ADD_TYPED_MEMBER(sdlevent_t,sdlevent_key_t,SDL_Event,key);
        }
        {
            CreateMacroInt("SDL_TEXTEDITING",SDL_TEXTEDITING);
            CType *sdlevent_edit_t=IMPORT_CLASS_WO_MEMBERS(SDL_TextEditingEvent);
            ADD_UPRIM_MEMBER(sdlevent_edit_t,SDL_TextEditingEvent,type);
            ADD_UPRIM_MEMBER(sdlevent_edit_t,SDL_TextEditingEvent,timestamp);
            ADD_TYPED_MEMBER(sdlevent_edit_t,CreateArrayType(u8,CreateI64(32)),SDL_TextEditingEvent,text);
            ADD_PRIM_MEMBER(sdlevent_edit_t,SDL_TextEditingEvent,start);
            ADD_PRIM_MEMBER(sdlevent_edit_t,SDL_TextEditingEvent,length);
            ADD_TYPED_MEMBER(sdlevent_t,sdlevent_edit_t,SDL_Event,edit);
        }
        {
            CreateMacroInt("SDL_TEXTINPUT",SDL_TEXTINPUT);
            CType *sdlevent_text_t=IMPORT_CLASS_WO_MEMBERS(SDL_TextInputEvent);
            ADD_UPRIM_MEMBER(sdlevent_text_t,SDL_TextInputEvent,type);
            ADD_UPRIM_MEMBER(sdlevent_text_t,SDL_TextInputEvent,timestamp);
            ADD_TYPED_MEMBER(sdlevent_text_t,CreateArrayType(u8,CreateI64(32)),SDL_TextInputEvent,text);
            ADD_TYPED_MEMBER(sdlevent_t,sdlevent_text_t,SDL_Event,text);
        }
        {
            CreateMacroInt("SDL_MOUSEMOTION",SDL_MOUSEMOTION);
            CreateMacroInt("SDL_BUTTON_LMASK",SDL_BUTTON_LMASK);
            CreateMacroInt("SDL_BUTTON_RMASK",SDL_BUTTON_RMASK);
            CreateMacroInt("SDL_BUTTON_MMASK",SDL_BUTTON_MMASK);
            CType *sdlevent_mousemot_t=IMPORT_CLASS_WO_MEMBERS(SDL_MouseMotionEvent);
            ADD_UPRIM_MEMBER(sdlevent_mousemot_t,SDL_MouseMotionEvent,type);
            ADD_UPRIM_MEMBER(sdlevent_mousemot_t,SDL_MouseMotionEvent,timestamp);
            ADD_UPRIM_MEMBER(sdlevent_mousemot_t,SDL_MouseMotionEvent,which);
            ADD_UPRIM_MEMBER(sdlevent_mousemot_t,SDL_MouseMotionEvent,state);
            ADD_PRIM_MEMBER(sdlevent_mousemot_t,SDL_MouseMotionEvent,x);
            ADD_PRIM_MEMBER(sdlevent_mousemot_t,SDL_MouseMotionEvent,y);
            ADD_PRIM_MEMBER(sdlevent_mousemot_t,SDL_MouseMotionEvent,xrel);
            ADD_PRIM_MEMBER(sdlevent_mousemot_t,SDL_MouseMotionEvent,yrel);
            ADD_TYPED_MEMBER(sdlevent_t,sdlevent_mousemot_t,SDL_Event,motion);
        }
        {
            CreateMacroInt("SDL_MOUSEBUTTONUP",SDL_MOUSEBUTTONUP);
            CreateMacroInt("SDL_MOUSEBUTTONDOWN",SDL_MOUSEBUTTONDOWN);
            CType *sdlevent_mouse_t=IMPORT_CLASS_WO_MEMBERS(SDL_MouseButtonEvent);
            ADD_UPRIM_MEMBER(sdlevent_mouse_t,SDL_MouseButtonEvent,type);
            ADD_UPRIM_MEMBER(sdlevent_mouse_t,SDL_MouseButtonEvent,timestamp);
            ADD_UPRIM_MEMBER(sdlevent_mouse_t,SDL_MouseButtonEvent,which);
            ADD_UPRIM_MEMBER(sdlevent_mouse_t,SDL_MouseButtonEvent,button);
            CreateMacroInt("SDL_PRESSED",SDL_PRESSED);
            CreateMacroInt("SDL_RELEASED",SDL_RELEASED);
            ADD_UPRIM_MEMBER(sdlevent_mouse_t,SDL_MouseButtonEvent,state);
            ADD_UPRIM_MEMBER(sdlevent_mouse_t,SDL_MouseButtonEvent,clicks);
            ADD_PRIM_MEMBER(sdlevent_mouse_t,SDL_MouseMotionEvent,x);
            ADD_PRIM_MEMBER(sdlevent_mouse_t,SDL_MouseMotionEvent,y);
            ADD_TYPED_MEMBER(sdlevent_t,sdlevent_mouse_t,SDL_Event,button);
        }
        {
            CreateMacroInt("SDL_MOUSEMOTION",SDL_MOUSEMOTION);
            CType *sdlevent_wheel_t=IMPORT_CLASS_WO_MEMBERS(SDL_MouseMotionEvent);
            ADD_UPRIM_MEMBER(sdlevent_wheel_t,SDL_MouseWheelEvent,type);
            ADD_UPRIM_MEMBER(sdlevent_wheel_t,SDL_MouseWheelEvent,timestamp);
            ADD_UPRIM_MEMBER(sdlevent_wheel_t,SDL_MouseWheelEvent,which);
            ADD_PRIM_MEMBER(sdlevent_wheel_t,SDL_MouseWheelEvent,x);
            ADD_PRIM_MEMBER(sdlevent_wheel_t,SDL_MouseWheelEvent,y);
            ADD_UPRIM_MEMBER(sdlevent_wheel_t,SDL_MouseWheelEvent,direction);
            ADD_TYPED_MEMBER(sdlevent_t,sdlevent_wheel_t,SDL_Event,wheel);
        }
        {
            CreateMacroInt("SDL_QUIT",SDL_QUIT);
            CType *sdlevent_quit_t=IMPORT_CLASS_WO_MEMBERS(SDL_QuitEvent);
            ADD_UPRIM_MEMBER(sdlevent_quit_t,SDL_QuitEvent,type);
            ADD_UPRIM_MEMBER(sdlevent_quit_t,SDL_QuitEvent,timestamp);
            ADD_TYPED_MEMBER(sdlevent_t,sdlevent_quit_t,SDL_Event,quit);
        }
    }
    CType *sdleventp_t=CreatePtrType(sdlevent_t);
    CreateBuiltin(&SDL_PollEvent,i64,"SDL_PollEvent",0,sdleventp_t,NULL);
    CreateBuiltin(&SDL_WaitEvent,i64,"SDL_WaitEvent",0,sdleventp_t,NULL);
    CreateBuiltin(&SDL_DestroyRenderer,u0,"SDL_DestroyRenderer",0,sdlrendp_t,NULL);
    CreateBuiltin(&SDL_DestroyTexture,u0,"SDL_DestroyTexture",0,sdltextp_t,NULL);
    CreateBuiltin(&SDL_StartTextInput,u0,"SDL_StartTextInput",0,NULL);
    CreateBuiltin(&SDL_StopTextInput,u0,"SDL_StopTextInput",0,NULL);
    CreateBuiltin(&SDL_GetError,u8p,"SDL_GetError",0,NULL);
    CreateBuiltin(&SDL_ClearError,u0,"SDL_ClearError",0,NULL);
    CreateBuiltin(&SDL_FlushEvent,u0,"SDL_FlushEvent",0,i64,NULL);
    //Surfaces
    CType *u32p=CreatePtrType(u32);
    {
        CType *sdlsurf_t=IMPORT_CLASS_WO_MEMBERS(SDL_Surface);
        ADD_TYPED_MEMBER(sdlsurf_t,u0p,SDL_Surface,pixels);
        CType *sdlsurfp_t=CreatePtrType(sdlsurf_t);
        CreateBuiltin(&GetSurfaceW,i64,"SDL_GetSurfaceWidth",0,sdlsurfp_t,NULL);
        CreateBuiltin(&GetSurfaceH,i64,"SDL_GetSurfaceHeight",0,sdlsurfp_t,NULL);
        CreateBuiltin(&SDL_CreateRGBSurface,sdlsurfp_t,"SDL_CreateRGBSurface",0,u32,i32,i32,i32,u32,u32,u32,u32,NULL);
        CreateBuiltin(&SDL_CreateRGBSurfaceFrom,sdlsurfp_t,"SDL_CreateRGBSurfaceFrom",0,u0p,i32,i32,i32,u32,u32,u32,u32,NULL);
        CreateBuiltin(&SDL_UpperBlit,i64,"SDL_UpperBlit",0,sdlsurfp_t,sdlrp_t,sdlsurfp_t,sdlrp_t,NULL);
        CreateBuiltin(&SDL_FillRect,i64,"SDL_FillRect",0,sdlsurfp_t,sdlrp_t,u32,NULL);
        CreateBuiltin(&SDL_FillRects,i64,"SDL_FillRects",0,sdlsurfp_t,sdlrp_t,i32,u32,NULL);
        CreateBuiltin(&SDL_GetClipRect,u0,"SDL_GetClipRect",0,sdlsurfp_t,sdlrp_t,NULL);
        CreateBuiltin(&SDL_GetColorKey,i64,"SDL_GetColorKey",0,sdlsurfp_t,u32p,NULL);
        CreateBuiltin(&SDL_GetSurfaceAlphaMod,i64,"SDL_GetSurfaceAlphaMod",0,sdlsurfp_t,u8p,NULL);
        CreateBuiltin(&SDL_GetSurfaceColorMod,i64,"SDL_GetSurfaceColorMod",0,sdlsurfp_t,u8p,u8p,u8p,NULL);
        CreateBuiltin(&SDL_LockSurface,i64,"SDL_LockSurface",0,sdlsurfp_t,NULL);
        CreateBuiltin(&SDL_UnlockSurface,i64,"SDL_UnlockSurface",0,sdlsurfp_t,NULL);
        CreateBuiltin(&SDL_SetClipRect,i64,"SDL_SetClipRect",0,sdlsurfp_t,sdlrp_t,NULL);
        CreateBuiltin(&SDL_SetColorKey,i64,"SDL_SetColorKey",0,sdlsurfp_t,i32, u32,NULL);
        CreateBuiltin(&SDL_SetSurfaceAlphaMod,i64,"SDL_SetSurfaceAlphaMod",0,sdlsurfp_t,u8,NULL);
        CreateBuiltin(&SDL_SetSurfaceColorMod,i64,"SDL_SetSurfaceColorMod",0,sdlsurfp_t,u8,u8,u8,NULL);
        CreateBuiltin(&SDL_SetSurfaceRLE,i64,"SDL_SetSurfaceRLE",0,sdlsurfp_t,i32,NULL);
        CreateBuiltin(&SDL_BlitSurface,i64,"SDL_BlitSurface",0,sdlsurfp_t,sdlrp_t,sdlsurfp_t,sdlrp_t,NULL);
        CreateBuiltin(&SDL_BlitScaled,i64,"SDL_BlitScaled",0,sdlsurfp_t,sdlrp_t,sdlsurfp_t,sdlrp_t,NULL);
        CreateBuiltin(&SDL_FreeSurface,i64,"SDL_FreeSurface",0,sdlsurfp_t,NULL);
        CreateBuiltin(&SDL_UpdateWindowSurface,i64,"SDL_UpdateWindowSurface",0,sdlwindp_t,NULL);
        CreateBuiltin(&SDL_GetWindowSurface,sdlsurfp_t,"SDL_GetWindowSurface",0,sdlwindp_t,NULL);
    }
    //Timer

}
