#include "poopalloc.h"
#include <sys/mman.h>
#include <stdio.h>
#include <assert.h>
#include <stdint.h>
#include <string.h>
#include <setjmp.h>
#include <stdlib.h>
#include <stddef.h>
#include <unistd.h>
#include "3d.h"
#define PALLOC_ALIGN (16)
//
// Align to 4MB
// (1<<22)-(8+8+16*246722+246722) leaves us with 14 bytes extra space
//
#define PALLOC_ITEMS2 (246722)
struct CPoopSlab2;
typedef struct {
  struct CPoopSlab2 *slab;
  int32_t len;
  int16_t offset,gc_age;
} CPoopPtr;
typedef enum {
    SLAB_32=0,
    SLAB_128=1,
    SLAB_512=2,
    SLAB_1024=3,
    SLAB_BIG=4,
} ESlabType;
typedef struct CPoopSlab2 {
  struct CPoopSlab2 *prev,*next;
  int32_t filled;
  int64_t ident;
}  CPoopSlab2;
void PoopCollect();
#define FIFO_MASK (0x1000-1)
typedef struct CFifo {
    CPoopPtr items[FIFO_MASK+1];
    int64_t in_ptr;
    int64_t out_ptr;
} CFifo;
static int64_t SixteenBlocks(int64_t n);
static void *StackStart;
static int64_t FifoIns(CFifo *fifo,CPoopPtr value) {
    int64_t new_in_ptr=(fifo->in_ptr+1)&FIFO_MASK;
    if(new_in_ptr==fifo->out_ptr)
        return 0;
    else {
        fifo->items[fifo->in_ptr]=value;
        fifo->in_ptr=new_in_ptr;
        return 1;
    }
}
static CPoopPtr *FifoDel(CFifo *f) {
    CFifo *ret;
    if(f->in_ptr==f->out_ptr)
        return NULL;
    else {
        ret=&f->items[f->out_ptr];
        f->out_ptr=(f->out_ptr+1)&FIFO_MASK;
        return ret;
    }
}
static int64_t FifoSize(CFifo *f) {
    int64_t res;
    if (f->out_ptr>f->in_ptr)
        res=FIFO_MASK+1-(f->in_ptr-f->out_ptr);
    else
        res=f->in_ptr-f->out_ptr;
    return res;
}

static CPoopPtr *FifoPeek(CFifo *f,int64_t offset) {
    if(offset>=FifoSize(f)) return NULL;
    if(offset<0) return NULL;
    else return &f->items[(f->out_ptr+offset)&FIFO_MASK];
}
typedef struct CPoopSlab32 {
    struct CPoopSlab32 *prev,*next;
    int32_t filled;
    int64_t ident;
    int8_t items[2650*48];
    int8_t occupied[2650];
} CPoopSlab32;
typedef struct CPoopSlab128 {
    struct CPoopSlab128 *prev,*next;
    int32_t filled;
    int64_t ident;
    int8_t items[800*(128+16)];
    int8_t occupied[800];
} CPoopSlab128;
typedef struct CPoopSlab512 {
    struct CPoopSlab512 *prev,*next;
    int32_t filled;
    int64_t ident;
    int8_t items[220*(512+16)];
    int8_t occupied[220];
} CPoopSlab512;
typedef struct CPoopSlab1024 {
  struct CPoopSlab1024 *prev,*next;
  int32_t filled;
  int64_t ident;
  int8_t items[125*(1024+16)];
  int8_t occupied[125];
} CPoopSlab1024;
static void *__Alloc(CPoopSlab2 *slab,int8_t *data,int8_t *filled,size_t count,size_t width) {
    int64_t idx=rand()%count,idx2=count;
    for(;--idx2>=0&&!filled[idx%count];idx--) {
            slab->filled++;
            filled[idx%count]=1;
            CPoopPtr *ptr=data+width*(idx%count);
            ptr->slab=slab;
            ptr->offset=idx%count;
            return ptr;
    }
    return NULL;
}
static int64_t gc_age;
static CFifo Arena32;
static CFifo Arena128;
static CFifo Arena512;
static CFifo Arena1024;
#define HASH_MASK (0x10000-1)
static CPoopSlab2 *HashTable[HASH_MASK+1];
#define HASH_PTR(ptr) ((int64_t)(ptr)/4096)
static int64_t LargestAlloc;
static int64_t since_last_collect;

