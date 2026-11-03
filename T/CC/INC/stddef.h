/* adapted from musl (https://musl.libc.org, MIT license) — trimmed to
 * what the CC subset supports: no __extension__ dance, no wchar, no
 * va_list. Unsigned-long stands in for size_t because CC's only int is
 * 64-bit; see T:/CC/PLAN.md M13. */
#ifndef _STDDEF_H
#define _STDDEF_H
typedef unsigned long size_t;
#define NULL 0
#endif
