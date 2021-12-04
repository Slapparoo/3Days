#include "tgc.h"
#include <stdio.h>
#include <assert.h>
#include "../../linjmp.h"
#define TGC_PROBES 32
#define GC_ALIGN_BITS 7
static int tgc_rehash(tgc_t* gc);
static size_t tgc_hash(void *ptr) {
  return ((uintptr_t)ptr)>>3 ;
}
#define GET_PINFO(ptr) (-1+(tgc_ptr_t*)(ptr))[0]
static void tgc_mark(tgc_t *gc);
static size_t tgc_probe(tgc_t* gc, size_t i,size_t hash) {
  size_t v = hash;
  if (v < 0) { v = gc->nslots + v; }
  return (v+i)%gc->nslots;
}

static tgc_ptr_t *tgc_get_ptr(tgc_t *gc, void *ptr) {
  size_t i, j, h;
  j = 0;
  while (j<TGC_PROBES) {
    i=tgc_probe(gc,j,tgc_hash(ptr));
    if (gc->items[i] == ptr) { return gc->items[i]; }
    j++;
  }
  return NULL;
}
static int tgc_resize_more(tgc_t *gc);
void tgc_add_root_ptr(
  tgc_t *gc, void *ptr, size_t size) {
    void *_ptr=tgc_calloc(gc, 1, size);
    GET_PINFO(_ptr).root_ptr=ptr;
    GET_PINFO(_ptr).flags|=TGC_ROOT;
}
void tgc_add_ptr(
  tgc_t *gc, void *ptr, size_t size,
  int flags, void(*dtor)(void*,void*)) {

  tgc_ptr_t *item, tmp;
  size_t h, p, i, j;
    if(gc->nslots==0) {
        tgc_resize_more(gc);
    }

  j = 0;

  item=ptr;
  item--;
  item->flags = flags;
  item->size = size;
  item->dtor = dtor;
  item->user_data=NULL;

  int tried_cleaup=0;
  loop:
    j = 0;
  while (j<TGC_PROBES) {
    i=tgc_probe(gc,j,tgc_hash(ptr));
    if (gc->items[i] == ptr) { return; }
    else if(!gc->items[i]) {gc->items[i]=ptr; gc->nitems++; return;}
    j++;
  }
  tgc_resize_more(gc);
  goto loop;
}

static void tgc_rem_ptr(tgc_t *gc, void *ptr) {

  size_t i, j, h, nj, nh;

  if (gc->nitems == 0) { return; }

  i = tgc_hash(ptr) % gc->nslots; j = 0;

  while (j<TGC_PROBES) {
    i=tgc_probe(gc,j,tgc_hash(ptr));
    if (gc->items[i] == ptr) {
      gc->items[i]=0;
      j = i;
      gc->nitems--;
      return;
    }
    j++;
  }
}


enum {
  TGC_PRIMES_COUNT = 24
};

static const size_t tgc_primes[TGC_PRIMES_COUNT] = {
  0,       1,       5,       11,
  23,      53,      101,     197,
  389,     683,     1259,    2417,
  4733,    9371,    18617,   37097,
  74093,   148073,  296099,  592019,
  1100009, 2200013, 4400021, 8800019,
  13466917, 20996011, 24036583, 25964951
};

static size_t tgc_ideal_size(tgc_t* gc, size_t size) {
  size_t i, last;
  size = (size_t)((double)(size+1) /0.9);
  for (i = 0; i < TGC_PRIMES_COUNT; i++) {
    if (tgc_primes[i] >= size) { return tgc_primes[i]; }
  }
  last = tgc_primes[TGC_PRIMES_COUNT-1];
  for (i = 0;; i++) {
    if (last * i >= size) { return last * i; }
  }
  return 0;
}
static int tgc_resize_less(tgc_t *gc);
static int tgc_rehash(tgc_t* gc) {
  size_t i0;;
  if(gc->nslots&&!gc->paused) {
        tgc_mark(gc);
  }

  gc->largest_alloc_size=0;
  void **old_items = gc->items;
  size_t old_size = gc->nslots;

  size_t p, last;
  size_t size = 10*gc->nitems;
  if(4400021<=size) size=4400021;
  size = tgc_ideal_size(gc,size);
  gc->nslots=size;
  gc->items = calloc(gc->nslots, sizeof(tgc_ptr_t));
  restart:
  gc->nitems=0;
  for (i0 = 0; i0 < old_size; i0++) {
      void *ptr;
    if ((ptr=old_items[i0]) != 0) {
            if(!(GET_PINFO(old_items[i0]).flags&TGC_MARK)&&!(GET_PINFO(old_items[i0]).flags&TGC_ROOT)&&!gc->paused) {
                tgc_ptr_t *p=&GET_PINFO(old_items[i0]);
                if (p->dtor) {
                  p->dtor(ptr,p->user_data);
                }
                memset(ptr,0xbe,tgc_get_size(gc,ptr));
#ifndef TARGET_WIN32
                free(p);
#else
                if(gc->boundsCheckMode) _aligned_free(p);
                else free(p);
#endif
                old_items[i0]=NULL;
                continue;
            }
            if(gc->largest_alloc_size<GET_PINFO(old_items[i0]).size) gc->largest_alloc_size=GET_PINFO(old_items[i0]).size;
            size_t j,i;
            j = 0;
          while (j<TGC_PROBES) {
            i=tgc_probe(gc,j,tgc_hash(ptr));
            if(!gc->items[i]) {
                gc->items[i]=old_items[i0];
                goto found;
            }
            j++;
          }
        free(gc->items);
        gc->items=calloc(gc->nslots*=2,sizeof(tgc_ptr_t));
        goto restart;
        found:;
        gc->nitems++;
    }
  }

    free(old_items);
  return 1;
}

