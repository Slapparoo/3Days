#include "poopalloc.h"
#include <stdio.h>
#include <assert.h>
#include <stdint.h>
#include <string.h>
#include <setjmp.h>
#include <stdlib.h>
#include <signal.h>
#ifndef TARGET_WIN32
#include <stddef.h>
#include <sys/mman.h>
#include <unistd.h>
#include <stdatomic.h>
#include <assert.h>
#include <errno.h>
#else
#include <memoryapi.h>
#include <sysinfoapi.h>
#include <winnt.h>
#endif
static int64_t VirtIsAvail(void *ptr,int64_t sz) {
	return 0!=msync(ptr,sz,MS_ASYNC);
}
static void *min32;
void *NewVirtualChunk(int64_t sz,int64_t low32) {
	#ifndef TARGET_WIN32
	static int64_t ps;
	if(!ps) {
			ps=sysconf(_SC_PAGE_SIZE);
			min32=ps;
	}
	int64_t pad=ps;
	void *ret;
	sz+=ps;
	pad=sz%ps;
	if(pad)
		pad=ps;
    if(low32) {
		for(;min32<(1ll<<31);min32+=ps) {
			if(VirtIsAvail(min32,sz/ps*ps+pad)) {
				ret=mmap(min32,sz/ps*ps+pad,PROT_EXEC|PROT_WRITE|PROT_READ,MAP_PRIVATE|MAP_ANON|MAP_FIXED,-1,0);
				if(ret!=MAP_FAILED) {
					min32+=sz/ps*ps+pad;
					break;
				}
			}
		}
    } else
        ret=mmap(NULL,sz/ps*ps+pad,PROT_EXEC|PROT_WRITE|PROT_READ,MAP_PRIVATE|MAP_ANON,-1,0);
    return ret;
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
	static int64_t ps;
	if(!ps) {
			ps=sysconf(_SC_PAGE_SIZE);
	}
	int64_t pad;
	s+=ps;
	pad=s%ps;
	if(pad)
		pad=ps;
	if(ptr<min32)
		min32=ptr;
	munmap(ptr,s/ps*ps+pad);
	#endif
}
