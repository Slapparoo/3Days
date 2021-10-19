#pragma once
void *GC_Realloc(void *ptr,long size);
void GC_Free(void *ptr);
void *GC_Calloc(long sz,long num);
void GC_Collect();
void DestroyGC();
void CreateGC(void *stacktop,int boundscheckmode);
void AddGCRoot(void *addr,long len);
long MSize(void *ptr);
void GC_SetDestroy(void *ptr,void(*destroy)(void *));

#define GC_FREE(ptr) GC_Free(ptr)
#define GC_MALLOC(sz) GC_Calloc(sz,1)
#define GC_REALLOC(ptr,l) GC_Realloc(ptr,l)
//Align to 1<<GC_ALIGN_BITS
#define GC_ALIGN_BITS 8
int InBounds(void *ptr);
void GC_Enable();
void GC_Disable();
void *GC_Malloc(long num);
void *GCCreateExtPtr(void *ext,void(* destroy)(void*));
