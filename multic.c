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
void SleepABit() {
	sleep(1);
}
void *GetFs() {
	return __fs;
}
void SetFs(void *f) {
	__fs=f;
}
int CoreNum() {
	return 1;
}
void *GetGs() {
	return 0;
}
