/* adapted from musl (https://musl.libc.org, MIT license): only the
 * surface CC's stdlib actually implements — the delegated builtins
 * (strlen/memcpy/memset/memcmp via kernel helpers, PLAN.md M12) and
 * the pure prelude functions (M12/M12.1). void* folds to the
 * byte-pointer model, which is exactly the kernel MemCpy/MemSet
 * contract; size_t folds to the compiler's single 64-bit int. No
 * restrict qualifiers, no wmem*, no strcoll/strxfrm. */
#ifndef _STRING_H
#define _STRING_H
#include "stddef.h"
void *memchr(const void *s,int c,unsigned long n);
int memcmp(const void *a,const void *b,unsigned long n);
void *memcpy(void *dst,const void *src,unsigned long n);
void *memmove(void *dst,const void *src,unsigned long n);
void *memset(void *dst,int c,unsigned long n);
char *strcat(char *dst,const char *src);
char *strncat(char *dst,const char *src,unsigned long n);
char *strcpy(char *dst,const char *src);
char *strncpy(char *dst,const char *src,unsigned long n);
char *strchr(const char *s,int c);
char *strrchr(const char *s,int c);
char *strstr(const char *hay,const char *needle);
size_t strlen(const char *s);
int strcmp(const char *a,const char *b);
int strncmp(const char *a,const char *b,unsigned long n);
char *strdup(const char *s);
#endif
