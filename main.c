#include "3d.h"
#include "ext/argtable3/argtable3.h"
#include "PARSER.HH"
#include <signal.h>
#ifndef TARGET_WIN32
#include <sys/syscall.h>
#include <unistd.h>
#define HCRT_INSTALLTED_DIR "/HolyC/HCRT.BIN"
#include <libgen.h>
#include "ext/C_Unescaper/escaper.h"
#else
#include <windows.h>
#include <libloaderapi.h>
#include <fileapi.h>
#include <userenv.h>
#include <winnt.h>
#define HCRT_INSTALLTED_DIR "\\HCRT\\HCRT.BIN"
#endif
static struct arg_lit *helpArg;
static struct arg_lit *dbgArg;
static struct arg_lit *boundsArg;
static struct arg_lit *silentArg;
static struct arg_end *endArg;
static struct arg_file *includeArg;
static struct arg_file *tagsArg;
static struct arg_file *errsFile;
static struct arg_file *compileTo;
static struct arg_file *binHeader;
static struct arg_lit *noRuntime;
static struct arg_lit *forcenoregsArg;
ExceptBuf SigPad;
char CompilerPath[1024];
#ifdef TARGET_WIN32
#define ENABLE_VIRTUAL_TERMINAL_PROCESSING  0x4
#define ENABLE_VIRTUAL_TERMINAL_INPUT 0x200
static LONG WINAPI VectorHandler (struct _EXCEPTION_POINTERS *info) {
  #ifdef BOOTSTRAPED
  Backtrace();
  #endif
  switch(info->ExceptionRecord->ExceptionCode) {
    #define FERR(code) case code: printf("Caught %s.\nType 'Exit(0);' to exit.\n",#code); throw(0);
    FERR(EXCEPTION_ACCESS_VIOLATION);
    FERR(EXCEPTION_ARRAY_BOUNDS_EXCEEDED);
    FERR(EXCEPTION_DATATYPE_MISALIGNMENT);
    FERR(EXCEPTION_FLT_DENORMAL_OPERAND);
    FERR(EXCEPTION_FLT_DIVIDE_BY_ZERO);
    FERR(EXCEPTION_FLT_INEXACT_RESULT);
    FERR(EXCEPTION_FLT_INVALID_OPERATION);
    FERR(EXCEPTION_FLT_OVERFLOW);
    FERR(EXCEPTION_FLT_STACK_CHECK);
    FERR(EXCEPTION_FLT_UNDERFLOW);
    FERR(EXCEPTION_ILLEGAL_INSTRUCTION);
    FERR(EXCEPTION_IN_PAGE_ERROR);
    FERR(EXCEPTION_INT_DIVIDE_BY_ZERO);
    FERR(EXCEPTION_INVALID_DISPOSITION);
    FERR(EXCEPTION_STACK_OVERFLOW);
    default:;
  }
  //SignalHandler(0);
  return EXCEPTION_CONTINUE_EXECUTION;
}\
BOOL WINAPI CtrlCHandlerRoutine(DWORD c) {
  printf("User Abort.\n");
  return FALSE;
}
#endif
#include <signal.h>
void SignalHandler(int sig) {
    CSymbol *sym=map_get(&Loader.symbols,"Backtrace");
    if(sym) {
        ((void(*)(long ,long))sym->value_ptr)(0,0);
    }
    throw('?');
}
int main(int argc,char **argv) {
    char *header=NULL;
    #ifndef TARGET_WIN32
    char *rp=realpath(argv[0],NULL);
    if(rp==NULL)
      rp=strcpy(calloc(strlen(argv[0])+1,1),argv[0]);
    strcpy(CompilerPath,rp);
    free(rp);
    #else
    SetConsoleCtrlHandler(CtrlCHandlerRoutine,TRUE);
    GetFullPathNameA(argv[0],sizeof(CompilerPath),CompilerPath,NULL);
    DWORD omode, origOmode;
    GetConsoleMode(GetStdHandle(STD_OUTPUT_HANDLE), &omode);
    origOmode = omode;
    omode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    SetConsoleMode(GetStdHandle(STD_OUTPUT_HANDLE), omode);
    #endif
    void *argtable[]= {
        helpArg=arg_lit0("h", "help", "Display this help message."),
        dbgArg=arg_lit0("d","debug", "Start in debug mode(Use \"Debugger;\")."),
        boundsArg=arg_lit0("b", "bounds", "This enables bounds checking."),
        errsFile=arg_file0("e", "errs", "<file>","Dump diagnostic messages to file(doesnt run code)."),
        tagsArg=arg_file0("t", "tags", "<file>","Dump symbols to tags file(doesnt run code)."),
        noRuntime=arg_lit0(NULL,"noruntime","Don't include the runtime(useful for compiling the runtime and the compiler)."),
        includeArg=arg_filen(NULL, NULL, "<file>", 0, 1024, "Files to include after loading."),
        endArg=arg_end(1),
    };
    int errs=arg_parse(argc, argv, argtable);
    int run=1;
    if(helpArg->count||errs) {
        printf("Usage is: 3d");
        arg_print_syntaxv(stdout, argtable, "\n");
        arg_print_glossary(stdout, argtable, "  %-25s %s\n");
        exit(0);
    }
    PoopInit(__builtin_frame_address(0));
    RegisterFuncPtrs();
    if(1) {
        int flags=noRuntime->count?AOT_NO_IMPORT_SYMS:0;
    #ifndef TARGET_WIN32
        if(0==access("HCRT/HCRT.BIN",F_OK)) {
            FILE *rt=fopen("HCRT/HCRT.BIN","rb");
            LoadAOTBin(rt,flags,&header);
            fclose(rt);
        } else if(0==access("/usr/local/include" HCRT_INSTALLTED_DIR,F_OK)) {
            FILE *rt=fopen("/usr/local/include" HCRT_INSTALLTED_DIR,"rb");
            LoadAOTBin(rt,flags,&header);
            fclose(rt);
        } else {
            /*
          strcpy(buffer,argv[0]);
          strcat(dirname(buffer),"/HCRT/HCRT.HC");
          if(0==access(buffer, F_OK)) {
            sprintf(buffer2, "#include \"%s\"", buffer);
            mrope_append_text(Lexer.source, strdup(buffer2));
          }
          */
        }
    #else
      GetModuleFileNameA(NULL,buffer,sizeof(buffer));
      dirname(buffer);
      strcat(buffer,HCRT_INSTALLTED_DIR);
      if(GetFileAttributesA(buffer)!=INVALID_FILE_ATTRIBUTES) {
        FILE *rt=fopen(buffer,"rb");
        LoadAOTBin(rt,1,&header);
        fclose(rt);
      }
    #endif
    }
    InitRL();
    int flags=0,idx;
    if(dbgArg->count) flags|=PF_DEBUGGER;
    if(boundsArg->count) flags|=PF_BOUNDS_CHECK|PF_DEBUGGER;
    vec_char_t includes;
    vec_init(&includes);
    char buffer[1024],es[1024];
    for(idx=0;idx!=includeArg->count;idx++) {
        unescapeString(includeArg->filename[idx],es);
        sprintf(buffer,"#include \"%s\" ",es);
        vec_pusharr(&includes,buffer,strlen(buffer));
    }
    vec_push(&includes,0);
    vec_char_t prefixed;
    vec_init(&prefixed);
    if(header) {
        vec_pusharr(&prefixed,header,strlen(header));
        TD_FREE(header);
    }
    if(tagsArg->count||errsFile->count) {
        char buffer2[1024];
        unescapeString(includes.data,buffer);
        char es1[1024];
        char es2[1024];
        char *tags=(tagsArg->count)?unescapeString(tagsArg->filename[0],es1),es1:"NULL";
        char *errs=(errsFile->count)?unescapeString(errsFile->filename[0],es2),es2:"NULL";
        sprintf(buffer2,"WriteTagsAndErrsToFile(\"%s\",\"%s\",\"%s\"),Exit;;",buffer,tags,errs);
        vec_deinit(&includes);
        vec_init(&includes);
        vec_pusharr(&includes,buffer2,strlen(buffer2)+1);
    }
    RegisterRuntimeClasses(NULL,NULL,NULL,NULL);
    RegisterMacrosAndREPL(prefixed.data,flags,includes.data);
    return 0;
}
CLoader Loader;
