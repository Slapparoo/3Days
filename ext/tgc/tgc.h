#ifndef TGC_H
#define TGC_H

#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <setjmp.h>
#define TGC_COLLECT_AFTER 3000000
enum {
  TGC_MARK = 0x01,
  TGC_ROOT = 0x02,
  TGC_LEAF = 0x04
};

typedef struct tgc_ptr_t {
  int flags;
  unsigned int size;
  union {
  void *user_data;
  void *root_ptr;
};
  void (*dtor)(void*,void *);
} tgc_ptr_t;

typedef struct {
  void *bottom;
  int paused:1,collecting:1,boundsCheckMode:1,collectionPending:1;
  uintptr_t minptr, maxptr;
  void **items;
  double loadfactor, sweepfactor;
  size_t nitems, nslots, mitems, nfrees,largest_alloc_size;
} tgc_t;

void tgc_start(tgc_t *gc, void *stk,int bounds_check);
void tgc_stop(tgc_t *gc);
void tgc_pause(tgc_t *gc);
void tgc_resume(tgc_t *gc);
void tgc_run(tgc_t *gc);

void *tgc_alloc(tgc_t *gc, size_t size);
void *tgc_calloc(tgc_t *gc, size_t num, size_t size);
void *tgc_realloc(tgc_t *gc, void *ptr, size_t size);
void tgc_free(tgc_t *gc, void *ptr);

void *tgc_alloc_opt(tgc_t *gc, size_t size, int flags, void(*dtor)(void*,void*));
void *tgc_calloc_opt(tgc_t *gc, size_t num, size_t size, int flags, void(*dtor)(void*,void*));

void tgc_set_dtor(tgc_t *gc, void *ptr, void(*dtor)(void*,void*));
void tgc_set_flags(tgc_t *gc, void *ptr, int flags);
int tgc_get_flags(tgc_t *gc, void *ptr);
void(*tgc_get_dtor(tgc_t *gc, void *ptr))(void*,void*);
size_t tgc_get_size(tgc_t *gc, void *ptr);

void tgc_set_user_data(tgc_t *gc, void *ptr,void *ud);
void *tgc_add(
  tgc_t *gc, void *ptr, size_t size,
  int flags, void(*dtor)(void*,void*));
extern tgc_t gc;

int InBounds(tgc_t *_gc,void *ptr);
void tgc_add_root_ptr(
  tgc_t *gc, void *ptr, size_t size);
#endif
