/* adapted from musl (MIT), trimmed: unsigned folds to the 64-bit int
 * the compiler has (musl's unsigned int is narrower); malloc/calloc
 * memory is permanent by design (no free, PLAN.md); abort exits with
 * 134 from any depth (M12.1). */
#ifndef _STDLIB_H
#define _STDLIB_H
#include "stddef.h"
#define EXIT_SUCCESS 0
#define EXIT_FAILURE 1
void abort(void);
void exit(int code);
int atoi(const char *s);
int abs(int n);
void *malloc(unsigned long size);
void *calloc(unsigned long n,unsigned long m);
int rand(void);
void srand(unsigned seed);
#endif
