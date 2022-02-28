#pragma once
#include <stdlib.h>
#include "poopalloc.h"
#define TD_MALLOC(sz) PoopMAlloc(sz)
#define TD_CALLOC(n,sz) PoopMAlloc((sz)*(n))
#define TD_REALLOC(p,sz) PoopReAlloc(p,sz)
#define TD_FREE(p) PoopFree(p)
#include <string.h>
#define strdup(s) strcpy(TD_MALLOC(strlen((s))+2),(s))