static CPoopSlab2 *GetSlabByPtr(void *ptr) {
    int64_t scale=-1;
    loop:
    scale++;
    assert(getpagesize()==4096);
    int64_t chopped=((int64_t )ptr)/(4096)*4096;
    chopped-=scale*4096;
    long idx=HASH_PTR(chopped)&HASH_MASK;
    CPoopSlab2 *slab;
    for(slab=HashTable[idx];slab;slab=slab->next) {
        switch(slab->ident) {
        case SLAB_32:
            if(slab<=ptr&&ptr<=sizeof(CPoopSlab32)+(void*)slab)
                return slab;
        case SLAB_128:
            if(slab<=ptr&&ptr<=sizeof(CPoopSlab128)+(void*)slab)
                return slab;
        case SLAB_512:
            if(slab<=ptr&&ptr<=sizeof(CPoopSlab512)+(void*)slab)
                return slab;
        case SLAB_1024:
            if(slab<=ptr&&ptr<=sizeof(CPoopSlab1024)+(void*)slab)
                return slab;
        case SLAB_BIG:
            if(slab<=ptr&&ptr<=(void*)slab+slab->filled)
                return slab;
        }
    }
    if(scale*4096<=(1<<17))
        goto loop;
    if(LargestAlloc>=scale*4096)
        goto loop;
    return NULL;
}
static void AddSlabToHash(CPoopSlab2 *slab) {
    CPoopSlab2 *first=HashTable[HASH_PTR(slab)&HASH_MASK];
    if(!first) {
        slab->prev=slab->next=NULL;
    } else {
        slab->next=first;
        if(first) first->prev=slab;
        slab->prev=NULL;
    }
    HashTable[HASH_PTR(slab)&HASH_MASK]=slab;
}
static void *Alloc32(size_t size) {
    CPoopSlab32 *slab,*min_slab=NULL;
    CPoopPtr *ptr,*fif,dummy;
    int64_t tried_collect=0;
    loop:;
    int64_t min=0xfffff,min_idx;
    if(FifoSize(&Arena32)) {
        fif=FifoDel(&Arena32);
        slab=fif->slab;
        uint64_t off=fif->offset;
        ptr=&slab->items[(32+16)*off];
        ptr->len=size;
        ptr->slab=slab;
        ptr->offset=fif->offset;
        slab->occupied[ptr->offset]=1;
        return memset(ptr+1,0,size);
    } else {
        if(1) {
            add:
            assert((1<<17)>=sizeof(CPoopSlab32));
            slab=mmap(NULL,1<<17,PROT_EXEC|PROT_WRITE|PROT_READ,MAP_PRIVATE|MAP_ANON,-1,0);
            memset(slab,0,sizeof(CPoopSlab2));
            dummy.slab=slab;
            dummy.len=32;
            dummy.offset=sizeof(slab->occupied);
            while(--dummy.offset>=0) {
                if(!FifoIns(&Arena32,dummy))break;
            }
            slab->ident=SLAB_32;
            AddSlabToHash(slab);
            goto loop;
        } else
            min_slab=slab;
        ptr=__Alloc(min_slab,min_slab->items,min_slab->occupied,sizeof(min_slab->occupied),48);
        if(!ptr) goto add;
        ptr->len=size;
        ptr->slab->filled++;
        return memset(ptr+1,0,32);
    }
}
static void *Alloc128(size_t size) {
    CPoopSlab128 *slab,*min_slab=NULL;
    int64_t tried_collect=0;
    CPoopPtr *ptr,*fif,dummy;
    loop:;
    int64_t min=0xfffff,min_idx;
    if(FifoSize(&Arena128)) {
        fif=FifoDel(&Arena128);
        slab=fif->slab;
        uint64_t off=fif->offset;
        ptr=&slab->items[(128+16)*off];
        ptr->len=size;
        ptr->slab=slab;
        ptr->offset=fif->offset;
        slab->occupied[ptr->offset]=1;
        return memset(ptr+1,0,size);
    } else {
        if(1) {
            add:
            assert((1<<17)>=sizeof(CPoopSlab128));
            slab=mmap(NULL,1<<17,PROT_EXEC|PROT_WRITE|PROT_READ,MAP_PRIVATE|MAP_ANON,-1,0);
            memset(slab,0,sizeof(CPoopSlab2));
            dummy.slab=slab;
            dummy.len=128;
            dummy.offset=sizeof(slab->occupied);
            while(--dummy.offset>=0) {
                if(!FifoIns(&Arena128,dummy))break;;
            }
            slab->ident=SLAB_128;
            AddSlabToHash(slab);
            goto loop;
        } else
            min_slab=slab;
        ptr=__Alloc(min_slab,min_slab->items,min_slab->occupied,sizeof(min_slab->occupied),128+16);
        if(!ptr) goto add;
        ptr->len=size;
        ptr->slab->filled++;
        return memset(ptr+1,0,size);
    }
}

