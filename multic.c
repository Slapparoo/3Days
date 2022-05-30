#include "3d.h"
#include <sched.h>
#include <signal.h>
#include <stdatomic.h>
static void MakeContext(ctx_t *ctx,void *stack,void(*fptr)(void*),void *data) {
    ctx->rip=fptr;
    #ifdef TARGET_WIN32
    ctx->rcx=data;
    #else
    ctx->rdi=data;
    #endif
    ctx->rsp=stack+MSize(stack)-8*8;//Include area for home registers and minimum of 4 stack items
}
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
    void *Fs;
    uint64_t sleep_until;
    int8_t locked;
    int8_t dead;
    int8_t to_kill;
    int64_t *wait_for_ptr;
    int64_t wait_for_ptr_old_value;
    int64_t wait_for_ptr_mask;
    void *stack;
    char *cur_dir;
    char cur_drv;
    ctx_t ctx;
} CThread;
typedef vec_t(CThread*) vec_CThread_t;
static  CThread *cur_thrd;
static  vec_CThread_t threads;
static  vec_CThread_t dead_threads;
void __FreeThread(CThread *t) {
	if(!t) return;
    PoopAllocFreeTaskMem(t->Fs);
}
void __Exit() {
    vec_remove(&threads,cur_thrd);
    vec_push(&dead_threads,cur_thrd);
    cur_thrd->dead=1;
    __Yield();
}
void __SetThreadPtr(CThread *t,void *ptr) {
	if(!t) return;
    t->ctx.rip=ptr;
}
void __KillThread(CThread *t) {
	if(!t) return;
    CHash **ex=map_get(&TOSLoader,"Exit");
    if(t->to_kill) return;
    t->to_kill=1;
    if(cur_thrd==t) {
        if(ex)
            FFI_CALL_TOS_0(ex[0]->val);
        __Exit();
    } else {
        __SetThreadPtr(t,ex[0]->val);
    }
}
static int64_t __SpawnFFI(CPair *p) {
    CHash **ex;
    FFI_CALL_TOS_1(p->fp,p->data);
    if(ex=map_get(&TOSLoader,"Exit")) {
        FFI_CALL_TOS_0(ex[0]->val);
    }
    __Exit();
}
CThread *__Spawn(void *fs,void *fp,void *data,char *name) {
    CThread *thd=PoopMAlloc(sizeof(CThread));
    CPair *p=PoopMAlloc(sizeof(CPair));
    p->fp=fp,p->data=data,p->fs=fs,p->thd=thd;
    signal(SIGSEGV,FualtCB);
    #ifndef TARGET_WIN32
    signal(SIGBUS,FualtCB);
    #endif
    VFsThrdInit();
    thd->Fs=p->fs;
    thd->cur_dir=cur_dir;
    thd->cur_drv=cur_drv;
    GetContext(&thd->ctx);
    if(!thd->dead) {
        vec_push(&threads,thd);
        thd->stack=PoopMAlloc(1<<16);
        MakeContext(&thd->ctx,thd->stack,&__SpawnFFI,p);
    }
    return thd;
}
void __AwakeThread(CThread *t) {
	if(!t) return;
    t->locked=0;
}
void __Suspend(CThread *t) {
	if(!t) return;
    t->locked=1;
    if(t==cur_thrd) __Yield();
}
void __Sleep(int64_t t) {
    if(cur_thrd)
        cur_thrd->sleep_until=t+SDL_GetTicks();
    __Yield();
}
void __SleepUntilChange(int64_t *ptr,int64_t mask) {
    if(cur_thrd) {
        cur_thrd->wait_for_ptr=ptr;
        cur_thrd->wait_for_ptr_old_value=*ptr;
        cur_thrd->wait_for_ptr_mask=mask;
    }
    __Yield();
}
void __Yield() {
	static int flop;
    enter:;
    #ifndef TARGET_WIN32
    sched_yield();
    #endif
    ctx_t old;
    GetContext(&old);
    int64_t i;
    for(i=0;i!=threads.length;i++)
        if(threads.data[i]->to_kill) {
            if(cur_thrd!=threads.data[i]) {
                __KillThread(threads.data[i]);
                threads.data[i]->to_kill=0;
            }
        } else if(threads.data[i]->wait_for_ptr) {
            int64_t mask=threads.data[i]->wait_for_ptr_mask;
            if((threads.data[i]->wait_for_ptr_old_value&mask)==(mask&atomic_load(threads.data[i]->wait_for_ptr))) {
                threads.data[i]->locked=1;
            } else {
                threads.data[i]->wait_for_ptr=NULL;
                threads.data[i]->locked=0;
            }
        }
    rem:
    for(i=0;i!=dead_threads.length;i++) {
		if(cur_thrd!=dead_threads.data[i]) {
            __FreeThread(dead_threads.data[i]);
			PoopFree(dead_threads.data[i]->stack);
			PoopFree(dead_threads.data[i]);
			vec_remove(&dead_threads,dead_threads.data[i]);
            goto rem;
		}
	}
    loop:;
    int64_t ms=SDL_GetTicks(),min_sleep=-1,t,most_overdue_i=-1;
    int64_t dont_sleep=0;
    int64_t idx,idx2=threads.length-1;
    vec_find(&threads,cur_thrd,idx);
    if(idx==-1) {
        idx=0;
        idx2=threads.length-1;
        goto ent;
    }
    GetContext(&threads.data[idx]->ctx);
    if(!(flop^=1)) {
		return;
	}
	//idx doesnt change but the phyiscall index of the CAN CHANGE,SO RECOMPUTE 
    vec_find(&threads,cur_thrd,idx);
    check:
    for(idx2=(idx+1)%threads.length;idx!=idx2;idx2=(1+idx2)%threads.length) {
        if(threads.data[idx2]->locked||threads.data[idx2]->dead) {
            continue;
        } else if(threads.data[idx2]->sleep_until) {
			t=threads.data[idx2]->sleep_until-ms;
			if(t<=0) {
				if(most_overdue_i==-1)
					most_overdue_i=idx2;
				else if(threads.data[most_overdue_i]->sleep_until>threads.data[idx2]->sleep_until)
					most_overdue_i=idx2;			
			}
            if(min_sleep==-1)
				min_sleep=t;
            else if(min_sleep>0)
                min_sleep=(min_sleep<t)?min_sleep:t;
            if(min_sleep<0)
                min_sleep=-1;
            if(threads.data[idx2]->sleep_until<=ms) {
				threads.data[idx2]->sleep_until=0;
            }
        } else {
            //We got here if threads.data[idx2]->sleep_until==0
            dont_sleep=1;
            pass:
            threads.data[idx]->cur_drv=cur_drv;
            threads.data[idx]->cur_dir=cur_dir;
            threads.data[idx]->Fs=Fs;
            //
            ent:
            cur_thrd=threads.data[idx2];
            Fs=threads.data[idx2]->Fs;
            cur_dir=threads.data[idx2]->cur_dir;
            cur_drv=threads.data[idx2]->cur_drv;
            SetContext(&threads.data[idx2]->ctx);
        }
    }
    if(most_overdue_i!=-1) {
		idx2=most_overdue_i;
		goto pass;
	}
    if(min_sleep>0&&!dont_sleep) {//We skipped over the current thread when looking for canidates to swap too
		SDL_Delay((int64_t)min_sleep);
    }
    goto loop;
}
void __AwaitThread(CThread *t) {
	if(!t) return;
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
