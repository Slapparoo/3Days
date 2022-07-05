#include "3d.h"
#include "ext/argtable3/argtable3.h"
#include <signal.h>
#ifndef TARGET_WIN32
#include <pthread.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <X11/Xlib.h>
#define HCRT_INSTALLTED_DIR "/usr/local/include/3Days/HCRT.BIN"
#include <libgen.h>
#include "ext/C_Unescaper/escaper.h"
#define DFT_T_DRIVE ".3DAYS_BOOT"
#define DFT_TEMPLATE "/usr/local/include/3Days/T/"
#else
#include <windows.h>
#include <libloaderapi.h>
#include <processthreadsapi.h>
#include <fileapi.h>
#include <userenv.h>
#include <winnt.h>
#include <synchapi.h> 
#define HCRT_INSTALLTED_DIR "\\HCRT\\HCRT_TOS.BIN"
#define DFT_T_DRIVE "3DAYS_BOOT"
//Is relative to install dir on windows
#define DFT_TEMPLATE ".\\T\\"
#endif
static void Core0Exit(int sig) {
	/*CHash **ka=map_get(&TOSLoader,"KillAdam");
	if(ka)
		FFI_CALL_TOS_0(ka[0]->val);*/
	#ifndef TARGET_WIN32
	pthread_exit(0);
	#else
	ExitThread(0);
	#endif
} 
static struct arg_lit *helpArg;
static struct arg_file *TDriveArg;
static struct arg_file *cmdLineFiles;
static struct arg_lit *OverwriteBootDrvArg;
static struct arg_lit *commandLineArg;
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
static char *hcrt_bin_loc=NULL;
static void Core0() {
	VFsThrdInit();
	#ifndef TARGET_WIN32
	signal(SIGUSR1,&Core0Exit);
	#endif
	Load(hcrt_bin_loc,0);
} 
#ifndef TARGET_WIN32
static pthread_t core0;
#else
static HANDLE core0;
#endif
static int is_cmd_line=0;
static int64_t _shutdown=0;
int64_t IsCmdLine() {
	return is_cmd_line;
}
static char *cmd_ln_boot_txt=NULL;
char *CmdLineBootText() {
	if(!cmd_ln_boot_txt) return NULL;
	return strdup(cmd_ln_boot_txt);
}
#ifdef TARGET_WIN32
int _main(int argc,char **argv)
#else
int main(int argc,char **argv)
#endif
{
	#ifndef TARGET_WIN32
	assert(XInitThreads());
	#endif 
	BoundsCheckTests();
    char *header=NULL,*t_drive=NULL,*tmp;
    VFsGlobalInit();
    void *argtable[]= {
        helpArg=arg_lit0("h", "help", "Display this help message."),
        commandLineArg=arg_lit0("c", "com", "Start in command line mode,mount drive '/' at /."),
        TDriveArg=arg_file0("t",NULL,"T(boot) Drive","This tells 3days where to use(or create) the boot drive folder."),
        OverwriteBootDrvArg=arg_lit0("O", "overwrite", "Create a fresh version of the boot drive folder."),
        cmdLineFiles=arg_filen(NULL,NULL,"<files>",0,100,"Files for use with command line mode."),
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
		t_drive=TD_MALLOC(1+1+strlen(tmp)+strlen(DFT_T_DRIVE));
		strcpy(t_drive,tmp);
		TD_FREE(tmp);
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
    if(commandLineArg->count) {
		char buf[1024];
		VFsMountDrive('R',"/");
		is_cmd_line=1;
		vec_char_t boot_str;
		vec_init(&boot_str);
		strcpy(buf,"ChDrv('R');\nCd(\"");
		vec_pusharr(&boot_str,buf,strlen(buf));
		#ifndef TARGET_WIN32
		getcwd(buf,sizeof(buf));
		#else
		_getcwd(buf,sizeof(buf));
		#endif
		vec_pusharr(&boot_str,buf,strlen(buf));
		strcpy(buf,"\");\n");
		vec_pusharr(&boot_str,buf,strlen(buf));
		int64_t i;
		for(i=0;i!=cmdLineFiles->count;i++) {
			sprintf(buf,"#include \"%s\";\n",cmdLineFiles->filename[i]);
			vec_pusharr(&boot_str,buf,strlen(buf));
		}
		vec_push(&boot_str,0);
		cmd_ln_boot_txt=boot_str.data;
	} else {
		InitSound();
	}
	if(1) {
		//Create the Window,there is 1 screen God willing.
		if(!is_cmd_line)
			NewDrawWindow();
        int flags=0;
    #ifndef TARGET_WIN32
		pthread_t core0;
        if(0==access("HCRT.BIN",F_OK)) {
			puts("Using ./HCRT.BIN as the default binary.");
			hcrt_bin_loc="HCRT.BIN";
			pthread_create(&core0,NULL,Core0,NULL);
        } else if(0==access( HCRT_INSTALLTED_DIR,F_OK)) {
			hcrt_bin_loc=HCRT_INSTALLTED_DIR;
			pthread_create(&core0,NULL,Core0,NULL);
        }
    #else
      char buffer[MAX_PATH];
      GetModuleFileNameA(NULL,buffer,sizeof(buffer));
      dirname(buffer);
      strcat(buffer,"\\HCRT.BIN");
      hcrt_bin_loc=strdup(buffer);
      puts(buffer);
      if(GetFileAttributesA(buffer)!=INVALID_FILE_ATTRIBUTES) {
		CreateThread(NULL,0,Core0,NULL,0,NULL);
      }
    #endif
    }
    #ifndef TARGET_WIN32
    if(!commandLineArg->count) {
		InputLoop(&_shutdown);
		exit(0);
	} else {
		#ifndef TARGET_WIN32
		pthread_join(core0,NULL);
		#else
		WaitForSingleObject(core0,INFINITE);
		#endif
	}
    exit(0);
    #endif
    return 0;
}
CLoader Loader;
void __Shutdown() {
	_shutdown=1;
	#ifndef TARGET_WIN32
	pthread_kill(core0,SIGUSR1);
	pthread_join(core0,NULL);
	#else
	TerminateThread(core0,0);
	WaitForMultipleObjects(1,&core0,TRUE,INFINITE);
	#endif
}