static void *Alloc512(size_t size) {
    CPoopSlab512 *slab,*min_slab=NULL;
    CPoopPtr *ptr,*fif,dummy;
    int64_t tried_collect=0;
    loop:;
    int64_t min=0xfffff,min_idx;
    if(FifoSize(&Arena512)) {
        fif=FifoDel(&Arena512);
        slab=fif->slab;
        uint64_t off=fif->offset;
        ptr=&slab->items[(512+16)*off];
        ptr->len=size;
        ptr->slab=slab;
        ptr->offset=fif->offset;
        slab->occupied[ptr->offset]=1;
        return memset(ptr+1,0,size);
    } else {
        if(1) {
            add:
            assert((1<<17)>=sizeof(CPoopSlab512));
            slab=mmap(NULL,1<<17,PROT_EXEC|PROT_WRITE|PROT_READ,MAP_PRIVATE|MAP_ANON,-1,0);
            memset(slab,0,sizeof(CPoopSlab2));
            dummy.slab=slab;
            dummy.len=512;
            dummy.offset=sizeof(slab->occupied);
            while(--dummy.offset>=0) {
                    if(!FifoIns(&Arena512,dummy))break;
            }
            slab->ident=SLAB_512;
            AddSlabToHash(slab);
            goto loop;
        } else
            min_slab=slab;
        ptr=__Alloc(min_slab,min_slab->items,min_slab->occupied,sizeof(min_slab->occupied),512+16);
        if(!ptr) goto add;
        ptr->len=size;
        ptr->slab->filled++;
        return memset(ptr+1,0,size);
    }
}
static void *Alloc1024(size_t size) {
    CPoopSlab1024 *slab,*min_slab=NULL;
    CPoopPtr *ptr,*fif,dummy;
    int64_t tried_collect=0;
    loop:;
    int64_t min=0xfffff,min_idx;
    if(FifoSize(&Arena1024)) {
        fif=FifoDel(&Arena1024);
        slab=fif->slab;
        uint64_t off=fif->offset;
        ptr=&slab->items[(1024+16)*off];
        ptr->len=size;
        ptr->slab=slab;
        ptr->offset=fif->offset;
        slab->occupied[ptr->offset]=1;
        return memset(ptr+1,0,size);
    } else {
        if(1) {
            add:
            assert((1<<17)>=sizeof(CPoopSlab1024));
            slab=mmap(NULL,1<<17,PROT_EXEC|PROT_WRITE|PROT_READ,MAP_PRIVATE|MAP_ANON,-1,0);
            memset(slab,0,sizeof(CPoopSlab2));
            dummy.slab=slab;
            dummy.len=1024;
            dummy.offset=sizeof(slab->occupied);
            while(--dummy.offset>=0) {
                    if(!FifoIns(&Arena1024,dummy))break;
            }
            slab->ident=SLAB_1024;
            AddSlabToHash(slab);
            goto loop;
        } else
            min_slab=slab;
        ptr=__Alloc(min_slab,min_slab->items,min_slab->occupied,sizeof(min_slab->occupied),1024+16);
        if(!ptr) goto add;
        ptr->len=size;
        ptr->slab->filled++;
        return memset(ptr+1,0,size);
    }
}
int64_t MSize(void *ptr) {
    if(ptr==NULL) return 0;
    CPoopPtr *p=ptr;
    p--;
    return p->len;
}
void PoopFree(void *ptr) {
    if(!ptr) return;
    if(!GetSlabByPtr(ptr)) {
        printf("Invalid free of %p.\n",ptr);
        return ;
    }
    CPoopSlab2 *slab;
    CPoopPtr *p=ptr;
    int64_t width,offset,o_width,orig_offset,idx;
    p--;
    CPoopPtr orig=*p;
    int8_t *occupied=NULL;
    switch(p->slab->ident) {
    case SLAB_32:
        occupied=((CPoopSlab32*)p->slab)->occupied;
        break;
    case SLAB_128:
        occupied=((CPoopSlab128*)p->slab)->occupied;
        break;
    case SLAB_512:
        occupied=((CPoopSlab512*)p->slab)->occupied;
        break;
    case SLAB_1024:
        occupied=((CPoopSlab1024*)p->slab)->occupied;
        break;
    case SLAB_BIG:
        goto end;
    }
    if(!occupied[p->offset]) {
        fail:
        return;
    }
    since_last_collect-=MSize(ptr);
    assert(occupied[p->offset]==1);
    occupied[p->offset]=0;
    memset(ptr,0xbe,p->len);
    if(p->len<=32) {
        FifoIns(&Arena32,orig);
    } else if(p->len<=128) {
        FifoIns(&Arena128,orig);
    } else if(p->len<=512) {
        FifoIns(&Arena512,orig);
    } else if(p->len<=1024) {
        FifoIns(&Arena1024,orig);
    } else {
        end:;
        CPoopSlab2 *prev=p->slab->prev,*next=p->slab->next;
        if(HashTable[HASH_PTR(ptr)&HASH_MASK]==p->slab) {
            HashTable[HASH_PTR(ptr)&HASH_MASK]=next;
            if(next) next->prev=NULL;
            munmap(p->slab,p->slab->filled);
            return;
        }
        munmap(p->slab,p->slab->filled);
        if(prev) prev->next=next;
        if(next) next->prev=prev;
        return;
    }
    if(0==--p->slab->filled) {
        CFifo *to_filter;
        switch(p->slab->ident) {
          case SLAB_128:
          to_filter=&Arena128;
          break;
          case SLAB_512:
          to_filter=&Arena512;
          break;
          case SLAB_32:
          to_filter=&Arena32;
          break;
          case SLAB_1024:
          to_filter=&Arena1024;
          break;
        }
        long i;
        for(i=0;i!=FIFO_MASK+1;i++) {
          next:
          if(i==FifoSize(to_filter)) break;
          if(FifoPeek(to_filter,i)->slab==orig.slab) {
              CPoopPtr tmp=*FifoPeek(to_filter,0);
              *FifoPeek(to_filter,i)=tmp;
              FifoDel(to_filter);
              goto next;
          }
        }
        CPoopSlab2 *next=orig.slab->next,*prev=orig.slab->prev;
        munmap(orig.slab,1l<<17l);
        if(prev) prev->next=next;
        if(next) next->prev=prev;
    }
}
void *PoopReAlloc(void *ptr,int64_t size) {
  int64_t size2=MSize(ptr);
  void *ret=PoopMAlloc(size);
  LargestAlloc=(LargestAlloc>size)?LargestAlloc:size;
  memcpy(ret,ptr,size>size2?size2:size);
  assert(ret!=ptr);
  PoopFree(ptr);
  return ret;
}
void *PoopMAlloc(int64_t size) {
  if(size==761680639942081328l) return NULL;
  since_last_collect+=size;
  void *ret;
  if(size<=32) {
        ret=Alloc32(size);
        goto end;
  }
  if(size<=128) {
        ret=Alloc128(size);
        goto end;
  }
  if(size<=512) {
        ret=Alloc512(size);
        goto end;
  }
  if(size<=1024) {
        ret=Alloc1024(size);
        goto end;
  }
  if(since_last_collect>=(1l<<20l)) {
  	since_last_collect=0;
    PoopCollect();
  }
  int64_t size2;
  CPoopSlab2 *big=mmap(NULL,size2=sizeof(CPoopSlab2)+sizeof(CPoopPtr)+((sizeof(CPoopPtr)+sizeof(CPoopSlab2))%16)+size,PROT_EXEC|PROT_WRITE|PROT_READ,MAP_PRIVATE|MAP_ANON,-1,0);
  big->ident=SLAB_BIG;
  big->filled=size2;
  ret=memset((void*)big+size2-size,0,size);
  ((CPoopPtr*)ret)[-1].len=size;
  ((CPoopPtr*)ret)[-1].slab=big;
  AddSlabToHash(big);
  end:
  ((CPoopPtr*)ret)[-1].gc_age=gc_age;
  assert(MSize(ret)==size);
  return ret;
}
int InBounds(void *ptr,int64_t size) {
    if(StackStart>=ptr&&ptr>=__builtin_frame_address(0))
    	return 1;
    int64_t idx,cnt,size2;
    CPoopSlab2 *cur_slab=GetSlabByPtr(ptr);
    void *base;
    check:
    if(cur_slab) {
        switch(cur_slab->ident) {
        case SLAB_32:;
            CPoopSlab32 *s32=cur_slab;
            idx=((int8_t*)ptr-s32->items)/(32+16);
            if(idx>=sizeof(s32->occupied)) goto oob;
            for(;idx>0&&!s32->occupied[idx];idx--);
            base=s32->items+idx*(32+16);
            cnt=((CPoopPtr*)base)[0].len;
            base+=16;
            break;
        case SLAB_128:;
            CPoopSlab128 *s128=cur_slab;
            idx=((int8_t*)ptr-s128->items)/(128+16);
            if(idx>=sizeof(s128->occupied)) goto oob;
            for(;idx>0&&!s128->occupied[idx];idx--);
            base=s128->items+idx*(128+16);
            cnt=((CPoopPtr*)base)[0].len;
            base+=16;
            break;
        case SLAB_512:;
            CPoopSlab512 *s512=cur_slab;
            idx=((int8_t*)ptr-s512->items)/(512+16);
            if(idx>=sizeof(s512->occupied)) goto oob;
            for(;idx>0&&!s512->occupied[idx];idx--);
            base=s512->items+idx*(512+16);
            cnt=((CPoopPtr*)base)[0].len;
            base+=16;
            break;
        case SLAB_1024:;
            CPoopSlab1024 *s1024=cur_slab;
            idx=((int8_t*)ptr-s1024->items)/(1024+16);
            if(idx>=sizeof(s1024->occupied)) goto oob;
            for(;idx>0&&!s1024->occupied[idx];idx--);
            base=s1024->items+idx*(1024+16);
            cnt=((CPoopPtr*)base)[0].len;
            base+=16;
            break;
        case SLAB_BIG:
            size2=sizeof(*cur_slab)+sizeof(CPoopPtr);
            size2+=size2%16;
            base=size2+(void*)cur_slab;
            cnt=cur_slab->filled-size2;
            break;
        }
        if(!((base+cnt>=ptr+size)&&(base<=ptr))) {
            if(ptr-(base+cnt)>=0)
                fprintf(stderr,"Access is %ld bytes beyond %ld byte area.\n",ptr-(base+cnt),cnt);
            else
                fprintf(stderr,"Access is %ld bytes before %ld byte area.\n",base-(ptr),cnt);
        } else
            return 1;
    } else {
        oob:
        fprintf(stderr,"Access is out of bounds.\n");
    }
    return 0;
}
static int64_t __ScanPtr(void **ptr,int64_t len) {
    int64_t idx,cnt=0;
    CPoopSlab2 *cur_slab;
    void **end=ptr+len/sizeof(void*);
    LargestAlloc=0;
    while(ptr<end) {
        cur_slab=GetSlabByPtr(*ptr);
        if(!cur_slab) goto next;
        void *end_ptr=cur_slab+1;
        switch(cur_slab->ident) {
        case SLAB_32:
        end_ptr=(void*)(cur_slab+1)+sizeof(((CPoopSlab32*)NULL)->items);
        break;
        case SLAB_128:
        end_ptr=(void*)(cur_slab+1)+sizeof(((CPoopSlab128*)NULL)->items);
        break;
        case SLAB_512:
        end_ptr=(void*)(cur_slab+1)+sizeof(((CPoopSlab512*)NULL)->items);
        break;
        case SLAB_1024:
        end_ptr=(void*)(cur_slab+1)+sizeof(((CPoopSlab1024*)NULL)->items);
        break;
        case SLAB_BIG:
            end_ptr=(void*)cur_slab+cur_slab->filled;
        }
        if(
            (void*)(cur_slab+1)<=*ptr&&*ptr <= end_ptr
        ) {
            idx=((CPoopPtr*)*ptr)[-1].offset;
            switch(cur_slab->ident) {
            case SLAB_32:
                if(sizeof(((CPoopSlab32*)cur_slab)->occupied)>idx&&idx>=0)
                    if(((CPoopSlab32*)cur_slab)->occupied[idx])
                        if(&((CPoopSlab32*)cur_slab)->items[idx*(32+16)+sizeof(CPoopPtr)]==*ptr)
                            goto scan;
                break;
            case SLAB_128:
                if(sizeof(((CPoopSlab128*)cur_slab)->occupied)>idx&&idx>=0)
                    if(((CPoopSlab128*)cur_slab)->occupied[idx])
                        if(&((CPoopSlab128*)cur_slab)->items[idx*(128+16)+sizeof(CPoopPtr)]==*ptr)
                            goto scan;
                break;
            case SLAB_512:
                if(sizeof(((CPoopSlab512*)cur_slab)->occupied)>idx&&idx>=0)
                    if(((CPoopSlab512*)cur_slab)->occupied[idx])
                        if(&((CPoopSlab512*)cur_slab)->items[idx*(512+16)+sizeof(CPoopPtr)]==*ptr)
                            goto scan;
                break;
            case SLAB_1024:
                if(sizeof(((CPoopSlab1024*)cur_slab)->occupied)>idx&&idx>=0)
                    if(((CPoopSlab1024*)cur_slab)->occupied[idx])
                        if(&((CPoopSlab1024*)cur_slab)->items[idx*(1024+16)+sizeof(CPoopPtr)]==*ptr)
                            goto scan;
                break;
            case SLAB_BIG:
                if(((CPoopPtr*)*ptr)[-1].slab==cur_slab)
                    goto scan;
            }
            goto next;
            scan:
            cnt++;
            if(((CPoopPtr*)*ptr)[-1].gc_age<gc_age) {
                ((CPoopPtr*)*ptr)[-1].gc_age=gc_age;
                cnt+=__ScanPtr(*ptr,MSize(*ptr));
            }
        }
        next:;
        ptr++;
    }
    return cnt;
}
static int64_t __PoopCollect(ExceptBuf *regs) {
    gc_age++;
    void *frame_start=StackStart,*frame_end=__builtin_frame_address(0);
    int64_t idx,cnt=0,sz;
    CPoopSlab2 *cur_slab,*next;
    //Stack grows down
    __ScanPtr(frame_end,StackStart-frame_end);
    __ScanPtr(regs,sizeof(*regs));
    __ScanPtr(&Loader,sizeof(Loader));
    __ScanPtr(&curframe,sizeof(curframe));
    __ScanPtr(&Fs,sizeof(Fs));
    LargestAlloc=0;
    for(idx=0;idx!=HASH_MASK+1;idx++) {
        cur_slab=HashTable[idx];
        loop:
        for(;cur_slab;cur_slab=cur_slab->next) {
            switch(cur_slab->ident) {
            case SLAB_32:;
            CPoopSlab32 *s32=(void*)cur_slab;
            cnt=sizeof(s32->occupied);
            while(--cnt>=0) {
                if(s32->occupied[cnt])
                    if(((CPoopPtr*)&s32->items[cnt*(32+16)])->gc_age!=gc_age) {
                        PoopFree(((CPoopPtr*)&s32->items[cnt*(32+16)])+1);
                    } else {
                        sz=((CPoopPtr*)&s32->items[cnt*(32+16)])->len;
                        LargestAlloc=(sz>LargestAlloc)?sz:LargestAlloc;
                    }
                }
            break;
            case SLAB_128:;
            CPoopSlab128 *s128=(void*)cur_slab;
            cnt=sizeof(s128->occupied);
            while(--cnt>=0) {
                if(s128->occupied[cnt])
                if(((CPoopPtr*)&s128->items[cnt*(128+16)])->gc_age!=gc_age) {
                    PoopFree(((CPoopPtr*)&s128->items[cnt*(128+16)])+1);
                } else {
                    sz=((CPoopPtr*)&s128->items[cnt*(128+16)])->len;
                    LargestAlloc=(sz>LargestAlloc)?sz:LargestAlloc;
                }
            }
            break;
            case SLAB_512:;
            CPoopSlab512 *s512=(void*)cur_slab;
            cnt=sizeof(s512->occupied);
            while(--cnt>=0){
                if(s512->occupied[cnt])
                if(((CPoopPtr*)&s512->items[cnt*(512+16)])->gc_age!=gc_age) {
                    PoopFree(((CPoopPtr*)&s512->items[cnt*(512+16)])+1);
                } else {
                    sz=((CPoopPtr*)&s512->items[cnt*(512+16)])->len;
                    LargestAlloc=(sz>LargestAlloc)?sz:LargestAlloc;
                }
            }
            case SLAB_1024:;
            CPoopSlab1024 *s1024=(void*)cur_slab;
            cnt=sizeof(s1024->occupied);
            while(--cnt>=0){
                if(s1024->occupied[cnt])
                if(((CPoopPtr*)&s1024->items[cnt*(1024+16)])->gc_age!=gc_age) {
                    PoopFree(((CPoopPtr*)&s1024->items[cnt*(1024+16)])+1);
                } else {
                    sz=((CPoopPtr*)&s1024->items[cnt*(1024+16)])->len;
                    LargestAlloc=(sz>LargestAlloc)?sz:LargestAlloc;
                }
            }
            break;
            case SLAB_BIG:
                sz=((CPoopPtr*)(cur_slab+1))[0].len;
                if(((CPoopPtr*)(cur_slab+1))[0].gc_age!=gc_age) {
                    next=cur_slab->next;
                    PoopFree((sizeof(CPoopSlab2)+sizeof(CPoopPtr))%16+sizeof(CPoopSlab2)+sizeof(CPoopPtr)+(void*)cur_slab);
                    cur_slab=next;
                    goto loop;
                } else
                    LargestAlloc=(sz>LargestAlloc)?sz:LargestAlloc;
            break;
            }
        }
    }
}
static int gc_enabled=1;
int PoopSetGCEnable(int en) {
  int old=gc_enabled;
  gc_enabled=en;
  return old;
}
void PoopCollect() {
    if(!gc_enabled) return;
    ExceptBuf pad;
    if(HCSetJmp(&pad))
    	__PoopCollect(&pad);
    else
    	HCLongJmp(&pad);
}
void PoopInit(void *frame_start) {
    StackStart=frame_start;
}
