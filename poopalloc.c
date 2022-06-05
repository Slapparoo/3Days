#include "poopalloc.h"
#ifndef TARGET_WIN32
#include <sys/mman.h>
#include <unistd.h>
#include <stdatomic.h>
#include <assert.h>
#ifdef __linux__
//https://man7.org/linux/man-pages/man2/futex.2.html
#include <sys/syscall.h>
#include <linux/futex.h>
struct CMemBlk;
int64_t InBounds(void *ptr,int64_t sz,void **target);
static uint32_t fmtx=1;
static void LockHeap() {
		uint32_t one=1;
		for(;;) {
			if(atomic_compare_exchange_strong(&fmtx,&one,0)) 
				break;
			syscall(SYS_futex,&fmtx,FUTEX_WAIT,0,NULL,NULL,0);
		}
}
static void UnlockHeap() {
		uint32_t zero=0;
		if(atomic_compare_exchange_strong(&fmtx,&zero,1))  {
			syscall(SYS_futex,&fmtx,FUTEX_WAKE,1,NULL,NULL,0);
		}
}
#elif defined __FreeBSD__
#include <sys/types.h>
#include <sys/umtx.h>
static long fmtx=1;
static void LockHeap() {
    long one=1;
    for(;;) {
        if(atomic_compare_exchange_strong(&fmtx,&one,0)) 
            break;
        assert(-1!=_umtx_op(&fmtx,UMTX_OP_WAIT_UINT_PRIVATE,0,NULL,NULL));
    }
}
static void UnlockHeap() {
    long zero=0;
    if(atomic_compare_exchange_strong(&fmtx,&zero,1))  {
        assert(-1!=_umtx_op(&fmtx,UMTX_OP_WAKE_PRIVATE,1,NULL,NULL));
    }
}
#else
#error "Not supported platfrom,feel free to port this mutex code to your platform."
#endif //__linux
#else
#include <windows.h>
#include <memoryapi.h>
#include <synchapi.h>
#include <winnt.h>
CRITICAL_SECTION fmtex;
static void LockHeap() {
        static init;
        if(!init) {
                init=1;
                InitializeCriticalSectionAndSpinCount(&fmtex, 0x00000400);
        }
		EnterCriticalSection(&fmtex);
}
static void UnlockHeap() {
		LeaveCriticalSection(&fmtex);
}
#endif
void *NewVirtualChunk(int64_t sz,int64_t low32) {
	#ifndef TARGET_WIN32
    if(low32)
        return mmap(NULL,sz,PROT_EXEC|PROT_WRITE|PROT_READ,MAP_PRIVATE|MAP_ANON|MAP_32BIT,-1,0);
    else
        return mmap(NULL,sz,PROT_EXEC|PROT_WRITE|PROT_READ,MAP_PRIVATE|MAP_ANON,-1,0);
	#else
    if(low32) {
        //https://stackoverflow.com/questions/54729401/allocating-memory-within-a-2gb-range
        MEMORY_BASIC_INFORMATION ent;
        static int64_t dwAllocationGranularity;
        if (!dwAllocationGranularity)
        {
            SYSTEM_INFO si;
            GetSystemInfo(&si);
            dwAllocationGranularity = si.dwAllocationGranularity;
        }
        int64_t try=dwAllocationGranularity,addr;
        for(;(try&0xFFffFFff)==try;) {
            if(!VirtualQuery((void*)try,&ent,sizeof(ent))) return NULL;
            try=(int64_t)ent.BaseAddress+ent.RegionSize;
            //Fancy code to round up because address is rounded down with VirtualAlloc
            addr=((int64_t)ent.BaseAddress+dwAllocationGranularity-1)&~(dwAllocationGranularity-1);	
            if((ent.State==MEM_FREE)&&(sz<=(try-addr))) {
                return VirtualAlloc((void*)addr,sz,MEM_COMMIT|MEM_RESERVE, PAGE_EXECUTE_READWRITE);
            }
        }
        printf("Out of 32bit address space memory.");
        exit(-1);
    } else
        return VirtualAlloc(NULL,sz,MEM_COMMIT|MEM_RESERVE, PAGE_EXECUTE_READWRITE);
	#endif
}
void FreeVirtualChunk(void *ptr,size_t s) {
	#ifdef TARGET_WIN32
	VirtualFree(ptr,0,MEM_RELEASE);
	#else
	munmap(ptr,s);
	#endif
}
#include <stdio.h>
#include <assert.h>
#include <stdint.h>
#include <string.h>
#include <setjmp.h>
#include <stdlib.h>
#include <stddef.h>
//https://www.techiedelight.com/round-next-highest-power-2/
static int64_t SizeUp(int64_t n) {
    int64_t k=1;
    while(k<n)
        k<<=1;
    if(k<32) k=32;
    return k;
}
static int64_t Log2I(int64_t n) {
    return (int64_t)log2(n);
}
typedef struct CMemBlk {
    int8_t is_self_contained,is_32bit;
    int64_t item_sz,blk_sz,use_cnt;
    struct CMemBlk *last,*next;
} CMemBlk;
typedef struct CMemUnused {
    struct CMemUnused *next;
    CMemBlk *parent;
    int32_t sz;
    int8_t occupied;
    void *task;
    void *caller[5];
} CMemUnused;
void PoopAllocSetCallers(void *ptr,int64_t c,void **callers) {
	int64_t i;
	CMemUnused *blk=ptr;
	blk--;
	for(i=0;i!=c&&i<5;i++) {
		blk->caller[i]=callers[i];
	}
}
typedef struct CHeapFL {
	CMemUnused *first,*final;
} CHeapFL;
typedef struct CHeap {
   CHeapFL cache64[16+1];
   CHeapFL cache32[16+1];
   CMemBlk *blks;
} CHeap;
static CHeap heap;
struct CMemBlk;
CMemBlk *AllocateBlock(int64_t sz,int64_t low32) {
    CMemBlk *ret=NewVirtualChunk(0x400000,low32); //4MB
    CMemUnused *un=ret+1,*last=NULL,*first=NULL;
    CHeapFL *cache=(low32)?heap.cache32:heap.cache64;
    ret->is_self_contained=0;
    ret->item_sz=sz=SizeUp(sz);
    ret->blk_sz=0x400000;
    ret->use_cnt=0;
    while((void*)un+sizeof(CMemUnused)+sz<(void*)ret+0x400000) {
        un->parent=ret;
        if(last) last->next=un;
        if(!first) first=un;
        last=un;
        un=(void*)un+sizeof(CMemUnused)+sz;
    }
    LockHeap();
    ret->next=heap.blks;
    ret->last=NULL;
    if(heap.blks)
		heap.blks->last=ret;
    heap.blks=ret;
    if(last) {
        last->next=cache[Log2I(sz)].first;
    }
    cache[Log2I(sz)].first=first;
    cache[Log2I(sz)].final=last;
    UnlockHeap();
    return ret;
}
static void *__PoopMAlloc(int64_t sz,int64_t low32,void *task) {
    int64_t l=Log2I(SizeUp(sz));
    CMemBlk *ret_blk;
    CMemUnused *unused;
    CHeapFL *cache=(low32)?heap.cache32:heap.cache64;
    void *ptr;
    if(l>16) {
        ret_blk=NewVirtualChunk(sz+sizeof(CMemBlk)+sizeof(CMemUnused),low32);
        ret_blk->is_self_contained=1;
        ret_blk->item_sz=sz;
        ret_blk->blk_sz=sz+sizeof(CMemBlk)+sizeof(CMemUnused);
        LockHeap();
		ret_blk->next=heap.blks;
		ret_blk->last=NULL;
		if(heap.blks)
			heap.blks->last=ret_blk;
		heap.blks=ret_blk;
        UnlockHeap();
        unused=ret_blk+1;
        unused->parent=ret_blk;
        unused->sz=sz;
        unused->task=task;
        unused->occupied=1;
        return memset(unused+1,0,sz);
    }
    loop:
    LockHeap();
    if(cache[l].first) {
		CMemUnused *last=NULL;
		unused=cache[l].first;
		cache[l].first=unused->next;
		if(!unused) goto alloc;
        assert(!unused->occupied);
        if(cache[l].first==unused)
			cache[l].first=cache[l].first->next;
		if(cache[l].final==unused)
			cache[l].final=cache[l].first;
        UnlockHeap();
        unused->sz=sz;
        unused->occupied=1;
        unused->task=task;
        assert(unused!=unused->next);
        return memset(unused+1,0,SizeUp(sz));
    } else {
		alloc:
        UnlockHeap();
        AllocateBlock(sz,low32);
        goto loop;
    }
}
void *PoopMallocTask(int64_t sz,void *t) {
	return __PoopMAlloc(sz,0,t);
}
void *PoopMalloc32Task(int64_t sz,void *t) {
	return __PoopMAlloc(sz,1,t);
}
int64_t MSize(void *ptr) {
    if(!ptr) return 0;
    CMemUnused *un=ptr;
    if(!un[-1].occupied)
		return -1;
    return un[-1].sz;
}
int64_t MSize2(void *ptr) {
    if(!ptr) return 0;
    CMemUnused *un=ptr;
    return un[-1].parent->item_sz+sizeof(CMemUnused);
}
void PoopFree(void *ptr) {
    if(!ptr) return;
    CMemUnused *un=ptr;
    CHeapFL *cache;
    CMemBlk *next,*last;
    if(!InBounds(ptr,0,&un)) return;
    if(!un) return;
    un--;
    if(!un->occupied) return;
    un->occupied=0;
    int64_t l=Log2I(un->parent->item_sz);
    if(un->parent->is_self_contained) {
        LockHeap();
        next=un->parent->next;
        last=un->parent->last;
        if(last) last->next=next;
        if(next) next->last=last;
        if(heap.blks==un->parent)
            heap.blks=(next)?next:last;
        assert(un!=un->next);
        UnlockHeap();
        FreeVirtualChunk(un->parent,un->parent->blk_sz);
        return;
    }
    LockHeap();
    cache=(un->parent->is_32bit)?heap.cache32:heap.cache64;
    if(l<=16) {
        if(cache[l].final) cache[l].final->next=un;
        cache[l].final=un;
        un->next=NULL;
    }
    UnlockHeap();
}
void *PoopMAlloc(int64_t sz) {
    return __PoopMAlloc(sz,0,NULL);
}
void *PoopMAlloc32(int64_t sz) {
    return __PoopMAlloc(sz,1,NULL);
}
void *PoopReAlloc(void *ptr,int64_t sz) {
    void *ret=PoopMAlloc(sz);
    int64_t copy=(sz>MSize(ptr))?MSize(ptr):sz;
    memcpy(ret,ptr,copy);
    PoopFree(ptr);
    return ret;
} 
void *PoopAllocSetTask(void *ptr,void *task) {
    if(!ptr) return;
    if(!task) task=GetFs();
    ((CMemUnused*)ptr)[-1].task=task;
    return ptr;
}
void PoopAllocFreeTaskMem(void *task) {
    CMemBlk *blk,*blk2;
    CMemUnused *un;
    int64_t sz;
    LockHeap();
    for(blk=heap.blks;blk;blk=blk2) {
		blk2=blk->next;
        if(blk->is_self_contained) {
			if(((CMemUnused*)(blk+1))->task==task) {
				UnlockHeap();
				PoopFree((void*)(blk+1)+sizeof(CMemUnused));
				LockHeap();
			}
        } else {
            un=blk+1;
            sz=SizeUp(blk->item_sz);
            while((void*)un+sizeof(CMemUnused)+sz<(void*)blk+0x400000) {
                if(!un->occupied) goto next;
                if(un->task==task) {
					UnlockHeap();
                    un->task=NULL;
                    PoopFree(un+1);
                    LockHeap();
                }
                next:
                un=(void*)un+sizeof(CMemUnused)+sz;
            }
        }
    }
    UnlockHeap();
}
#define LINEAR_PROBES 16
#define HASH_SHIFT 9
int64_t InBounds(void *ptr,int64_t sz,void **target) {
	LockHeap();
	if(target) *target=NULL;
	int64_t probe=0,idx,pidx;
	CMemBlk *blk=heap.blks;
	if(!ptr) {
		UnlockHeap();
		return 0;
	}
	for(;blk;blk=blk->next) {
		if(ptr>=blk&&ptr<=(void*)blk+blk->blk_sz)
			break;
	} 
	if(!blk) {
		UnlockHeap();
		return 0;
	}
	ptr+=sz;
	if(blk+1<=ptr&&ptr<=(void*)blk+blk->blk_sz) {
		if(blk->is_self_contained) {
			if(blk+1<=ptr) {
				if(target) *target=1+(CMemUnused*)(blk+1);
				UnlockHeap();
				return 1;
			}
		} else {
			void *delta=ptr-(void*)(blk+1);
			int64_t i=(int64_t)delta/(sizeof(CMemUnused)+blk->item_sz);
			loop:;
			void *valid_start=sizeof(CMemUnused)+i*(sizeof(CMemUnused)+blk->item_sz);
			valid_start+=(int64_t)(blk+1);
			CMemUnused *un=valid_start;
			--un;
			if(target) *target=un+1;
			if(valid_start<=ptr&&ptr<=valid_start+un->sz) {
				UnlockHeap();
				return un->occupied;
			} else if(un>=ptr&&ptr<=un+1) {
				if(i) {
					--i;
					goto loop;
				}
				UnlockHeap();
				return 0;
			}
		}
	}
	UnlockHeap();
	return 0;
}
void BoundsCheckTests() {
	void *ptr=PoopMAlloc(10);
	assert(!InBounds(ptr+10,1,NULL));
	PoopFree(ptr);
	assert(!InBounds(ptr,1,NULL));
	void *ptr1=PoopMAlloc(10);
	void *ptr2=PoopMAlloc(10);
	void *ptr3=PoopMAlloc(10);
	void *ptr4=PoopMAlloc(10);
	PoopFree(ptr4);
	PoopFree(ptr3);
	PoopFree(ptr2);
	assert(!InBounds(ptr,1,NULL));
	PoopFree(ptr1);
	ptr=PoopMAlloc(1<<17);
	assert(InBounds(ptr,1,NULL));
	assert(InBounds(ptr+(1<<17)-2,1,NULL));
	assert(!InBounds(ptr+(1<<17),1,NULL));
	PoopFree(ptr);
}