static int tgc_resize_more(tgc_t *gc) {
    size_t old_size = gc->nslots*1.6;
    loop:;
   size_t new_size = tgc_ideal_size(gc, old_size );
   if(new_size>=old_size)
        return tgc_rehash(gc);
    else old_size *=2;
    goto loop;
}

static int tgc_resize_less(tgc_t *gc) {
  size_t old_size = gc->nslots;
  size_t new_size = tgc_ideal_size(gc, gc->nitems);
  if(new_size < old_size) return tgc_rehash(gc); else return  1;
}

static void tgc_mark_ptr(tgc_t *gc, void *ptr) {

  size_t i, j, h, k;
j = 0;
  if(!ptr) return;
  while (j<TGC_PROBES) {
    i=tgc_probe(gc,j,tgc_hash(ptr));
    if (ptr == gc->items[i]) {
      if (GET_PINFO(gc->items[i]).flags & TGC_MARK) { return; }
      GET_PINFO(gc->items[i]).flags |= TGC_MARK;
      gc->nitems++;
      for (k = 0; k < GET_PINFO(gc->items[i]).size/sizeof(void*); k++) {
        tgc_mark_ptr(gc, ((void**)gc->items[i])[k]);
      }
      return;
    }
    j++;
  }

}

static void tgc_mark_stack(tgc_t *gc,ExceptBuf *buf) {

  void *stk, *bot, *top, *p;
  bot = gc->bottom; top = __builtin_frame_address(0);

  if (bot == top) { return; }

  if (bot < top) {
    for (p = top; p >= bot; p = ((char*)p) - sizeof(void*)) {
      tgc_mark_ptr(gc, *((void**)p));
    }
  }

  if (bot > top) {
    for (p = top; p <= bot; p = ((char*)p) + sizeof(void*)) {
      tgc_mark_ptr(gc, *((void**)p));
    }
  }

    for (p = buf; p <= (void*)((char*)buf+sizeof(*buf)); p++) {
      tgc_mark_ptr(gc, *((void**)p));
    }
}
static void tgc_mark(tgc_t *gc) {

  size_t i, k;
  if (gc->nitems == 0) { return; }
  gc->nitems=0;

  for (i = 0; i < gc->nslots; i++) {
    if (gc->items[i] ==        0) { continue; }
    GET_PINFO(gc->items[i]).flags &= ~(TGC_MARK|TGC_LEAF);
  }
  for (i = 0; i < gc->nslots; i++) {
    if (gc->items[i] ==        0) { continue; }
    if (GET_PINFO(gc->items[i]).flags & TGC_ROOT) {
      GET_PINFO(gc->items[i]).flags |= TGC_MARK;
      gc->nitems++;
      for (k = 0; k < GET_PINFO(gc->items[i]).size/sizeof(void*); k++) {
        void *at=((void**)(GET_PINFO(gc->items[i]).root_ptr))[k];
        tgc_mark_ptr(gc, at);
      }
      continue;
    }
  }

  ExceptBuf env;
  if(HCSetJmp(&env)) {
  tgc_mark_stack(gc,&env);
  } else HCLongJmp(&env);
}
void tgc_start(tgc_t *gc, void *stk,int bounds_check) {
  gc->bottom = stk;
  gc->paused = 0;
  gc->nitems = 0;
  gc->nslots = 0;
  gc->mitems = 0;
  gc->nfrees = 0;
  gc->maxptr = 0;
  gc->items = NULL;
  gc->minptr = UINTPTR_MAX;
  gc->loadfactor = 0.9;
  gc->sweepfactor = 0.5;
  gc->largest_alloc_size=0;
  gc->boundsCheckMode=bounds_check;
}

void tgc_stop(tgc_t *gc) {
  size_t i=0;
  for(;i!=gc->nslots;i++) {
    tgc_free(gc, gc->items[i]);
  }
  free(gc->items);
}

void tgc_pause(tgc_t *gc) {
  gc->paused = 1;
}

void tgc_resume(tgc_t *gc) {
  gc->paused = 0;
}

