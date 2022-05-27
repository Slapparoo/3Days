#include "3d.h"
#include "ext/argtable3/argtable3.h"
#include <signal.h>
#ifndef TARGET_WIN32
#include <sys/syscall.h>
#include <unistd.h>
#define HCRT_INSTALLTED_DIR "/usr/local/include/3Days/HCRT.BIN"
#include <libgen.h>
#include "ext/C_Unescaper/escaper.h"
#define DFT_T_DRIVE ".3DAYS_BOOT"
#define DFT_TEMPLATE "/usr/local/include/3Days/T/"
#else
#include <windows.h>
#include <libloaderapi.h>
#include <fileapi.h>
#include <userenv.h>
#include <winnt.h>
#define HCRT_INSTALLTED_DIR "\\HCRT\\HCRT_TOS.BIN"
#define DFT_T_DRIVE "3DAYS_BOOT"
//Is relative to install dir on windows
#define DFT_TEMPLATE ".\\T\\"
#endif

static struct arg_lit *helpArg;
static struct arg_file *TDriveArg;
static struct arg_lit *OverwriteBootDrvArg;
static struct arg_end *endArg;
char CompilerPath[1024];
#ifdef TARGET_WIN32
#define ENABLE_VIRTUAL_TERMINAL_PROCESSING  0x4
#define ENABLE_VIRTUAL_TERMINAL_INPUT 0x200
static LONG WINAPI VectorHandler (struct _EXCEPTION_POINTERS *info) {
  #ifdef BOOTSTRAPED
  #endif
  switch(info->ExceptionRecord->ExceptionCode) {
    #define FERR(code) case code: printf("Caught %s.\nType 'Exit(0);' to exit.\n",#code); exit(0);
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
#include <windows.h>
#include <dbghelp.h>
#endif
#include <signal.h>
void SignalHandler(int sig) {
    CSymbol *sym=map_get(&Loader.symbols,"Backtrace");
    if(sym) {
        ((void(*)(long ,long))sym->value_ptr)(0,0);
    }
    exit(0);
}
static void Core0(char *name) {
	Load(name,0);
} 
int main(int argc,char **argv) {
    char *header=NULL,*t_drive=NULL,*tmp;
    SDL_Init(SDL_INIT_EVERYTHING);
    InitSound();
    VFsGlobalInit();
    InitThreads();
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
        TDriveArg=arg_file0("t",NULL,"T(boot) Drive","This tells 3days where to use(or create) the boot drive folder."),
        OverwriteBootDrvArg=arg_lit0("O", "overwrite", "Create a fresh version of the boot drive folder."),
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
    if(TDriveArg->count) {
		t_drive=TDriveArg->filename[0];
	} else  {
		tmp=HostHomeDir();
		t_drive=PoopMAlloc(1+1+strlen(tmp)+strlen(DFT_T_DRIVE));
		strcpy(t_drive,tmp);
		PoopFree(tmp);
		#ifdef TARGET_WIN32
		strcat(t_drive,"\\" DFT_T_DRIVE);
		#else
		strcat(t_drive,"/" DFT_T_DRIVE);
		#endif
	}
    TOS_RegisterFuncPtrs();
    char *template=DFT_TEMPLATE;
    #ifndef TARGET_WIN32 
    if(0==access("./T",F_OK))
		template="./T",puts("Using ./T as the template directory.");
    #endif
    //CreateTemplateBootDrv Checks if exists too
    //DFT_TEMPLATE IS RELATIVE TO PROGRAM IN WINDOWS
    CreateTemplateBootDrv(t_drive,template,OverwriteBootDrvArg->count);
    //IMPORTANT,init thread VFs after we make drive T
    VFsThrdInit();
    if(1) {
		//Create the Window,there is 1 screen God willing.
		NewDrawWindow();
        int flags=0;
    #ifndef TARGET_WIN32
        if(0==access("HCRT.BIN",F_OK)) {
			puts("Using ./HCRT.BIN as the default binary.");
            SDL_CreateThread(Core0,"Core0","HCRT.BIN");
        } else if(0==access( HCRT_INSTALLTED_DIR,F_OK)) {
			SDL_CreateThread(Core0,"Core0",HCRT_INSTALLTED_DIR);
        }
    #else
      char buffer[MAX_PATH];
      GetModuleFileNameA(NULL,buffer,sizeof(buffer));
      dirname(buffer);
      strcat(buffer,"\\HCRT.BIN");
      puts(buffer);
      if(GetFileAttributesA(buffer)!=INVALID_FILE_ATTRIBUTES) {
        SDL_CreateThread(Core0,"Core0",buffer);
      }
    #endif
    }
    InputLoop(NULL);
    return 0;
}
CLoader Loader;
