#pragma once
#include <stdlib.h>
#include "ext/tgc/tgc.h"
#ifdef USEGC
#define TD_MALLOC(sz) tgc_calloc(&gc,1,sz)
#define TD_CALLOC(n,sz) tgc_calloc(&gc,sz,n)
#define TD_REALLOC(p,sz) tgc_realloc(&gc,p,sz)
#define TD_FREE(p) tgc_free(&gc,p)
#else
#define TD_MALLOC(sz) calloc(1,sz)
#define TD_CALLOC(n,sz) calloc(sz,n)
#define TD_REALLOC(p, sz) realloc(p,sz)
#define TD_FREE(p) free(p)
#endif
#include <string.h>
#define strdup(s) strcpy(TD_MALLOC(strlen((s))+2),(s))
#define MSize(p) tgc_get_size(&gc,p)
#define AddGCRoot(ptr,sz) tgc_add_root_ptr(&gc, ptr, sz)
