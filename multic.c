#include "3d.h"
#include <sched.h>
#include <signal.h>
#ifdef TARGET_WIN32
static void makecontextWIN(win_ctx_t *ctx,void *stack,void(*fptr)(void*),void *data) {
    ctx->rip=fptr;
    ctx->rcx=data;
    ctx->rsp=stack+MSize(stack)-8*8;//Include area for home registers and minimum of 4 stack items
}
#else
#include <ucontext.h>
#endif
#include <sys/time.h>
static void *Fs;
void *GetFs() {
    loop:
    if(!Fs) {
        Fs=PoopMAlloc(2048); //Is acutally 1198 but it could change
        goto loop;
    }
    return Fs;
}
struct CThread;
typedef struct {
    void *fp;
    void *data;
    void *fs;
    struct CThread *thd;
} CPair;
typedef struct CThread {
    struct CThread *self;
    void *Fs;
    uint64_t sleep_until;
    uint64_t locked:1;
    uint64_t dead:1;
    void *stack;
    char *cur_dir;
    char cur_drv;
    #ifdef TARGET_WIN32
    win_ctx_t ctx;
    #else
    ucontext_t ctx;
    #endif
} CThread;
typedef vec_t(CThread*) vec_CThread_t;
static CThread *cur_thrd;
static vec_CThread_t threads;
static vec_CThread_t dead_threads;
void __FreeThread(CThread *t) {
}
void __Exit() {
    __FreeThread(cur_thrd);
    vec_remove(&threads,cur_thrd);
    PoopFree(cur_thrd);
    __Yield();
}
void __KillThread(CThread *t) {
    t->dead=1;
    __Yield();
}
static int64_t __SpawnFFI(CPair *p) {
    FFI_CALL_TOS_1(p->fp,p->data);
    __Exit();
}
CThread *__Spawn(void *fs,void *fp,void *data,char *name) {
    CThread *ret=PoopMAlloc(sizeof(CThread));
    CPair *p=PoopMAlloc(sizeof(CPair));
    p->fp=fp,p->data=data,p->fs=fs,p->thd=ret;
    signal(SIGSEGV,FualtCB);
    #ifndef TARGET_WIN32
    signal(SIGBUS,FualtCB);
    #endif
    VFsThrdInit();
    CThread *thd=PoopMAlloc(sizeof(CThread));
    thd->self=thd;
    thd->Fs=p->fs;
    thd->cur_dir=cur_dir;
    thd->cur_drv=cur_drv;
    getcontext(&thd->ctx);
    if(!thd->dead) {
        vec_push(&threads,thd);
        #ifndef TARGET_WIN32
        thd->ctx.uc_stack.ss_sp=PoopMAlloc(1<<16);
        thd->ctx.uc_stack.ss_size=1<<16;
        thd->ctx.uc_stack.ss_flags=0;
        makecontext(&thd->ctx,&__SpawnFFI,1,p);
        #else
        thd->stack=PoopMAlloc(1<<16);
        makecontextWIN(&thd->ctx,thd->stack,&__SpawnFFI,p);
        #endif
    }
    return ret;
}
void __AwakeThread(CThread *t) {
    t->locked=0;
}
void __Suspend(CThread *t) {
    t->locked=1;
    if(t==cur_thrd) __Yield();
}
void __Sleep(int64_t t) {
    cur_thrd->sleep_until=t+SDL_GetTicks();
    __Yield();
}
void __Yield() {
    static int flop;
    #ifndef TARGET_WIN32
    ucontext_t old;
    #else
    win_ctx_t old;
    #endif
    getcontext(&old);
    int64_t i;
    for(i=0;i!=dead_threads.length;i++) {
		if(cur_thrd!=dead_threads.data[i]) {
			PoopFree(dead_threads.data[i]->stack);
			PoopFree(dead_threads.data[i]);
			vec_remove(&dead_threads,dead_threads.data[i]);
		}
	}
    loop:;
    int64_t ms=SDL_GetTicks(),min_sleep=-1.,t;
    int64_t idx,idx2=threads.length-1;
    vec_find(&threads,cur_thrd,idx);
    if(idx!=-1) {
        getcontext(&threads.data[idx]->ctx);
        if(!(flop^=1))
            return;
        for(idx2=(idx+1)%threads.length;idx!=idx2;idx2=(1+idx2)%threads.length) {
            if(threads.data[idx2]->locked||threads.data[idx2]->dead) {
                continue;
            } else if(threads.data[idx2]->sleep_until) {
                t=threads.data[idx2]->sleep_until-ms;
                if(min_sleep>=0)
                    min_sleep=(min_sleep<t)?min_sleep:t;
                else
                    min_sleep=t;
                if(threads.data[idx2]->sleep_until<ms) {
                    threads.data[idx2]->sleep_until=0;
                    goto pass;
                }
            } else {
                pass:
                threads.data[idx]->cur_drv=cur_drv;
                threads.data[idx]->cur_dir=cur_dir;
                threads.data[idx]->Fs=Fs;
                threads.data[idx]->self=cur_thrd;
                //
                ent:
                cur_thrd=threads.data[idx2]->self;
                Fs=threads.data[idx2]->Fs;
                cur_dir=threads.data[idx2]->cur_dir;
                cur_drv=threads.data[idx2]->cur_drv;
                setcontext(&threads.data[idx2]->ctx);
            }
        }
    } else  {
        if(!(flop^=1))
            return;
        goto ent;
    }
    if(min_sleep>0)
    SDL_Delay(1+(int64_t)min_sleep);
    goto loop;
}
void __AwaitThread(CThread *t) {
    for(;;) {
        if(t->dead) {
            break;
        } else
            __Yield();
    }
}
void InitThreads() {
    vec_init(&threads);
    vec_init(&dead_threads);
    /*
    struct itimerval timer;
    timer.it_value.tv_sec=0;
    timer.it_value.tv_usec=10000;
    timer.it_interval.tv_sec=0;
    timer.it_interval.tv_usec=10000;
    signal(SIGPROF,&Yield);
    setitimer(ITIMER_PROF,&timer,NULL);
    */
}
