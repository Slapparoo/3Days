#include "3d.h"
#include <sched.h>
#include <signal.h>
#include <stdatomic.h>
#include <unistd.h>
#ifndef TARGET_WIN32
#include <errno.h>
#include <pthread.h>
#else
#include <windows.h>
#include <synchapi.h>
#include <sysinfoapi.h>
#include <processthreadsapi.h>
static SYSTEMTIME genesis;
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
	if(!__fs) __fs=PoopMAlloc(2048);
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
		__gs=PoopMAlloc(1024);
	return __gs;
}
typedef struct {
	pthread_t thread;
	int core_num;
	void (*fp)();
} CCore;
static CCore cores[64];
static void LaunchCore(void *c) {
	VFsThrdInit();
	CHash init=map_get(&TOSLoader,"TaskInit")->data[0];
	FFI_CALL_TOS_2(init.val,GetFs(),0);
	__core_num=c;
	(*cores[__core_num].fp)();
}
void CreateCore(int core,void *fp) {
	cores[core].core_num=core;
	cores[core].fp=fp;
	pthread_create(&cores[core].thread,NULL,LaunchCore,core);
}
