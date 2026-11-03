/* adapted from musl (MIT): only the surface this compiler actually
 * implements is declared. FILE is a stream TOKEN, not a pointer to a
 * control block: cc->out IS the single stdio buffer, and fputc/putc/
 * fputs/fgetc/getc accept a stream argument and ignore it (PLAN.md
 * M12). Declaring these names is inert for calls: printf/putchar/
 * getchar/sprintf dispatch as parser builtins, the rest resolve to
 * the prelude functions compiled into every module. No FILE* stdio
 * buffering, fopen, or fgets yet — see the limitations list. */
#ifndef _STDIO_H
#define _STDIO_H
#include "stddef.h"
typedef long FILE;
int printf(const char *fmt,...);
int sprintf(char *dst,const char *fmt,...);
int puts(const char *s);
int putchar(int c);
int fputc(int c,FILE *f);
int putc(int c,FILE *f);
int fputs(const char *s,FILE *f);
int getchar(void);
int fgetc(FILE *f);
int getc(FILE *f);
#define EOF (-1)
#endif
