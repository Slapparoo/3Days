#pragma once
#include <stdlib.h>
#include "gc.h"
#ifdef USEGC
#define TD_MALLOC(sz) GC_Calloc(1,sz)
#define TD_CALLOC(n,sz) GC_Calloc(sz,n)
#define TD_REALLOC(p,sz) GC_Realloc(p,sz)
#define TD_FREE(p) GC_Free(p)
#else
#define TD_MALLOC(sz) calloc(1,sz)
#define TD_CALLOC(n,sz) calloc(sz,n)
#define TD_REALLOC(p, sz) realloc(p,sz)
#define TD_FREE(p) free(p)
#endif
#include <string.h>
#define strdup(s) strcpy(TD_MALLOC(strlen((s))+2),(s))
