#include "3d.h"
#include "ext/argtable3/argtable3.h"
#include <signal.h>
#ifndef TARGET_WIN32
#include <sys/syscall.h>
#include <unistd.h>
#define HCRT_INSTALLTED_DIR "/HolyC/HCRT.BIN"
#include <libgen.h>
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
int main(int argc,char **argv) {
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
        silentArg=arg_lit0("s","silent", "Dont report warnings and errors(usefull when dumping tags)."),
        boundsArg=arg_lit0("b", "bounds", "This enables bounds checking."),
        tagsArg=arg_file0("t", "tags", "<file>", "File to dump tags to(ctags compatable)."),
        errsFile=arg_file0("e","diags","<file>","Dump diagnostics to a file."),
        compileTo=arg_file0("c","compile","<file>","Compile code to a .BIN file for faster loading."),
        noRuntime=arg_lit0(NULL,"noruntime","Don't include the runtime(useful for compiling the runtime)."),
        includeArg=arg_filen(NULL, NULL, "<file>", 0, 1024, "Files to include after loading."),
        binHeader=arg_file0(NULL, "binheader", "<file>", "Header to embed in the binary."),
        endArg=arg_end(1),
    };
    int errs=arg_parse(argc, argv, argtable);
    int run=1;
    if(helpArg->count||errs) {
        printf("Usage is: 3d");
        arg_print_syntaxv(stdout, argtable, "\n");
        arg_print_glossary(stdout, argtable, "  %-25s %s\n");
        run=0;
    }
    if(errsFile->count) {
      Compiler.errorsFile=fopen(errsFile->filename[0],"w");
    }
    if(boundsArg->count)
        Compiler.boundsCheckMode=1;
    if(dbgArg->count||boundsArg->count)
        Compiler.debugMode=1;
    #ifndef BOOTSTRAPED
    if(Compiler.debugMode) {
        fprintf(stderr,"Debugging not allowed on bootstrap compiler.\n");
        exit(1);
    }
    #endif
    if(silentArg->count)
      Compiler.silentMode=1;
    CreateGC(__builtin_frame_address(0),Compiler.boundsCheckMode);
    AddGCRoot(&Compiler, sizeof(Compiler));
    AddGCRoot(&Fs, sizeof(Fs));
    AddGCRoot(&curframe, sizeof(curframe));
    #ifdef  BOOTSTRAPED
    AddGCRoot(&Debugger, sizeof(Debugger));
    #endif
    AddGCRoot(&Lexer, sizeof(Lexer));
    #ifndef BOOTSTRAPED
    EnableREPL();
    CreateLexer(0);
    #endif
    RegisterBuiltins();
    char *tagf=NULL;
    char buffer[2048];
    char buffer2[2048];
    char *binf=NULL;
    if(compileTo->count) {
        if(Compiler.debugMode||Compiler.boundsCheckMode) {
            fprintf(stderr,"Compiled binaries cannot be used with debugger(which is required by bounds check mode).");
            return EXIT_FAILURE;
        }
        Compiler.AOTMode=1;
        Compiler.allowRedeclarations=0;
        Compiler.AOTMain=jit_init();
        binf=(char*)compileTo->filename[0];
        int deleteDummy=0;
        #ifndef TARGET_WIN32
        if(0!=access(binf,F_OK)) {
          FILE *dummy=fopen(binf,"w");
          fwrite("",0,0,dummy);
          fclose(dummy);
          deleteDummy=1;
        }
        rp=realpath(binf,NULL);
        binf=strdup(rp);
        if(deleteDummy)
          remove(rp);
        free(rp);
        #else
        if(INVALID_FILE_ATTRIBUTES==GetFileAttributesA(binf)) {
          FILE *dummy=fopen(binf,"w");
          fwrite("",0,0,dummy);
          fclose(dummy);
          deleteDummy=1;
        }
        GetFullPathNameA(binf,sizeof(buffer),buffer,NULL);
        binf=strdup(binf);
        if(deleteDummy)
          remove(buffer);
        #endif
    } else
        Compiler.allowRedeclarations=1;
    if(tagsArg->count) {
        tagf=strdup((char*)tagsArg->filename[0]);
        FILE *dummy=fopen(tagf,"w");
        fwrite("",0,0,dummy);
        fclose(dummy);
        #ifndef TARGET_WIN32
        rp=realpath(tagf,NULL);
        Compiler.tagsFile=strdup(rp);
        free(rp);
        #else
        GetFullPathNameA(tagsArg->filename[0],sizeof(buffer),buffer,NULL);
        Compiler.tagsFile=strdup(buffer);
        #endif
    }
    long iter;
    if(1) {
    #ifndef TARGET_WIN32
        if(0==access("HCRT/HCRT.BIN",F_OK)) {
            FILE *rt=fopen("HCRT/HCRT.BIN","rb");
            if(noRuntime->count)
                map_set(&Compiler.binModules,"HCRT/HCRT.BIN",LoadAOTBin(rt,AOT_F_NO_ADD_SYMBOLS));
            else
                map_set(&Compiler.binModules,"HCRT/HCRT.BIN",LoadAOTBin(rt,0));
            fclose(rt);
            Compiler.loadedHCRT=1;
            Compiler.hcrt=map_get(&Compiler.binModules, "HCRT/HCRT.BIN");
        } else if(0==access("/usr/local/include" HCRT_INSTALLTED_DIR,F_OK)) {
            FILE *rt=fopen("/usr/local/include" HCRT_INSTALLTED_DIR,"rb");
            if(noRuntime->count)
                map_set(&Compiler.binModules,"HCRT/HCRT.BIN",LoadAOTBin(rt,AOT_F_NO_ADD_SYMBOLS));
            else
                map_set(&Compiler.binModules,"HCRT/HCRT.BIN",LoadAOTBin(rt,0));
            Compiler.hcrt=map_get(&Compiler.binModules, "/usr/local/include" HCRT_INSTALLTED_DIR);
            fclose(rt);
            Compiler.loadedHCRT=1;
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
        if(noRuntime->count)
                map_set(&Compiler.binModules,buffer,LoadAOTBin(rt,AOT_F_NO_ADD_SYMBOLS));
        else
            map_set(&Compiler.binModules,buffer,LoadAOTBin(rt,0));
        fclose(rt);
        Compiler.loadedHCRT=1;
        Compiler.hcrt=map_get(&Compiler.binModules, buffer);
      }
    #endif
    }
    ARM_SIGNALS;
    #ifndef BOOTSTRAPED
    for(iter=0;iter!=includeArg->count;iter++) {
      unescapeString(includeArg->filename[iter],buffer2);
      sprintf(buffer, "#include \"%s\"\n", buffer2);
      mrope_append_text(Lexer.source, strdup(buffer));
    }
    #else
    //Used for including command line arguments
    vec_char_t includetext;
    vec_init(&includetext);
    for(iter=0;iter!=includeArg->count;iter++) {
      unescapeString(includeArg->filename[iter],buffer2);
      sprintf(buffer, "#include \"%s\"\n", buffer2);
      vec_pusharr(&includetext, buffer, strlen(buffer));
    }
    vec_push(&includetext, 0);
    {
      CVariable *is=GetHCRTVar("LexIncludeStr");
      assert(is);
      void(*fp)(void*,char*,char*,int64_t)=((void(*)(void*,char*,char*,int64_t))is->func->funcptr);
      fp(Lexer.HCLexer,"@TMP",includetext.data,0);
    }
    #endif
    if(Compiler.tagsFile||Compiler.AOTMode) DisableREPL();
    InitREPL();
    long extraFileIndex=0;
    if(run&&!errs) {
      for(;;) {
set:
            ;
            int sig;
            #ifdef TARGET_WIN32
            HANDLE h=AddVectoredExceptionHandler(1,VectorHandler);
            #endif
            if(HCSetJmp(EnterTry())) {
              err:
                #ifdef TARGET_WIN32
                RemoveVectoredExceptionHandler(h);
                #endif
                #ifdef BOOTSTRAPED
                vec_truncate(&Debugger.callStack,0);
                #endif // BOOTSTRAPED
                printf("Recieved signal %d. Discarding input.\n",sig);
                #ifndef BOOTSTRAPED
                vec_truncate(&Lexer.ifStates,0);
                FlushLexer();
                #endif // BOOTSTRAPED
                ARM_SIGNALS;
                goto set;
            }
            Compiler.errorFlag=0;
            Compiler.addrofFrameoffsetMode=0;
	    Assembler.active=0;
            Compiler.inFunction=0;
            if(!(Compiler.tagsFile||Compiler.AOTMode)) EnableREPL();
            else DisableREPL();
            #ifndef BOOTSTRAPED
            Lexer.isFreeToFlush=1;
            #endif
            vec_truncate(&Compiler.asmPatches, 0);
            vec_truncate(&Compiler.__addedGlobalLabels, 0);
            map_init(&Assembler.imports);
            HC_parse();
            PopTryFrame();
            #ifdef TARGET_WIN32
            RemoveVectoredExceptionHandler(h);
            #endif
            if(Compiler.tagsFile||Compiler.AOTMode) {
              break;
            }
        }
    }
    if(binf) {
        if(Compiler.errorCount) {
                fprintf(stderr,"Errors detected,not compiling to binary.\n");
                abort();
        }
        FILE *f=fopen(binf,"wb");
        char *head=NULL;
        if(binHeader->count)
          head=binHeader->filename[0];
        SerializeModule(f,head);
        fclose(f);
    }
    arg_freetable(argtable, sizeof(argtable)/sizeof(*argtable));
	Compiler.tagsFile=tagf;
    if(Compiler.tagsFile)
      DumpTagsToFile(Compiler.tagsFile);
    TD_FREE(tagf),TD_FREE(binf);
    if(Compiler.errorsFile) fclose(Compiler.errorsFile);
    return (!errs)?EXIT_SUCCESS:EXIT_FAILURE;
}
