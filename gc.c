#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <setjmp.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <assert.h>
#include <string.h>
#include "3d.h"
#include "gc.h"
#include "ext/vec/src/vec.h"
#include <signal.h>
#ifndef TARGET_WIN32
#include <pthread.h>
#endif
#define DEBUG (0)
#if DEBUG
static void DebugPrint( char *fmt,...)
{
    va_list args;
    va_start(args,fmt);
    vfprintf(stderr,fmt,args);
    va_end(args);
}
#else
static void inline DebugPrint(char *fmt,...) {};
#define printf(fmt,...) (1);
#endif // DEBUG
typedef struct {
    uint8_t marked:1;
    uint8_t isRoot:1;
    uint8_t isNoFree:1;
    uint8_t isExtPtr:1;
    long size;
    void(*destroy)(void*);
    void *rootPointer;
    //Data starts here
} CPtrInfo ;
#ifndef TARGET_WIN32
typedef vec_t(pthread_t) vec_pthread_t;
#endif
static struct CGarbageCollector {
    long bucketCount;
    long filledBuckets;
    void **buckets;
    void *stackStart;
    unsigned long sinceLastCollect;
    unsigned int boundsCheckMode:1;
    unsigned int enabled:1;
    unsigned int collecting:1;
    unsigned long largestAllocSize;
    //Incremented when perfroming a bounds check,decremented when done.
    //Used as a spinlock to not resize the pointer table while a bounds check is active
    int boundsCheckCounter;
    int64_t totalMem;
    int64_t scanedMem;
#ifndef TARGET_WIN32
    pthread_mutex_t mutex;
    vec_pthread_t threads;
#endif
} gc;
static void LockGC() {
    return;
#ifndef TARGET_WIN32
    pthread_mutex_lock(&gc.mutex);
#endif
}
static void UnlockGC() {
    return;
#ifndef TARGET_WIN32
    pthread_mutex_unlock(&gc.mutex);
#endif
}
typedef struct CPair {
    void*(*func)(void*);
    void *data;
} CPair;
#ifndef TARGET_WIN32
static void *__GCSpawnThread(void *data) {
    CPair *pair=data;
    LockGC();
    vec_push(&gc.threads,pthread_self());
    UnlockGC();
    signal(SIGILL,SignalHandler);
    void *ret=pair->func(pair->data);
    LockGC();
    pthread_t t;
    int iter;
    vec_foreach(&gc.threads,t, iter) {
        if(pthread_equal(t, pthread_self())) {
            vec_remove(&gc.threads, t);
            break;
        }
    }
    UnlockGC();
    return ret;
}
pthread_t GCSpawnThread(void*(*func)(void*),void *data) {
    pthread_t thread;
    CPair pair= {.func=func,.data=data};
    pthread_create(&thread, NULL, __GCSpawnThread,&pair);
    return thread;
}
#endif
//From https://github.com/orangeduck/tgc/blob/master/tgc.c
static long Primes[]= {
    1,       5,       11,
    23,      53,      101,     197,
    389,     683,     1259,    2417,
    4733,    9371,    18617,   37097,
    74093,   148073,  296099,  592019,
    1100009, 2200013, 4400021, 8800019
};
static uint64_t HashPtr(void *ptr) {
    return (uint64_t)ptr>>3;
}
#define LINEAR_PROBE_TRIES (8)
static void **GCGetPtr(void *ptr) {
    if(!ptr) return NULL;
    unsigned long h=HashPtr(ptr)%gc.bucketCount;
    long i;
    for(i=h; i!=h+LINEAR_PROBE_TRIES; i++) {
        if(i>=gc.bucketCount) break;
        if(!gc.buckets[i]) continue;
        CPtrInfo *inf=gc.buckets[i];
        if(inf[-1].isExtPtr)
            if(inf[-1].rootPointer==ptr)
                return &gc.buckets[i];
        if(gc.buckets[i]==ptr) return &gc.buckets[i];
    }
    return NULL;
}
/**
 * 1 if success
 */