void *tgc_add(
  tgc_t *gc, void *ptr, size_t size,
  int flags, void(*dtor)(void*,void*)) {

  gc->nitems++;
  gc->maxptr = ((uintptr_t)ptr) + size > gc->maxptr ?
    ((uintptr_t)ptr) + size : gc->maxptr;
  gc->minptr = ((uintptr_t)ptr)        < gc->minptr ?
    ((uintptr_t)ptr)        : gc->minptr;

  tgc_add_ptr(gc,ptr,size,flags,dtor);
  return ptr;
}

static void tgc_rem(tgc_t *gc, void *ptr) {
  tgc_rem_ptr(gc, ptr);
  //tgc_resize_less(gc);
  gc->mitems = gc->nitems + gc->nitems / 2 + 1;
}
void *tgc_calloc(tgc_t *gc, size_t num, size_t size) {
  return tgc_calloc_opt(gc, num, size, 0, NULL);
}

void *tgc_realloc(tgc_t *gc, void *ptr, size_t size) {
  long osz=tgc_get_size(gc,ptr);
  void *new=tgc_calloc(gc,size,1);
  if(osz)
    memcpy(new,ptr,(osz>size)?size:osz);
  assert(osz==tgc_get_size(gc,ptr));
  if(ptr) tgc_free(gc,ptr);
  assert(size==tgc_get_size(gc,new));
  return new;
}

void tgc_free(tgc_t *gc, void *ptr) {
   if(!ptr) return;
   if(!tgc_get_ptr(gc, ptr)) {
     fprintf(stderr, "Invalid free %p\n", ptr);
     return;
   }
  tgc_ptr_t *p=&GET_PINFO(ptr);
  if (p) {
    if (p->dtor) {
      p->dtor(ptr,p->user_data);
    }
    memset(ptr,0xbe,tgc_get_size(gc,ptr));
#ifndef TARGET_WIN32
    free(p);
#else
    if(gc->boundsCheckMode) _aligned_free(p);
    else free(p);
#endif
    tgc_rem(gc, ptr);
  }
}

void *tgc_calloc_opt(
  tgc_t *gc, size_t num, size_t size,
  int flags, void(*dtor)(void*,void*)) {
      void *res;
    #ifndef TARGET_WIN32
    if(gc->boundsCheckMode)
        res=aligned_alloc(1<<GC_ALIGN_BITS,16+size*num+sizeof(tgc_ptr_t));
    else
        res=malloc(size*num+sizeof(tgc_ptr_t));
#else
    if(gc->boundsCheckMode)
        res=_aligned_malloc (size*num+sizeof(tgc_ptr_t),1<<GC_ALIGN_BITS);
    else
        res=malloc(size*num+sizeof(tgc_ptr_t));
#endif
    res+=+sizeof(tgc_ptr_t);
  if(gc->largest_alloc_size<size*num)  gc->largest_alloc_size=size*num;
  if (res != NULL) {
    tgc_add(gc, res , num * size, flags, dtor);
  }
  memset(res,0,size*num);
  return res;
}

void tgc_set_dtor(tgc_t *gc, void *ptr, void(*dtor)(void*,void*)) {
  if (ptr) { GET_PINFO(ptr).dtor = dtor; }
}

void tgc_set_flags(tgc_t *gc, void *ptr, int flags) {
  if (ptr) { GET_PINFO(ptr).flags = flags; }
}

int tgc_get_flags(tgc_t *gc, void *ptr) {
  if (ptr) { return GET_PINFO(ptr).flags; }
  return 0;
}

void(*tgc_get_dtor(tgc_t *gc, void *p))(void*,void*) {
  if (p) { return GET_PINFO(p).dtor; }
  return NULL;
}

size_t tgc_get_size(tgc_t *gc, void *p) {
  if (p) { return GET_PINFO(p).size; }
  return 0;
}

void tgc_set_user_data(tgc_t *gc, void *p,void *ud) {
    if (p) { GET_PINFO(p).user_data=ud; }
}

int InBounds(tgc_t *_gc,void *ptr) {
    if(!_gc) _gc=&gc;
    void *orig=ptr;;
    void *bottom=__builtin_frame_address(0);
    if(orig>=bottom)
        if(orig<=_gc->bottom) {
            return 1;
        }
    uint64_t ptri=(void*)ptr;
    ptri>>=GC_ALIGN_BITS;
    ptri<<=GC_ALIGN_BITS;
    for(;;) {
        if(orig-(void*)ptri>_gc->largest_alloc_size) {
            fprintf(stderr,"Access is out of bounds.");
            return 0;
        }
        if(tgc_get_ptr(_gc,(void*)ptri)) {
            long size=tgc_get_size(_gc,(void*)ptri);
            if((void*)ptri+size>=orig)
                if(orig>=(void*)ptri)
                    return 1;
            fprintf(stderr,"Access is %li bytes beyond %li byte area.",orig-(void*)ptri-size,size);
            return 0;
        }
        ptri-=1<<GC_ALIGN_BITS;
    }
}

tgc_t gc;

void tgc_run(tgc_t *gc) {
    tgc_rehash(gc); //"Collects"
}
