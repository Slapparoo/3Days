#include "3d.h"
#include <sched.h>
#include <signal.h>
#include <stdatomic.h>
#include <unistd.h>
#include <setjmp.h>
#ifndef TARGET_WIN32
#include <errno.h>
#include <pthread.h>
#else
#include <windows.h>
#include <synchapi.h>
#include <sysinfoapi.h>
#include <processthreadsapi.h>
#endif
#ifdef linux
//https://man7.org/linux/man-pages/man2/futex.2.html
#include <sys/syscall.h>
#include <linux/futex.h>
#elif defined __FreeBSD__
#include <sys/types.h>
#include <sys/umtx.h>
#endif
static int64_t GetTicks() {
	#ifndef TARGET_WIN32
	//https://stackoverflow.com/questions/2958291/equivalent-to-gettickcount-on-linux
	struct timespec ts;
    int64_t theTick = 0U;
    clock_gettime( CLOCK_REALTIME, &ts );
    theTick  = ts.tv_nsec / 1000000;
    theTick += ts.tv_sec * 1000;
    return theTick;
    #else
    return GetTickCount();
    #endif
}
static __thread void *__fs;
static __thread void *__gs;
static __thread int __core_num;
void SleepABit() {
	sleep(1);
}
void *GetFs() {
	if(!__fs) __fs=TD_MALLOC(2048);
	return __fs;
}
void SetFs(void *f) {
	__fs=f;
}
int CoreNum() {
	return __core_num;
}
void *GetGs() {
	if(!__gs)
		__gs=TD_MALLOC(1024);
	return __gs;
}
typedef struct {
	#ifndef TARGET_WIN32
	pthread_t thread;
	int64_t is_sleeping;
	jmp_buf jmp_to;
	#else
	HANDLE thread;
	CRITICAL_SECTION sleep_CS;
	CONDITION_VARIABLE sleep_cond;
	#endif
	int core_num;
	void (*fp)();
} CCore;
static CCore cores[64];
static void ExitCore(int sig) {
	#ifndef TARGET_WIN32
	pthread_exit(0);
	#else
	ExitThread(0);
	#endif
} 
static void LaunchCore(void *c) {
	SetupDebugger();
	VFsThrdInit();
	CHash init=map_get(&TOSLoader,"TaskInit")->data[0];
	FFI_CALL_TOS_2(init.val,GetFs(),0);
	__core_num=c;
	#ifndef TARGET_WIN32
	CHash yield=map_get(&TOSLoader,"__InteruptCoreRoutine")->data[0];
	signal(SIGUSR2,yield.val);
	signal(SIGUSR1,&ExitCore);
	#endif
	FFI_CALL_TOS_0_ZERO_BP(cores[__core_num].fp);
}
void InteruptCore(int core) {
	#ifndef TARGET_WIN32
	pthread_kill(cores[core].thread,SIGUSR2);
	#else
	vec_CHash_t *hash;
	CONTEXT ctx;
	hash=map_get(&TOSLoader,"__InteruptCoreRoutine");
	if(hash) {
		memset(&ctx,0 ,sizeof ctx);
		ctx.ContextFlags=CONTEXT_FULL; 
		SuspendThread(cores[core].thread);
		GetThreadContext(cores[core].thread,&ctx);
		ctx.Rsp-=8;
		((int64_t*)ctx.Rsp)[0]=ctx.Rip;
		ctx.Rip=hash->data[0].val;
		SetThreadContext(cores[core].thread,&ctx);
		ResumeThread(cores[core].thread);
	}
	assert(hash);
	#endif
}
void LaunchCore0(void *fp) {
	int core=0;
	cores[core].core_num=core;
	cores[core].fp=NULL;
	#ifndef TARGET_WIN32
	pthread_create(&cores[core].thread,NULL,FFI_CALL_TOS_0_ZERO_BP,fp);
	#else
	cores[core].thread=CreateThread(NULL,0,FFI_CALL_TOS_0_ZERO_BP,fp,0,NULL);
	InitializeConditionVariable(&cores[core].sleep_cond);
	InitializeCriticalSection(&cores[core].sleep_CS);
	#endif
}
void WaitForCore0() {
	#ifndef TARGET_WIN32
	pthread_join(cores[0].thread,NULL);
	#else
	WaitForSingleObject(cores[0].thread,INFINITE);
	#endif
}
void CreateCore(int core,void *fp) {
	cores[core].core_num=core;
	cores[core].fp=fp;
	#ifndef TARGET_WIN32
	pthread_create(&cores[core].thread,NULL,LaunchCore,core);
	#else
	cores[core].thread=CreateThread(NULL,0,LaunchCore,core,0,NULL);
	InitializeConditionVariable(&cores[core].sleep_cond);
	InitializeCriticalSection(&cores[core].sleep_CS);
	#endif
}
void __ShutdownCore(int core) {
	#ifndef TARGET_WIN32
	pthread_kill(cores[core].thread,SIGUSR1);
	pthread_join(cores[core].thread,NULL);
	#else
	CONTEXT ctx;
	SuspendThread(cores[core].thread);
	GetThreadContext(cores[core].thread,&ctx);
	ctx.Rip=&ExitCore;
	SetThreadContext(cores[core].thread,&ctx);
	ResumeThread(cores[core].thread);
	WaitForSingleObject(cores[core].thread,INFINITE);
	#endif
}
void __ShutdownCores() {
	int c;
	for(c=0;c<mp_cnt(NULL);c++) {
	   if(c!=__core_num)
	   	  __ShutdownCore(c);
	}
	__ShutdownCore(__core_num);
}
void multicAwaken(int64_t core) {
	#ifndef TARGET_WIN32
	if(__atomic_load_n(&cores[core].is_sleeping,__ATOMIC_RELAXED))
		pthread_kill(cores[core].thread,SIGPWR);
	#else
	WakeConditionVariable(&cores[core].sleep_cond);
	#endif
}
static void  Awaken(int sig) {
	longjmp(cores[__core_num].jmp_to,1);
}
void multicSleep(int64_t ms) {
	#ifndef TARGET_WIN32
	if(!setjmp(cores[__core_num].jmp_to)) {
		__atomic_store_n(&cores[__core_num].is_sleeping,1,__ATOMIC_RELAXED);
		signal(SIGPWR,&Awaken);
		usleep(ms*1000);
	} else {
		UnblockSignals();
	}
	__atomic_store_n(&cores[__core_num].is_sleeping,0,__ATOMIC_RELAXED)	;
	#else
	EnterCriticalSection(&cores[__core_num].sleep_CS);
	SleepConditionVariableCS(&cores[__core_num].sleep_cond,&cores[__core_num].sleep_CS,ms);
	LeaveCriticalSection(&cores[__core_num].sleep_CS);
	#endif
}