static int __GCInsertPtr(void **buckets,long count,void *_ptr) {
    CPtrInfo *ptr=_ptr;
    if(ptr[-1].isExtPtr) ptr=ptr[-1].rootPointer;
    unsigned long h=HashPtr(ptr)%count;
    long i=0;
    long try=0;
    for(i=h; i!=count; i++) {
        if(i==h+LINEAR_PROBE_TRIES) break;
        if(!buckets[i]) {
            buckets[i]=_ptr;
            return 1;
        }
    }
    return 0;
}
static int GCRehash(void **dst,long bucketcount) {
    long i;
    for(i=0; i!=gc.bucketCount; i++) {
        if(!gc.buckets[i]) continue;
        if(gc.buckets[i]) if(!__GCInsertPtr(dst,bucketcount,gc.buckets[i])) return 0;
    }
    return 1;
}
static void GCScaleUp() {
    if(gc.collecting) return;
    BLOCK_SIGS;
    int old=gc.collecting;
    gc.collecting=1;
    long p;
    for(p=0; p!=sizeof(Primes)/sizeof(*Primes); p++) {
        if(Primes[p]==gc.bucketCount) break;
    }
    p++;
    if(p>=sizeof(Primes)/sizeof(*Primes)) {
      GC_Collect();
      gc.collecting=old;
      UNBLOCK_SIGS;
      return;
    }
    while(p<sizeof(Primes)/sizeof(*Primes)) {
        if(((double)gc.filledBuckets)/Primes[p]>0.50)
            p++;
        else break;
    }
loop:
    ;
    void **buckets=calloc(sizeof(void*),Primes[p]);
    if(p==sizeof(Primes)/sizeof(*Primes)) {
      GC_Collect();
      p--;
    }
    if(!GCRehash(buckets,Primes[p])) {
        p++;
        //Spinlock
        while(gc.boundsCheckCounter);
        free(buckets);
        goto loop;
    }
    free(gc.buckets);
    gc.bucketCount=Primes[p];
    gc.buckets=buckets;
    gc.collecting=old;
    UNBLOCK_SIGS;
}
static void GCScaleDown() {
    if(gc.collecting) return;
    BLOCK_SIGS;
    long p;
    for(p=0; p!=sizeof(Primes)/sizeof(*Primes); p++) {
        if(Primes[p]==gc.bucketCount) break;
    }
    p--;
    if(p==sizeof(Primes)/sizeof(*Primes)) {
      UNBLOCK_SIGS;
      return;
    }
    while(p>1) {
        if(((double)gc.filledBuckets)/Primes[p]<0.50)
            p--;
        else if(gc.filledBuckets>Primes[p]) {
            p++;
            break;
        } else break;
    }
loop:;
    void **buckets=calloc(sizeof(void*),Primes[p]);
    if(!GCRehash(buckets,Primes[p])) {
        p++;
        free(buckets);
        goto loop;;
    }
    free(gc.buckets);
    gc.bucketCount=Primes[p];
    gc.buckets=buckets;
    UNBLOCK_SIGS;
}
static void GCInsertPtr(void *ptr) {
    gc.filledBuckets++;
    if(((double)gc.filledBuckets/gc.bucketCount)>0.90) GCScaleUp();
loop:
    if(!__GCInsertPtr(gc.buckets,gc.bucketCount,ptr)) {
        GCScaleUp();
        goto loop;
    }
}
static void GCRemovePtr(void *ptr) {
    if(!GCGetPtr(ptr)) return;

    gc.filledBuckets--;
    double scale=((double)gc.filledBuckets)/gc.bucketCount;
    *GCGetPtr(ptr)=NULL;
}
void *GC_Malloc(long num) {
    return GC_Calloc(1,num);
}
void *GC_Calloc(long sz,long num) {
    LockGC();
    if(gc.sinceLastCollect>30000000&&gc.enabled)
      GC_Collect();
    gc.totalMem+=sz*num;
    void *res;
#ifndef TARGET_WIN32
    if(gc.boundsCheckMode)
        res=aligned_alloc(1<<GC_ALIGN_BITS,16+sz*num+sizeof(CPtrInfo));
    else
        res=malloc(sz*num+sizeof(CPtrInfo));
#else
    if(gc.boundsCheckMode)
        res=_aligned_malloc (sz*num+sizeof(CPtrInfo),1<<GC_ALIGN_BITS);
    else
        res=malloc(sz*num+sizeof(CPtrInfo));
#endif
    gc.sinceLastCollect+=sz*num;
    memset(res,0,sz*num+sizeof(CPtrInfo));
    ((CPtrInfo*)res)->size=sz*num;
    if(sz*num>gc.largestAllocSize)
        gc.largestAllocSize=sz*num;
    void *ret=res+sizeof(CPtrInfo);
    GCInsertPtr(ret);
    assert(GCGetPtr(ret));
    UnlockGC();
    return ret;
}
static void __GC_Free(void *ptr) {
    void **has;
    if(!(has=GCGetPtr(ptr))) return UnlockGC();
    void (*destroy)(void*)=((CPtrInfo*)*has)[-1].destroy;
    if(destroy) {
        if(((CPtrInfo*)*has)[-1].isExtPtr)
            destroy(((CPtrInfo*)*has)[-1].rootPointer);
        else
            destroy(ptr);
    }
    if(((CPtrInfo*)*has)[-1].isExtPtr) {
        void *old=*has;
        *GCGetPtr(ptr)=NULL;
        free(&((CPtrInfo*)old)[-1]);
        return;
    } else {
      gc.sinceLastCollect-=MSize(ptr);
      gc.totalMem-=MSize(ptr);
      GCRemovePtr(ptr);
    }
#ifndef TARGET_WIN32
    free(ptr-sizeof(CPtrInfo));
#else
    if(gc.boundsCheckMode) _aligned_free(ptr-sizeof(CPtrInfo));
    else free(ptr-sizeof(CPtrInfo));
#endif
}
void GC_Free(void *ptr) {
    LockGC();
    __GC_Free(ptr);
    UnlockGC();
}
void *GC_Realloc(void *ptr,long size) {
    if(!ptr) {
        return GC_Calloc(size,1);
    }
    long msize=MSize(ptr);
    void *ptr2=GC_MALLOC(size);
    memcpy(ptr2,ptr,(msize>size)?size:msize);
    GC_FREE(ptr);
    return ptr2;
}
static void CheckPtr(void **ptr,int depth);
static void ScanRange(void **from,long size,int depth) {
    void **end=(void*)from+size;
    for(; from+1<=end; from++) {
        CheckPtr(from,depth+1);
    }
}
static void CheckPtr(void **ptr,int depth) {
    void **bucket=NULL;
    if(bucket=GCGetPtr(*ptr)) {
        CPtrInfo *info=((CPtrInfo*)*bucket)-1;
        if(info->marked) return;
        info->marked=1;
        if(info->isExtPtr)
            return;
        gc.scanedMem+=info->size;
        ScanRange(*ptr,info->size,depth);
    }
}
static void __GC_Collect(jmp_buf *regs) {
  long origmem=gc.totalMem;

    void *dummy=NULL;
    void **stend=__builtin_frame_address(0);

    long b;
    printf("=== [MARKING AS 0] ===\n");
    long items=0;
    for(b=0; b!=gc.bucketCount; b++) {
        if(!gc.buckets[b]) continue;
        CPtrInfo *info=((CPtrInfo*)(gc.buckets[b]))-1;
        info->marked=0;
        items++;
    }
    printf("STARTING\n");
    int rootcount=0;
    for(b=0; b!=gc.bucketCount; b++) {
        if(!gc.buckets[b]) continue;
        CPtrInfo *info=((CPtrInfo*)(gc.buckets[b]))-1;

        if(info->isRoot) {
            rootcount++;
            printf("ROOTS:%p\n",info->rootPointer);
            gc.scanedMem=0;
            ScanRange(info->rootPointer,info->size,0);
            //fprintf(stderr,"Scanned %ld bytes at root %p\n",gc.scanedMem,info->rootPointer);
            printf("ROOTE\n");
        }
    }
    ScanRange((void*)regs, sizeof(*regs), 0);
    printf("ROOT COUNT:%i\n",rootcount);
    printf("MARKED %li items.\n",items);

    void **top=gc.stackStart;
    void **bottom=stend;
    if(top<bottom) {
        void **tmp=bottom;
        bottom=top;
        top=tmp;
    }
    gc.scanedMem=0;
    ScanRange(bottom,(void*)top-(void*)bottom,0);
    //fprintf(stderr,"Scanned %ld bytes at stack\n",gc.scanedMem);

    gc.largestAllocSize=0;
    long newcount=0;
    DebugPrint("=== [FREEING ITEMS] ===\n");
    long ritems=0;
    gc.totalMem=0;
    for(b=0; b!=gc.bucketCount; b++) {
        if(!gc.buckets[b]) continue;
        CPtrInfo *info=((CPtrInfo*)(gc.buckets[b]))-1;
        if(info->isRoot) continue;
        if(!info->marked) {
            printf("FREE:%p\n",gc.buckets[b]);
            if(!info->isExtPtr)
                GC_Free(gc.buckets[b]);
            else
                GC_Free(info->rootPointer);
            ritems++;
        } else if(gc.largestAllocSize<info->size) {
            gc.largestAllocSize=info->size;
        } else if(info->marked)
            if(!info->isExtPtr) newcount+=info->size;
    }
    /*
    long nitems=0;
    for(b=0; b!=gc.bucketCount; b++) {
        if(!gc.buckets[b]) continue;
        nitems++;
    }
    assert(nitems==items-ritems);
    */
    //fprintf(stderr,"CLEANUED UP:%ld,%ld\n",gc.totalMem,newcount);
    gc.totalMem=newcount;
    gc.sinceLastCollect=0;
}
void GC_Collect() {
#ifndef USEGC
    return;
#endif
    BLOCK_SIGS;
    if(!gc.enabled) return;
    gc.collecting=1;
    LockGC();
    jmp_buf regs;
    setjmp(regs);
    __GC_Collect(&regs);
    gc.collecting=0;
    if(gc.filledBuckets/(double)gc.bucketCount<0.30) GCScaleDown();
    UnlockGC();
    UNBLOCK_SIGS;
}
void CreateGC(void *stacktop,int boundscheck) {
    gc.bucketCount=1;
    gc.buckets=calloc(sizeof(void*),1);
    gc.filledBuckets=0;
    gc.stackStart=stacktop;
    gc.sinceLastCollect=0;
    gc.boundsCheckMode=boundscheck;
    gc.enabled=1;
#ifndef TARGET_WIN32
    pthread_mutex_init(&gc.mutex,NULL);
#endif
}
void DestroyGC() {
    long b;
    for(b=0; b!=gc.bucketCount; b++)  {
        if(!gc.buckets[b]) continue;
        GC_Free(gc.buckets[b]);
    }
    free(gc.buckets);
}
void AddGCRoot(void *addr,long len) {
    CPtrInfo *ptr=calloc(sizeof(CPtrInfo),1);
    ptr->isRoot=1;
    ptr->size=len;
    ptr->rootPointer=addr;
    GCInsertPtr(ptr+1);
}
long MSize(void *ptr) {
    if(!(ptr=GCGetPtr(ptr)))
        return -1;
    CPtrInfo *info=*(void**)ptr;
    return info[-1].size;
}
void GC_SetDestroy(void *ptr,void(*destroy)(void *)) {
    if(!(ptr=GCGetPtr(ptr))) abort();
    CPtrInfo *p=*(void**)ptr;
    p[-1].destroy=destroy;
}
int InBounds(void *ptr) {
    __sync_fetch_and_add(&gc.boundsCheckCounter,1);
    void *orig=ptr;
    ptr-=sizeof(CPtrInfo);
    void *bottom=__builtin_frame_address(0);
    if(orig>=bottom)
        if(orig<=gc.stackStart) {
            __sync_fetch_and_sub(&gc.boundsCheckCounter,1);
            return 1;
        }
    uint64_t ptri=(void*)ptr;
    ptri>>=GC_ALIGN_BITS;
    ptri<<=GC_ALIGN_BITS;
    ptri+=sizeof(CPtrInfo);
    for(;;) {
        if(orig-(void*)ptri>gc.largestAllocSize) {
            fprintf(stderr,"Access is out of bounds.");
            return 0;
        }
        if(GCGetPtr((void*)ptri)) {
            __sync_fetch_and_sub(&gc.boundsCheckCounter,1);
            long size=MSize((void*)ptri);
            if((void*)ptri+size>=orig)
                if(orig>=(void*)ptri)
                    return 1;
            fprintf(stderr,"Access is %li bytes beyond %li byte area.",orig-(void*)ptri-size,size);
            return 0;
        }
        ptri-=1<<GC_ALIGN_BITS;
    }
}
void GC_Disable() {
    gc.enabled=0;
}
void GC_Enable() {
    gc.enabled=1;
}
void *GCCreateExtPtr(void *ext,void(* destroy)(void*)) {
    CPtrInfo *inf=calloc(sizeof(CPtrInfo),1);
    inf->rootPointer=ext;
    inf->isNoFree=1;
    inf->isExtPtr=1;
    inf->destroy=destroy;
    GCInsertPtr(inf+1);
    assert(GCGetPtr(ext));
    return inf+1;
}
