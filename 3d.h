#pragma once
#include "alloc.h"
#include <stdint.h>
#include <assert.h>
#include <stdio.h>
#include <signal.h>
typedef int8_t ExceptBuf[216];
#include "ext/map/src/map.h"
#include "ext/vec/src/vec.h"
#include "ext/myjit/myjit/jitlib.h"
#include "ext/mropes/include/mropes/rope.h"
#include "rl.h"
#define AOT_NO_IMPORT_SYMS (1<<0)
extern char CompilerPath[1024];
typedef vec_t(struct jit_op*) vec_jit_op_t;
typedef map_t(vec_jit_op_t) map_vec_jit_op_t;
extern ExceptBuf SigPad;
typedef struct CRelocation {
    void **ptr;
} CRelocation;
typedef vec_t(CRelocation*) vec_CRelocationp_t;
typedef map_t(vec_CRelocationp_t) map_vec_CRelocationp_t;
struct CSymbol;
typedef struct CSymbol {
    enum {
        SYM_VAR,
        SYM_FUNC,
        SYM_LABEL,
    } type;
    int8_t is_importable;
    int64_t size;
    void *value_ptr;
    map_vec_CRelocationp_t relocs;
    map_void_t label_ptrs;
    map_void_t statics;
    char *strings;
} CSymbol;
typedef map_t(struct CSymbol) map_CSymbol_t;
typedef struct {
    map_CSymbol_t symbols;
} CLoader;
extern CLoader Loader;
char *MStrPrint(const char *fmt,int64_t argc,int64_t *argv);
typedef struct TOS_Fs {
    int64_t except_ch;
    int64_t catch_except;
    int64_t rand_seed;
    void *last_cc;
} TOS_Fs;
typedef struct ExceptFrame {
    ExceptBuf pad;
    struct ExceptFrame *parent;
} ExceptFrame;
void TOSPrint(const char *fmt,int64_t argc,int64_t *argv);
extern TOS_Fs Fs;
void RegisterFuncPtrs();
extern ExceptFrame *curframe;
void LoadAOTBin(FILE *f,int flags,char **header);
void InitRL();
//Leave args NULL to use Loader's symbols
void RegisterRuntimeClasses(void *gt,void *ctf,void *add_mem,void *c_arr_t);
void RegisterMacrosAndREPL(char *includes,int flags,char *body_code) ;
