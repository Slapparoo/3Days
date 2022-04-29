#pragma once
#include <stdint.h>
void *PoopMAlloc(int64_t size);
void PoopFree(void *ptr);
void *PoopReAlloc(void *ptr,int64_t size);
int64_t MSize(void *ptr);
void PoopCollect();
void PoopInit(void *frame_start);
void *BoundsCheck(void *ptr,int64_t size);
int PoopSetGCEnable(int en);
