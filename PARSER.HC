#include "HCRT/HCRT.HC" //Requires nitty gritty of the runtime
#include "VECTOR.HC"
#ifdef SIGNAL
#include "SIGNAL.HC"
ArmSignals;
#endif
U0 CatchIf(...) {
    Fs->catch_except=FALSE;
    while(--argc>=0)
        if(argv[argc]==Fs->except_ch) {
            return Fs->catch_except=TRUE;
        }
}
#define HTT_VAR 1
#define HTT_TYPE 2
#define HTT_ARRAY_TYPE 3
#define HTT_FUNC_TYPE 4
#define HTT_LABEL 6
#define HTT_COMP_TYPE 7
#define HTT_FUNC 8
#define HTT_CLASS 9
#define HTT_UNION 10
#define STAR_CNT 4
Bool IsHTTType(I64 t) {
    switch(t) {
        case HTT_TYPE:
        case HTT_ARRAY_TYPE:
        case HTT_FUNC_TYPE:
        case HTT_UNION:
        case HTT_CLASS:
        case HTT_COMP_TYPE:
        return TRUE;
    }
    return FALSE;
}
class _CBreakpoint;
class jit_debugger_regs;
U0 DebuggerRoutine(jit_debugger_regs *regs,_CBreakpoint *bp,U8 *caller_ptr);
class CType;
class AST;
class CRelocation;
CType *AssignTypeToNode(AST *a,CType *arr_t=NULL);
extern U0 FreeAST(AST *a);
class CCompStringRef {
    CCompStringRef *next;
    U0 **ptr;
};
class CCompString:CHash {
    U8 *data;
    CCompStringRef *refs;
};
class CIRValue {
    CIRValue *last;
    #define IRV_INT 1
    #define IRV_FLOAT 2
    #define IRV_FREG 3
    #define IRV_REG 4
    #define IRV_STR 5
    #define IRV_INDIRX 6 //Comes with offsett
    I64 type;
    union {
        I64 reg;
        F64 flt;
        I64 int;
        class {
            CType *type;
            I64 reg;
            I64 offset;
        } indirx;
        U0 *indir;
    };
    CType *type2;
};
CIRValue *IRValueClone(CIRValue *v) {
    return MemCpy(MAlloc(sizeof(*v)),v,sizeof(*v));
}
class CBreakLst {
    CBreakLst *last;
    U0 *start_code_label; //This is used for sub-switches
    CVector *breaks;
};
class CLabel:CHash {
    U0 *op;
    U0 *ptr;
    I64 local_scope;
    CVector *refs;
    Bool is_exported;
    Bool is_local;
};
class CSwitchTable {
    CSwitchTable *last;
    I64 lo,hi;
    U0  **table;
    AST **ast_table;
    I64 subswit_var_offset;
    U0 *enter_point;
    /*
     * j=condition
     * subswit_var=0
     * enter_point:
     * JMP table[j]
     * ....
     * subswit_start:
     * Compiler.cur_breaks.start_code_label
     * subswit_var=1
     * JMP enter_point
     * subswit_case:
     * if(subswit_var==0) JMP Compiler.cur_breaks.start_code_label
     *
     */
    U0 *dft1,*dft2;
};
class CAsmPatcher;
U8 *Error_file_text=NULL;
class CUnresolvedLabel:CHash {
    AST *stmt;
};
class CCompiler {
    CIRValue *stack;
    U0 *jit;
    CHashTable *globals,*locals,
    //See HexifyString and CCompString
    *cur_strings,*breakpoints,*labels,
    //For asm blocks
    *imports;
    //See GetUnique for a unique number
    #define AOT_FUNC_DFT_ARG_FMT ".%d"

    //no_array_access_mode is usefull for assembler forms like like FS:DISP[SIB]
    Bool in_func,aot_mode,debug_mode,no_array_access_mode
    //Will cause unresolved names in AssignTypeToNode to be treatede at U8*'s (they may be labels)
    //Will cause names in Compiler.imports to be treated at pointer to said item
    ,asm_mode,bounds_check_mode,force_noregs,
    allow_breaks,allow_subswit;
    CBreakLst *cur_breaks;
    CSwitchTable *swit_table;
    CType
     *ret_type;
    CRelocation *relocations;
    I64 first_avail_ireg;
    I64 first_avail_freg,
    error_count,
    //Used for local labels in assembler
    label_scope;
    U0 *func_ptr;
    I64 frame_off;
    CCmpCtrl *cc;
    CAsmPatcher *asm_patches;
    CHashTable *asm_tainted_label_names,*unresolved_labels;
} Compiler,
//GlobalCompiler is used for computing compile time constants in global scope in AOT mode
*GlobalCompiler;
GlobalCompiler=&Compiler;
I64 CompEnterSwitch() {
    U16 ret;
    ret.i8[0]=Compiler.allow_breaks;
    ret.i8[1]=Compiler.allow_subswit;
    Compiler.allow_breaks=TRUE;
    Compiler.allow_subswit=TRUE;
    return ret;
}
//Pass value from CompEnterSwitch
U0 CompLeaveSwitch(I64 old) {
    Compiler.allow_breaks=old.i8[0];
    Compiler.allow_subswit=old.i8[1];
}
I64 CompEnterLoop() {
    Bool old=Compiler.allow_breaks;
    Compiler.allow_breaks=TRUE;
    return old;
}
//Pass value from CompEnterLoop
U0 CompLeaveLoop(I64 old) {
    Compiler.allow_breaks=old;
}
//For AOT
U0 GlobalCompileASTTo(U8 *to,AST *ast);
Compiler.globals=HashTableNew(0x1000);
Compiler.locals=HashTableNew(0x100);
Compiler.asm_tainted_label_names=HashTableNew(0x10);
Compiler.cur_strings=HashTableNew(0x100);
Compiler.breakpoints=HashTableNew(0x1000);
class CCompilerState:CCompiler {};
class CMemberMetaData {
    CMemberMetaData *last,*next;
    U8 *name;
    I64 value;
};
class CTypeMember {
    CTypeMember *next;
    CMemberMetaData *meta;
    CType *type;
    I64 offset;
    U8 *name,*src_link;
};
class CType:CHashSrcSym {
    I64 star_cnt,sz,align;
    CTypeMember *first_member;
    union {
        CType *ret_type;
        CType *base;
    };
    CType *inher;
    CType **arg_types;
    I64 **arg_values; //May be a pointer to Lastclass
    U8 **arg_names;
    //This is used when a aot compiling to refer the the dft arguments
    //of functions because they are computed at compile time.
    //See AOT_FUNC_DFT_ARG_FMT
    U8 **aot_arg_names;
    union {
        I64 dim;
        I64 star_cnt;
    };
    Bool has_vargs,is_int,is_flt,is_signed,ambig_dim,is_fwd,is_builtin;
};
class CLinkage {
    I64 type;
    #define LINK_PUBLIC 1
    #define LINK_EXTERN (1<<1)
    #define LINK_IMPORT (2<<1)
    #define LINK__EXTERN (3<<1)
    #define LINK__IMPORT (4<<1)
    #define LINK_STATIC (5<<1)
    U8 _link[64];
};
class CFunction:CHashSrcSym {
    CLinkage link;
    U0 *jit;
    U0 *func_ptr;
    CType *hc_type;
    U0 *func_ptr;
    CRelocation *relocs;
    CHashTable *statics,*locals,*labels,*aot_strings;
    I64 frame_offset,aot_bin_size;
    //Use VirtFree(funcptr,aot_bin_size);
    Bool is_aot;
    CAsmPatcher *asm_patches;
};
class CAsmPatcher:CFunction {
    U0 *ptr;
    CAsmPatcher *next_patch;
    I64 width,label_scope;
    Bool is_rel;
};
#if IsWindows
U0 WindowsEnterFunc(U0 *func,U0 *frame);
U0 WindowsLeaveFunc();
I64 WindowsCallStackSize();
I64 WindowsSetCallStackSize(I64 sz);
#endif
CCompilerState *NewCompilerState() {
    JIT_OP *enter;
    CCompilerState *ret=MAlloc(sizeof(CCompilerState));
    *ret=Compiler;
    if(GlobalCompiler==&Compiler)
        GlobalCompiler=ret;
    Compiler.stack=NULL;
    Compiler.locals=HashTableNew(0x100);
    Compiler.labels=HashTableNew(0x10);
    Compiler.asm_tainted_label_names=HashTableNew(0x10);
    Compiler.imports=HashTableNew(0x10);
    Compiler.relocations=NULL;
    Compiler.cur_breaks=NULL;
    Compiler.relocations=NULL;
    Compiler.swit_table=NULL;
    Compiler.asm_patches=NULL;
    Compiler.in_func=0;
    Compiler.jit=jit_init();
    Compiler.frame_off=0;
    Compiler.unresolved_labels=HashTableNew(0x10);
    if(Compiler.aot_mode)
        Compiler.cur_strings=HashTableNew(0x10);
    #if IsWindows
    enter=jit_get_label(Compiler.jit);
    jit_prolog(Compiler.jit,&Compiler.func_ptr);
    if(Compiler.debug_mode) {
      jit_ref_code(Compiler.jit,jit_R(0),enter);
      jit_movr(Compiler.jit,jit_R(1),R_FP);
      jit_prepare(Compiler.jit);
      jit_putargr(Compiler.jit,jit_R(0));
      jit_putargr(Compiler.jit,jit_R(1));
      jit_call(Compiler.jit,&WindowsEnterFunc);
    }
    #else
    jit_prolog(Compiler.jit,&Compiler.func_ptr);
    #endif
    Compiler.first_avail_freg=2,Compiler.first_avail_ireg=3;
    return ret;
}
U0 CCompStringDestroy(CCompString *str) {
    Free(str->data);
    //TODO free refs
}
U0 RestoreCompilerState(CCompilerState *state) {
    if(state!=GlobalCompiler)
    HashTableDel(Compiler.asm_tainted_label_names);
    HashTableDel(Compiler.unresolved_labels);
    Compiler=*state;
    if(GlobalCompiler==state)
        GlobalCompiler=&Compiler;
    Free(state);
}
//Create primitives
U0 CreatePrimTypes() {
    class {
        U8 *name;
        Bool is_int;
        Bool is_signed;
        I64 size;
    } arr[]={
        {"U0",TRUE,FALSE,0}, //Is an "integer",not a floating point
        {"Bool",TRUE,FALSE,1},
        {"U8i" ,TRUE,FALSE,1},
        {"U16i",TRUE,FALSE,2},
        {"U32i",TRUE,FALSE,4},
        {"U64i",TRUE,FALSE,8},
        {"I8i" ,TRUE,TRUE,1},
        {"I16i",TRUE,TRUE,2},
        {"I32i",TRUE,TRUE,4},
        {"I64i",TRUE,TRUE,8},
        {"F64" ,FALSE,TRUE,8},
    };
    I64 cnt=sizeof(arr)/sizeof(*arr),cnt2;
    while(--cnt>=0) {
        CType *new=MAlloc(sizeof(CType)*(1+STAR_CNT));
        new->str=StrNew(arr[cnt].name);
        if(arr[cnt].is_int)
            new->is_int=TRUE;
        else
            new->is_flt=TRUE;
        new->is_signed=arr[cnt].is_signed;
        new->sz=arr[cnt].size;
        new->align=MaxI64(1,arr[cnt].size);
        new->type=HTT_TYPE;
        HashAdd(new,Compiler.globals);
        cnt2=STAR_CNT;
        while(--cnt2>=0) {
            new[cnt2+1].type=HTT_TYPE;
            new[cnt2+1].base=&new[cnt2];
            new[cnt2+1].align=new[cnt2+1].sz=8;
            new[cnt2+1].star_cnt=cnt2+1;
            switch(cnt2+1) {
            case 1:
            new[cnt2+1].str=MStrPrint("%s*",arr[cnt].name);
            break;
            case 2:
            new[cnt2+1].str=MStrPrint("%s**",arr[cnt].name);
            break;
            case 3:
            new[cnt2+1].str=MStrPrint("%s***",arr[cnt].name);
            break;
            case 4:
            new[cnt2+1].str=MStrPrint("%s****",arr[cnt].name);
            break;
            }
        }
    }
}
CreatePrimTypes;
Bool IsStructType(CType *a) {
    return a->type==HTT_CLASS||a->type==HTT_UNION;
}
Bool CompsAreCompat(CType *a,CType *b) {
    if(a==b) return TRUE;
    if(a->type==HTT_FUNC_TYPE||b->type==HTT_FUNC_TYPE) return TRUE; //TODO
    if(IsStructType(a)&&a->base)
        return CompsAreCompat(a->base,b);
    if(IsStructType(a)&&a->inher)
        return CompsAreCompat(a->inher,b);
    return FALSE;
}
class CVariable:CHashSrcSym {
    CLinkage link;
    Bool isReg;
    Bool is_local;
    Bool is_static;
    //Used for debugging(points to variable being debugged).
    Bool is_extern_ptr;
    //USed for debugger when looking for optimoized out values
    Bool is_opted_out;
    union {
        I64 frame_off;
        I64 reg;
        U0 *global_ptr;
        U0 *extern_ptr;
        class {
            U0 *static_ptr;
            CVector *static_refs;
        };

    };
    CType *hc_type;
};
class CRelocation {
    CRelocation *next;
    U0 **ptr;
    U8 name[64];
};
U0 FillInRelocations(CFunction *f);
Bool IsPtrType(CType *t) {
    if(t->star_cnt||t->type==HTT_ARRAY_TYPE)
        return TRUE;
    if(IsStructType(t))
        if(t->base)
            return IsPtrType(t->base);
    return FALSE;
}
Bool IsF64(CType *t) {
    if(t->is_flt) return TRUE;
    if(IsStructType(t))
        if(t->base)
            return IsF64(t->base);
    return FALSE;
}
Bool IsSigned(CType *t) {
    if(t->is_flt||t->is_signed) return TRUE;
    if(IsStructType(t))
        if(t->base)
            return IsSigned(t->base);
    return FALSE;
}
Bool IsInteger(CType *t) {
    if(IsPtrType(t)) return TRUE;
    if(t->is_int) return TRUE;
    if(IsStructType(t))
        if(t->base)
            return IsInteger(t->base);
    return FALSE;
}
U0 Backtrace(I64 hl_at,I64 trim=0);
CType *DerefedType(CType *t) {
    if(t->type==HTT_ARRAY_TYPE) {
        return t(CType*)->base;
    }
    if(t->star_cnt==0) {
        "DEREF NONPTR,%s\n",t->str;
        Backtrace(0,0);
        throw('Compile');
    }
    //Pointer types appear after base type in memory
    return t-1;
}
I64 MemberOffset(CType *t,U8 *name) {
    if(t->star_cnt||t->type==HTT_ARRAY_TYPE) goto fail;
    CTypeMember *mem=t->first_member;
    for(;mem;mem=mem->next) {
        if(!StrCmp(mem->name,name)) return mem->offset;
    }
    fail:
    throw('Compile');
}
CType *MemberType(CType *t,U8 *name) {
    if(t->star_cnt||t->type==HTT_ARRAY_TYPE) goto fail;
    CTypeMember *mem=t->first_member;
    for(;mem;mem=mem->next) {
        if(!StrCmp(mem->name,name)) return mem->type;
    }
    fail:
    throw('Compile');
}
class AST {
#define AST_NOP 1
#define AST_DECL 2
#define AST_ARRAY_LITERAL 4
#define AST_MEMBER 5
#define AST_PAREN 6
#define AST_INT 7
#define AST_FLOAT 8
#define AST_CHAR 9
#define AST_STRING 10
#define AST_COMMA 11
#define AST_ADD 12
#define AST_SUB 13
#define AST_MUL 14
#define AST_DIV 15
#define AST_MOD 16
#define AST_LSHIFT 17
#define AST_RSHIFT 18
#define AST_POW 19
#define AST_BAND 20
#define AST_BXOR 21
#define AST_BOR 22
#define AST_LT 23
#define AST_GT 24
#define AST_LE 25
#define AST_EQ 26
#define AST_NE 27
#define AST_GE 28
#define AST_LAND 29
#define AST_LOR 30
#define AST_LXOR 31
#define AST_ASSIGN 32
#define AST_ASSIGN_SHL 33
#define AST_ASSIGN_SHR 34
#define AST_ASSIGN_MUL 35
#define AST_ASSIGN_DIV 36
#define AST_ASSIGN_BAND 37
#define AST_ASSIGN_BOR 38
#define AST_ASSIGN_BXOR 39
#define AST_ASSIGN_ADD 40
#define AST_ASSIGN_SUB 41
#define AST_POST_INC 42
#define AST_POST_DEC 43
#define AST_ARRAY_ACCESS 44
#define AST_PRE_DEC 45
#define AST_PRE_INC 46
#define AST_POS 47
#define AST_NEG 48
#define AST_LNOT 49
#define AST_BNOT 50
#define AST_DERREF 51
#define AST_ADDROF 52
#define AST_SIZEOF 53
#define AST_IMPLICIT_TC 55
#define AST_EXPLICIT_TC 56
#define AST_FUNC_CALL 57
#define AST_IF 58
#define AST_FOR 59
#define AST_DO 60
#define AST_WHILE 61
#define AST_META_DATA 62
#define AST_CASE 63
#define AST_SWITCH 64
#define AST_SUBSWIT 65
#define AST_BREAK 66
#define AST_DFT 67
#define AST_RETURN 68
#define AST_TYPE 69
#define AST_GOTO 70
#define AST_LABEL 71
#define AST_TRY 72
#define AST_LASTCLASS 73
#define AST_RANGE 74
#define AST_ASSIGN_MOD 75
#define AST_NAME 76
#define AST_PRINT 77
#define AST_X64_MODE 78
#define AST_OPCODE 79
#define AST_GLOBAL_LABEL 80
#define AST_LOCAL_LABEL 81
#define AST_DU8 82
#define AST_DU16 83
#define AST_DU32 84
#define AST_DU64 86
#define AST_IMPORT 87
#define AST_ALIGN 88
#define AST_BINFILE 89
    I64 type;
    union {
        class {
            I64 to,fill;
        } align;
        class {
            CHash *opcode;
            AST **args;
        } opcode;
        class {
            U8 *text;
            AST **args;
        } print;
        class {
            AST *a,*b;
        } binop;
        AST *unopArg;
        class {
            AST *low,*high;
            I64 _li,_hi;
            Bool resolved;
        } cs;
        class {
            AST *name,*dft,*fbody;
            CType *type;
            CLinkage link;
        } decl;
        U8 *string;
        F64 floating;
        U64 chr;
        I64 int;
        //Use MSize
        AST **arr_lit;
        class {
            //Use MSize
            AST **values;
            I64 *types;
        } range;
        class {
            AST *base;
            U8 *member;
        } member_acc;
        class {
            AST *base;
            AST *index;
        } arr_acc;
        class {
            AST *base;
            CType *to_type;
        } typecast;
        class {
            AST *base;
            AST **args; //See MSize
        } func_call;
        class {
            AST *cond;
            AST *body;
            AST *el;
        } if_stmt;
        class {
            AST *init,*cond,*inc;
            AST *body;
        } for_stmt;
        class {
            AST *cond;
            AST *body;
        } do_stmt;
        class {
            AST *cond;
            AST *body;
        } while_stmt;
        class {
            AST *cond;
            AST *body;
        } switch_stmt;
        class {
            AST *start_code;
            AST *body;
        } sub_swit;
        class {
            AST *body;
            AST *cat;
        } try_block;
        U8 *name;
        class {
            X64Mode *x64_mode;
            //Bodacious platopus(plato-pus,plato + platipus)
            // Assembler immediates can be labels so they need to be patched,so use x64_mode_offset to store the expression of the immedaite
            //  ____          __
            // (    )        /**/
            //(      )      /**/
            // | .. |______/**/
            // |\  /|------**/
            // | -- |------*/
            // |wwww|______/
            //  \ww/  w  w
            //   \/   w  w
            AST *x64_mode_offset;
        };
    };
    CType *type2;
    AST *last,*next;
    U8 *fn;
    I64 ln,label_scope; //Used for AST_OPCODE
};
U0 HTT_LocalDestroy(CHash *h) {
    CVariable *var=h;
    Free(var->src_link);
    CLabel *lab;
    switch(h->type) {
        case HTT_VAR:
        var=h;
        if(var->is_static) {
            VectorFree(var->static_refs);
        }
        break;
        case HTT_LABEL:
        lab=h;
        VectorFree(lab->refs);
        break;
    }
}
U0 FreeFunction(CFunction *f) {
    if(f->locals) HashTableDel(f->locals,&HTT_LocalDestroy);
    if(f->jit) jit_free(f->jit);
}
U0 PatchAsmPatcher(CFunction *parent,CAsmPatcher *patcher);
AST *LSP(CCmpCtrl *ctrl,AST *a=NULL) {
    if(!a) {
        a=MAlloc(sizeof(*a));
        a->type=AST_NOP;
    }
    CHashSrcSym dummy;
    MemSet(&dummy,0,sizeof(dummy));
    HashSrcFileSet(ctrl,&dummy);
    a->fn=StrNew(dummy.src_link);
    StrScan(dummy.src_link,"%s,%d",&a->fn,&a->ln);
    Free(dummy.src_link);
    return a;
}
extern I64 EvalAST(AST *a);
extern F64 EvalASTF64(AST *a);
U0 __ScanForCases(AST *a,I64 last=0,I64 *min,I64 *max,Bool *has_dft) {
    I64 lo,hi;
    for(;a;a=a->next) {
        if(has_dft&&a->type==AST_DFT)
            *has_dft=TRUE;
        else if(a->type==AST_CASE) {
            if(Compiler.error_count) {
                lo=hi=++last;
            } else if(!a->cs.resolved) {
                if(a->cs.low)
                    last=lo=EvalAST(a->cs.low);
                else
                    lo=++last;
                if(a->cs.high)
                    last=hi=EvalAST(a->cs.high);
                else
                    hi=lo;
            } else {
                lo=a.cs->_li;
                hi=a.cs->_hi;
            }
            a->cs.resolved=TRUE;
            if(min) *min=MinI64(*min,lo);
            if(max) *max=MaxI64(*max,hi);
            a->cs._li=lo,a->cs._hi=hi;
        } else if(a->type==AST_SUBSWIT) {
            __ScanForCases(a->sub_swit.body,last,min,max,has_dft);
        }
    }
}
U0 ScanForCases(AST *a,I64 *min,I64 *max,Bool *has_dft) {
    if(min) *min=I64_MAX;
    if(max) *max=I64_MIN;
    if(has_dft) *has_dft=FALSE;
    __ScanForCases(a,,min,max,has_dft);
}
U0 __GetCases(AST *a,I64 last=0,I64 min,I64 max,U0 **tab) {
    I64 lo,hi;
    for(;a;a=a->next) {
        if(a->type==AST_CASE) {
            if(a->cs.low)
                last=lo=EvalAST(a->cs.low);
            else
                lo=++last;
            if(a->cs.high)
                last=hi=EvalAST(a->cs.high);
            else
                hi=lo;
            for(;lo<=hi;lo++)
                tab[lo-min]=a;
        } else if(a->type==AST_SUBSWIT) {
            __GetCases(a->sub_swit.body,last,min,max,tab);
        } else if(a->type==AST_DFT) {
            for(lo=min;lo<max;lo++)
                if(!tab[lo-min])
                    tab[lo-min]=a;
        }
    }
}

U0 **__CreateCaseTable(AST *a) {
    I64 lo,hi;
    ScanForCases(a,&lo,&hi,NULL);
    U0 **ret=MAlloc(sizeof(U0*)*(hi-lo+1));
    __GetCases(a,,lo,hi,ret);
    return ret;
}
U0 ParserWarn2(AST *a,U8 *msg,...) {
    U8 *fmted=StrPrintJoin(NULL,msg,argc,argv),*tmp;
    if(!Error_file_text) {
        "%s:%d \x1b[33mWARNING\x1b[0m %s\n",a->fn,a->ln,fmted;
    } else {
        tmp=MStrPrint("%s%s:%d WARNING %s\n",Error_file_text,a->fn,a->ln,fmted);
        Free(Error_file_text);
        Error_file_text=tmp;
    }
    Free(fmted);
}
U0 ParserFail2(AST *a,U8 *msg,...) {
    Compiler.error_count++;
    U8 *fmted=StrPrintJoin(NULL,msg,argc,argv),*tmp;
    if(!Error_file_text) {
        "%s:%d \x1b[31mERROR\x1b[0m %s\n",a->fn,a->ln,fmted;
    } else {
        tmp=MStrPrint("%s%s:%d ERROR %s\n",Error_file_text,a->fn,a->ln,fmted);
        Free(Error_file_text);
        Error_file_text=tmp;
    }
    throw('Parser');
}
U0 ParserFail(CCmpCtrl *cc,U8 *msg,...) {
    Compiler.error_count++;
    U8 *fmted=StrPrintJoin(NULL,msg,argc,argv),*tmp;
    CLexFile *tmpf=cc->lex_include_stk;
    loop:
    if(!tmpf) throw('Lexer');
    if(tmpf->flags&LFSF_DEFINE) {
        tmpf=tmpf->next;
        goto loop;
    }
    I64 line_num=tmpf->line_num;
    if (line_num<1) line_num=1;
    if(!Error_file_text) {
        "%s:%d \x1b[31mERROR\x1b[0m %s\n",tmpf->full_name,line_num,fmted;
    } else {
        tmp=MStrPrint("%s%s:%d ERROR %s\n",Error_file_text,tmpf->full_name,line_num,fmted);
        Free(Error_file_text);
        Error_file_text=tmp;
    }
    //Clear lexer
    LexSetReplMode(cc,FALSE);
    while(Lex(cc));
    throw('ParseFtl');
}
U0 ParserWarn(CCmpCtrl *cc,U8 *msg,...) {
    U8 *fmted=StrPrintJoin(NULL,msg,argc,argv),*tmp;
    CLexFile *tmpf=cc->lex_include_stk;
    loop:
    if(!tmpf) throw('Lexer');
    if(tmpf->flags&LFSF_DEFINE) {
        tmpf=tmpf->next;
        goto loop;
    }
    I64 line_num=tmpf->line_num;
    if (line_num<1) line_num=1;
    if(!Error_file_text) {
        "%s:%d \x1b[33mWARNING\x1b[0m %s\n",tmpf->full_name,line_num,fmted;
    } else {
        tmp=MStrPrint("%s%s:%d WARNING %s\n",Error_file_text,tmpf->full_name,line_num,fmted);
        Free(Error_file_text);
        Error_file_text=tmp;
    }
    Free(fmted);
}

AST *ParseRBinop(CCmpCtrl *cc,AST *(*next)(CCmpCtrl *cc),...) {
    I64 types[0x20];
    AST *args[0x20];
    AST *posits[0x20];
    AST *l=(*next)(cc),*r,*ret=NULL,*ret2;
    if(!l) return NULL;
    I64 idx=0,ptr=0;
    for(idx=0; idx/2<argc/2; idx+=2) {
        next:
        if(cc->token==argv[idx]) {
            posits[ptr]=LSP(cc);
            Lex(cc);
            r=(*next)(cc);
            if(!r) ParserFail(cc,"Expected expression.");
            types[ptr]=argv[idx+1];
            args[ptr++]=l;
            l=r,idx=0;
            goto next;
        }
    }
    if(!ptr) {
        ret=l;
        goto en;
    }
    ret=l;
    while(--ptr>=0) {
        ret2=MAlloc(sizeof(*ret));
        ret2->binop.a=args[ptr];
        ret2->binop.b=ret;
        ret2->type=types[ptr];
        ret2->fn=StrNew(posits[ptr]->fn);
        ret2->ln=posits[ptr]->ln;
        FreeAST(posits[ptr]);
        ret=ret2;
    }
en:
    return ret;
}
AST *ParseLBinop(CCmpCtrl *cc,AST *(*next)(CCmpCtrl *cc),...) {
    I64 types[0x20];
    AST *args[0x20];
    AST *posits[0x20];
    AST *l=(*next)(cc),*r,*ret=NULL,*ret2=NULL;
    if(!l)
        return l;
    I64 idx=0,ptr=0;
    for(idx=0; idx/2<argc/2; idx+=2) {
        next:
        if(cc->token==argv[idx]) {
            posits[ptr]=LSP(cc);
            Lex(cc);
            r=(*next)(cc);
            if(!r) ParserFail(cc,"Expected expression.");
            types[ptr]=argv[idx+1];
            args[ptr++]=l;
            l=r;
            idx=0;
            goto next;
        }
    }
    if(!ptr) {
        ret=l;
        goto en;
    }
    args[ptr]=l;
    ret=args[0];
    for(idx=1; idx<=ptr; idx++) {
        ret2=MAlloc(sizeof(*ret));
        ret2->type=types[idx-1];
        ret2->binop.a=ret;
        ret2->binop.b=args[idx];
        ret2->fn=StrNew(posits[idx-1]->fn);
        ret2->ln=posits[idx-1]->ln;
        FreeAST(posits[idx-1]);
        ret=ret2;
    }
en:
    return ret;
}
CType *GetType(U8 *name,I64 ptr_lvl=0);
U0 WarnOnWeirdPass(CType *want,AST *have) {
    CType *have2=AssignTypeToNode(have);
    CType *ow=want,*oh=have2;
    if(IsPtrType(have2)&&!IsPtrType(want))
        ParserWarn2(have,"Passing pointer to %s.",want->str);
    if(!IsPtrType(have2)&&IsPtrType(want))
        ParserWarn2(have,"Passing non-pointer to %s.",want->str);
    if(IsPtrType(have2)&&IsPtrType(want))
        if(DerefedType(want)!=DerefedType(have2)) {
            want=DerefedType(want);
            if(GetType("U0")==want) return;
            have2=DerefedType(have2);
            if(GetType("U0")==have2) return;
            if(!CompsAreCompat(want,have2)&&!CompsAreCompat(have2,want))
                ParserWarn2(have,"Incompatable (pointer) type pass between %s and %s.",ow->str,oh->str);
            return;
        }
    if(IsInteger(want)||IsF64(want))
        if(IsInteger(have2)||IsF64(have2))
            return;
    if(IsStructType(want)) {
        if(!CompsAreCompat(want,have2)&&!CompsAreCompat(have2,want))
            try {
                ParserFail2(have,"Incompatable type pass between %s and %s.",want->str,have2->str);
            } catch {
                CatchIf('Parser','Lexer');
            }
    }
}
Bool IsLValue(AST *a) {
    CVariable *var;
    CType *t;
    switch(a->type) {
        case AST_EXPLICIT_TC:
            return IsLValue(a->typecast.base);
        case AST_NAME:
        if(Compiler.in_func)
            if(var=HashFind(a->name,Compiler.locals))
                return var->type==HTT_VAR;
        if(var=HashFind(a->name,Compiler.globals))
            return var->type==HTT_VAR;
        break;
        case AST_DERREF:
        if(DerefedType(t=AssignTypeToNode(a->unopArg))->type!=HTT_ARRAY_TYPE)
            return t->type!=HTT_FUNC_TYPE;
        break;
        case AST_ARRAY_ACCESS:
            if(DerefedType(t=AssignTypeToNode(a->binop.a))->type!=HTT_ARRAY_TYPE)
                return t->type!=HTT_FUNC_TYPE;
        break;
        case AST_MEMBER:
            return AssignTypeToNode(a->member_acc.base)->type!=HTT_ARRAY_TYPE;
    }
    return FALSE;
}
Bool CheckForCaseOverlap(AST *a,I64 lo,I64 hi,AST **cases,Bool has_dft) {
    AST *cur;
    I64 idx;
    Bool overlap;
    if(a->type==AST_SUBSWIT) {
        cur=a->sub_swit.body;
    } else
        cur=a->switch_stmt.body;
    for(;cur;cur=cur->next) {
        if(cur->type==AST_CASE) {
            overlap=FALSE;
            for(idx=cur->cs._li-lo;idx<=cur->cs._hi-hi;idx++) {
                if(cases[idx]) overlap=TRUE;
                cases[idx]=cur;
            }
            if(overlap) {
                try {
                    ParserFail2(cur,"Repeat case [%d - %d].",cur->cs._li,cur->cs._hi);
                } catch {
                    CatchIf('Parser','Lexer');
                }
            }
        } else if(cur->type==AST_DFT) {
            try {
                if(has_dft) ParserFail2(cur,"Repeat default");
            } catch {
                CatchIf('Parser','Lexer');
            }
            has_dft=TRUE;
        } else if(cur->type==AST_SUBSWIT) {
            has_dft=CheckForCaseOverlap(cur,lo,hi,cases,has_dft);
        }
    }
}
CType *AssignTypeToNode(AST *a,CType *arr_t=NULL) {
    if(a->type!=AST_SIZEOF&&a->type2) return a->type2;
    I64 dim,sdim,cnt,idx,lo,hi;
    CType *ret=HashFind("I64i",Compiler.globals),*type1,*type2;
    CVariable *var;
    CFunction *func;
    AST **swit_table;
    Bool has_dft;
    try {
    switch(a->type) {
    case AST_NOP:
    goto nul;
    break;
    case AST_DECL:
    try {
        if(a->decl.dft)
            type1=AssignTypeToNode(a->decl.dft),WarnOnWeirdPass(a->decl.type,a->decl.dft);
        else
            type1=a->decl.type;
    } catch {
        CatchIf('Parser','Lexer');
    }
    goto nul;
    nul:
    ret=GetType("U0");
    break;
    break;
    case AST_ARRAY_LITERAL:
    if(!arr_t) {
        "Provide array base type!!!\n";
        throw('Parser');
    }
    cnt=MSize(a->arr_lit)/sizeof(AST*);
    ret=arr_t;
    if(arr_t->dim) {
        dim=arr_t->dim;
    } else
        dim=cnt;
    sdim=0;
    if(arr_t->type==HTT_ARRAY_TYPE) {
        if(arr_t->ambig_dim) {
            arr_t->dim=MaxI64(arr_t->dim,dim);
            arr_t->sz=DerefedType(arr_t)->sz*arr_t->dim;
        }
        arr_t=DerefedType(arr_t);
        if(arr_t->type==HTT_ARRAY_TYPE) {
            while(--cnt>=0) {
                type1=AssignTypeToNode(a->arr_lit[cnt],arr_t);
                sdim=MaxI64(sdim,type1->dim);
            }
            //Ensure all sub-array dimensions are the same
            cnt=MSize(a->arr_lit)/sizeof(AST*);
            if(arr_t->ambig_dim) {
                while(--cnt>=0) {
                    //AssignTypeToNode sets type2
                    a->arr_lit[cnt]->type2->dim=sdim;
                }
                arr_t->sz=sdim*arr_t->base->sz;
                arr_t->align=arr_t->base->align;
            }
        }
    }
    break;
    case AST_MEMBER:
    try {
        ret=AssignTypeToNode(a->member_acc.base);
        if(IsPtrType(ret))
            ret=DerefedType(ret);
        ret=MemberType(ret,a->member_acc.member);
    } catch {
        ParserFail2(a,"Missing member '%s'.",a->member_acc.member);
        ret=HashFind("I64i",Compiler.globals);
        CatchIf('Parser','Lexer');
    };
    if(Compiler.in_func) {
        if(a->member_acc.base->type==AST_NAME) {
            var=HashFind(a->member_acc.base->name,Compiler.locals);
            if(var)
                if(var->type==HTT_VAR&&!IsPtrType(var->hc_type))
                        var->isReg=FALSE;
        }
    }
    break;
    case AST_PAREN:
    ret=AssignTypeToNode(a->unopArg);
    break;
    case AST_INT:
    ret=HashFind("I64i",Compiler.globals);
    break;
    case AST_FLOAT:
    ret=HashFind("F64",Compiler.globals);
    break;
    case AST_CHAR:
    ret=HashFind("I64i",Compiler.globals);
    break;
    case AST_STRING:
    ret=GetType("U8i")+1;
    break;
    case AST_COMMA:
    AssignTypeToNode(a->binop.a);
    ret=AssignTypeToNode(a->binop.b);
    break;
    case AST_ADD:
    type1=AssignTypeToNode(a->binop.a);
    type2=AssignTypeToNode(a->binop.b);
    if(IsPtrType(type1))
        return a->type2=type1;
    if(IsPtrType(type2))
        return a->type2=type2;
    abinop:
    type1=AssignTypeToNode(a->binop.a);
    if(!IsF64(type1)&&!IsInteger(type1))
        ParserFail2(a->binop.a,"Expected arithmetic type.");
    type2=AssignTypeToNode(a->binop.b);
    if(!IsF64(type2)&&!IsInteger(type2))
        ParserFail2(a->binop.b,"Expected arithmetic type.");
    //F64>ptr>unsigned>i64
    if(IsF64(type1)||IsF64(type2)) {
        ret=HashFind("F64",Compiler.globals);
    } else if(IsPtrType(type1)) {
        ret=type1;
    } else if(IsPtrType(type2)) {
        ret=type2;
    } else if(type1->sz==8&&!IsSigned(type1)) {
        ret=type1;
    } else if(type2->sz==8&&!IsSigned(type2)) {
        ret=type2;
    } else
        ret=HashFind("I64i",Compiler.globals);
    break;
    case AST_SUB:
    type1=AssignTypeToNode(a->binop.a);
    type2=AssignTypeToNode(a->binop.b);
    if(IsPtrType(type1)&&IsPtrType(type2))
        return a->type2=HashFind("I64i",Compiler.globals);
    if(IsPtrType(type1))
        return a->type2=type1;
    if(IsPtrType(type2))
        return a->type2=type2;
    goto abinop;
    break;
    case AST_MUL:
    goto abinop;
    break;
    case AST_DIV:
    goto abinop;
    break;
    case AST_MOD:
    goto abinop;
    break;
    case AST_LSHIFT:
    goto abinop;
    break;
    case AST_RSHIFT:
    goto abinop;
    break;
    case AST_POW:
    goto abinop;
    break;
    case AST_BAND:
    goto abinop;
    case AST_BXOR:
    goto abinop;
    break;
    case AST_BOR:
    goto abinop;
    break;
    case AST_LT:
    goto lbinop;
    break;
    case AST_GT:
    goto lbinop;
    break;
    case AST_LE:
    goto lbinop;
    break;
    case AST_EQ:
    goto lbinop;
    break;
    case AST_NE:
    goto lbinop;
    break;
    case AST_GE:
    goto lbinop;
    break;
    case AST_LAND:
    lbinop:
    type1=AssignTypeToNode(a->binop.a);
    if(!IsF64(type1)&&!IsInteger(type1))
        ParserFail2(a->binop.a,"Expected arithmetic type.");
    type2=AssignTypeToNode(a->binop.b);
    if(!IsF64(type2)&&!IsInteger(type2))
        ParserFail2(a->binop.b,"Expected arithmetic type.");
    ret=HashFind("Bool",Compiler.globals);
    break;
    case AST_LOR:
    goto lbinop;
    break;
    case AST_LXOR:
    goto lbinop;
    break;
    case AST_ASSIGN:
    if(!IsLValue(a->binop.a)) ParserFail2(a->binop.a,"Expected lvalue.");
    type1=AssignTypeToNode(a->binop.a);
    ret=type1;
    type2=AssignTypeToNode(a->binop.b);
    WarnOnWeirdPass(type1,a->binop.b);
    if(IsF64(type1)||IsInteger(type1)||IsPtrType(type1))
        if(IsF64(type2)||IsInteger(type2)||IsPtrType(type2))
            break;; //Both are arithmetic
    break;
    case AST_ASSIGN_SHL:
    asn_binop:
    if(!IsLValue(a->binop.a)) ParserFail2(a->binop.a,"Expected lvalue.");
    type1=AssignTypeToNode(a->binop.a);
    type2=AssignTypeToNode(a->binop.b);
    ret=type1;
    if(IsF64(type1)||IsInteger(type1))
        if(IsF64(type1)||IsInteger(type1))
            break;; //Both are arithmetic
    break;
    case AST_ASSIGN_SHR:
    goto asn_binop;
    break;
    case AST_ASSIGN_MUL:
    goto asn_binop;
    break;
    case AST_ASSIGN_DIV:
    goto asn_binop;
    break;
    case AST_ASSIGN_BAND:
    goto asn_binop;
    break;
    case AST_ASSIGN_BOR:
    goto asn_binop;
    break;
    case AST_ASSIGN_BXOR:
    goto asn_binop;
    break;
    case AST_ASSIGN_ADD:
    goto asn_binop;
    break;
    case AST_ASSIGN_SUB:
    goto asn_binop;
    break;
    case AST_POST_INC:
    if(!IsLValue(a->unopArg)) ParserFail2(a->unopArg,"Expected lvalue.");
    type1=AssignTypeToNode(a->unopArg);
    if(IsF64(type1))
        ret=type1;
    else if(IsInteger(type1)) {
        if(type1->sz<8) ret=HashFind("I64i",Compiler.globals);
        else ret=type1;
    } else {
        ParserFail2(a,"Invalid arithemtic type %s.",type1->str);
    }
    break;
    case AST_POST_DEC:
    if(!IsLValue(a->unopArg)) ParserFail2(a->unopArg,"Expected lvalue.");
    aunop:
    type1=AssignTypeToNode(a->unopArg);
    if(IsF64(type1))
        ret=type1;
    else if(IsInteger(type1)) {
        if(type1->sz<8) ret=HashFind("I64i",Compiler.globals);
        else ret=type1;
    } else {
        ParserFail2(a,"Invalid arithemtic type %s.",type1->str);
    }
    break;
    case AST_ARRAY_ACCESS:
    type1=AssignTypeToNode(a->binop.a);
    if(!IsPtrType(type1))
            ParserFail2(a,"Array access takes ptr type,got %s.",type1->str);
    if(!IsInteger(type2=AssignTypeToNode(a->binop.b)))
        ParserFail2(a,"Array index takes integer type,got %s.",type2->str);
    ret=DerefedType(type1);
    break;
    case AST_PRE_DEC:
    if(!IsLValue(a->unopArg)) ParserFail2(a->unopArg,"Expected lvalue.");
    goto aunop;
    break;
    case AST_PRE_INC:
    if(!IsLValue(a->unopArg)) ParserFail2(a->unopArg,"Expected lvalue.");
    goto aunop;
    break;
    case AST_POS:
    goto aunop;
    break;
    case AST_NEG:
    goto aunop;
    break;
    case AST_LNOT:
    type1=AssignTypeToNode(a->unopArg);
    if(IsF64(type1))
        ret=HashFind("I64i",Compiler.globals);
    else if(IsInteger(type1)) {
        ret=HashFind("I64i",Compiler.globals);
    } else {
        ParserFail2(a,"Invalid arithemtic type %s.",type1->str);
    }
    break;
    case AST_BNOT:
    goto aunop;
    break;
    case AST_DERREF:
    if(IsPtrType(type1=AssignTypeToNode(a->unopArg)))
        ret=DerefedType(type1);
    else
        ParserFail2(a,"Can't derefence type %s.",type1->str);
    break;
    case AST_ADDROF:
    if(Compiler.in_func)
        if(a->unopArg->type==AST_NAME) {
            if(var=HashFind(a->unopArg->name,Compiler.locals)) {
                if(var->type==HTT_VAR)
                    var->isReg=FALSE;
            }
        }
    if(a->unopArg.type==AST_NAME)
    if(var=HashFind(a->unopArg->name,Compiler.globals)) {
        if(var->type==HTT_FUNC) {
            type1=(func=var)->hc_type;
            goto skip_addrof;
        }
    }
    type1=AssignTypeToNode(a->unopArg);
    skip_addrof:
    //Addr of array does nothing dude.
    if(type1->type==HTT_ARRAY_TYPE)
        ret=type1;
    else
        ret=type1+1;
    break;
    case AST_SIZEOF:
    //Dont assign I64i to the ast's type2(which is used for the sizeof type here).
    return HashFind("I64i",Compiler.globals);
    case AST_NAME:
    if(Compiler.in_func) {
        if(var=HashFind(a->name,Compiler.locals)) {
            if(var->type==HTT_VAR) {
                if(var->is_opted_out)
                    ParserFail2(a,"Variable is optimized out.",var->str);
                ret=var->hc_type;
                break;
            } else //????
                ParserFail2(a,"Expected variable.");
        }
    }
    if(var=HashFind(a->name,Compiler.globals)) {
            if(var->type==HTT_VAR) {
                ret=var->hc_type;
            } else if(var->type==HTT_FUNC) {
                func=var;
                //Ensure all arguments are implicit
                idx=MSize(func->hc_type->arg_values)/sizeof(I64**);
                while(--idx>=0)
                    if(!func->hc_type->arg_values[idx])
                        ParserFail2(a,"Implicit function call is missing defualt arguments.");
                //Functions on their own are impliclty called
                ret=func->hc_type->ret_type;
            } else
                ParserFail2(a,"Expected variable.");
    } else if(Compiler.asm_mode) {
        if(HashFind(a->name,regs)) {
            ret=GetType("I64");
            break;
        }
        //May be a label
        ret=GetType("U8i")+1;
        break;
    } else {
        ParserFail2(a,"Unknown symbol '%s'.",a->name);
    }
    break;
    case AST_IMPLICIT_TC:
    //TODO
    break;
    case AST_EXPLICIT_TC:
    type1=AssignTypeToNode(a->typecast.base);
    if(!IsF64(type1)&&!IsInteger(type1))
        ParserFail2(a,"Can't typecast non-primtive %s to %s.",type1->str,a->typecast.to_type->str);
    if(!IsF64(type2=a->typecast.to_type)&&!IsInteger(a->typecast.to_type))
        ParserFail2(a,"Can't typecast non-primtive %s to %s.",type1->str,a->typecast.to_type->str);
    ret=a->typecast.to_type;
    break;
    case AST_FUNC_CALL:
    ret=NULL;
    if(a->func_call.base->type==AST_NAME)
        if(var=HashFind(a->func_call.base->name,Compiler.globals))
            if(var->type==HTT_VAR) {
                ret=var->hc_type;
            } else if(var->type==HTT_FUNC) {
                func=var;
                ret=func->hc_type;
            } else
                ParserFail2(a,"Expected variable.");
    if(!ret)
        ret=AssignTypeToNode(a->func_call.base);
    if(IsPtrType(ret))
        ret=DerefedType(ret);
    if(ret->type!=HTT_FUNC_TYPE)
        ParserFail2(a->func_call.base,"Expected function type(got %s).",ret->str);
    cnt=MSize(ret->arg_types)/sizeof(CType*);
    for(idx=0;idx!=cnt;idx++) {
        if(idx>=MSize(a->func_call.args)/sizeof(AST*)) {
            goto check_dft;
        } else {
            if(a->func_call.args[idx]) {
                type2=AssignTypeToNode(a->func_call.args[idx]);
                WarnOnWeirdPass(ret->arg_types[idx],a->func_call.args[idx]);
            } else {
                check_dft:
                if(!ret->arg_values[idx]) {
                    nodft:
                    ParserFail2(a,"No defualt argument for operand %d.",idx);
                }
            }
        }
    }
    if(cnt<MSize(a->func_call.args)/sizeof(AST*)&&!ret->has_vargs)
        ParserFail2(a,"Excess arguments for type '%s'.",ret->str);
    for(;idx<MSize(a->func_call.args)/sizeof(AST*);idx++) {
        AssignTypeToNode(a->func_call.args[idx]);
        if(!a->func_call.args[idx])
            ParserFail2(a,"Expected value to varg.");
    }
    ret=ret->ret_type;
    break;
    case AST_IF:
    type1=AssignTypeToNode(a->if_stmt.cond);
    if(IsF64(type1)||IsInteger(type1)||IsPtrType(type1))
        ;
    else {
        ParserFail2(a,"Passing non-arithmetic type to if statement.");
    }
    goto nul;
    case AST_FOR:
    if(a->for_stmt.init) AssignTypeToNode(a->for_stmt.init);
    if(a->for_stmt.cond)
        type1=AssignTypeToNode(a->for_stmt.cond);
    else
        type1=GetType("I64i");
    if(a->for_stmt.inc) AssignTypeToNode(a->for_stmt.inc);
    if(IsF64(type1)||IsInteger(type1)||IsPtrType(type1))
        ;
    else {
        ParserFail2(a,"Passing non-arithmetic type to for's condtional statement.");
    }
    goto nul;
    case AST_DO:
    type1=AssignTypeToNode(a->do_stmt.cond);
    if(IsF64(type1)||IsInteger(type1)||IsPtrType(type1))
        ;
    else {
        ParserFail2(a,"Passing non-arithmetic type to do statement.");
    }
    goto nul;
    case AST_WHILE:
    type1=AssignTypeToNode(a->while_stmt.cond);
    if(IsF64(type1)||IsInteger(type1)||IsPtrType(type1))
        ;
    else {
        ParserFail2(a,"Passing non-arithmetic type to while statement.");
    }
    goto nul;
    case AST_META_DATA:
    //Nonsense????
    break;
    case AST_CASE:
    type1=AssignTypeToNode(a->cs.low);
    if(a->cs.high) type2=AssignTypeToNode(a->cs.high);
    else type2=GetType("I64i");
    if(IsF64(type1)||IsInteger(type1)||IsPtrType(type1))
        ;
    else {
        cs_fail:
        ParserFail2(a,"Passing non-arithmetic type to while statement.");
    }
    if(IsF64(type2)||IsInteger(type2)||IsPtrType(type2))
        ;
    else
        goto cs_fail;
    goto nul;
    case AST_SWITCH:
    ScanForCases(a->switch_stmt.body,&lo,&hi,&has_dft);
    if(hi-lo>U16_MAX) ParserFail2(a,"Gaint switch table,raising error.");
    type1=AssignTypeToNode(a->switch_stmt.cond);
    swit_table=MAlloc(sizeof(AST*)*(hi-lo+1));
    CheckForCaseOverlap(a,lo,hi,swit_table,FALSE);
    Free(swit_table);
    if(IsF64(type1)||IsInteger(type1)||IsPtrType(type1))
        ;
    else
        ParserFail2(a,"Passing non-arithmetic type to switch statement.");
    goto nul;
    case AST_SUBSWIT:
    case AST_BREAK:
    case AST_DFT:
    goto nul;
    case AST_RETURN:
    if(!Compiler.in_func)
        ParserFail2(a,"Return outside of function.");
    if(a->unopArg) WarnOnWeirdPass(Compiler.ret_type,a->unopArg);
    goto nul;
    case AST_TYPE:
    goto nul;
    case AST_GOTO:
    case AST_LABEL:
    case AST_TRY:
    goto nul;
    case AST_LASTCLASS:
    ret=GetType("U8i")+1;
    break;
    case AST_RANGE:
    cnt=MSize(a->range.values)/sizeof(AST*);
    while(--cnt>=0) {
        if(!IsF64(AssignTypeToNode(a->range.values[cnt]))&&!IsInteger(AssignTypeToNode(a->range.values[cnt])))
            ParserFail2(a->range.values[cnt],"Expected arithmetic type.");
    }
    ret=HashFind("Bool",Compiler.globals);
    break;
    case AST_ASSIGN_MOD:
    goto asn_binop;
    break;
    }
    } catch {
        ret=HashFind("I64i",Compiler.globals);
        CatchIf('Parser','Lexer');
    }
    a->type2=ret;
    return a->type2;
}
_extern Prec14 AST * ParseExprNoComma(CCmpCtrl *cc);
_extern Prec15 AST * ParseExpr(CCmpCtrl *cc);

AST *Prec0(CCmpCtrl *cc) {
    AST *ret;
    U8 *tmp_s;
    switch(cc->token) {
        case TK_STR:
        ret=MAlloc(sizeof(AST));
        ret->type=AST_STRING;
        ret->string=MemCpy(MAlloc(cc->cur_str_len),cc->cur_str,cc->cur_str_len);
        LSP(cc,ret);
        sloop:
        Lex(cc);
        if(cc->token==TK_STR) {
            //-1 ignores nul byte
            tmp_s=MAlloc(MSize(ret->string)-1+cc->cur_str_len);
            StrNCpy(tmp_s,ret->string,MSize(ret->string)-1);
            StrNCpy(tmp_s+MSize(ret->string)-1,cc->cur_str,cc->cur_str_len);
            Free(ret->string);
            ret->string=tmp_s;
            goto sloop;
        }
        return ret;

start:
        ret=MAlloc(sizeof(*ret));
    case TK_I64:
        ret->type=AST_INT;
        ret->int=cc->cur_i64;
        break;
    case TK_F64:
        ret->floating=cc->cur_f64;
        ret->type=AST_FLOAT;
        break;
    case TK_CHAR_CONST:
        ret->type=AST_CHAR;
        ret->int=cc->cur_i64;
        break;
    case TK_IDENT:
        if(HashFind(cc->cur_str,keywords)) return NULL;
        ret->type=AST_NAME;
        ret->name=StrNew(cc->cur_str);
        break;
    case '(':
        Lex(cc);
        ret=ParseExpr(cc);
        if(cc->token!=')') ParserFail(cc,"Expected ')'.");
        break;
end:
        LSP(cc,ret);
        Lex(cc);
        return ret;
    default:
        return NULL;
    }
}
extern CType *ParseType(CType *_bt=NULL,CCmpCtrl *cc,AST **name,Bool base_type_only=FALSE);
AST *Prec1(CCmpCtrl *cc) {
    AST *un=Prec0(cc),*args[0xff];
    if(!un) return NULL;
    AST *ret=NULL,*tmp,*tmp2;
    CType *t;
    I64 idx=0;
    loop:
    if(Compiler.no_array_access_mode&&cc->token=='[') return ret;
    switch(cc->token) {
start:
        ret=MAlloc(sizeof(*ret));
    case TK_PLUS_PLUS:
        ret->type=AST_POST_INC;
        ret->unopArg=un;
        break;
    case TK_MINUS_MINUS:
        ret->type=AST_POST_DEC;
        ret->unopArg=un;
        break;
    case '(':
        Lex(cc);
        t=ParseType(NULL,cc,NULL);
        if(t) {
            if(cc->token!=')') {
                ParserFail(cc,"Expected ')'.");
            }
            ret->type=AST_EXPLICIT_TC;
            ret->typecast.base=un;
            ret->typecast.to_type=t;
            break;
        }
        ret->func_call.base=un;
        ret->type=AST_FUNC_CALL;
        while(cc->token!=')') {
next_arg:
            ;
            tmp=ParseExprNoComma(cc);
            args[idx++]=tmp;
            if(cc->token==',') {
                Lex(cc);
                goto next_arg;
            }
            else if(cc->token!=')') {
                ParserFail(cc,"Expected ')'.");
            }
        }
        MemCpy(ret->func_call.args=MAlloc(sizeof(AST*)*idx),args,sizeof(AST*)*idx);
        break;
    case '[':
        ret->type=AST_ARRAY_ACCESS;
        ret->binop.a=un;
        Lex(cc);
        ret->binop.b=ParseExpr(cc);
        if(cc->token!=']') {
            ParserFail(cc,"Expected ']'.");
        }
        break;
    case '.':
    case TK_DEREFERENCE:
        ret->type=AST_MEMBER;
        ret->member_acc.base=un;
        Lex(cc);
        if(cc->token!=TK_IDENT) ParserFail(cc,"Expected member.");
        ret->member_acc.member=StrNew(cc->cur_str);
        break;
end:
        LSP(cc,ret);
        Lex(cc);
        un=ret;
        goto loop;
    default:
        return un;
    }
}
AST *Prec2(CCmpCtrl *cc) {
    AST *un=NULL,*ret,*tmp,*tmp2;
    Bool close;
    CType *t;
    if(!un) {
    loop:
    switch(cc->token) {
    case TK_IDENT:
        if(!StrCmp(cc->cur_str,"sizeof")) {
            ret=MAlloc(sizeof(*ret));
            LSP(cc,ret);
            Lex(cc);
            ret->type=AST_SIZEOF;
            ret->type2=ParseType(NULL,cc,NULL);
            close=FALSE;
            if((!ret->type2)&&cc->token=='(') {
                Lex(cc),close=TRUE;
                    ret->type2=ParseType(NULL,cc,NULL);
            }
            if(!ret->type2) {
                tmp=Prec2(cc);
                if(!tmp) ParserFail(cc,"Expected expression or type.");
                ret->type2=AssignTypeToNode(tmp);
                FreeAST(tmp);
            }
            if(close&&cc->token!=')') ParserFail(cc,"Expected ')'.");
            else if(close&&cc->token==')') Lex(cc);
            return ret;
        } else if(!StrCmp(cc->cur_str,"offsetof")) {
            ret=MAlloc(sizeof(*ret));
            LSP(cc,ret);
            Lex(cc);
            if(cc->token!='(') ParserFail(cc,"Expected ')'.");
            t=ParseType(NULL,cc,NULL);
            if(!t) ParserFail(cc,"Expected type.");
            if(cc->token!=',') ParserFail(cc,"Expected ','.");
            Lex(cc);
            if(cc->token!=TK_IDENT) ParserFail(cc,"Expected member.");
            ret->type=AST_INT;
            try {
                try {
                    ret->int=MemberOffset(t,cc->cur_str);
                } catch {
                    CatchIf('Parser','Lexer');
                    ParserFail2(ret,"Missing member %s for type %s.",cc->cur_str,t->str);
                }
            } catch {
                CatchIf('Parser','Lexer');
            }
            Lex(cc);
            if(cc->token!=')') ParserFail(cc,"Expected ')'.");
            return ret;
        }
        goto dft;
start:
        ret=MAlloc(sizeof(*ret));
        LSP(cc,ret);
        Lex(cc);
        un=Prec2(cc);
        if(!un) un=Prec1(cc);
        if(!un) ParserFail(cc,"Expected operand.");
    case TK_PLUS_PLUS:
        ret->type=AST_PRE_INC;
        ret->unopArg=un;
        break;
    case TK_MINUS_MINUS:
        ret->type=AST_PRE_DEC;
        ret->unopArg=un;
        break;
    case '+':
        ret->type=AST_POS;
        ret->unopArg=un;
        break;
    case '-':
        ret->type=AST_NEG;
        ret->unopArg=un;
        break;
    case '!':
        ret->type=AST_LNOT;
        ret->unopArg=un;
        break;
    case '~':
        ret->type=AST_BNOT;
        ret->unopArg=un;
        break;
    case '*':
        ret->type=AST_DERREF;
        ret->unopArg=un;
        break;
    case '&':
        ret->type=AST_ADDROF;
        ret->unopArg=un;
        break;
end:
        return ret;
    default:
dft:
        return Prec1(cc);
    }
    }
    return un;
}
AST *Prec3(CCmpCtrl *cc) {
    return ParseLBinop(cc,&Prec2,'`',AST_POW,TK_SHL,AST_LSHIFT,TK_SHR,AST_RSHIFT);
}
AST *Prec4(CCmpCtrl *cc) {
    return ParseLBinop(cc,&Prec3,'*',AST_MUL,'/',AST_DIV,'%',AST_MOD);
}
AST *Prec5(CCmpCtrl *cc) {
    return ParseLBinop(cc,&Prec4,'&',AST_BAND);
}
AST *Prec6(CCmpCtrl *cc) {
    return ParseLBinop(cc,&Prec5,'^',AST_BXOR);
}
AST *Prec7(CCmpCtrl *cc)     {
    return ParseLBinop(cc,&Prec6,'|',AST_BOR);
}
AST *Prec8(CCmpCtrl *cc) {
    return ParseLBinop(cc,&Prec7,'+',AST_ADD,'-',AST_SUB);
}
AST *Prec9(CCmpCtrl *cc) {
    AST *ranges[0x20],*ret=NULL,*posit=LSP(cc);
    I64 types[0x20];
    I64 idx=0;
    for(;;) {
        ranges[idx]=Prec8(cc);
        if(idx&&!ranges[idx]) ParserFail(cc,"Expected expression.");
        switch(cc->token) {
        case '<':
            types[idx++]=AST_LT;
            Lex(cc);
            break;
        case '>':
            types[idx++]=AST_GT;
            Lex(cc);
            break;
        case TK_GREATER_EQU:
            types[idx++]=AST_GE;
            Lex(cc);
            break;
        case TK_LESS_EQU:
            types[idx++]=AST_LE;
            Lex(cc);
            break;
        default:
            if(!idx) return ranges[0];
            ret=MAlloc(sizeof(*ret));
            ret->type=AST_RANGE;
            ret->range.values=MAlloc(sizeof(AST*)*(1+idx));
            ret->range.types=MAlloc(sizeof(I64)*idx);
            MemNCpy(ret->range.values,ranges,(1+idx)*sizeof(AST*));
            MemNCpy(ret->range.types,types,idx*sizeof(I64));
            ret->fn=StrNew(posit->fn);
            ret->ln=posit->ln;
            FreeAST(posit);
            return ret;
        }
    }
    FreeAST(posit);
    return ret;
}
AST *Prec10(CCmpCtrl *cc) {
    return ParseLBinop(cc,&Prec9,TK_EQU_EQU,AST_EQ,TK_NOT_EQU,AST_NE);
}
AST *Prec11(CCmpCtrl *cc) {
    return ParseLBinop(cc,&Prec10,TK_AND_AND,AST_LAND);
}
AST *Prec12(CCmpCtrl *cc) {
    return ParseLBinop(cc,&Prec11,TK_XOR_XOR,AST_LXOR);
}
AST *Prec13(CCmpCtrl *cc) {
    return ParseLBinop(cc,&Prec12,TK_OR_OR,AST_LOR);
}
AST *Prec14(CCmpCtrl *cc) {
    AST *ret=ParseRBinop(cc,&Prec13,
                       '=',AST_ASSIGN,
                       TK_SHL_EQU,AST_ASSIGN_SHL,
                       TK_SHR_EQU,AST_ASSIGN_SHR,
                       TK_MUL_EQU,AST_ASSIGN_MUL,
                       TK_DIV_EQU,AST_ASSIGN_DIV,
                       TK_MOD_EQU,AST_ASSIGN_MOD,
                       TK_XOR_EQU,AST_ASSIGN_BXOR,
                       TK_OR_EQU,AST_ASSIGN_BOR,
                       TK_AND_EQU,AST_ASSIGN_BAND,
                       TK_ADD_EQU,AST_ASSIGN_ADD,
                       TK_SUB_EQU,AST_ASSIGN_SUB
                      );
    //Quick hack to check for label
    if(ret&&ret->type==AST_NAME&&cc->token==':')
        return ret;
    if(ret) AssignTypeToNode(ret);
    return ret;
}
AST *Prec15(CCmpCtrl *cc) {
    AST *ret=ParseLBinop(cc,&Prec14,',',AST_COMMA);
    //Quick hack to check for label
    if(ret&&ret->type==AST_NAME&&cc->token==':')
        return ret;
    if(ret) AssignTypeToNode(ret);
    return ret;
}
U8 *Cat(...) {
    U8 *cur=argv[0],*tmp;
    I64 idx=1;
    while(idx<argc) {
        tmp=MStrPrint("%s%s",cur,argv[idx]);
        Free(cur),Free(argv[idx]);
        cur=tmp;
        idx++;
    }
    return cur;
}
Bool IsForward(CHash *item) {
    CVariable *var;
    CFunction *func;
    var=func=item;
    switch(item->type) {
        case HTT_VAR:
        return !var->global_ptr;
        case HTT_FUNC:
        return !(func->jit&&func->func_ptr);
    }
    return FALSE;
}
extern U8 *PrintAST(AST *a);
U8 *__PrintAST(AST *a) {
    I64 idx,cnt;
    U8 *tmp1,*tmp2,*tmp3,*tmp4;
    AST *ast;
    if(!a) return StrNew(""    );
    switch(a->type) {
    case AST_NOP:
        return StrNew("NOP");
    case AST_DECL:
        tmp3=MStrPrint("%s %s=%s",a->decl.type->str,a->decl.name,tmp2=PrintAST(a->decl.dft));
        Free(tmp1),Free(tmp2);
        return tmp3;
    case AST_ARRAY_LITERAL:
        cnt=MSize(a->arr_lit)/sizeof(AST*);
        tmp1=StrNew("{\n");
        for(idx=0;idx!=cnt;idx++)
            tmp1=Cat(tmp1,PrintAST(a->arr_lit[idx]),StrNew(",\n"));
        return Cat(tmp1,StrNew("\n}\n"));
    case AST_NAME:
        return StrNew(a->name);
    case AST_MEMBER:
        tmp2=".";
        goto binop;
binop:
        tmp4=MStrPrint("(%s%s%s)",tmp1=PrintAST(a->binop.a),tmp2,tmp3=PrintAST(a->binop.b));
        Free(tmp1),Free(tmp3);
        return tmp4;
    case AST_PAREN:
        tmp2=MStrPrint("(%s)",tmp1=PrintAST(a->unopArg));
        Free(tmp1);
        return tmp2;
    case AST_INT:
        return MStrPrint("%d",a->int);
    case AST_FLOAT:
        return MStrPrint("%n",a->floating);
    case AST_CHAR:
        return MStrPrint("%c",a->int);
    case AST_STRING:
        return MStrPrint("\"%s\"",a->string);
    case AST_COMMA:
        tmp2=",";
        goto binop;
    case AST_ADD:
        tmp2="+";
        goto binop;
    case AST_SUB:
        tmp2="-";
        goto binop;
    case AST_MUL:
        tmp2="*";
        goto binop;
    case AST_DIV:
        tmp2="/";
        goto binop;
    case AST_MOD:
        tmp2="%";
        goto binop;
    case AST_LSHIFT:
        tmp2="<<";
        goto binop;
    case AST_RSHIFT:
        tmp2=">>";
        goto binop;
    case AST_POW:
        tmp2="`";
        goto binop;
    case AST_BAND:
        tmp2="&";
        goto binop;
    case AST_BXOR:
        tmp2="^";
        goto binop;
    case AST_BOR:
        tmp2="|";
        goto binop;
    case AST_LT:
        tmp2="<";
        goto binop;
    case AST_GT:
        tmp2=">";
        goto binop;
    case AST_LE:
        tmp2="<=";
        goto binop;
    case AST_EQ:
        tmp2="==";
        goto binop;
    case AST_NE:
        tmp2="!=";
        goto binop;
    case AST_GE:
        tmp2=">=";
        goto binop;
    case AST_LAND:
        tmp2="&&";
        goto binop;
    case AST_LOR :
        tmp2="||";
        goto binop;
    case AST_LXOR:
        tmp2="^^";
        goto binop;
    case AST_ASSIGN :
        tmp2="=";
        goto binop;
    case AST_ASSIGN_SHL:
        tmp2="<<=";
        goto binop;
    case AST_ASSIGN_SHR:
        tmp2=">>=";
        goto binop;
    case AST_ASSIGN_MUL:
        tmp2="*=";
        goto binop;
    case AST_ASSIGN_DIV:
        tmp2="/=";
        goto binop;
    case AST_ASSIGN_BAND:
        tmp2="&=";
        goto binop;
    case AST_ASSIGN_BOR:
        tmp2="|=";
        goto binop;
    case AST_ASSIGN_BXOR:
        tmp2="^=";
        goto binop;
    case AST_ASSIGN_ADD:
        tmp2="+=";
        goto binop;
    case AST_ASSIGN_SUB:
        tmp2="-=";
        goto binop;
    case AST_POST_INC:
        tmp2=MStrPrint("%s++",tmp1=PrintAST(a->unopArg));
        Free(tmp1);
        return tmp2;
    case AST_POST_DEC:
        tmp2=MStrPrint("%s--",tmp1=PrintAST(a->unopArg));
        Free(tmp1);
        return tmp2;
    case AST_ARRAY_ACCESS:
        tmp3=MStrPrint("%s[%s]",tmp1=PrintAST(a->binop.a),tmp2=PrintAST(a->binop.b));
        Free(tmp1),Free(tmp2);
        return tmp3;
    case AST_PRE_DEC:
        tmp2=MStrPrint("--%s",tmp1=PrintAST(a->unopArg));
        Free(tmp1);
        return tmp2;
    case AST_PRE_INC:
        tmp2=MStrPrint("++%s",tmp1=PrintAST(a->unopArg));
        Free(tmp1);
        return tmp2;
    case AST_POS:
        tmp2="+";
        goto unop;
unop:
        tmp3=MStrPrint("(%s%s)",tmp2,tmp1=PrintAST(a->unopArg));
        Free(tmp1);
        return tmp3;
    case AST_NEG:
        tmp2="-";
        goto unop;
    case AST_LNOT:
        tmp2="!";
        goto unop;
    case AST_BNOT:
        tmp2="~";
        goto unop;
    case AST_DERREF:
        tmp2="*";
        goto unop;
    case AST_ADDROF:
        tmp2="&";
        goto unop;
    case AST_SIZEOF:
        tmp3=MStrPrint("(sizeof %s)",tmp1=PrintAST(a->unopArg));
        Free(tmp1);
        return tmp3;
    case AST_EXPLICIT_TC:
    case AST_IMPLICIT_TC:
        tmp3=MStrPrint("(TYPECAST %s %s)",tmp1=PrintAST(a->typecast.base),a->typecast.to_type->str);
        Free(tmp2),Free(tmp1);
        return tmp3;
    case AST_FUNC_CALL:
        tmp1=PrintAST(a->func_call.base);
        ast=a->func_call.args;
        tmp2=StrNew("(");
        for(; ast; ast=ast->next) {
            tmp3=__PrintAST(ast);
            if(ast->next)
                tmp4=MStrPrint("%s%s,",tmp2,tmp3);
            else
                tmp4=MStrPrint("%s%s",tmp2,tmp3);
            Free(tmp3),Free(tmp2);
            tmp2=tmp4;
        }
        tmp3=MStrPrint("%s%s)",tmp1,tmp2);
        Free(tmp1),Free(tmp2);
        return tmp3;
    case AST_IF:
        return Cat(
                   StrNew("If("),
                   PrintAST(a->if_stmt.cond),
                   StrNew("){\n"),
                   PrintAST(a->if_stmt.body),
                   StrNew("\n} else {\n"),
                   PrintAST(a->if_stmt. el),
                   StrNew("}\n"));
    case AST_FOR:
        return Cat(
                   StrNew("For("),
                   PrintAST(a->for_stmt.init),
                   StrNew(";"),
                   PrintAST(a->for_stmt.cond),
                   StrNew(";"),
                   PrintAST(a->for_stmt.inc),
                   StrNew(") {\n"),
                   PrintAST(a->for_stmt.body),
                   StrNew("\n}\n")
               );
    case AST_DO:
        return Cat(
                   StrNew("do {\n"),
                   PrintAST(a->do_stmt.body),
                   StrNew("\n}while("),
                   PrintAST(a->do_stmt.cond),
                   StrNew(");\n")
               );
    case AST_WHILE:
        return Cat(
                   StrNew("while("),
                   PrintAST(a->while_stmt.cond),
                   StrNew("){\n"),
                   PrintAST(a->while_stmt.body),
                   StrNew("\n}\n")
               );
    case AST_META_DATA:
        return StrNew("TODO");
    case AST_CASE:
        if(a->cs.low&&a->cs.high)
            return Cat(
                       StrNew("case"),
                       PrintAST(a->cs.low),
                       StrNew("..."),
                       PrintAST(a->cs.high),
                       StrNew(":"));
        if(a->cs.low)
            return Cat(
                       StrNew("Case"),
                       PrintAST(a->cs.low),
                       StrNew(":"));
        return StrNew("Case");
    case AST_SWITCH:
        return Cat(
                   StrNew("switch("),
                   PrintAST(a->switch_stmt.cond),
                   StrNew(") {\n"),
                   PrintAST(a->switch_stmt.body),
                   StrNew("\n}\n"));
    case AST_SUBSWIT:
        return Cat(
            StrNew("start:\n"),
            PrintAST(a->sub_swit.start_code),
            PrintAST(a->sub_swit.body),
            StrNew("end:\n")
        );
    case AST_BREAK:
        return StrNew("Break\n");
    case AST_DFT:
        return StrNew("Default\n");
    case AST_RETURN:
        return Cat(StrNew("Return\n"),PrintAST(a->unopArg));
    case AST_TYPE:
    case AST_GOTO:
        return Cat(StrNew("Goto\n"),PrintAST(a->unopArg));
    case AST_LABEL:
        return Cat(PrintAST(a->string),StrNew(":\n"));
    case AST_RANGE:
        cnt=MSize(a->range.values)/sizeof(AST*)-1;
        tmp2=StrNew("(");
        for(idx=0;idx!=cnt;idx++) {
            tmp2=Cat(tmp2,PrintAST(a->range.values[idx]));
            switch(a->range.types[idx]) {
                case AST_LT:
                tmp2=Cat(tmp2,StrNew("<"));
                break;
                case AST_GT:
                tmp2=Cat(tmp2,StrNew(">"));
                break;
                case AST_LE:
                tmp2=Cat(tmp2,StrNew("<="));
                break;
                case AST_GE:
                tmp2=Cat(tmp2,StrNew(">="));
                break;
            }
        }
        tmp2=Cat(tmp2,a->range.values[idx],StrNew(")"));
        return tmp2;
    case AST_TRY:
        return Cat(
                   StrNew("try {\n"),
                   PrintAST(a->try_block.body),
                   StrNew("} catch {\n"),
                   PrintAST(a->try_block.cat),
                   StrNew("}\n")
               );
    case AST_LASTCLASS:
        return StrNew("Lastclass\n");
    default:
        throw('AST');
    }
}
U8 *HexifyString(U8 *str,I64 len) {
    U8 *ret=MAlloc(2*len+1);
    U8 *digits="0123456789ABCDEF";
    while(--len>=0) {
        ret[len*2+1]=digits[str[len]>>4];
        ret[len*2]=digits[str[len]&0b1111];
    }
    return ret;
}
U8 *PrintAST(AST *a) {
    U8 *ret,*tmp,*tmp2;
    if(a&&a->next&&a->type!=AST_FUNC_CALL) {
        ret=StrNew("{\n");
        for(;a;a=a->next) {
            ret=Cat(ret,__PrintAST(a),StrNew(";\n"));
        }
        return Cat(ret,StrNew("}\n"));
    }
    return __PrintAST(a);
}
extern AST *ParseStmt(CCmpCtrl *cc,Bool instant_eval=FALSE,Bool allow_case=FALSE);
AST *ParseReturn(CCmpCtrl *cc,Bool instant_eval=FALSE) {
    AST *ret,*expr;
    if(cc->token!=TK_IDENT) return NULL;
    if(StrCmp(cc->cur_str,"return")) return NULL;
    ret=MAlloc(sizeof(AST));
    LSP(cc,ret);
    Lex(cc);
    expr=ParseExpr(cc);
    if(expr) AssignTypeToNode(expr);
    ret->type=AST_RETURN;
    ret->unopArg=expr;
    if(cc->token!=';') ParserFail(cc,"Expected ';'.");
    if(!instant_eval)
        Lex(cc);
    AssignTypeToNode(ret);
    return ret;
}
AST *ParseBreak(CCmpCtrl *cc) {
    AST *ret;
    if(cc->token==TK_IDENT)
        if(!StrCmp(cc->cur_str,"break")) {
            Lex(cc);
            if(cc->token!=';') ParserFail(cc,"Expected ';'.");
            Lex(cc);
            ret=MAlloc(sizeof(AST));
            ret->type=AST_BREAK;
            LSP(cc,ret);
            try {
                if(!Compiler.allow_breaks)
                    ParserFail2(ret,"Breaks not allowed here.");
            } catch {
                CatchIf('Compiler','Lexer');
            }
            return ret;
        }
    return NULL;
}
AST *ParsePrint(CCmpCtrl *cc,Bool instant_eval=FALSE) {
    I64 idx=0,len;
    AST *ret,*args[0xff];
    U8 *old;
    if(cc->token!=TK_STR&&cc->token!=TK_CHAR_CONST) return NULL;
    ret=MAlloc(sizeof(AST));
    ret->type=AST_PRINT;
    if(cc->token==TK_CHAR_CONST) {
        U8 arr[]={
            cc.cur_i64.u8[0],
            cc.cur_i64.u8[1],
            cc.cur_i64.u8[2],
            cc.cur_i64.u8[3],
            cc.cur_i64.u8[4],
            cc.cur_i64.u8[5],
            cc.cur_i64.u8[6],
            cc.cur_i64.u8[7],
            0
        };
        ret->print.text=StrNew(arr);
    } else {
        ret->print.text=StrNew(cc->cur_str);
    }
    for(Lex(cc);cc->token==TK_STR;Lex(cc)) {
        len=MSize(ret->print.text)-1+cc->cur_str_len;
        old=ret->print.text;
        ret->print.text=StrNCpy(MAlloc(len),ret->print.text,MSize(old)); //Ignore NULL byte
        StrNCpy(ret->print.text+MSize(old)-1,cc->cur_str,cc->cur_str_len);
        Free(old);
    }
    loop:
    if(cc->token==',') {
        Lex(cc);
        args[idx++]=ParseExprNoComma(cc);
        AssignTypeToNode(args[idx-1]);
        if(!args[idx-1]) ParserFail(cc,"Expected argument.");
        goto loop;
    } else if(cc->token==';') {
        if(!instant_eval)
            Lex(cc);
    } else {
        ParserFail(cc,"Expected ';'.");
    }
    ret->print.args=MAlloc(sizeof(AST*)*idx);
    MemCpy(ret->print.args,args,idx*sizeof(AST*));
    return ret;
}
AST *ParseGoto(CCmpCtrl *cc) {
    U8 *name=NULL;
    AST *r;
    CUnresolvedLabel *unresolved;
    if(cc->token==TK_IDENT&&!StrCmp("goto",cc->cur_str)) {
        Lex(cc);
        if(cc->token!=TK_IDENT) ParserFail(cc,"Expected name.");
        name=StrNew(cc->cur_str);
        if(!Compiler.in_func) ParserFail(cc,"No global labels.");
        Lex(cc);
        if(cc->token!=';') ParserFail(cc,"Expected ';'.");
        Lex(cc);
        r=MAlloc(sizeof(AST));
        r->type=AST_GOTO;
        r->name=name;
        if(HashFind(name,Compiler.labels)) {
        } else {
            unresolved=MAlloc(sizeof(CUnresolvedLabel));
            unresolved->str=StrNew(name);
            unresolved->stmt=r;
            HashAdd(unresolved,Compiler.unresolved_labels);
        }
        return r;
    }
    return NULL;
}
AST *ParseIf(CCmpCtrl *cc,Bool instant_eval=FALSE) {
    AST *cond,*body=NULL,*el=NULL,*ret,*posit;
    if(cc->token!=TK_IDENT) return NULL;
    if(StrCmp(cc->cur_str,"if")) return NULL;
    posit=LSP(cc);
    Lex(cc);
    if(cc->token!='(') ParserFail(cc,"Expected '('.");
    Lex(cc);
    cond=ParseExpr(cc);
    if(cc->token!=')') ParserFail(cc,"Expected ')'.");
    Lex(cc);
    body=ParseStmt(cc,instant_eval);
    if(cc->token==TK_IDENT&&!instant_eval)
        if(!StrCmp(cc->cur_str,"else")) {
            Lex(cc);
            el=ParseStmt(cc);
        }
    ret=MAlloc(sizeof(*ret));
    ret->type=AST_IF;
    ret->if_stmt.cond=cond;
    ret->if_stmt.body=body;
    ret->if_stmt. el=el;
    ret->fn=StrNew(posit->fn);
    ret->ln=posit->ln;
    FreeAST(posit);
    AssignTypeToNode(ret);
    return ret;
    fail:
    FreeAST(posit);
    return NULL;
}
AST *ParseTry(CCmpCtrl *cc,Bool instant_eval=FALSE) {
    AST *body,*cat,*ret;
    if(cc->token==TK_IDENT&&!StrCmp(cc->cur_str,"try")) {
        Lex(cc);
        Compiler.force_noregs=TRUE;
        body=ParseStmt(cc);
        if(cc->token==TK_IDENT&&!StrCmp(cc->cur_str,"catch")) {
            Lex(cc);
            cat=ParseStmt(cc,instant_eval);
            ret=MAlloc(sizeof(AST));
            ret->type=AST_TRY;
            ret->try_block.body=body;
            ret->try_block.cat=cat;
            return ret;
        } else {
            ParserFail(cc,"Expected catch.");
        }
    }
    return NULL;
}
AST *ParseFor(CCmpCtrl *cc,Bool instant_eval=FALSE) {
    AST *init=NULL,*cond=NULL,*next=NULL,*body=NULL,*ret,*posit;
    I64 old;
    if(cc->token!=TK_IDENT) return NULL;
    if(StrCmp(cc->cur_str,"for")) return NULL;
    posit=LSP(cc);
    Lex(cc);
    if(cc->token!='(') ParserFail(cc,"Expected '('.");
    Lex(cc);
    init=ParseExpr(cc);
    if(cc->token!=';') ParserFail(cc,"Expected ';'.");
    Lex(cc);
    cond=ParseExpr(cc);
    if(cc->token!=';') ParserFail(cc,"Expected ';'.");
    Lex(cc);
    next=ParseExpr(cc);
    if(cc->token!=')') ParserFail(cc,"Expected ')'.");
    Lex(cc);
    old=CompEnterLoop;
    body=ParseStmt(cc,instant_eval);
    CompLeaveLoop(old);
    ret=MAlloc(sizeof(*ret));
    ret->type=AST_FOR;
    ret->for_stmt.init=init;
    ret->for_stmt.cond=cond;
    ret->for_stmt.inc=next;
    ret->for_stmt.body=body;
    ret->fn=StrNew(posit->fn);
    ret->ln=posit->ln;
    FreeAST(posit);
    AssignTypeToNode(ret);
    return ret;
}
CFunction *CompilerEmitCode();
AST *ParseWhile(CCmpCtrl *cc,Bool instant_eval=FALSE) {
    AST *cond=NULL,*body=NULL,*ret,*posit;
    I64 old;
    if(cc->token!=TK_IDENT) return NULL;
    if(StrCmp(cc->cur_str,"while")) return NULL;
    posit=LSP(cc);
    Lex(cc);
    if(cc->token!='(') ParserFail(cc,"Expected '('.");
    Lex(cc);
    cond=ParseExpr(cc);
    if(!cond) ParserFail(cc,"Expected condition.");
    if(cc->token!=')') ParserFail(cc,"Expected ')'.");
    Lex(cc);
    old=CompEnterLoop;
    body=ParseStmt(cc,instant_eval);
    CompLeaveLoop(old);
    if(!body) ParserFail(cc,"Expected body.");
    ret=MAlloc(sizeof(*ret));
    ret->type=AST_WHILE;
    ret->while_stmt.cond=cond;
    ret->while_stmt.body=body;
    ret->fn=StrNew(posit->fn);
    ret->ln=posit->ln;
    FreeAST(posit);
    AssignTypeToNode(ret);
    return ret;

}
AST *ParseSubswit(CCmpCtrl *cc) {
    AST *ret,*first=NULL,*tmp=NULL,*tmp2=NULL,*st_code=NULL,*body=NULL, *posit;
    Bool in_start_code=TRUE;
    if(cc->token!=TK_IDENT) return NULL;
    if(StrCmp(cc->cur_str,"start")) return NULL;
    posit=LSP(cc);
    Lex(cc);
    if(cc->token!=':') ParserFail(cc,"Expected ':'.");
    Lex(cc);
    loop:
    if(cc->token==TK_IDENT)
        if(!StrCmp(cc->cur_str,"end"))
            goto en;
    if(!(tmp2=ParseStmt(cc,,TRUE))) ParserFail(cc,"Expected statement.");
    if(in_start_code) {
        if(tmp2->type==AST_DFT||tmp2->type==AST_CASE) {
            st_code=first;
            tmp=first=NULL;
            in_start_code=FALSE;
        }
    }
    if(!first) first=tmp2;
    if(tmp) tmp->next=tmp2;
    tmp=tmp2;
    goto loop;
    en:
    Lex(cc);
    if(cc->token!=':') ParserFail(cc,"Expected ':'.");
    Lex(cc);
    body=first;
    ret=MAlloc(sizeof(*ret));
    ret->type=AST_SUBSWIT;
    ret->sub_swit.start_code=st_code;
    ret->sub_swit.body=first;
    ret->fn=StrNew(posit->fn);
    ret->ln=posit->ln;
    FreeAST(posit);
    AssignTypeToNode(ret);
    try {
        if(!Compiler.allow_subswit)
            ParserFail2(ret,"Subswit not allowed here.");
    } catch {
        CatchIf('Compiler','Lexer');
    }
    return ret;
}
AST *ParseDft(CCmpCtrl *cc) {
    if(cc->token!=TK_IDENT) return NULL;
    if(StrCmp(cc->cur_str,"default")) return NULL;
    AST *ret,*posit=LSP(cc);
    Lex(cc);
    if(cc->token!=':') ParserFail(cc,"Expected ':'.");
    Lex(cc);
    ret=MAlloc(sizeof(*ret));
    ret->type=AST_DFT;
    ret->fn=StrNew(posit->fn);
    ret->ln=posit->ln;
    FreeAST(posit);
    return ret;
}
AST *ParseCase(CCmpCtrl *cc) {
    AST *bottom=NULL,*top=NULL,*ret,*posit;
    if(cc->token!=TK_IDENT) return NULL;
    if(StrCmp(cc->cur_str,"case")) return NULL;
    posit=LSP(cc);
    ret=MAlloc(sizeof(*ret));
    ret->type=AST_CASE;
    Lex(cc);
    bottom=ParseExpr(cc);
    if(!bottom) goto check_colon;
    ret->cs.low=bottom;
    if(cc->token==TK_ELLIPSIS) {
        Lex(cc);
        top=ParseExpr(cc);
        if(!top) ParserFail(cc,"Expected expression.");
        ret->cs.high=top;
    }
    check_colon:
    if(cc->token!=':') ParserFail(cc,"Expected ':'.");
    Lex(cc);
    ret->fn=StrNew(posit->fn);
    ret->ln=posit->ln;
    FreeAST(posit);
    return ret;
}
AST *ParseDo(CCmpCtrl *cc,Bool instant_eval=FALSE) {
    AST *cond=NULL,*body=NULL,*ret,*posit;
    I64 old;
    if(cc->token!=TK_IDENT) return NULL;
    if(StrCmp(cc->cur_str,"do")) return NULL;
    posit=LSP(cc);
    Lex(cc);
    old=CompEnterLoop;
    body=ParseStmt(cc);
    CompLeaveLoop(old);
    if(cc->token!=TK_IDENT) ParserFail(cc,"Expected 'while'.");
    if(StrCmp(cc->cur_str,"while")) ParserFail(cc,"Expected 'while'.");
    Lex(cc);
    if(cc->token!='(') ParserFail(cc,"Expected '('.");
    Lex(cc);
    cond=ParseExpr(cc);
    if(!cond) ParserFail(cc,"Expected condition.");
    if(cc->token!=')') ParserFail(cc,"Expected ')'.");
    Lex(cc);
    if(cc->token!=';') ParserFail(cc,"Expected ';'.");
    if(!instant_eval)
        Lex(cc);
    ret=MAlloc(sizeof(*ret));
    ret->type=AST_DO;
    ret->do_stmt.cond=cond;
    ret->do_stmt.body=body;
    ret->fn=StrNew(posit->fn);
    ret->ln=posit->ln;
    FreeAST(posit);
    AssignTypeToNode(ret);
    return ret;
}
AST *ParseScope(CCmpCtrl *cc,Bool instant_eval=FALSE,Bool allow_cases=FALSE) {
    AST *first=NULL,*tmp=NULL,*tmp2=NULL;
    if(cc->token!='{') return NULL;
    Lex(cc);
    loop:
    if(cc->token=='}') {
        if(!first) {
            first=MAlloc(sizeof(*first));
            first->type=AST_NOP;
        }
        if(!instant_eval)
            Lex(cc);
        return first;
    }
    tmp2=ParseStmt(cc,,allow_cases);
    if(!tmp2) ParserFail(cc,"Expected '}'.");
    if(!first) first=tmp2;
    if(tmp) tmp->next=tmp2;
    for(;tmp2&&tmp2->next;tmp2=tmp2->next);
    tmp=tmp2;
    goto loop;
}
AST *ParseSwitch(CCmpCtrl *cc,Bool instant_eval=FALSE) {
    I64 old;
    AST *cond=NULL,*body=NULL,*ret,*posit;
    if(cc->token!=TK_IDENT) return NULL;
    if(StrCmp(cc->cur_str,"switch")) return NULL;
    posit=LSP(cc);
    Lex(cc);
    if(cc->token!='(') ParserFail(cc,"Expected '('.");
    Lex(cc);
    cond=ParseExpr(cc);
    if(!cond) ParserFail(cc,"Expected condition.");
    if(cc->token!=')') ParserFail(cc,"Expected ')'.");
    Lex(cc);
    old=CompEnterSwitch;
    body=ParseScope(cc,instant_eval,TRUE);
    CompLeaveSwitch(old);
    ret=MAlloc(sizeof(*ret));
    ret->type=AST_SWITCH;
    ret->switch_stmt.cond=cond;
    ret->switch_stmt.body=body;
    ret->fn=StrNew(posit->fn);
    ret->ln=posit->ln;
    FreeAST(posit);
    AssignTypeToNode(ret);
    return ret;
}
I64 Lastclass=0;
//Includes '()',use MSize
extern AST **ParseFuncArgs(CCmpCtrl *cc,Bool *has_vargs);
CTypeMember *ParseMembers(CCmpCtrl *cc,CTypeMember *inher=NULL) {
    if(cc->token!='{') return NULL;
    CTypeMember *first=NULL,*tmp,*cur=NULL;
    CType *btype=NULL,*type,*meta_type;
    AST *nt,*meta_exp,*cur_pos;
    CMemberMetaData *meta;
    for(;inher;inher=inher->next) {
        tmp=MAllocIdent(inher);
        if(cur) cur->next=tmp;
        cur=tmp;
        if(!first) first=cur;
    }
    loop:
    Lex(cc);
    if(cc->token=='}') {
        Lex(cc);
        return first;
    }
    if(!(btype=ParseType(NULL,cc,NULL,TRUE))) ParserFail(cc,"Expected member.");
    type=ParseType(btype,cc,&nt);
    tmp=MAlloc(sizeof(CTypeMember));
    tmp->type=type;
    if(nt)
        tmp->src_link=MStrPrint("%s,%d",nt->fn,nt->ln);
    if(nt)
        tmp->name=StrNew(nt->name),FreeAST(nt);
    if(!first) first=cur=tmp;
    else {
        cur->next=tmp;
        cur=tmp;
    }
    for(;cc->token!=';';) {
        if(cc->token==TK_IDENT) {
            meta=MAlloc(sizeof(CMemberMetaData));
            meta->name=StrNew(cc->cur_str);
            cur_pos=LSP(cc);
            Lex(cc);
            FreeAST(cur_pos);
            meta_exp=ParseExpr(cc),AssignTypeToNode(meta_exp);
            meta->value=EvalAST(meta_exp);
            meta->last=tmp->meta;
            if(tmp->meta)
                tmp->meta->next=meta;
            tmp->meta=meta;
        } else if(cc->token==',') {
            cur_pos=LSP(cc);
            Lex(cc);
            meta=NULL;
            if(!(type=ParseType(btype,cc,&nt))) ParserFail(cc,"Expected member.");
            if(!nt) ParserFail(cc,"Expected member name.");
            tmp=MAlloc(sizeof(CTypeMember));
            tmp->src_link=MStrPrint("%s,%d",cur_pos->fn,cur_pos->ln);
            FreeAST(cur_pos);
            tmp->type=type;
            tmp->name=StrNew(nt->name),FreeAST(nt);
            if(!first) first=cur=tmp;
            else {
                cur->next=tmp;
                cur=tmp;
            }
        } else {
            ParserFail(cc,"Expected member.");
        }
        next:;
    }
    goto loop;
}
Bool IsForwardType(CType *t) {
    if(t->type==HTT_UNION||t->type==HTT_CLASS)
        return t->is_fwd;
    return FALSE;
}
I64 GetUnique() {static I64 x;return x++;}
CType *ParseType(CType *_bt=NULL,CCmpCtrl *cc,AST **name,Bool base_type_only=FALSE) {
    if(name) *name=NULL;
    AST *dim,**args,*dummy_ast;
    I64 dimi,**dfts=NULL,*dft;
    CType *find,*bt,*ftype,*forward_t,**arg_types,*un_base=NULL,*inher=NULL;
    I64 star_cnt=0,cnt,idx,cur_off,max_align,max_size,align;
    U8 *tname=NULL,**arg_names,**aot_arg_names=NULL;
    CFifoI64 *fdims=NULL;
    CTypeMember *members=NULL,*members2,*fmem=NULL,*anon_mem,*tmp_mem;
    if(!_bt) {
        if(cc->token==TK_IDENT) {
            if(!StrCmp(cc->cur_str,"class")) {
                Lex(cc);
                if(cc->token==TK_IDENT) {
                    tname=StrNew(cc->cur_str);
                    Lex(cc);
                }
                if(cc->token==':') {
                    Lex(cc);
                    inher=ParseType(NULL,cc,NULL);
                } else
                    inher=NULL;
                if(!inher) {
                    max_align=1;
                    cur_off=max_size=0;
                } else {
                    max_align=inher->align;
                    cur_off=inher->sz;
                }
                if(tname) {
                    //In AOT mode,all references to a symbol point to symbol. In JIT mode,forward declarations shadow previous declarations
                    if(GetType(tname)) {
                        forward_t=GetType(tname);
                    } else {
                        forward_t=MAlloc(sizeof(CType)*(1+STAR_CNT));
                        forward_t->type=HTT_CLASS;
                        forward_t->is_fwd=TRUE;
                        forward_t->str=StrNew(tname);
                        for(idx=0;idx!=STAR_CNT;idx++) {
                            forward_t[idx+1]->base=_bt+idx;
                            forward_t[idx+1]->sz=8;
                            forward_t[idx+1]->align=8;
                            forward_t[idx+1]->type=HTT_TYPE;
                            forward_t[idx+1]->star_cnt=idx+1;
                            forward_t[idx+1]->str=MStrPrint("%s*",forward_t[idx].str);
                        }
                    }
                    _bt=forward_t;
                } else {
                    _bt=MAlloc(sizeof(CType)*(1+STAR_CNT));
                    _bt->type=HTT_CLASS;
                }
                dummy_ast=LSP(cc);
                if(tname) _bt->src_link=MStrPrint("%s,%d",dummy_ast->fn,dummy_ast->ln);
                FreeAST(dummy_ast);
                if(cc->token==';') {
                    //Is a forward declaration(leave semicolon to declaration)
                    HashAdd(_bt,Compiler.globals);
                    return NULL;
                }
                if(tname)
                forward_t=GetType(tname);
                else forward_t=NULL;
                if(Compiler.aot_mode&&forward_t&&!forward_t->is_fwd) ParserFail2(LSP(cc,NULL),"Redefinition of class %s.",forward_t->str);
                if(tname) HashAdd(_bt,Compiler.globals);
                forward_t=NULL;
                if(inher) {
                    _bt->inher=inher;
                    fmem=members=ParseMembers(cc,inher->first_member);
                    //Skip to first unique member
                    for(tmp_mem=inher->first_member;tmp_mem;tmp_mem=tmp_mem->next) {
                        members2=members;
                        members=members->next;
                    }
                } else
                    members=ParseMembers(cc),members2=NULL;
                for(;members;members=members->next) {
                    cnext:
                    if(!members) break;
                    align=members->type->align;
                    cur_off+=(align-(cur_off%align))%align;
                    if(!members->name) {
                        anon_mem=members->type->first_member;
                        for(;anon_mem;anon_mem=anon_mem->next) {
                            if(!fmem) fmem=anon_mem;
                            if(!members2) members2=anon_mem;
                            else members2->next=anon_mem;
                            members2=anon_mem;
                            members2->offset+=cur_off;
                            max_align=MaxI64(members2->type->align,max_align);
                        }
                        tmp_mem=members->next;
                        cur_off+=members->type->sz;
                        //Anonoymus type is now useless
                        Free(members);
                        members=tmp_mem;
                        goto cnext;
                    }
                    if(!fmem) fmem=members;
                    if(!members2) members2=members;
                    else members2->next=members;
                    members2=members;
                    members2->offset=cur_off;
                    max_align=MaxI64(members2->type->align,max_align);
                    cur_off+=members2->type->sz;
                }
                align=max_align;
                cur_off+=(align-(cur_off%align))%align;
                if(members2) members2->next=NULL;
                _bt->first_member=fmem;
                _bt->sz=cur_off;
                _bt->align=max_align;
                _bt->str=tname;
                _bt->inher=inher;
                _bt->is_fwd=FALSE;
                if(_bt->str)
                    HashAdd(_bt,Compiler.globals);
                else
                    _bt->str=MStrPrint("<Anon-class:%p>",_bt);
            } else if(!StrCmp(cc->cur_str,"union")) {
                un:
                Lex(cc);
                if(cc->token==TK_IDENT) {
                    tname=StrNew(cc->cur_str);
                    Lex(cc);
                }
                if(cc->token==':') {
                    Lex(cc);
                    inher=ParseType(NULL,cc,NULL);
                } else
                    inher=NULL;
                if(!inher) {
                    max_align=1;
                    cur_off=0;
                } else {
                    max_align=inher->align;
                    cur_off=inher->sz;
                }
                max_size=0;
                forward_t=NULL;
                if(tname) {
                    //In AOT mode,all references to a symbol point to symbol. In JIT mode,forward declarations shadow previous declarations
                    if(GetType(tname)) {
                        forward_t=GetType(tname);
                    } else {
                        forward_t=MAlloc(sizeof(CType)*(1+STAR_CNT));
                        forward_t->type=HTT_UNION;
                        forward_t->is_fwd=TRUE;
                        forward_t->str=StrNew(tname);
                        for(idx=0;idx!=STAR_CNT;idx++) {
                            forward_t[idx+1]->base=_bt+idx;
                            forward_t[idx+1]->sz=8;
                            forward_t[idx+1]->align=8;
                            forward_t[idx+1]->type=HTT_TYPE;
                            forward_t[idx+1]->star_cnt=idx+1;
                            forward_t[idx+1]->str=MStrPrint("%s*",forward_t[idx].str);
                        }
                    }
                    _bt=forward_t;
                } else {
                    _bt=MAlloc(sizeof(CType)*(1+STAR_CNT));
                    _bt->type=HTT_CLASS;
                }
                dummy_ast=LSP(cc);
                if(tname) _bt->src_link=MStrPrint("%s,%d",dummy_ast->fn,dummy_ast->ln);
                FreeAST(dummy_ast);
                if(cc->token==';') {
                    //Is a forward declaration(leave semicolon to declaration)
                    HashAdd(_bt,Compiler.globals);
                    return NULL;
                }
                if(tname)
                ftype=GetType(tname);
                else ftype=NULL;
                if(Compiler.aot_mode&&ftype&&!ftype->is_fwd) ParserFail2(LSP(cc,NULL),"Redefinition of union %s.",ftype->str);
                if(tname) HashAdd(_bt,Compiler.globals);
                if(inher) {
                    _bt->inher=inher;
                    fmem=members=ParseMembers(cc,inher->first_member);
                    //Skip to first unique member
                    for(tmp_mem=inher->first_member;tmp_mem;tmp_mem=tmp_mem->next) {
                        members2=members;
                        members=members->next;
                    }
                } else
                    members=ParseMembers(cc),members2=NULL;
                for(;members;members=members->next) {
                    unext:
                    if(!members) break;
                    if(!members->name) {
                        anon_mem=members->type->first_member;
                        for(;anon_mem;anon_mem=anon_mem->next) {
                            if(!fmem) fmem=anon_mem;
                            if(!members2) members2=anon_mem;
                            else members2->next=anon_mem;
                            members2=anon_mem;
                            anon_mem->offset+=cur_off;
                            max_size=MaxI64(members2->offset+members2->type->sz,max_size);
                            max_align=MaxI64(members2->type->align,max_align);
                        }
                        tmp_mem=members->next;
                        //Anonoymus type is now useless
                        Free(members);
                        members=tmp_mem;
                        goto unext;
                    }
                    if(!fmem) fmem=members;
                    if(!members2) members2=members;
                    else members2->next=members;
                    members2=members;
                    members->offset=cur_off;
                    max_size=MaxI64(members2->type->sz,max_size);
                    max_align=MaxI64(members2->type->align,max_align);
                }
                if(members2) members2->next=NULL;
                _bt->first_member=fmem;
                _bt->sz=max_size+(max_size%max_align);
                _bt->align=max_align;
                if(un_base)
                    _bt->sz=un_base->sz,_bt->align=un_base->align;
                _bt->str=tname;
                _bt->base=un_base;
                _bt->is_fwd=FALSE;
                _bt->inher=inher;
                if(_bt->str)
                    HashAdd(_bt,Compiler.globals);
                else
                    _bt->str=MStrPrint("<Anon-union:%p>",_bt);
            }
        }
        if(_bt) {
            if(!forward_t)
                for(idx=0;idx!=STAR_CNT;idx++) {
                    _bt[idx+1]->base=_bt+idx;
                    _bt[idx+1]->sz=8;
                    _bt[idx+1]->align=8;
                    _bt[idx+1]->type=HTT_TYPE;
                    _bt[idx+1]->star_cnt=idx+1;
                    _bt[idx+1]->str=MStrPrint("%s*",_bt[idx].str);
                }
            bt=_bt;
            goto loop1;
        }
        if(cc->token==TK_IDENT&&(find=HashFind(cc->cur_str,Compiler.globals))    );
        else return NULL;
        if(!IsHTTType(find->type)) return NULL;
        Lex(cc);
        bt=find;
    } else {
        bt=_bt;
    }
    if(cc->token==TK_IDENT) {
        if(!StrCmp(cc->cur_str,"union")) {
            if(IsForwardType(bt))
                ParserFail(cc,"Union base-type is a forward declaration!");
            un_base=bt;
            goto un;
        }
    }
    if(base_type_only) return bt;
    loop1:
    if(cc->token=='*') {
        if(++star_cnt>=STAR_CNT) ParserFail(cc,"Only %d stars allowed for pointers.",STAR_CNT);
        Lex(cc);
        bt++;
        goto loop1;
    }
    star_cnt=0;
    if(name&&cc->token==TK_IDENT) {
        *name=MAlloc(sizeof(AST));
        name[0]->type=AST_NAME;
        name[0]->name=StrNew(cc->cur_str);
        LSP(cc,*name);
        Lex(cc);
    }
    loop2:
    if(cc->token=='[') {
        Lex(cc);
        if(cc->token==']') {
            Lex(cc);
            tname=MStrPrint("%s[?]",bt->str);
            _bt=MAlloc(sizeof(*bt)*(1+STAR_CNT));
            _bt->type=HTT_ARRAY_TYPE;
            _bt->dim=0;
            _bt->ambig_dim=TRUE;
            _bt->base=bt;
            _bt->align=_bt->sz=8;
            _bt->str=StrNew(tname);
            bt=_bt;
            for(idx=0;idx!=STAR_CNT;idx++) {
                _bt[idx+1]->base=_bt+idx;
                _bt[idx+1]->sz=8;
                _bt[idx+1]->align=8;
                _bt[idx+1]->type=HTT_TYPE;
                _bt[idx+1]->star_cnt=idx+1;
                _bt[idx+1]->str=MStrPrint("%s*",_bt[idx].str);
            }
            goto loop2;
        } else {
            dim=ParseExpr(cc);
            if(!dim) ParserFail(cc,"Expected expression.");
            if(cc->token!=']') ParserFail(cc,"Expected ']'.");
            Lex(cc);
            tname=MStrPrint("%s[%d]",bt->str,dimi=EvalAST(dim));
            _bt=MAlloc(sizeof(*bt)*(1+STAR_CNT));
            _bt->type=HTT_ARRAY_TYPE;
            _bt->dim=dimi;
            _bt->base=bt;
            _bt->str=StrNew(tname);
            _bt->align=bt->align;
            _bt->sz=bt->sz*dimi;
            bt=_bt;
            for(idx=0;idx!=STAR_CNT;idx++) {
                _bt[idx+1]->base=_bt+idx;
                _bt[idx+1]->sz=8;
                _bt[idx+1]->align=8;
                _bt[idx+1]->type=HTT_TYPE;
                _bt[idx+1]->star_cnt=idx+1;
                _bt[idx+1]->str=MStrPrint("%s*",_bt[idx].str);
            }
            Free(tname);
            goto loop2;
        }
    }
    if(cc->token=='(') {
        fdims=FifoI64New(0x10);
        if(name)
            if(*name)
                goto pargs;
        Lex(cc);
        star_cnt=0;
        loop3:
        if(cc->token=='*') {
            if(++star_cnt>=STAR_CNT) ParserFail(cc,"Only %d stars allowed for pointers.",STAR_CNT);
            Lex(cc);
            goto loop3;
        }
        if(name&&(!*name)&&cc->token==TK_IDENT) {
            *name=MAlloc(sizeof(AST));
            name[0]->type=AST_NAME;
            name[0]->name=StrNew(cc->cur_str);
            LSP(cc,*name);
            Lex(cc);
        }
        loop4:
        if(cc->token=='[') {
            Lex(cc);
            if(cc->token==']') {
                Lex(cc);
                FifoI64Ins(fdims,0);
            } else {
                dim=ParseExpr(cc);
                if(!dim) ParserFail(cc,"Expected expression.");
                if(cc->token!=']') ParserFail(cc,"Expected ']'.");
                Lex(cc);
                FifoI64Ins(fdims,EvalAST(dim));
                goto loop4;
            }
        }
        if(cc->token==TK_IDENT) {
            if(name) {
                *name=MAlloc(sizeof(AST));
                name[0]->type=AST_NAME;
                name[0]->name=StrNew(cc->cur_str);
                LSP(cc,name[0]);
            }
            Lex(cc);
        }
        if(cc->token!=')') ParserFail(cc,"Expected ')'");
        Lex(cc);
        pargs:
        Bool has_vargs=FALSE;
        args=ParseFuncArgs(cc,&has_vargs);
        cnt=MSize(args)/sizeof(AST*);
        dfts=MAlloc(sizeof(I64*)*cnt);
        arg_types=MAlloc(sizeof(CType*)*cnt);
        arg_names=MAlloc(sizeof(U8*)*cnt);
        if(Compiler.aot_mode)
            aot_arg_names=MAlloc(sizeof(U8*)*cnt);
        tname=Cat(StrNew(bt->str),StrNew("("));
        for(idx=0;idx!=cnt;idx++) {
            if(args[idx]->decl.dft) {
                if(args[idx]->decl.dft->type==AST_LASTCLASS) {
                    dft=&Lastclass;
                    dfts[idx]=dft;
                    tname=Cat(tname,StrNew(args[idx]->decl.type->str),StrNew("=lastclass,"));
                    goto narg;
                } else
                    dft=MAlloc(sizeof(I64));
                if(IsF64(args[idx]->decl.type)) {
                    if(!Compiler.error_count)
                        *dft=EvalASTF64(args[idx]->decl.dft)(I64);
                    tname=Cat(tname,StrNew(args[idx]->decl.type->str),MStrPrint("=%n,",*dft));
                } else {
                    if(!Compiler.error_count)
                        *dft=EvalAST(args[idx]->decl.dft);
                    tname=Cat(tname,StrNew(args[idx]->decl.type->str),MStrPrint("=%d,",*dft));
                }
                dfts[idx]=dft;
            } else
                tname=Cat(tname,StrNew(args[idx]->decl.type->str),StrNew(","));
            narg:
            arg_types[idx]=args[idx]->decl.type;
            if(args[idx]->decl.name)
                arg_names[idx]=StrNew(args[idx].decl.name->name);
            if(Compiler.aot_mode&&args[idx].decl.dft) {
                aot_arg_names[idx]=MStrPrint(AOT_FUNC_DFT_ARG_FMT,GetUnique);
                GlobalCompileASTTo(aot_arg_names[idx],args[idx].decl.dft);
            }
            FreeAST(args[idx]);
        }
        Free(args);
        if(has_vargs) {
            tname=Cat(tname,StrNew("...)"));
        } else
            tname=Cat(tname,StrNew(")"));
        ftype=MAlloc(sizeof(CType)*(STAR_CNT+1));
        ftype->type=HTT_FUNC_TYPE;
        ftype->ret_type=bt;
        ftype->sz=0;
        ftype->align=1;
        ftype->arg_types=arg_types;
        ftype->arg_values=dfts;
        ftype->arg_names=arg_names;
        ftype->str=tname;
        ftype->has_vargs=has_vargs;
        ftype->aot_arg_names=aot_arg_names;
        cnt=STAR_CNT;
        for(idx=0;idx!=cnt;idx++) {
            ftype[idx+1]->base=&ftype[idx];
            ftype[idx+1]->star_cnt=idx+1;
            ftype[idx+1]->type=HTT_TYPE;
            ftype[idx+1]->str=MStrPrint("%s*",ftype[idx]->str);
            ftype[idx+1]->sz=ftype[idx+1]->align=8;
        }
        //Prevent from getting sweep away by gc
        HashAdd(ftype,Compiler.globals);
        bt=ftype+star_cnt;
        while(FifoI64Cnt(fdims)) {
            FifoI64Rem(fdims,&dimi);
            ftype=MAlloc(sizeof(CType)*(1+STAR_CNT));
            ftype->type=HTT_ARRAY_TYPE;
            ftype->base=bt;
            ftype->dim=dimi;
            if(dimi!=0) {
                ftype->str=MStrPrint("%s[%d]",bt->str,dimi);
                ftype->sz=ftype->align=bt->sz*dimi;
                ftype->align=bt->align;
            } else {
                ftype->str=MStrPrint("%s[?]",bt->str,0);
                ftype->sz=ftype->align=8;
                ftype->ambig_dim=TRUE;
            }
            _bt=bt=ftype;
            for(idx=0;idx!=STAR_CNT;idx++) {
                _bt[idx+1]->base=_bt+idx;
                _bt[idx+1]->sz=8;
                _bt[idx+1]->align=8;
                _bt[idx+1]->type=HTT_TYPE;
                _bt[idx+1]->star_cnt=idx+1;
                _bt[idx+1]->str=MStrPrint("%s*",_bt[idx].str);
            }
        }
        FifoI64Del(fdims);
    }
    if(IsForwardType(bt))
        ParserFail(cc,"Type is a forward declaration!");
    return bt;
}
AST *ParseArrayLiteral(CCmpCtrl *cc) {
    AST *lit;
    AST *tmp,*first,*chain=NULL,**ret,*pos=LSP(cc);
    I64 idx=0;
    if(cc->token!='{') return NULL;

    loop:
    Lex(cc);
    if(tmp=ParseExprNoComma(cc)) goto found;
    else if(tmp=ParseArrayLiteral(cc)) goto found;
    goto en;
    found:
    idx++;
    if(!chain) first=chain=tmp;
    else {
        chain->next=tmp;
        chain=tmp;
    }
    if(cc->token=='}') {
        goto en;
    } if(cc->token==',') {
        goto loop;
    }
    en:
    if(cc->token!='}') ParserFail(cc,"Expected '}'.");
    Lex(cc);
    ret=MAlloc(sizeof(AST*)*idx),idx=0;
    for(tmp=first;tmp;tmp=tmp->next) {
        ret[idx++]=tmp;
    }
    while(--idx>=0) ret[idx]->next=NULL;
    lit=MAlloc(sizeof(AST));
    lit->type=AST_ARRAY_LITERAL;
    lit->ln=pos->ln;
    lit->fn=StrNew(pos->fn);
    FreeAST(pos);
    lit->arr_lit=ret;
    return lit;
}
CLinkage *ParseLinkage(CCmpCtrl *cc) {
    if(cc->token!=TK_IDENT) return NULL;
    CLinkage *ret=MAlloc(sizeof(CLinkage));
    if(!StrCmp(cc->cur_str,"public")) {
        Lex(cc);
        ret->type|=LINK_PUBLIC;
    }
    if(!StrCmp(cc->cur_str,"static")) {
        ret->type=LINK_STATIC;
        Lex(cc);
    } else if(!StrCmp(cc->cur_str,"extern")) {
        ret->type=LINK_EXTERN;
        Lex(cc);
    } else if(!StrCmp(cc->cur_str,"import")) {
        ret->type=LINK_IMPORT;
        Lex(cc);
    } else if(!StrCmp(cc->cur_str,"_extern")) {
        ret->type=LINK__EXTERN;
        Lex(cc);
        if(cc->token!=TK_IDENT) ParserFail(cc,"Expected identifer.");
        StrCpy(ret->_link,cc->cur_str);
        Lex(cc);
    } else if(!StrCmp(cc->cur_str,"_import")) {
        ret->type=LINK__IMPORT;
        Lex(cc);
        if(cc->token!=TK_IDENT) ParserFail(cc,"Expected identifer.");
        StrCpy(ret->_link,cc->cur_str);
        Lex(cc);
    }
    return ret;
}
U0 *ResolveLinkage(AST *at,CLinkage *link,U8 *name=NULL) {
    CVariable *var=NULL;
    CFunction *func=NULL;
    I64 t;
    try {
        loop:
        switch(link->type&~LINK_PUBLIC) {
            case LINK_EXTERN:
            if(func=var=HashFind(name,Compiler.globals)) {
                goto resolve;
            }
            return NULL;
            case LINK_IMPORT:
            if(func=var=HashFind(name,Compiler.globals)) {
                goto resolve;
            }
            ParserFail2(at,"Unable to resolve symbol \"%s\".",name);
            case LINK__IMPORT:
            if(func=var=HashFind(link->_link,Compiler.globals)) {
                goto resolve;
            }
            ParserFail2(at,"Unable to resolve symbol \"%s\".",link->_link);
            case LINK__EXTERN:
            if(func=var=HashFind(link->_link,Compiler.globals)) {
                goto resolve;
            }
            return NULL;
            default:
            return NULL;
        }
        resolve:
        if(var->type==HTT_VAR) {
            t=var->link.type&~LINK_PUBLIC;
            if(t==LINK__EXTERN||t==LINK__IMPORT)
                goto resolve;
            return var->global_ptr;
        } else if(var->type==HTT_FUNC) {
            t=func->link.type&~LINK_PUBLIC;
            if(t==LINK__EXTERN||t==LINK__IMPORT)
                goto resolve;
            return func->func_ptr;
        }
    goto loop;
    } catch {
        CatchIf('Parser','Lexer');
    }
    return NULL;
}
CFunction *CompileFunction(AST *at,CCmpCtrl *cc,CType *type,U8 *name,Bool instant_eval=FALSE);
AST *ParseDecls(CCmpCtrl *cc,Bool instant_eval=FALSE) {
    CLinkage *link=ParseLinkage(cc);
    if(cc->token!=TK_IDENT) return NULL;
    CType *bt,*t;
    AST *name=NULL,*decl=NULL,*first=NULL,*last=NULL,*fbody,*cur_pos;
    CVariable *var,*var_resolve;
    CFunction *func,*func_resolve;
    if(bt=ParseType(,cc,NULL,TRUE));
    else return NULL;
    loop:
    if(t=ParseType(bt,cc,&name)) {
        if(!name) {
            if(!first) {
                first=MAlloc(sizeof(AST));
                first->type=AST_NOP;
            }
            goto en;
        }
        fbody=NULL;
        if(t->type==HTT_FUNC_TYPE&&!first) {
            if(Compiler.in_func)
                ParserFail(cc,"No nested functions.");
            if(cc->token=='{') {
                CompileFunction(name,cc,t,name->name,instant_eval);
                first=MAlloc(sizeof(AST));
                first->type=AST_NOP;
                return first;
            }
        }
        if(link&&t->type==HTT_FUNC_TYPE) {
            if(Compiler.in_func)
                ParserFail(cc,"No nested functions.");
            func=MAlloc(sizeof(CFunction));
            cur_pos=LSP(cc);
            func->src_link=MStrPrint("%s,%d",cur_pos->fn,cur_pos->ln);
            FreeAST(cur_pos);
            func->link=*link;
            func->str=StrNew(name->name);
            func->hc_type=t;
            func->type=HTT_FUNC;
            if((link->type&~LINK_PUBLIC)==0||(link->type&~LINK_PUBLIC)==LINK_STATIC)
                link->type=LINK_EXTERN;
            f_resolve:
            //In AOT Mode,there is one version of each symbol. In JIT Mode,forward declarations void previous versions of the symbol
            if(Compiler.aot_mode)
                func->func_ptr=ResolveLinkage(name,link,name->name);
            else
                ;
            goto func_fwd_end;
        } else if(t->type==HTT_FUNC_TYPE) {
            if(Compiler.in_func)
                ParserFail(cc,"No nested functions.");
            func=MAlloc(sizeof(CFunction));
            cur_pos=LSP(cc);
            func->src_link=MStrPrint("%s,%d",cur_pos->fn,cur_pos->ln);
            FreeAST(cur_pos);
            func->str=StrNew(name->name);
            func->hc_type=t;
            func->type=HTT_FUNC;
            func_resolve=func;
            func_fwd_end:
            HashAdd(func,Compiler.globals);
            if(cc->token!=';') ParserFail(cc,"Expected ';'");
            if(!instant_eval)
                Lex(cc);
            first=MAlloc(sizeof(AST));
            first->type=AST_NOP;
            return first;
        }
        decl=MAlloc(sizeof(*decl));
        decl->type=AST_DECL;
        decl->decl.name=name;
        decl->decl.type=t;
        decl->fn=StrNew(name->fn);
        decl->ln=name->ln;
        decl->decl.fbody=fbody;
        if(link) decl->decl.link=*link;
        if(cc->token=='=') {
            Lex(cc);
            if(decl->decl.dft=ParseExprNoComma(cc));
            else if(decl->decl.dft=ParseArrayLiteral(cc)) {
                //Assume array dims
                AssignTypeToNode(decl.decl->dft,t);
            } else ParserFail(cc,"Expected expression.");
        }
        if(!first) first=decl;
        if(last) last->next=decl;
        last=decl;
        if(cc->token==',') {
            Lex(cc);
            goto loop;
        }
    }
    if(!first) ParserFail(cc,"Expected declaration.");
    if(cc->token!=';') ParserFail(cc,"Expected ';'.");
    if(!instant_eval)
        Lex(cc);
    for(decl=first;decl;decl=decl->next) {
        AssignTypeToNode(decl);
        var=MAlloc(sizeof(*var));
        var->src_link=MStrPrint("%s,%d",decl->fn,decl->ln);
        var->type=HTT_VAR;
        var->str=StrNew(decl->decl.name->name);
        var->is_local=Compiler.in_func;
        var->hc_type=decl->decl.type;
        if(link) var->link=*link;
        try {
            if(Compiler.in_func) {
                if(IsInteger(var->hc_type)||IsF64(var->hc_type))
                    if(var->hc_type->type!=HTT_ARRAY_TYPE)
                        var->isReg=TRUE;
                if(HashFind(var->str,Compiler.locals))
                        ParserFail2(decl,"Redefinition of %s.",decl->decl.name->name);
                if(link&&(link->type&~LINK_PUBLIC)==LINK_STATIC) {
                    var->is_static=TRUE;
                    var->static_refs=VectorNew();
                    var->isReg=FALSE;
                }
                HashAdd(var,Compiler.locals);
            } else {
                if(link) {
                    if(LINK_STATIC==(link->type&~LINK_PUBLIC)||!(link->type&~LINK_PUBLIC)) goto alloc;
                    var->global_ptr=ResolveLinkage(decl,link,decl->decl.name.name);
                } else {
                    alloc:
                        if(Compiler.aot_mode&&HashFind(var->str,Compiler.globals))
                            if(!IsForward(HashFind(var->str,Compiler.globals)))
                                ParserFail2(decl->decl.name,"Redefinition of \"%s\".",var->str);
                    var->global_ptr=MAlloc(var->hc_type->sz);
                }
                if(Compiler.aot_mode&&HashFind(var->str,Compiler.globals)&&!IsForward(HashFind(var->str,Compiler.globals)))
                    ParserFail2(decl,"Redefinition of symbol %s.",var->str);
                HashAdd(var,Compiler.globals);
            }
            next:
        } catch {
            CatchIf('Parser','Lexer');
        }
    }
    en:
    if(link) Free(link);
    return first;
}
AST **ParseFuncArgs(CCmpCtrl *cc,Bool *has_vargs) {
    I64 idx=0;
    AST *name,*decls[0xff],*dft;
    CType *type;
    if(cc->token!='(') ParserFail(cc,"Expected '('.");
    Lex(cc);
    goto first;
    loop:
    if(cc->token==',')
        Lex(cc);
    else goto last;
    first:
    type=ParseType(,cc,&name);
    if(!type) {
        if(cc->token==TK_ELLIPSIS) {
            Lex(cc);
            if(has_vargs) *has_vargs=TRUE;
        }
        last:
        if(cc->token!=')') ParserFail(cc,"Expected ')'.");
        Lex(cc);
    } else {
        decls[idx]=MAlloc(sizeof(AST));
        if(cc->token=='=') {
            Lex(cc);
            if(cc->token==TK_IDENT)
                if(!StrCmp("lastclass",cc->cur_str)) {
                    if(idx==0) ParserFail(cc,"Lastclass cannot be first default argument");
                    Lex(cc);
                    dft=MAlloc(sizeof(AST));
                    dft->type=AST_LASTCLASS;
                    goto add_dft;
                }
            dft=ParseExprNoComma(cc);
        } else
            dft=NULL;
        add_dft:
        decls[idx]->type=AST_DECL;
        decls[idx]->decl.name=name;
        decls[idx]->decl.dft=dft;
        if(name)     {
            decls[idx]->fn=StrNew(name->fn);
            decls[idx]->ln=name->ln;
        }
        decls[idx++]->decl.type=type;
        goto loop;
    }
    return MemNCpy(MAlloc(idx*sizeof(AST*)),decls,idx*sizeof(AST*));
}
AST *ParseAsmBlock(CCmpCtrl *cc);
AST *ParseStmt(CCmpCtrl *cc,Bool instant_eval=FALSE,Bool allow_cases=FALSE) {
    AST *ret;
    CHash *find;
    CLabel *label;
    if(ret=ParseAsmBlock(cc)) return ret;
    if(ret=ParseTry(cc,instant_eval)) return ret;
    if(ret=ParseBreak(cc)) return ret;
    if(ret=ParsePrint(cc,instant_eval)) return ret;
    if(ret=ParseReturn(cc,instant_eval)) return ret;
    if(ret=ParseDecls(cc,instant_eval)) return ret;
    if(ret=ParseDo(cc,instant_eval)) return ret;
    if(ret=ParseFor(cc,instant_eval)) return ret;
    if(ret=ParseWhile(cc,instant_eval)) return ret;
    if(ret=ParseSwitch(cc,instant_eval)) return ret;
    if(ret=ParseSubswit(cc)) return ret;
    if(ret=ParseCase(cc)) {
        try {
            if(!allow_cases)
                ParserFail2(ret,"Cases not allowed here.");
        } catch{
            CatchIf('Parser');
        }
        return ret;
    }
    if(ret=ParseDft(cc)) {
        try {
            if(!allow_cases)
                ParserFail2(ret,"Defaults not allowed here.");
        } catch{
            CatchIf('Parser');
        }
        return ret;
    }
    if(ret=ParseIf(cc,instant_eval)) return ret;
    if(ret=ParseScope(cc,instant_eval)) return ret;
    if(ret=ParseGoto(cc)) return ret;
    if(ret=ParseExpr(cc)) {
        if(ret->type==AST_NAME)
            if(cc->token==':') {
                if(!Compiler.in_func) ParserFail(cc,"No global labels.");
                find=HashFind(ret->name,Compiler.labels);
                if(find&&find->type==HTT_LABEL)
                    ParserFail(cc,"Repeat label '%s'.",ret->name);
                ret->type=AST_LABEL;
                label=MAlloc(sizeof(*label));
                label->type=HTT_LABEL;
                label->str=StrNew(ret->name);
                label->refs=VectorNew;
                HashAdd(label,Compiler.labels);
                Lex(cc);
                return ret;
            }
        if(cc->token!=';') ParserFail(cc,"Expected ';'.");
        if(!instant_eval)
            Lex(cc);
        return ret;
    }
    if(cc->token==';') {
        ret=MAlloc(sizeof(AST));
        ret->type=AST_NOP;
        if(!instant_eval)
            Lex(cc);
        return ret;
    }
    return NULL;
}
U0 PushIRReg(CType *type,I64 reg) {
    CIRValue *new=MAlloc(sizeof(CIRValue));
    new->type=IRV_REG;
    new->reg=reg;
    new->last=Compiler.stack;
    Compiler.stack=new;
    new->type2=type;
}
U0 PushIRFReg(CType *type,I64 reg) {
    CIRValue *new=MAlloc(sizeof(CIRValue));
    new->type=IRV_FREG;
    new->reg=reg;
    new->last=Compiler.stack;
    Compiler.stack=new;
    new->type2=type;
}
U0 PushIRInt(I64 i) {
    CIRValue *new=MAlloc(sizeof(CIRValue));
    new->type=IRV_INT;
    new->int=i;
    new->last=Compiler.stack;
    new->type2=HashFind("I64i",Compiler.globals);
    Compiler.stack=new;
}
U0 PushIRFlt(F64 f) {
    CIRValue *new=MAlloc(sizeof(CIRValue));
    new->type=IRV_FLOAT;
    new->flt=f;
    new->last=Compiler.stack;
    new->type2=HashFind("F64",Compiler.globals);
    Compiler.stack=new;
}
U0 PushIRIndirX(CType *type,I64 reg,I64 off) {
    CIRValue *new=MAlloc(sizeof(CIRValue));
    new->type=IRV_INDIRX;
    new->indirx.reg=reg;
    new->indirx.offset=off;
    new->indirx.type=type;
    new->last=Compiler.stack;
    new->type2=type;
    Compiler.stack=new;
}
U0 PopIRValue(CIRValue **top=NULL) {
    CIRValue *ret=Compiler.stack;
    if(!ret) {
        "NO VALUE ON STACK\n";
        throw('Compile');
    }
    Compiler.stack=ret->last;
    if(top) {
        *top=ret;
        ret->last=NULL;
    } else {
        Free(ret);
    }
}
I64 LoadSymPtrIntoReg(U8 *name,I64 reg) {
    CVariable *var=HashFind(name,Compiler.globals);
    CFunction *func=var;
    if(var&&var->type==HTT_VAR) {
        switch(var->link.type&~LINK_PUBLIC) {
            case LINK__EXTERN:
            case LINK__IMPORT:
            name=&var->link._link;
        }
    } else if(var&&var->type==HTT_FUNC) {
        switch(func->link.type&~LINK_PUBLIC) {
            case LINK__EXTERN:
            case LINK__IMPORT:
            name=&func->link._link;
        }
    }
    CRelocation *new=MAlloc(sizeof(CRelocation)),*last=Compiler.relocations;
    StrCpy(new->name,name);
    jit_relocation(Compiler.jit,jit_R(reg),&new->ptr);
    if(!last)
        Compiler.relocations=new;
    else {
        new->next=last;
        Compiler.relocations=new;
    }
    return jit_R(reg);
}
CType *GetType(U8 *name,I64 ptr_level=0) {
    CType *ret=HashFind(name,Compiler.globals);
    if(ret&&(ret->type==HTT_TYPE||ret->type==HTT_ARRAY_TYPE||ret->type==HTT_FUNC_TYPE||ret->type==HTT_CLASS||ret->type==HTT_UNION))
        return ret+ptr_level;
    return NULL;
}
I64 GetFreeIReg(I64 off=0) {
    I64 max=Compiler.first_avail_ireg;
    CIRValue *cur=Compiler.stack;
    while(cur) {
        if(cur->type==IRV_REG) {
            max=MaxI64(cur->reg,max);
        } else if(cur->type==IRV_INDIRX) {
            max=MaxI64(cur->indirx.reg,max);
        }
        cur=cur->last;
    }
    return max+1+off;
}
I64 GetFreeFReg(I64 off=0) {
    I64 max=Compiler.first_avail_freg;
    CIRValue *cur=Compiler.stack;
    while(cur) {
        if(cur->type==IRV_FREG) {
            max=MaxI64(cur->reg,max);
        }
        cur=cur->last;
    }
    return max+1+off;
}
CIRValue *MovePtrToReg(CIRValue *var,I64 r=-1) {
    CIRValue *ret;
    if(r==-1) r=GetFreeIReg;
    if(var->type!=IRV_INDIRX) {
        "VAIRALBLE INSNT INDIRECT\n";
        throw('Compile');
    }
    if(!var->indirx.offset) {
        jit_movr(Compiler.jit,jit_R(r),jit_R(var->indirx.reg));
    } else {
        jit_movr(Compiler.jit,jit_R(r),jit_R(var->indirx.reg));
        jit_addi(Compiler.jit,jit_R(r),jit_R(r),var->indirx.offset);
    }
    PushIRReg(var->type2,r);
    PopIRValue(&ret);
    return ret;
}
U0 CompileBoundsCheck(I64 r,I64 sz,I64 off=0);
/* 0 is b registeddddr
 * 1 is a destination register(if needed)
 * 2 is func ptr register
 */
U0 Assign(CIRValue *a,CIRValue *b) {
    if(Compiler.error_count) return;
    I64 br,ar,fr;
    //Load b into register(0) first,then load into a
    if(IsInteger(b->type2)||IsPtrType(b->type2)) {
        br=jit_R(0);
        switch(b->type) {
            case IRV_INT:
            jit_movi(Compiler.jit,jit_R(0),b->int);
            break;
            case IRV_FLOAT:
            jit_movi(Compiler.jit,jit_R(0),b->flt);
            break;
            case IRV_FREG:
            "GOT FREG\n";
            throw('Compile');
            break;
            case IRV_REG:
            br=jit_R(b->reg);
            break;
            case IRV_STR:
            //TODO register string
            jit_movi(Compiler.jit,jit_R(0),b->int);
            break;
            case IRV_INDIRX: //Comes with offsett
            if(IsSigned(b->type2)) {
                if(!b->indirx.offset) {
                    CompileBoundsCheck(b->indirx.reg,b->type2->sz);
                    jit_ldr(Compiler.jit,jit_R(0),jit_R(b->indirx.reg),b->type2->sz);
                } else {
                    CompileBoundsCheck(b->indirx.reg,b->type2->sz,b->indirx.offset);
                    jit_ldxi(Compiler.jit,jit_R(0),jit_R(b->indirx.reg),b->indirx.offset,b->type2->sz);
                }
            } else {
                if(!b->indirx.offset) {
                    CompileBoundsCheck(b->indirx.reg,b->type2->sz);
                    jit_ldr_u(Compiler.jit,jit_R(0),jit_R(b->indirx.reg),b->type2->sz);
                } else {
                    CompileBoundsCheck(b->indirx.reg,b->type2->sz,b->indirx.offset);
                    jit_ldxi_u(Compiler.jit,jit_R(0),jit_R(b->indirx.reg),b->indirx.offset,b->type2->sz);
                }
            }
            break;
        }
        switch(a->type) {
            case IRV_INT:
            "GOT IRV_INT\n";
            throw('Compile');
            break;
            case IRV_FLOAT:
            "GOT IRV_FLOAT\n";
            throw('Compile');
            break;
            case IRV_FREG:
            jit_extr(Compiler.jit,jit_FR(a->reg),br);
            break;
            case IRV_REG:
            jit_movr(Compiler.jit,jit_R(a->reg),br);
            break;
            case IRV_STR:
            "GOT IRV_STR\n";
            throw('Compile')    ;
            break;
            case IRV_INDIRX:
            if(!IsF64(a->type2)) {
                CompileBoundsCheck(a->indirx.reg,a->type2->sz,a->indirx.offset);
                    if(!a->indirx.offset) {
                        jit_str(Compiler.jit,jit_R(a->indirx.reg),br,a->type2->sz);
                    } else {
                        jit_stxi(Compiler.jit,a->indirx.offset,jit_R(a->indirx.reg),br,a->type2->sz);
                    }
            } else {
                jit_extr(Compiler.jit,jit_FR(0),br);
                br=jit_FR(0);
                CompileBoundsCheck(a->indirx.reg,8,a->indirx.offset);
                if(!a->indirx.offset) {
                    jit_fstr(Compiler.jit,jit_R(a->indirx.reg),br,8);
                } else {
                    jit_fstxi(Compiler.jit,a->indirx.offset,jit_R(a->indirx.reg),br,8);
                }
            }
            break;
        }
    } else if(IsF64(b->type2)) {
        br=jit_FR(0);
        switch(b->type) {
            case IRV_INT:
            jit_fmovi(Compiler.jit,jit_FR(0),b->int);
            break;
            case IRV_FLOAT:
            jit_fmovi(Compiler.jit,jit_FR(0),b->flt);
            break;
            case IRV_FREG:
            br=jit_FR(b->reg);
            break;
            case IRV_REG:
            jit_extr(Compiler.jit,jit_FR(0),jit_R(b->reg));
            break;
            case IRV_STR:
            //str->flt??? have fun
            //TODO register string
            jit_fmovi(Compiler.jit,jit_FR(0),b->int);
            break;
            case IRV_INDIRX: //Comes with offsett
            CompileBoundsCheck(b->indirx.reg,8,b->indirx.offset);
            if(!b->indirx.offset) {
                jit_fldr(Compiler.jit,jit_FR(0),jit_R(b->indirx.reg),8);
            } else {
                jit_fldxi(Compiler.jit,jit_FR(0),jit_R(b->indirx.reg),b->indirx.offset,8);
            }
            break;
        }
        switch(a->type) {
            case IRV_INT:
            "GOT IRV_INT\n";
            throw('Compile');
            break;
            case IRV_FLOAT:
            "GOT IRV_FLOAT\n";
            throw('Compile');
            break;
            case IRV_FREG:
            jit_fmovr(Compiler.jit,jit_FR(a->reg),br);
            break;
            case IRV_REG:
            jit_truncr(Compiler.jit,jit_R(a->reg),br);
            break;
            case IRV_STR:
            "GOT IRV_STR\n";
            throw('Compile');
            break;
            case IRV_INDIRX:
            if(IsF64(a->type2)) {
                if(!a->indirx.offset) {
                    jit_fstr(Compiler.jit,jit_R(a->indirx.reg),br,8);
                } else {
                    jit_fstxi(Compiler.jit,a->indirx.offset,jit_R(a->indirx.reg),br,8);
                }
            } else {
                jit_truncr(Compiler.jit,jit_R(0),br);
                br=jit_R(0);
                CompileBoundsCheck(a->indirx.reg,a->type2->sz,a->indirx.offset);
                if(!a->indirx.offset) {
                    jit_str(Compiler.jit,jit_R(a->indirx.reg),br,a->type2->sz);
                } else {
                    jit_stxi(Compiler.jit,a->indirx.offset,jit_R(a->indirx.reg),br,a->type2->sz);
                }
            }
            break;
        }
    } else {
        //Is an object so copy using MemCpy
        fr=LoadSymPtrIntoReg("MemCpy",2);
        br=jit_R(MovePtrToReg(b,0)->reg);
        ar=jit_R(MovePtrToReg(a,1)->reg);
        jit_prepare(Compiler.jit);
        jit_putargr(Compiler.jit,ar);
        jit_putargr(Compiler.jit,br);
        jit_putargi(Compiler.jit,b->type2->sz);
        jit_callr(Compiler.jit,fr);
        return;
    }
}
CIRValue *MoveToFReg(CIRValue *value_a,I64 off) {
    if(value_a->type==IRV_FREG)
        return value_a;
    CIRValue *value_b;
    PushIRFReg(GetType("F64"),GetFreeFReg+off);
    PopIRValue(&value_b);
    Assign(value_b,value_a);
    Free(value_a);
    return value_b;
}
CIRValue *MoveToReg(CIRValue *value_a,I64 off) {
    if(value_a->type==IRV_REG)
        return value_a;
    if(value_a->type==IRV_INDIRX&&value_a->type2->type==HTT_ARRAY_TYPE) {
        value_a->type=IRV_REG;
        value_a->reg=value_a->indirx.reg;
        return value_a;
    }
    CIRValue *value_b;
    if(!(IsInteger(value_a->type2)||IsPtrType(value_a->type2)))
        PushIRReg(value_a->type2,GetFreeIReg(off));
    else
        PushIRReg(value_a->type2,GetFreeIReg(off));
    PopIRValue(&value_b);
    Assign(value_b,value_a);
    Free(value_a);
    return value_b;
}
CIRValue *BoolifyValue(CIRValue *value) {
    CIRValue *ret;
    I64 r,fr;
    U0 *pat;
    PushIRReg(GetType("I64i"),r=GetFreeIReg);
    if(IsF64(value->type2)) {
        fr=GetFreeFReg;
        value=MoveToFReg(IRValueClone(value),0);
        jit_movi(Compiler.jit,jit_R(r),1);
        pat=jit_fbnei(Compiler.jit,NULL,jit_FR(value->reg),0.0);
        jit_movi(Compiler.jit,jit_R(r),0);
        jit_patch(Compiler.jit,pat);
    } else {
        value=MoveToReg(IRValueClone(value),0);
        jit_nei(Compiler.jit,jit_R(r),jit_R(value->reg),0);
    }
    Free(value);
    PopIRValue(&ret);
    return ret;
}
U0 CompileAST(AST *ast,Bool pass_by_ref=FALSE,Bool dont_call_lone_name=FALSE);
U0 CompileArrayLiteral(CType *type,I64 pointer_reg,AST *literal,I64 offset=0) {
    I64 cnt=MSize(literal->arr_lit)/sizeof(AST*),idx,o_off=offset;
    CTypeMember *mem;
    CType *d;
    if(IsStructType(type)) {
        mem=type->first_member;
        for(idx=0;mem&&idx!=cnt;idx++) {
            if(!literal->arr_lit[idx]) {
            } if(literal->arr_lit[idx]->type!=AST_ARRAY_LITERAL) {
                PushIRIndirX(mem->type,pointer_reg,offset+mem->offset);
                CompileAST(literal->arr_lit[idx]);
                Assign(Compiler.stack->last,Compiler.stack);
                PopIRValue,PopIRValue;
            } else if(literal->arr_lit[idx]->type==AST_ARRAY_LITERAL) {
                CompileArrayLiteral(mem->type,pointer_reg,literal->arr_lit[idx],offset+mem->offset);
            }
            mem=mem->next;
        }
    } else {
        d=DerefedType(type);
        for(idx=0;idx!=cnt;idx++) {
            if(!literal->arr_lit[idx]) {
            } else if(literal->arr_lit[idx]->type!=AST_ARRAY_LITERAL) {
                PushIRIndirX(d,pointer_reg,offset);
                CompileAST(literal->arr_lit[idx]);
                Assign(Compiler.stack->last,Compiler.stack);
                PopIRValue,PopIRValue;
            } else {
                    CompileArrayLiteral(d,pointer_reg,literal->arr_lit[idx],offset);
            }
                offset+=d->sz;
        }
    }
}
Bool FoldIRF64() {
    CIRValue *a;
    if(Compiler.stack->type==IRV_FLOAT)
        return TRUE;
    else if(Compiler.stack->type==IRV_INT) {
        PopIRValue(&a);
        PushIRFlt(a->int);
        Free(a);
        return TRUE;
    }
    return FALSE;
}
Bool FoldIRInt() {
    CIRValue *a;
    if(Compiler.stack->type==IRV_INT)
        return TRUE;
    else if(Compiler.stack->type==IRV_FLOAT) {
        PopIRValue(&a);
        PushIRInt(a->flt);
        Free(a);
        return TRUE;
    }
    return FALSE;
}
U0 PushIRString(U8 *data,I64 len) {
    U8 *str=HexifyString(data,len);
    CCompString *comp_str;
    CCompStringRef *ref;
    U0 *op1;
    I64 idx;
    if(!Compiler.aot_mode) {
        reg_str:
        if(comp_str=HashFind(str,Compiler.cur_strings)) {
            jit_movi(Compiler.jit,jit_R(idx=GetFreeIReg),comp_str->data);
            PushIRReg(GetType("U8i")+1,idx);
        } else {
            comp_str=MAlloc(sizeof(CCompString));
            comp_str->data=MemCpy(MAlloc(len),data,len);
            comp_str->str=StrNew(str);
            HashAdd(comp_str,Compiler.cur_strings);
            goto reg_str;
        }
    } else {
        areg_str:
        if(comp_str=HashFind(str,Compiler.cur_strings)) {
            ref=MAlloc(sizeof(CCompStringRef));
            ref->next=comp_str->refs;
            comp_str->refs=ref;
            jit_relocation(Compiler.jit,jit_R(idx=GetFreeIReg),&ref->ptr);
            PushIRReg(GetType("U8i")+1,idx);
        } else {
            comp_str=MAlloc(sizeof(CCompString));
            comp_str->data=MemCpy(MAlloc(len),data,len);
            comp_str->str=StrNew(str);
            HashAdd(comp_str,Compiler.cur_strings);
            goto areg_str;
        }
    }
    Free(str);
}
U0 CompileAstWithBreakpoint(AST *oc);
//Will add a patcher to the current function
U0 CompileAsmConst(AST *mode,I64 width,Bool rel=FALSE,I64 label_scope=-2);
U0 CompileOpcode(AST *oc) {
    U8 buffer[32],*end_ptr;
    CReg *mbase;
    Bool is_jmp;
    I64 idx,siboff,immoff,immsz,operand_cnt=MSize(oc->opcode->args)/sizeof(AST*);
    retry:
    try {
        switch(operand_cnt) {
            case 0:
            end_ptr=EncodeOpcode(buffer,oc->opcode.opcode->str,&is_jmp,&siboff,&immoff,&immsz);
            break;
            case 1:
            end_ptr=EncodeOpcode(buffer,oc->opcode.opcode->str,&is_jmp,&siboff,&immoff,&immsz,oc->opcode->args[0]->x64_mode);
            break;
            case 2:
            end_ptr=EncodeOpcode(buffer,oc->opcode.opcode->str,&is_jmp,&siboff,&immoff,&immsz,oc->opcode->args[0]->x64_mode,oc->opcode->args[1]->x64_mode);
            break;
            case 3:
            end_ptr=EncodeOpcode(buffer,oc->opcode.opcode->str,&is_jmp,&siboff,&immoff,&immsz,oc->opcode->args[0]->x64_mode,oc->opcode->args[1]->x64_mode,oc->opcode->args[2]->x64_mode);
            break;
            default:
        }
    } catch {
        CatchIf('Asm');
    }
    if(!end_ptr) {
        //Try to shift down size of immedate to fit in smaller size
        for(idx=0;idx!=operand_cnt;idx++)
                if(oc->opcode.args[idx]->x64_mode.type==X64O_IMM)
                    if((oc->opcode.args[idx]->x64_mode.imm>>8)!=0) {
                        oc->opcode.args[idx]->x64_mode.imm>>=8;
                        goto retry;
                    }
        throw('Compile');
    }
    if(is_jmp)  {
        if(operand_cnt!=1) throw('Compile');
        jit_data_bytes(Compiler.jit,end_ptr-buffer,buffer);
        CompileAsmConst(oc->opcode->args[0]->x64_mode_offset,4,TRUE,oc->label_scope);
    } else if(immoff!=-1) {
        jit_data_bytes(Compiler.jit,immoff,buffer);
        for(idx=0;idx!=operand_cnt;idx++) {
            if(oc->opcode.args[idx]->x64_mode.type==X64O_IMM) {
                CompileAsmConst(oc->opcode.args[idx]->x64_mode_offset,immsz,FALSE,oc->label_scope); //See the plato-pus!! Seriously,its above someewhere
            }
        }
    } else if(siboff!=-1) {
        jit_data_bytes(Compiler.jit,end_ptr-buffer-4,buffer);
        for(idx=0;idx!=operand_cnt;idx++) {
            if(oc->opcode.args[idx]->x64_mode.type==X64O_MEM) {
                mbase=oc->opcode.args[idx]->x64_mode.mem.base;
                if(mbase&&(mbase==HashFind("RIP",regs)||mbase==HashFind("EIP",regs)))
                    CompileAsmConst(oc->opcode.args[idx]->x64_mode_offset,4,TRUE,oc->label_scope); //See the plato-pus!! Seriously,its above someewhere
                else
                    CompileAsmConst(oc->opcode.args[idx]->x64_mode_offset,4,FALSE,oc->label_scope); //See the plato-pus!! Seriously,its above someewhere
            }
        }
    } else {
        jit_data_bytes(Compiler.jit,end_ptr-buffer,buffer);
    }
}
U0 __CompileAST(AST *ast,Bool pass_by_ref=FALSE,Bool dont_call_lone_name=FALSE) {
    CIRValue *fargs[0xff];
    CHashTable *tar_tab;
    CVariable *var;
    CIRValue *value_a,*value_b,*value_c;
    I64 idx,cnt,reg_a,reg_b,offset,mult,hi,lo,idx2;
    CTypeMember *mem1;
    CType *type1,*type2;
    AST **table;
    CBreakLst *breaks;
    CSwitchTable *swit_table;
    CLabel *lab;
    CHash *imp;
    CFunction *func;
    CVector *to_patch;
    AST *dummy;
    U0 *op1,*op2,*op3,*op4;
    U8 *str;
    CCompString *comp_str;
    Bool has_dft;
    switch(ast->type) {
        case AST_OPCODE:
        CompileOpcode(ast);
        PushIRInt(0);
        break;
        case AST_GLOBAL_LABEL:
        lab=HashFind(ast->name,Compiler.labels);
        if(!lab) throw('Compile');
        if(lab->type!=HTT_LABEL||lab->op) throw('Compile');
        jit_dump_ptr(Compiler.jit,&lab->ptr);
        while(lab->refs->size) {
            op1=VectorPop(lab->refs);
            jit_patch(Compiler.jit,op1);
        }
        lab->op=jit_get_label(Compiler.jit);
        PushIRInt(0);
        break;
        case AST_LOCAL_LABEL:
        lab=HashFind(ast->name,Compiler.labels);
        if(!lab) throw('Compile');
        if(lab->type!=HTT_LABEL||lab->op) throw('Compile');
        jit_dump_ptr(Compiler.jit,&lab->ptr);
        while(lab->refs->size) {
            op1=VectorPop(lab->refs);
            jit_patch(Compiler.jit,op1);
        }
        lab->op=jit_get_label(Compiler.jit);
        PushIRInt(0);
        break;
        case AST_DU8:
        cnt=MSize(ast->arr_lit)/sizeof(AST*);
        for(idx=0;idx!=cnt;idx++) {
            if(ast->arr_lit[idx]->type==AST_STRING) {
                jit_data_bytes(Compiler.jit,ast->arr_lit[idx]->string,MSize(ast->arr_lit[idx]->string)-1); //-1 Ignores NULL terminator
            } else {
                CompileAsmConst(ast->arr_lit[idx],1,ast->label_scope);
            }
        }
        PushIRInt(0);
        break;
        case AST_DU16:
        cnt=MSize(ast->arr_lit)/sizeof(AST*);
        for(idx=0;idx!=cnt;idx++) {
            if(ast->arr_lit[idx]->type==AST_STRING) {
                hi=MSize(str=ast->arr_lit[idx]->string)-1; //-1 Ignores NULL terminator
                for(lo=0;lo!=hi;lo++)
                    jit_data_word(Compiler.jit,str[lo]);
            } else {
                CompileAsmConst(ast->arr_lit[idx],2,ast->label_scope);
            }
        }
        PushIRInt(0);
        break;
        case AST_DU32:
        cnt=MSize(ast->arr_lit)/sizeof(AST*);
        for(idx=0;idx!=cnt;idx++) {
            if(ast->arr_lit[idx]->type==AST_STRING) {
                hi=MSize(str=ast->arr_lit[idx]->string)-1; //-1 Ignores NULL terminator
                for(lo=0;lo!=hi;lo++)
                    jit_data_dword(Compiler.jit,str[lo]);
            } else {
                CompileAsmConst(ast->arr_lit[idx],4,ast->label_scope);
            }
        }
        PushIRInt(0);
        break;
        case AST_DU64:
        cnt=MSize(ast->arr_lit)/sizeof(AST*);
        for(idx=0;idx!=cnt;idx++) {
            if(ast->arr_lit[idx]->type==AST_STRING) {
                hi=MSize(str=ast->arr_lit[idx]->string)-1; //-1 Ignores NULL terminator
                for(lo=0;lo!=hi;lo++)
                    jit_data_qword(Compiler.jit,str[lo]);
            } else {
                CompileAsmConst(ast->arr_lit[idx],8,FALSE,ast->label_scope);
            }
        }
        PushIRInt(0);
        break;
        case AST_IMPORT:
        imp=MAlloc(sizeof(CHash));
        imp->str=StrNew(ast->name);
        HashAdd(imp,Compiler.imports);
        PushIRInt(0);
        break;
        case AST_ALIGN:
        jit_code_align_fill(Compiler.jit,ast->align.to,ast->align.fill);
        PushIRInt(0);
        break;
        case AST_BINFILE:
        str=FileRead(ast->name,&cnt);
        jit_data_bytes(Compiler.jit,str,cnt);
        Free(str);
        PushIRInt(0);
        break;
        case AST_PRINT:
    PushIRString(ast->print.text,MSize(ast->print.text));
    PushIRReg(HashFind("I64i",Compiler.globals),GetFreeIReg);
    LoadSymPtrIntoReg("Print",Compiler.stack->reg);
    cnt=MSize(ast->print.args)/sizeof(AST*);
    if(cnt) {
        Compiler.frame_off=jit_allocai(Compiler.jit,8*cnt);
        value_b=Compiler.stack;
        for(idx=cnt-1;idx>=0;idx--) {
            CompileAST(ast->print.args[idx]);
            switch(Compiler.stack->type) {
                start:
                case IRV_INDIRX:
                PopIRValue(&value_a);
                if(IsF64(value_a->type2)) {
                    value_a=MoveToFReg(value_a,0);
                } else
                    value_a=MoveToReg(value_a,0);
                break;
                end:
                value_a->last=Compiler.stack;
                Compiler.stack=value_a;
            }
            hi=GetFreeIReg;
            value_b=Compiler.stack;
            switch(value_b->type) {
                case IRV_INT:
                case IRV_STR: //TODO register string
                jit_movi(Compiler.jit,jit_R(hi),value_b->int);
                jit_stxi(Compiler.jit,Compiler.frame_off+8*idx,R_FP,jit_R(hi),8);
                break;
                case IRV_REG:
                jit_stxi(Compiler.jit,Compiler.frame_off+8*idx,R_FP,jit_R(value_b->reg),8);
                break;
                case IRV_FLOAT:
                hi=GetFreeFReg;
                jit_fmovi(Compiler.jit,jit_FR(hi),value_b->flt);
                jit_fstxi(Compiler.jit,Compiler.frame_off+8*idx,R_FP,jit_FR(hi),8);
                break;
                case IRV_FREG:
                jit_fstxi(Compiler.jit,Compiler.frame_off+8*idx,R_FP,jit_FR(value_b->reg),8);
                break;
            }
            PopIRValue;
        }
        hi=GetFreeIReg;
        jit_addi(Compiler.jit,jit_R(hi),R_FP,Compiler.frame_off);
        jit_prepare(Compiler.jit);
        jit_putargr(Compiler.jit,jit_R(Compiler.stack->last.reg));
        jit_putargi(Compiler.jit,cnt);
        jit_putargr(Compiler.jit,jit_R(hi));
    } else {
        jit_prepare(Compiler.jit);
        jit_putargr(Compiler.jit,jit_R(Compiler.stack->last.reg));
        jit_putargi(Compiler.jit,0);
    }
    jit_callr(Compiler.jit,jit_R(Compiler.stack->reg));
    PopIRValue,PopIRValue;
    PushIRInt(0);
    break;
    case AST_NOP:
    PushIRInt(0);
    break;
    case AST_DECL:
    if(Compiler.in_func) {
        tar_tab=Compiler.locals;
    } else {
        tar_tab=Compiler.globals;
    }
    var=HashFind(ast->decl.name->name,tar_tab);
    if(ast->decl.dft) {
        if(ast->decl.dft.type==AST_ARRAY_LITERAL) {
            if(Compiler.in_func) {
                var=HashFind(ast->decl.name.name,Compiler.locals);
                if(var&&var->type==HTT_VAR) {
                    PushIRReg(var->hc_type,idx=GetFreeIReg);
                    jit_addi(Compiler.jit,jit_R(idx),R_FP,var->frame_off);
                    arr_assign:
                    CompileArrayLiteral(var->hc_type,idx,ast->decl.dft);
                    PopIRValue;
                    PushIRInt(0);
                    break;
                }
            }
            LoadSymPtrIntoReg(ast->decl.name->name,idx=GetFreeIReg);
            PushIRReg(ast->decl.type,idx)    ;
            goto arr_assign;
        }
        dummy=MAlloc(sizeof(AST));
        dummy->type=AST_NAME;
        dummy->name=StrNew(var->str);
        CompileAST(dummy,TRUE);
        FreeAST(dummy);
        CompileAST(ast->decl.dft);
    } else if(IsPtrType(var->hc_type)||IsInteger(var->hc_type)) {
        if((!var->is_local)||(var->link.type&LINK_STATIC)||var->hc_type->type==HTT_ARRAY_TYPE) goto no_init;
        dummy=MAlloc(sizeof(AST));
        dummy->type=AST_NAME;
        dummy->name=StrNew(var->str);
        CompileAST(dummy,TRUE);
        FreeAST(dummy);
        PushIRInt(0);
    } else if(IsF64(var->hc_type)) {
        if((!var->is_local)||(var->link.type&LINK_STATIC)) goto no_init;
        dummy=MAlloc(sizeof(AST));
        dummy->type=AST_NAME;
        dummy->name=StrNew(var->str);
        CompileAST(dummy,TRUE);
        FreeAST(dummy);
        PushIRFlt(0.0);
    } else {
        no_init:;
        PushIRInt(0);
        break;
    }
    PopIRValue(&value_b);
    PopIRValue(&value_a);
    Assign(value_a,value_b);
    Free(value_a),Free(value_b);
    PushIRInt(0);
    break;
    //Expected the pointer to already be on the stack
    case AST_ARRAY_LITERAL:
    "ARRAY_LITERLA\n";
    throw('Compile');
    break;
    case AST_MEMBER: {
        type1=AssignTypeToNode(ast->member_acc.base);
        CompileAST(ast->member_acc.base,TRUE);
        PopIRValue(&value_a);
        if(IsPtrType(value_a->type2)) {
            type1=DerefedType(type1);
            if(IsPtrType(type1)) {
                "LOTS OF POINTER MEMBER\n";
                throw('Compile');
            }
            PushIRReg(type1+1,GetFreeIReg);
            PopIRValue(&value_b);
            Assign(value_b,value_a);
            PushIRIndirX(MemberType(type1,ast->member_acc.member),value_b->reg,MemberOffset(type1,ast->member_acc.member));
            Free(value_a),Free(value_b);
        } else {
            if(value_a->type==IRV_INDIRX) {
                value_b=IRValueClone(value_a);
                value_b->indirx.offset+=MemberOffset(type1,ast->member_acc.member);
                value_b->type2=MemberType(type1,ast->member_acc.member);
            } else {
                value_b=MoveToReg(IRValueClone(value_a),0);
                PushIRIndirX(MemberType(type1,ast->member_acc.member),value_b->reg,0);
                Free(value_b);
                PopIRValue(&value_b);
            }
            Free(value_a);
            value_a=value_b;
            value_a->last=Compiler.stack;
            Compiler.stack=value_a;
        }
    }
    //Turn arrays into pointers
    type1=type1=AssignTypeToNode(ast);
    if(type1->type==HTT_ARRAY_TYPE) {
        PopIRValue(&value_a);
        PushIRReg(type1,idx=value_a->indirx.reg);
        if(value_a->indirx.offset)
            jit_addi(Compiler.jit,jit_R(idx),jit_R(idx),value_a->indirx.offset);

        }
    break;
    case AST_PAREN:
    CompileAST(ast->unopArg);
    break;
    case AST_INT:
    PushIRInt(ast->int);
    break;
    case AST_FLOAT:
    PushIRFlt(ast->floating);
    break;
    case AST_CHAR:
    PushIRInt(ast->int);
    break;
    case AST_STRING:
    PushIRString(ast->string,MSize(ast->string));
    break;
    case AST_COMMA:
    CompileAST(ast->binop.a);
    PopIRValue;
    CompileAST(ast->binop.b);
    break;
    case AST_ADD:
    CompileAST(ast->binop.a);
    CompileAST(ast->binop.b);
    value_a=IRValueClone(Compiler.stack->last);
    value_b=IRValueClone(Compiler.stack);
    if(IsF64(ast->type2)) {
        if(FoldIRF64) {
            Free(value_b),PopIRValue(&value_b);
            if(FoldIRF64) {
                Free(value_a),PopIRValue(&value_a);
                PushIRFlt(value_a->flt+value_b->flt);
                Free(value_a),Free(value_b);
                break;
            }
            value_a=MoveToFReg(value_a,0);
            jit_faddi(Compiler.jit,jit_FR(value_a->reg),jit_FR(value_a->reg),value_b->flt);
            Free(value_b);
            PopIRValue;
            value_a->last=Compiler.stack;
            Compiler.stack=value_a;
            break;
        }
        value_a=MoveToFReg(value_a,0);
        value_b=MoveToFReg(value_b,1);
        jit_faddr(Compiler.jit,jit_FR(value_a->reg),jit_FR(value_a->reg),jit_FR(value_b->reg));
        PopIRValue,PopIRValue;
        Free(value_b);
        value_a->last=Compiler.stack;
        Compiler.stack=value_a;
        break;
    }
    if(IsPtrType(value_a->type2)) {
        if(FoldIRInt) {
            Free(value_b),PopIRValue(&value_b);
            if(FoldIRInt) {
                Free(value_a),PopIRValue(&value_a);
                PushIRInt(value_a->int+value_b->int*DerefedType(value_a->type2)->sz);
                Free(value_a),Free(value_b);
                break;
            }
            value_a=MoveToReg(value_a,0);
            jit_addi(Compiler.jit,jit_R(value_a->reg),jit_R(value_a->reg),value_b->int*DerefedType(value_a->type2)->sz);
            Free(value_b);
            PopIRValue;
            value_a->last=Compiler.stack;
            Compiler.stack=value_a;
            break;
        }
        value_c=MoveToReg(value_b,1);
        jit_muli(Compiler.jit,jit_R(value_c->reg),jit_R(value_c->reg),DerefedType(value_a->type2)->sz);
        value_b=value_c;
    } else if(IsPtrType(value_b->type2)) {
        value_c=MoveToReg(value_a,0);
        jit_muli(Compiler.jit,jit_R(value_c->reg),jit_R(value_c->reg),DerefedType(value_b->type2)->sz);
        value_a=value_c;
    }
    if(FoldIRInt) {
        Free(value_b),PopIRValue(&value_b);
        if(FoldIRInt) {
            Free(value_a),PopIRValue(&value_a);
            PushIRInt(value_a->int+value_b->int);
            Free(value_a),Free(value_b);
            break;
        }
        value_a=MoveToReg(value_a,0);
        jit_addi(Compiler.jit,jit_R(value_a->reg),jit_R(value_a->reg),value_b->int);
        Free(value_b);
        PopIRValue;
        value_a->last=Compiler.stack;
        Compiler.stack=value_a;
        break;
    }
    if(value_a->type!=IRV_REG) {
        value_c=MoveToReg(value_a,0);
        Free(value_a),value_a=value_c;
    }
    if(value_b->type!=IRV_REG) {
        value_c=MoveToReg(value_b,1);
        Free(value_b),value_b=value_c;
    }
    jit_addr(Compiler.jit,jit_R(value_a->reg),jit_R(value_a->reg),jit_R(value_b->reg));
    PopIRValue,PopIRValue;
    Free(value_b);
    value_a->last=Compiler.stack;
    Compiler.stack=value_a;
    break;
    case AST_SUB:
    CompileAST(ast->binop.a);
    CompileAST(ast->binop.b);
    value_b=IRValueClone(Compiler.stack);
    value_a=IRValueClone(Compiler.stack->last);
    if(IsF64(ast->type2)) {
        if(FoldIRF64) {
            Free(value_b),PopIRValue(&value_b);
            if(FoldIRF64) {
                Free(value_a),PopIRValue(&value_a);
                PushIRFlt(value_a->flt-value_b->flt);
                Free(value_a),Free(value_b);
                break;
            }
            PopIRValue;
            value_a=MoveToFReg(value_a,0);
            jit_fsubi(Compiler.jit,jit_FR(value_a->reg),jit_FR(value_a->reg),value_b->flt);
            Free(value_b);
            value_a->last=Compiler.stack;
            Compiler.stack=value_a;
            break;
        }
        value_a=MoveToFReg(value_a,0);
        value_b=MoveToFReg(value_b,1);
        jit_fsubr(Compiler.jit,jit_FR(value_a->reg),jit_FR(value_a->reg),jit_FR(value_b->reg));
        PopIRValue,PopIRValue;
        Free(value_b);
        value_a->last=Compiler.stack;
        Compiler.stack=value_a;
        break;
    }
    if(IsPtrType(value_a->type2)) {
        if(FoldIRInt) {
            Free(value_b),PopIRValue(&value_b);
            if(FoldIRInt) {
                Free(value_a),PopIRValue(&value_a);
                if(IsPtrType(value_b->type2))
                    PushIRInt((value_a->int-value_b->int)/DerefedType(value_a->type2)->sz);
                else
                    PushIRInt(value_a->int-value_b->int*DerefedType(value_a->type2)->sz);
                Free(value_a),Free(value_b);
                break;
            }
            value_a=MoveToReg(value_a,0);
            jit_subi(Compiler.jit,jit_R(value_a->reg),jit_R(value_a->reg),value_b->int*DerefedType(value_a->type2)->sz);
            Free(value_b);
            PopIRValue;
            value_a->last=Compiler.stack;
            Compiler.stack=value_a;
            break;
        }
        if(!IsPtrType(value_b->type2)) {
            value_c=MoveToReg(value_b,1);
            jit_muli(Compiler.jit,jit_R(value_c->reg),jit_R(value_c->reg),DerefedType(value_a->type2)->sz);
            value_b=value_c;
        }
    } else if(IsPtrType(value_b->type2)) {
        //value_a is not a ptr if here
        value_c=MoveToReg(value_a,0);
        jit_muli(Compiler.jit,jit_R(value_c->reg),jit_R(value_c->reg),DerefedType(value_b->type2)->sz);
        value_a=value_c;
    }
    if(value_a->type!=IRV_REG) {
        value_a=MoveToReg(value_a,0);
    }
    if(value_b->type!=IRV_REG) {
        value_b=MoveToReg(value_b,1);
    }
    jit_subr(Compiler.jit,jit_R(value_a->reg),jit_R(value_a->reg),jit_R(value_b->reg));
    if(IsPtrType(value_a->type2)&&IsPtrType(value_b->type2))
        jit_divi(Compiler.jit,jit_R(value_a->reg),jit_R(value_a->reg),DerefedType(value_a->type2)->sz);
    PopIRValue,PopIRValue;
    Free(value_b);
    value_a->last=Compiler.stack;
    Compiler.stack=value_a;
    break;
    case AST_MUL:
    CompileAST(ast->binop.a);
    CompileAST(ast->binop.b);
    value_b=IRValueClone(Compiler.stack);
    value_a=IRValueClone(Compiler.stack->last);
    if(IsF64(ast->type2)) {
        if(FoldIRF64) {
            Free(value_b),PopIRValue(&value_b);
            if(FoldIRF64) {
                PopIRValue(&value_a);
                PushIRFlt(value_a->flt*value_b->flt);
                Free(value_b),Free(value_a);
                break;
            }
            PopIRValue;
            value_a=MoveToFReg(value_a,0);
            jit_fmuli(Compiler.jit,jit_FR(value_a->reg),jit_FR(value_a->reg),value_b->flt);
            value_a->last=Compiler.stack;
            Free(value_b),Compiler.stack=value_a;
            break;
        }
        value_b=MoveToFReg(value_b,1);
        value_a=MoveToFReg(value_a,0);
        PopIRValue,PopIRValue;
        jit_fmulr(Compiler.jit,jit_FR(value_a->reg),jit_FR(value_a->reg),jit_FR(value_b->reg));
    } else {
        if(FoldIRInt) {
            Free(value_b),PopIRValue(&value_b);
            if(FoldIRInt) {
                PopIRValue(&value_a);
                PushIRInt(value_a->int*value_b->int);
                Free(value_b),Free(value_a);
                break;
            }
            PopIRValue;
            value_a=MoveToReg(value_a,0);
            jit_muli(Compiler.jit,jit_R(value_a->reg),jit_R(value_a->reg),value_b->int);
            value_a->last=Compiler.stack;
            Free(value_b),Compiler.stack=value_a;
            break;
        }
        value_a=MoveToReg(value_a,0);
        value_b=MoveToReg(value_b,1);
        PopIRValue,PopIRValue;
        jit_mulr(Compiler.jit,jit_R(value_a->reg),jit_R(value_a->reg),jit_R(value_b->reg));
    }
    Free(value_b);
    value_a->last=Compiler.stack;
    Compiler.stack=value_a;
    break;
    case AST_DIV:
    CompileAST(ast->binop.a);
    CompileAST(ast->binop.b);
    value_b=IRValueClone(Compiler.stack);
    value_a=IRValueClone(Compiler.stack->last);
    if(IsF64(ast->type2)) {
        if(FoldIRF64) {
            Free(value_b),PopIRValue(&value_b);
            if(FoldIRF64) {
                PopIRValue(&value_a);
                PushIRFlt(value_a->flt/value_b->flt);
                Free(value_b),Free(value_a);
                break;
            }
            PopIRValue;
            value_a=MoveToFReg(value_a,0);
            jit_fdivi(Compiler.jit,jit_FR(value_a->reg),jit_FR(value_a->reg),value_b->flt);
            value_a->last=Compiler.stack;
            Free(value_b),Compiler.stack=value_a;
            break;
        }
        value_b=MoveToFReg(value_b,1);
        value_a=MoveToFReg(value_a,0);
        PopIRValue,PopIRValue;
        jit_fdivr(Compiler.jit,jit_FR(value_a->reg),jit_FR(value_a->reg),jit_FR(value_b->reg));
    } else {
        if(FoldIRInt) {
            Free(value_b),PopIRValue(&value_b);
            if(FoldIRInt) {
                PopIRValue(&value_a);
                PushIRInt(value_a->int/value_b->int);
                Free(value_b),Free(value_a);
                break;
            }
            PopIRValue;
            value_a=MoveToReg(value_a,0);
            jit_divi(Compiler.jit,jit_R(value_a->reg),jit_R(value_a->reg),value_b->int);
            value_a->last=Compiler.stack;
            Free(value_b),Compiler.stack=value_a;
            break;
        }
        value_a=MoveToReg(value_a,0);
        value_b=MoveToReg(value_b,1);
        PopIRValue,PopIRValue;
        jit_divr(Compiler.jit,jit_R(value_a->reg),jit_R(value_a->reg),jit_R(value_b->reg));
    }
    Free(value_b);
    value_a->last=Compiler.stack;
    Compiler.stack=value_a;
    break;
    case AST_MOD:
    CompileAST(ast->binop.a);
    CompileAST(ast->binop.b);
    value_b=IRValueClone(Compiler.stack);
    value_a=IRValueClone(Compiler.stack->last);
    if(IsF64(ast->type2)) {
        if(FoldIRF64) {
            PopIRValue(&value_b);
            if(FoldIRF64) {
                PopIRValue(&value_a);
                PushIRFlt(FMod(value_a->flt,value_b->flt));
                Free(value_a),Free(value_b);
                break;
            }
            PopIRValue;
            value_a=MoveToFReg(value_a,0);
            LoadSymPtrIntoReg("FMod",0);
            jit_prepare(Compiler.jit);
            jit_fputargr(Compiler.jit,jit_FR(value_a->reg),8);
            jit_fputargr(Compiler.jit,jit_FR(value_b->reg),8);
            jit_callr(Compiler.jit,jit_R(0));
            jit_fretval(Compiler.jit,jit_FR(value_a->reg),8);
            goto mod_end;
        }
        PopIRValue,PopIRValue;
        value_a=MoveToFReg(value_a,0);
        value_b=MoveToFReg(value_b,1);
        LoadSymPtrIntoReg("FMod",0);
        jit_prepare(Compiler.jit);
        jit_fputargr(Compiler.jit,jit_FR(value_a->reg),8);
        jit_fputargr(Compiler.jit,jit_FR(value_b->reg),8);
        jit_callr(Compiler.jit,jit_R(0));
        jit_fretval(Compiler.jit,jit_FR(value_a->reg),8);
        Free(value_b);
    } else {
        if(FoldIRInt) {
            Free(value_b),PopIRValue(&value_b);
            if(FoldIRInt) {
                PopIRValue(&value_a);
                PushIRInt(value_a->int%value_b->int);
                Free(value_a),Free(value_b);
                break;
            }
            PopIRValue;
            value_a=MoveToReg(value_a,0);
            jit_modi(Compiler.jit,jit_R(value_a->reg),jit_R(value_a->reg),value_b->int);
            goto mod_end;
        }
        value_a=MoveToReg(value_a,0);
        value_b=MoveToReg(value_b,1);
        PopIRValue,PopIRValue;
        jit_modr(Compiler.jit,jit_R(value_a->reg),jit_R(value_a->reg),jit_R(value_b->reg));
    }
    mod_end:
    Free(value_b);
    value_a->last=Compiler.stack;
    Compiler.stack=value_a;
    break;
    case AST_LSHIFT:
    CompileAST(ast->binop.a);
    CompileAST(ast->binop.b);
    value_b=IRValueClone(Compiler.stack);
    value_a=IRValueClone(Compiler.stack->last);
    if(IsF64(ast->type2)) {
        value_a=MoveToFReg(value_a,0);
        value_b=MoveToFReg(value_b,1);
        PopIRValue,PopIRValue;
        LoadSymPtrIntoReg("F64Shl",0);
        jit_prepare(Compiler.jit);
        jit_fputargr(Compiler.jit,jit_FR(value_a->reg),8);
        jit_fputargr(Compiler.jit,jit_FR(value_b->reg),8);
        jit_callr(Compiler.jit,jit_R(0));
        jit_fretval(Compiler.jit,jit_FR(value_a->reg),8);
        Free(value_b);
    } else {
        if(FoldIRInt) {
            PopIRValue(&value_b);
            if(FoldIRInt) {
                PopIRValue(&value_a);
                PushIRInt(value_a->int<<value_b->int);
                Free(value_a),Free(value_b);
                break;
            }
            PopIRValue;
            value_a=MoveToReg(value_a,0);
            jit_lshi(Compiler.jit,jit_R(value_a->reg),jit_R(value_a->reg),value_b->int);
            goto lsh_end;
        }
        value_a=MoveToReg(value_a,0);
        value_b=MoveToReg(value_b,1);
        PopIRValue,PopIRValue;
        jit_lshr(Compiler.jit,jit_R(value_a->reg),jit_R(value_a->reg),jit_R(value_b->reg));
    }
    lsh_end:
    Free(value_b);
    value_a->last=Compiler.stack;
    Compiler.stack=value_a;
    break;
    case AST_RSHIFT:
    CompileAST(ast->binop.a);
    CompileAST(ast->binop.b);
    value_b=IRValueClone(Compiler.stack);
    value_a=IRValueClone(Compiler.stack->last);
    if(IsF64(ast->type2)) {
        value_a=MoveToFReg(value_a,0);
        value_b=MoveToFReg(value_b,1);
        PopIRValue,PopIRValue;
        LoadSymPtrIntoReg("F64Shr",0);
        jit_prepare(Compiler.jit);
        jit_fputargr(Compiler.jit,jit_FR(value_a->reg),8);
        jit_fputargr(Compiler.jit,jit_FR(value_b->reg),8);
        jit_callr(Compiler.jit,jit_R(0));
        jit_fretval(Compiler.jit,jit_FR(value_a->reg),8);
        Free(value_b);
    } else if(IsSigned(ast->type2)) {
        if(FoldIRInt) {
            PopIRValue(&value_b);
            if(FoldIRInt) {
                PopIRValue(&value_a);
                PushIRInt(value_a->int>>value_b->int);
                Free(value_a),Free(value_b);
                break;
            }
            value_a=MoveToReg(value_a,0);
            PopIRValue;
            jit_rshi(Compiler.jit,jit_R(value_a->reg),jit_R(value_a->reg),value_b->int);
            goto rsh_end;
        }
        value_a=MoveToReg(value_a,0);
        value_b=MoveToReg(value_b,1);
        PopIRValue,PopIRValue;
        jit_rshr(Compiler.jit,jit_R(value_a->reg),jit_R(value_a->reg),jit_R(value_b->reg));
    } else {
        if(FoldIRInt) {
            PopIRValue(&value_b);
            if(FoldIRInt) {
                PopIRValue(&value_a);
                PushIRInt(value_a->int(U64)>>value_b->int);
                Free(value_a),Free(value_b);
                break;
            }
            value_a=MoveToReg(value_a,0);
            PopIRValue;
            jit_rshi_u(Compiler.jit,jit_R(value_a->reg),jit_R(value_a->reg),value_b->int);
            goto rsh_end;
        }
        value_a=MoveToReg(value_a,0);
        value_b=MoveToReg(value_b,1);
        PopIRValue,PopIRValue;
        jit_rshr_u(Compiler.jit,jit_R(value_a->reg),jit_R(value_a->reg),jit_R(value_b->reg));
    }
    rsh_end:
    Free(value_b);
    value_a->last=Compiler.stack;
    Compiler.stack=value_a;
    break;
    case AST_POW:
    CompileAST(ast->binop.a);
    CompileAST(ast->binop.b);
    value_b=IRValueClone(Compiler.stack);
    value_a=IRValueClone(Compiler.stack->last);
    PopIRValue,PopIRValue;
    if(IsF64(AssignTypeToNode(ast))) {
        value_a=MoveToFReg(value_a,0);
        value_b=MoveToFReg(value_b,1);
        LoadSymPtrIntoReg("Pow",0);
        jit_prepare(Compiler.jit);
        jit_fputargr(Compiler.jit,jit_FR(value_a->reg),8);
        jit_fputargr(Compiler.jit,jit_FR(value_b->reg),8);
        jit_callr(Compiler.jit,jit_R(0));
        jit_fretval(Compiler.jit,jit_FR(value_a->reg),8);
        Free(value_b);
    } else {
        LoadSymPtrIntoReg("PowI64",2);
        value_a=MoveToReg(value_a,0);
        value_b=MoveToReg(value_b,1);
        jit_prepare(Compiler.jit);
        jit_putargr(Compiler.jit,jit_R(value_a->reg));
        jit_putargr(Compiler.jit,jit_R(value_b->reg));
        jit_callr(Compiler.jit,jit_R(2));
        jit_retval(Compiler.jit,jit_R(value_a->reg));
        Free(value_b);
    }
    value_a->last=Compiler.stack;
    Compiler.stack=value_a;
    break;
    case AST_BAND:
    CompileAST(ast->binop.a);
    CompileAST(ast->binop.b);
    value_b=IRValueClone(Compiler.stack);
    value_a=IRValueClone(Compiler.stack->last);
    if(IsF64(AssignTypeToNode(ast))) {
        value_a=MoveToFReg(value_a,0);
        value_b=MoveToFReg(value_b,1);
        PopIRValue,PopIRValue;
        LoadSymPtrIntoReg("F64And",0);
        jit_prepare(Compiler.jit);
        jit_fputargr(Compiler.jit,jit_FR(value_a->reg),8);
        jit_fputargr(Compiler.jit,jit_FR(value_b->reg),8);
        jit_callr(Compiler.jit,jit_R(0));
        jit_fretval(Compiler.jit,jit_FR(value_a->reg),8);
    } else {
        if(FoldIRInt) {
            PopIRValue(&value_b);
            if(FoldIRInt) {
                PopIRValue(&value_a);
                PushIRInt(value_a->int&value_b->int);
                Free(value_a),Free(value_b);
                break;
            }
            PopIRValue;
            value_a=MoveToReg(value_a,0);
            jit_andi(Compiler.jit,jit_R(value_a->reg),jit_R(value_a->reg),value_b->int);
            goto band_end;
        }
        value_a=MoveToReg(value_a,0);
        value_b=MoveToReg(value_b,1);
        PopIRValue,PopIRValue;
        jit_andr(Compiler.jit,jit_R(value_a->reg),jit_R(value_a->reg),jit_R(value_b->reg));
        Free(value_b);
    }
    band_end:
    Free(value_b);
    value_a->last=Compiler.stack;
    Compiler.stack=value_a;
    break;
    case AST_BXOR:
    CompileAST(ast->binop.a);
    CompileAST(ast->binop.b);
    value_b=IRValueClone(Compiler.stack);
    value_a=IRValueClone(Compiler.stack->last);
    if(IsF64(AssignTypeToNode(ast))) {
        value_a=MoveToFReg(value_a,0);
        value_b=MoveToFReg(value_b,1);
        PopIRValue,PopIRValue;
        LoadSymPtrIntoReg("F64Xor",0);
        jit_prepare(Compiler.jit);
        jit_fputargr(Compiler.jit,jit_FR(value_a->reg),8);
        jit_fputargr(Compiler.jit,jit_FR(value_b->reg),8);
        jit_callr(Compiler.jit,jit_R(0));
        jit_fretval(Compiler.jit,jit_FR(value_a->reg),8);
    } else {
        if(FoldIRInt) {
            PopIRValue(&value_b);
            if(FoldIRInt) {
                PopIRValue(&value_a);
                PushIRInt(value_a->int^value_b->int);
                Free(value_a),Free(value_b);
                break;
            }
            value_a=MoveToReg(value_a,0);
            PopIRValue;
            jit_xori(Compiler.jit,jit_R(value_a->reg),jit_R(value_a->reg),value_b->int);
            goto band_end;
        }
        value_a=MoveToReg(value_a,0);
        value_b=MoveToReg(value_b,1);
        PopIRValue,PopIRValue;
        jit_xorr(Compiler.jit,jit_R(value_a->reg),jit_R(value_a->reg),jit_R(value_b->reg));
    }
    bxor_end:
    Free(value_b);
    value_a->last=Compiler.stack;
    Compiler.stack=value_a;
    break;
    case AST_BOR:
    CompileAST(ast->binop.a);
    CompileAST(ast->binop.b);
    value_b=IRValueClone(Compiler.stack);
    value_a=IRValueClone(Compiler.stack->last);
    if(IsF64(AssignTypeToNode(ast))) {
        value_a=MoveToFReg(value_a,0);
        value_b=MoveToFReg(value_b,1);
        PopIRValue,PopIRValue;
        LoadSymPtrIntoReg("F64Or",0);
        jit_prepare(Compiler.jit);
        jit_fputargr(Compiler.jit,jit_FR(value_a->reg),8);
        jit_fputargr(Compiler.jit,jit_FR(value_b->reg),8);
        jit_callr(Compiler.jit,jit_R(0));
        jit_fretval(Compiler.jit,jit_FR(value_a->reg),8);
    } else {
        if(FoldIRInt) {
            PopIRValue(&value_b);
            if(FoldIRInt) {
                PopIRValue(&value_a);
                PushIRInt(value_a->int|value_b->int);
                Free(value_a),Free(value_b);
                break;
            }
            value_a=MoveToReg(value_a,0);
            PopIRValue;
            jit_ori(Compiler.jit,jit_R(value_a->reg),jit_R(value_a->reg),value_b->int);
            goto bor_end;
        }
        value_a=MoveToReg(value_a,0);
        value_b=MoveToReg(value_b,1);
        PopIRValue,PopIRValue;
        jit_orr(Compiler.jit,jit_R(value_a->reg),jit_R(value_a->reg),jit_R(value_b->reg));
            }
    bor_end:
    Free(value_b);
    value_a->last=Compiler.stack;
    Compiler.stack=value_a;
    break;
    case AST_LT:
    //Handled in AST_RANGE
    break;
    case AST_GT:
    //Handled in AST_RANGE
    break;
    case AST_LE:
    //Handled in AST_RANGE
    break;
    case AST_EQ:
    PushIRReg(GetType("Bool"),GetFreeIReg);
    value_c=Compiler.stack;
    CompileAST(ast->binop.a);
    CompileAST(ast->binop.b);
    value_b=IRValueClone(Compiler.stack);
    value_a=IRValueClone(Compiler.stack->last);
    if(IsF64(AssignTypeToNode(ast->binop.a))) {
        if(FoldIRF64) {
            Free(value_b),PopIRValue(&value_b);
            if(FoldIRF64) {
                Free(value_a),PopIRValue(&value_a);
                PopIRValue;
                PushIRInt(value_a->flt==value_b->flt);
                goto eq_end;
            }
            value_a=MoveToFReg(value_a,0);
            PopIRValue;
            jit_movi(Compiler.jit,jit_R(value_c->reg),1);
            op1=jit_fbeqi(Compiler.jit,NULL,jit_FR(value_a->reg),value_b->flt);
            jit_movi(Compiler.jit,jit_R(value_c->reg),0);
            jit_patch(Compiler.jit,op1);
            goto eq_end;
        }
        value_a=MoveToFReg(value_a,0);
        value_b=MoveToFReg(value_b,1);
        PopIRValue,PopIRValue;
        jit_movi(Compiler.jit,jit_R(value_c->reg),1);
        op1=jit_fbeqr(Compiler.jit,NULL,jit_FR(value_a->reg),jit_FR(value_b->reg));
        jit_movi(Compiler.jit,jit_R(value_c->reg),0);
        jit_patch(Compiler.jit,op1);
    } else {
        if(FoldIRInt) {
            Free(value_b),PopIRValue(&value_b);
            if(FoldIRInt) {
                Free(value_a),PopIRValue(&value_a);
                PopIRValue;
                PushIRInt(value_a->int==value_b->int);
                goto eq_end;
            }
            value_a=MoveToReg(value_a,0);
            PopIRValue;
            jit_eqi(Compiler.jit,jit_R(value_c->reg),jit_R(value_a->reg),value_b->int);
            goto eq_end;
        }
        value_a=MoveToReg(value_a,0);
        value_b=MoveToReg(value_b,1);
        PopIRValue,PopIRValue;
        jit_eqr(Compiler.jit,jit_R(value_c->reg),jit_R(value_a->reg),jit_R(value_b->reg));
    }
    eq_end:
    Free(value_b),Free(value_b);
    break;
    case AST_NE:
    CompileAST(ast->binop.a);
    CompileAST(ast->binop.b);
    value_b=IRValueClone(Compiler.stack);
    value_a=IRValueClone(Compiler.stack->last);
    if(IsF64(AssignTypeToNode(ast->binop.a))||IsF64(AssignTypeToNode(ast->binop.b))) {
        //TODO const fold
        PopIRValue,PopIRValue;
        value_a=MoveToFReg(value_a,0);
        value_b=MoveToFReg(value_b,1);
        PushIRReg(HashFind("I64i",Compiler.globals),GetFreeIReg);
        value_c=Compiler.stack;
        jit_movi(Compiler.jit,jit_R(value_c->reg),1);
        op1=jit_fbner(Compiler.jit,NULL,jit_FR(value_a->reg),jit_FR(value_b->reg));
        jit_movi(Compiler.jit,jit_R(value_c->reg),0);
        jit_patch(Compiler.jit,op1);
        break;
    } else {
        if(FoldIRInt) {
            Free(value_b),PopIRValue(&value_b);
            if(FoldIRInt) {
                Free(value_a),PopIRValue(&value_a);
                PushIRInt(value_a->int!=value_b->int);
                Free(value_a),Free(value_b);
                break;
            }
            PopIRValue;
            value_a=MoveToReg(value_a,0);
            jit_nei(Compiler.jit,jit_R(value_a->reg),jit_R(value_a->reg),value_b->int);
            goto neq_end;
        }
        value_a=MoveToReg(value_a,0);
        value_b=MoveToReg(value_b,1);
        PopIRValue,PopIRValue;
        jit_ner(Compiler.jit,jit_R(value_a->reg),jit_R(value_a->reg),jit_R(value_b->reg));
    }
    neq_end:
    Free(value_b);
    value_a->last=Compiler.stack;
    Compiler.stack=value_a;
    break;
    case AST_GE:
    //Will be handled in AST_RANGE
    break;
    case AST_LAND:
    PushIRReg(GetType("I64i"),GetFreeIReg);
    value_c=Compiler.stack;
    CompileAST(ast->binop.a);
    value_a=IRValueClone(Compiler.stack);
    value_a=BoolifyValue(value_a);
    op1=jit_beqi(Compiler.jit,NULL,jit_R(value_a->reg),0);
    PopIRValue,Free(value_a);
    CompileAST(ast->binop.b);
    value_b=BoolifyValue(IRValueClone(Compiler.stack));
    Assign(value_c,value_b);
    jit_nei(Compiler.jit,jit_R(value_c->reg),jit_R(value_c->reg),0);
    op2=jit_jmpi(Compiler.jit,NULL);
    jit_patch(Compiler.jit,op1);
    PushIRInt(0);
    Assign(value_c,Compiler.stack),PopIRValue;
    jit_patch(Compiler.jit,op2);
    PopIRValue,Free(value_b);
    break;
    case AST_LOR:
    PushIRReg(GetType("I64i"),GetFreeIReg);
    jit_movi(Compiler.jit,jit_R(Compiler.stack->reg),1);
    CompileAST(ast->binop.a);
    value_a=IRValueClone(Compiler.stack);
    value_a=BoolifyValue(value_a);
    op1=jit_bnei(Compiler.jit,NULL,jit_R(value_a->reg),0);
    PopIRValue;
    CompileAST(ast->binop.b);
    value_b=BoolifyValue(IRValueClone(Compiler.stack));
    op2=jit_bnei(Compiler.jit,NULL,jit_R(value_b->reg),0);
    PopIRValue;
    jit_movi(Compiler.jit,jit_R(Compiler.stack->reg),0);
    jit_patch(Compiler.jit,op1);
    jit_patch(Compiler.jit,op2);
    break;
    case AST_LXOR:
    CompileAST(ast->binop.a);
    value_a=BoolifyValue(IRValueClone(Compiler.stack));
    PopIRValue;
    value_c=IRValueClone(value_a);
    value_c->last=Compiler.stack;
    Compiler.stack=value_c;
    CompileAST(ast->binop.b);
    value_b=BoolifyValue(IRValueClone(Compiler.stack));
    PopIRValue,PopIRValue;
    jit_xorr(Compiler.jit,jit_R(value_a->reg),jit_R(value_a->reg),jit_R(value_b->reg));
    value_a->last=Compiler.stack;
    Free(value_b);
    value_a->last=Compiler.stack;
    Compiler.stack=value_a;
    break;
    case AST_ASSIGN:
    CompileAST(ast->binop.a,TRUE);
    CompileAST(ast->binop.b);
    value_b=IRValueClone(Compiler.stack);
    value_a=IRValueClone(Compiler.stack->last);
    PopIRValue,PopIRValue;
    Assign(value_a,value_b),Free(value_b);
    value_a->last=Compiler.stack;
    Compiler.stack=value_a;
    asn_end:
    if(!pass_by_ref&&IsF64(value_a->type2)) {
        PopIRValue(&value_a);
        PushIRFReg(value_a->type2,GetFreeFReg);
        Assign(Compiler.stack,value_a);
        Free(value_a);
    } else if(!pass_by_ref&&(IsPtrType(value_a->type2)||IsInteger(value_a->type2))) {
        PopIRValue(&value_a);
        PushIRReg(value_a->type2,GetFreeIReg);
        Assign(Compiler.stack,value_a);
        Free(value_a);
    }
    break;
    case AST_ASSIGN_SHL:
    CompileAST(ast->binop.a,TRUE);
    CompileAST(ast->binop.b);
    value_b=IRValueClone(Compiler.stack);
    value_a=IRValueClone(Compiler.stack->last);
    if(IsF64(value_a->type2)) {
        value_b=MoveToFReg(value_b,1);
        value_c=MoveToFReg(IRValueClone(value_a),0);
        LoadSymPtrIntoReg("F64Shl",0);
        jit_prepare(Compiler.jit);
        jit_putargr(Compiler.jit,jit_FR(value_c->reg));
        jit_putargr(Compiler.jit,jit_FR(value_b->reg));
        jit_callr(Compiler.jit,jit_R(0));
        PushIRFReg(GetType("F64"),GetFreeFReg);
        jit_fretval(Compiler.jit,jit_FR(Compiler.stack->reg),8);
        Assign(value_a,Compiler.stack);
        PopIRValue;
    } else {
        if(FoldIRInt) {
            Free(value_b),PopIRValue(&value_b);
            value_c=MoveToReg(IRValueClone(value_a),0);
            PopIRValue;
            jit_lshi(Compiler.jit,jit_R(value_c->reg),jit_R(value_c->reg),value_b->int);
            Assign(value_a,value_c);
            Free(value_c),Free(value_b);
            goto ashl_end;
        }
        value_c=MoveToReg(IRValueClone(value_a),0),value_b=MoveToReg(value_b,1);
        jit_lshr(Compiler.jit,jit_R(value_c->reg),jit_R(value_c->reg),jit_R(value_b->reg));
        Assign(value_a,value_c);
        Free(value_b);
    }
    PopIRValue,PopIRValue;
    ashl_end:
    value_a->last=Compiler.stack;
    Compiler.stack=value_a;
    goto asn_end;
    case AST_ASSIGN_SHR:
    CompileAST(ast->binop.a,TRUE);
    CompileAST(ast->binop.b);
    value_b=IRValueClone(Compiler.stack);
    value_a=IRValueClone(Compiler.stack->last);
    if(IsF64(value_a->type2)) {
        value_b=MoveToFReg(value_b,1);
        value_c=MoveToFReg(IRValueClone(value_a),0);
        LoadSymPtrIntoReg("F64Shr",0);
        jit_prepare(Compiler.jit);
        jit_putargr(Compiler.jit,jit_FR(value_c->reg));
        jit_putargr(Compiler.jit,jit_FR(value_b->reg));
        jit_callr(Compiler.jit,jit_R(0));
        PushIRFReg(GetType("F64"),GetFreeFReg);
        jit_fretval(Compiler.jit,jit_FR(Compiler.stack->reg),8);
        Assign(value_a,Compiler.stack);
        PopIRValue;
    } else if(IsSigned(value_a->type2)) {
        if(FoldIRInt) {
            Free(value_b),PopIRValue(&value_b);
            value_c=MoveToReg(IRValueClone(value_a),0);
            jit_rshi(Compiler.jit,jit_R(value_c->reg),jit_R(value_c->reg),value_b->int);
            Assign(value_a,value_c);
            Free(value_c),Free(value_c);
            PopIRValue;
            goto ashl_end;
        }
        value_c=MoveToReg(IRValueClone(value_a),0),value_b=MoveToReg(value_b,1);
        jit_rshr(Compiler.jit,jit_R(value_c->reg),jit_R(value_c->reg),jit_R(value_b->reg));
        Free(value_b);
        Assign(value_a,value_c);
    } else {
        if(FoldIRInt) {
            Free(value_b),PopIRValue(&value_b);
            value_c=MoveToReg(IRValueClone(value_a),0);
            jit_rshi_u(Compiler.jit,jit_R(value_c->reg),jit_R(value_c->reg),value_b->int);
            Assign(value_a,value_c);
            Free(value_c),Free(value_b);
            PopIRValue;
            goto ashl_end;
        }
        value_c=MoveToReg(IRValueClone(value_a),0),value_b=MoveToReg(value_b,1);
        jit_rshr_u(Compiler.jit,jit_R(value_c->reg),jit_R(value_c->reg),jit_R(value_b->reg));
        Free(value_b);
        Assign(value_a,value_c);
    }
    PopIRValue,PopIRValue;
    arshr_end:
    value_a->last=Compiler.stack;
    Compiler.stack=value_a;
    goto asn_end;
    case AST_ASSIGN_MUL:
    CompileAST(ast->binop.a,TRUE);
    CompileAST(ast->binop.b);
    value_b=IRValueClone(Compiler.stack);
    value_a=IRValueClone(Compiler.stack->last);
    if(IsF64(value_a->type2)) {
        if(FoldIRF64) {
            Free(value_b),PopIRValue(&value_b);
            value_c=MoveToFReg(IRValueClone(value_a),0);
            jit_fmuli(Compiler.jit,jit_FR(value_c->reg),jit_FR(value_c->reg),value_b->flt);
            Assign(value_a,value_c);
            Free(value_c);
            PopIRValue;
            goto amul_end;
        }
        value_c=MoveToFReg(IRValueClone(value_a),0),value_b=MoveToFReg(value_b,1);
        jit_fmulr(Compiler.jit,jit_FR(value_c->reg),jit_FR(value_c->reg),jit_FR(value_b->reg));
        Assign(value_a,value_c);
        Free(value_b),Free(value_c);
    } else {
        if(FoldIRInt) {
            Free(value_b),PopIRValue(&value_b);
            value_c=MoveToReg(IRValueClone(value_a),0);
            jit_muli(Compiler.jit,jit_R(value_c->reg),jit_R(value_c->reg),value_b->int);
            Assign(value_a,value_c);
            Free(value_c);
            PopIRValue;
            goto amul_end;
        }
        value_c=MoveToReg(IRValueClone(value_a),0),value_b=MoveToReg(value_b,1);
        jit_mulr(Compiler.jit,jit_R(value_c->reg),jit_R(value_c->reg),jit_R(value_b->reg));
        Assign(value_a,value_c);
        Free(value_b);
    }
    PopIRValue,PopIRValue;
    amul_end:
    value_a->last=Compiler.stack;
    Compiler.stack=value_a;
    goto asn_end;
    case AST_ASSIGN_DIV:
    CompileAST(ast->binop.a,TRUE);
    CompileAST(ast->binop.b);
    value_b=IRValueClone(Compiler.stack);
    value_a=IRValueClone(Compiler.stack->last);
    if(IsF64(value_a->type2)) {value_c=MoveToFReg(IRValueClone(value_a),0),value_b=MoveToFReg(value_b,1);
        jit_fdivr(Compiler.jit,jit_FR(value_c->reg),jit_FR(value_c->reg),jit_FR(value_b->reg));
        Assign(value_a,value_c);
        Free(value_b);
    } else {
        value_c=MoveToReg(IRValueClone(value_a),0),value_b=MoveToReg(value_b,1);
        jit_divr(Compiler.jit,jit_R(value_c->reg),jit_R(value_c->reg),jit_R(value_b->reg));
        Assign(value_a,value_c);
        Free(value_b);
    }
    PopIRValue,PopIRValue;
    value_a->last=Compiler.stack;
    Compiler.stack=value_a;
    goto asn_end;
    case AST_ASSIGN_BAND:
    CompileAST(ast->binop.a,TRUE);
    CompileAST(ast->binop.b);
    value_b=IRValueClone(Compiler.stack);
    value_a=IRValueClone(Compiler.stack->last);
    if(IsF64(value_a->type2)) {
        value_b=MoveToFReg(value_b,1);
        value_c=MoveToFReg(IRValueClone(value_a),0);
        LoadSymPtrIntoReg("F64And",0);
        jit_prepare(Compiler.jit);
        jit_putargr(Compiler.jit,jit_FR(value_c->reg));
        jit_putargr(Compiler.jit,jit_FR(value_b->reg));
        jit_callr(Compiler.jit,jit_R(0));
        PushIRFReg(GetType("F64"),GetFreeFReg);
        jit_fretval(Compiler.jit,jit_FR(Compiler.stack->reg),8);
        Assign(value_a,Compiler.stack);
        PopIRValue;
    } else {
        if(FoldIRInt) {
            Free(value_b),PopIRValue(&value_b);
            value_c=MoveToReg(IRValueClone(value_a),0);
            jit_andi(Compiler.jit,jit_R(value_c->reg),jit_R(value_c->reg),value_b->int);
            Assign(value_a,value_c);
            Free(value_c);
            PopIRValue;
            goto aband_end;
        }
        value_c=MoveToReg(IRValueClone(value_a),0),value_b=MoveToReg(value_b,1);
        jit_andr(Compiler.jit,jit_R(value_c->reg),jit_R(value_c->reg),jit_R(value_b->reg));
        Assign(value_a,value_c);
        Free(value_b);
    }
    PopIRValue,PopIRValue;
    aband_end:
    value_a->last=Compiler.stack;
    Compiler.stack=value_a;
    goto asn_end;
    case AST_ASSIGN_BOR:
    CompileAST(ast->binop.a,TRUE);
    CompileAST(ast->binop.b);
    value_b=IRValueClone(Compiler.stack);
    value_a=IRValueClone(Compiler.stack->last);
    if(IsF64(value_a->type2)) {
        value_b=MoveToFReg(value_b,1);
        value_c=MoveToFReg(IRValueClone(value_a),0);
        LoadSymPtrIntoReg("F64Or",0);
        jit_prepare(Compiler.jit);
        jit_putargr(Compiler.jit,jit_FR(value_c->reg));
        jit_putargr(Compiler.jit,jit_FR(value_b->reg));
        jit_callr(Compiler.jit,jit_R(0));
        PushIRFReg(GetType("F64"),GetFreeFReg);
        jit_fretval(Compiler.jit,jit_FR(Compiler.stack->reg),8);
        Assign(value_a,Compiler.stack);
        PopIRValue;
    } else {
        if(FoldIRInt) {
            Free(value_b),PopIRValue(&value_b);
            value_c=MoveToReg(IRValueClone(value_a),0);
            jit_ori(Compiler.jit,jit_R(value_c->reg),jit_R(value_c->reg),value_b->int);
            Assign(value_a,value_c);
            Free(value_c);
            PopIRValue;
            goto abor_end;
        }
        value_c=MoveToReg(IRValueClone(value_a),0),value_b=MoveToReg(value_b,1);
        jit_orr(Compiler.jit,jit_R(value_c->reg),jit_R(value_c->reg),jit_R(value_b->reg));
        Assign(value_a,value_c);
        Free(value_b);
    }
    PopIRValue,PopIRValue;
    abor_end:
    value_a->last=Compiler.stack;
    Compiler.stack=value_a;
    goto asn_end;
    case AST_ASSIGN_BXOR:
    CompileAST(ast->binop.a,TRUE);
    CompileAST(ast->binop.b);
    value_b=IRValueClone(Compiler.stack);
    value_a=IRValueClone(Compiler.stack->last);
    if(IsF64(value_a->type2)) {
        value_b=MoveToFReg(value_b,1);
        value_c=MoveToFReg(IRValueClone(value_a),0);
        LoadSymPtrIntoReg("F64Xor",0);
        jit_prepare(Compiler.jit);
        jit_putargr(Compiler.jit,jit_FR(value_c->reg));
        jit_putargr(Compiler.jit,jit_FR(value_b->reg));
        jit_callr(Compiler.jit,jit_R(0));
        PushIRFReg(GetType("F64"),GetFreeFReg);
        jit_fretval(Compiler.jit,jit_FR(Compiler.stack->reg),8);
        Assign(value_a,Compiler.stack);
        PopIRValue;
    } else {
        if(FoldIRInt) {
            Free(value_b),PopIRValue(&value_b);
            value_c=MoveToReg(IRValueClone(value_a),0);
            jit_xori(Compiler.jit,jit_R(value_c->reg),jit_R(value_c->reg),value_b->int);
            Assign(value_a,value_c);
            Free(value_c);
            PopIRValue;
            goto abxor_end;
        }
        value_c=MoveToReg(IRValueClone(value_a),0),value_b=MoveToReg(value_b,1);
        jit_xorr(Compiler.jit,jit_R(value_c->reg),jit_R(value_c->reg),jit_R(value_b->reg));
        Assign(value_a,value_c);
        Free(value_b);
    }
    PopIRValue,PopIRValue;
    abxor_end:
    value_a->last=Compiler.stack;
    Compiler.stack=value_a;
    goto asn_end;
    case AST_ASSIGN_ADD:
    type1=AssignTypeToNode(ast->binop.a);
    if(IsPtrType(type1))
        mult=DerefedType(type1)->sz;
    else
        mult=1;
    CompileAST(ast->binop.a,TRUE);
    CompileAST(ast->binop.b);
    value_b=IRValueClone(Compiler.stack);
    value_a=IRValueClone(Compiler.stack->last);
    if(IsF64(value_a->type2)) {
        if(FoldIRF64) {
            Free(value_b),PopIRValue(&value_b);
            value_c=MoveToFReg(IRValueClone(value_a),0);
            jit_faddi(Compiler.jit,jit_R(value_c->reg),jit_R(value_c->reg),value_b->flt);
            Assign(value_a,value_c);
            Free(value_c);
            PopIRValue;
            goto aadd_end;
        }
        value_c=MoveToFReg(IRValueClone(value_a),0),value_b=MoveToFReg(value_b,1);
        jit_faddr(Compiler.jit,jit_FR(value_c->reg),jit_FR(value_c->reg),jit_FR(value_b->reg));
        Assign(value_a,value_c);
        Free(value_b);
    } else {
        if(FoldIRInt) {
            Free(value_b),PopIRValue(&value_b);
            value_c=MoveToReg(IRValueClone(value_a),0);
            jit_addi(Compiler.jit,jit_R(value_c->reg),jit_R(value_c->reg),value_b->int*mult);
            Assign(value_a,value_c);
            Free(value_c);
            PopIRValue;
            goto aadd_end;
        }
        value_c=MoveToReg(IRValueClone(value_a),0),value_b=MoveToReg(value_b,1);
        if(mult)
            jit_muli(Compiler.jit,jit_R(value_b->reg),jit_R(value_b->reg),mult);
        jit_addr(Compiler.jit,jit_R(value_c->reg),jit_R(value_c->reg),jit_R(value_b->reg));
        Assign(value_a,value_c);
        Free(value_b);
    }
    PopIRValue,PopIRValue;
    aadd_end:
    value_a->last=Compiler.stack;
    Compiler.stack=value_a;
    goto asn_end;
    case AST_ASSIGN_SUB:
    type1=AssignTypeToNode(ast->binop.a);
    if(IsPtrType(type1))
        mult=DerefedType(type1)->sz;
    else
        mult=1;
    CompileAST(ast->binop.a,TRUE);
    CompileAST(ast->binop.b);
    value_b=IRValueClone(Compiler.stack);
    value_a=IRValueClone(Compiler.stack->last);
    if(IsF64(value_a->type2)) {
        value_c=MoveToFReg(IRValueClone(value_a),0),value_b=MoveToFReg(value_b,1);
        jit_fsubr(Compiler.jit,jit_FR(value_c->reg),jit_FR(value_c->reg),jit_FR(value_b->reg));
        Assign(value_a,value_c);
        Free(value_b);
    } else {
        if(FoldIRInt) {
            Free(value_b),PopIRValue(&value_b);
            value_c=MoveToReg(IRValueClone(value_a),0);
            jit_subi(Compiler.jit,jit_R(value_c->reg),jit_R(value_c->reg),value_b->int*mult);
            Assign(value_a,value_c);
            Free(value_c);
            PopIRValue;
            goto asub_end;
        }
        value_c=MoveToReg(IRValueClone(value_a),0),value_b=MoveToReg(value_b,1);
        if(mult)
            jit_muli(Compiler.jit,jit_R(value_b->reg),jit_R(value_b->reg),mult);
        jit_subr(Compiler.jit,jit_R(value_c->reg),jit_R(value_c->reg),jit_R(value_b->reg));
        Assign(value_a,value_c);
        Free(value_b);
    }
    PopIRValue,PopIRValue;
    asub_end:
    value_a->last=Compiler.stack;
    Compiler.stack=value_a;
    goto asn_end;
    case AST_POST_INC:
    CompileAST(ast->unopArg,TRUE);
    value_a=IRValueClone(Compiler.stack);
    if(IsF64(value_a->type2)) {
        PushIRFReg(value_a->type2,GetFreeFReg);
        Assign(value_b=Compiler.stack,value_a);
        PushIRFReg(value_a->type2,GetFreeFReg);
        Assign(value_c=Compiler.stack,value_a);
        jit_faddi(Compiler.jit,jit_FR(value_c->reg),jit_FR(value_c->reg),1.0);
        Assign(value_a,value_c);
        PopIRValue; //value_c
        value_b=IRValueClone(value_b); //Make unique
        PopIRValue; //value_b
    } else {
        PushIRReg(value_a->type2,GetFreeIReg);
        Assign(value_b=Compiler.stack,value_a);
        PushIRReg(value_a->type2,GetFreeIReg);
        Assign(value_c=Compiler.stack,value_a);
        mult=1;
        if(IsPtrType(value_a->type2)) {
            mult=DerefedType(value_a->type2)->sz;
        }
        jit_addi(Compiler.jit,jit_R(value_c->reg),jit_R(value_c->reg),mult);
        Assign(value_a,value_c);
        PopIRValue; //value_c
        value_b=IRValueClone(value_b); //MAke unique
        PopIRValue; //value_b
    }
    Free(value_a);
    PopIRValue; //value_a;
    value_b->last=Compiler.stack;
    Compiler.stack=value_b;
    break;
    case AST_POST_DEC:
    CompileAST(ast->unopArg,TRUE);
    value_a=IRValueClone(Compiler.stack);
    if(IsF64(value_a->type2)) {
        PushIRFReg(value_a->type2,GetFreeFReg);
        Assign(value_b=Compiler.stack,value_a);
        PushIRFReg(value_a->type2,GetFreeFReg);
        Assign(value_c=Compiler.stack,value_a);
        jit_fsubi(Compiler.jit,jit_FR(value_c->reg),jit_FR(value_c->reg),1.0);
        Assign(value_a,value_c);
        PopIRValue; //value_c
        value_b=IRValueClone(value_b); //Make unique
        PopIRValue; //value_b
    } else {
        PushIRReg(value_a->type2,GetFreeIReg);
        Assign(value_b=Compiler.stack,value_a);
        PushIRReg(value_a->type2,GetFreeIReg);
        Assign(value_c=Compiler.stack,value_a);
        mult=1;
        if(IsPtrType(value_a->type2)) {
            mult=DerefedType(value_a->type2)->sz;
        }
        jit_subi(Compiler.jit,jit_R(value_c->reg),jit_R(value_c->reg),mult);
        Assign(value_a,value_c);
        PopIRValue; //value_c
        value_b=IRValueClone(value_b); //MAke unique
        PopIRValue; //value_b
    }
    PopIRValue; //value_a;
    value_b->last=Compiler.stack;
    Compiler.stack=value_b;
    break;
    case AST_ARRAY_ACCESS:
    CompileAST(ast->arr_acc.base);
    CompileAST(ast->arr_acc.index);
    mult=DerefedType(Compiler.stack->last->type2)->sz;
    if(FoldIRInt) {
        PopIRValue(&value_b);
        hi=mult*value_b->int;
        Free(value_b);
        PopIRValue(&value_a);
        value_a=MoveToReg(value_a,0);
        jit_addi(Compiler.jit,jit_R(value_a->reg),jit_R(value_a->reg),hi);
        goto arr_acc_end;
    }
    value_b=IRValueClone(Compiler.stack);
    value_b=MoveToReg(value_b,1); //To integer
    value_a=IRValueClone(Compiler.stack->last);
    value_a=MoveToReg(value_a,0);
    jit_muli(Compiler.jit,jit_R(value_b->reg),jit_R(value_b->reg),mult);
    jit_addr(Compiler.jit,jit_R(value_a->reg),jit_R(value_a->reg),jit_R(value_b->reg));
    PopIRValue,PopIRValue;
    Free(value_b);
    arr_acc_end:
    if(DerefedType(value_a->type2)->type==HTT_ARRAY_TYPE) {
        PushIRReg(DerefedType(value_a->type2),value_a->reg);
    } else
        PushIRIndirX(DerefedType(value_a->type2),value_a->reg,0);
    Free(value_a);
    break;
    case AST_PRE_DEC:
    CompileAST(ast->unopArg,TRUE);
    value_a=IRValueClone(Compiler.stack);
    if(IsF64(value_a->type2)) {
        PushIRFReg(value_a->type2,GetFreeFReg);
        value_b=IRValueClone(Compiler.stack);
        Assign(value_b,value_a);
        jit_fsubi(Compiler.jit,jit_FR(value_b->reg),jit_FR(value_b->reg),1.0);
        PopIRValue;
    } else {
        PushIRReg(value_a->type2,GetFreeIReg);
        value_b=IRValueClone(Compiler.stack);
        Assign(value_b,value_a);
        mult=1;
        if(IsPtrType(value_a->type2)) {
            mult=DerefedType(value_a->type2)->sz;
        }
        jit_subi(Compiler.jit,jit_R(value_b->reg),jit_R(value_b->reg),mult);
        PopIRValue;
    }
    PopIRValue; //value_a;
    Assign(value_a,value_b);
    Free(value_a);
    value_b->last=Compiler.stack;
    Compiler.stack=value_b;
    break;
    case AST_PRE_INC:
    CompileAST(ast->unopArg,TRUE);
    value_a=IRValueClone(Compiler.stack);
    if(IsF64(value_a->type2)) {
        PushIRFReg(value_a->type2,GetFreeFReg);
        value_b=IRValueClone(Compiler.stack);
        Assign(value_b,value_a);
        jit_faddi(Compiler.jit,jit_FR(value_b->reg),jit_FR(value_b->reg),1.0);
        PopIRValue;
    } else {
        PushIRReg(value_a->type2,GetFreeIReg);
        value_b=IRValueClone(Compiler.stack);
        Assign(value_b,value_a);
        mult=1;
        if(IsPtrType(value_a->type2)) {
            mult=DerefedType(value_a->type2)->sz;
        }
        jit_addi(Compiler.jit,jit_R(value_b->reg),jit_R(value_b->reg),mult);
        PopIRValue;
    }
    PopIRValue; //value_a;
    Assign(value_a,value_b),Free(value_a);
    value_b->last=Compiler.stack;
    Compiler.stack=value_b;
    break;
    case AST_POS:
    CompileAST(ast->unopArg);
    break;
    case AST_NEG:
    CompileAST(ast->unopArg);
    value_a=IRValueClone(Compiler.stack);
    if(IsF64(value_a->type2)) {
        if(FoldIRF64) {
            Free(value_a);
            PopIRValue(&value_a);
            PushIRFlt(-value_a->flt);
            Free(value_a);
            break;
        }
        Free(value_a);
        PopIRValue(&value_a);
        value_a=MoveToFReg(value_a,0);
        jit_fnegr(Compiler.jit,jit_FR(value_a->reg),jit_FR(value_a->reg));
    } else {
        if(FoldIRInt) {
            Free(value_a);
            PopIRValue(&value_a);
            PushIRInt(-value_a->int);
            Free(value_a);
            break;
        }
        Free(value_a);
        PopIRValue(&value_a);
        value_a=MoveToReg(value_a,0);
        jit_negr(Compiler.jit,jit_R(value_a->reg),jit_R(value_a->reg));
    }
    value_a->last=Compiler.stack;
    Compiler.stack=value_a;
    break;
    case AST_LNOT:
    CompileAST(ast->unopArg);
    value_a=IRValueClone(Compiler.stack);
    if(IsF64(ast->unopArg->type2)) {
        if(FoldIRF64) {
            PopIRValue(&value_a);
            PushIRInt(!value_a->flt);
            Free(value_a);
            break;
        }
        value_a=MoveToFReg(value_a,0);
        PopIRValue;
        PushIRReg(GetType("I64i"),idx=GetFreeIReg);
        jit_movi(Compiler.jit,jit_R(idx),1);
        op1=jit_fbeqi(Compiler.jit,NULL,jit_FR(value_a->reg),0.0);
        jit_movi(Compiler.jit,jit_R(idx),0);
        jit_patch(Compiler.jit,op1);
        PopIRValue(&value_a);
    } else {
        if(FoldIRInt) {
            PopIRValue(&value_a);
            PushIRInt(!value_a->int);
            Free(value_a);
            break;
        }
        PopIRValue;
        value_a=MoveToReg(value_a,0);
        jit_eqi(Compiler.jit,jit_R(value_a->reg),jit_R(value_a->reg),0);
    }
    value_a->last=Compiler.stack;
    Compiler.stack=value_a;
    break;
    case AST_BNOT:
    CompileAST(ast->unopArg);
    if(IsF64(ast->unopArg->type2)) {
        //TODO
    } else {
        if(FoldIRInt) {
            PopIRValue(&value_a);
            PushIRInt(~value_a->int);
            Free(value_a);
            break;
        }
        PopIRValue(&value_a);
        value_a=MoveToReg(value_a,0);
        jit_notr(Compiler.jit,jit_R(value_a->reg),jit_R(value_a->reg));
    }
    value_a->last=Compiler.stack;
    Compiler.stack=value_a;
    break;
    case AST_DERREF:
    CompileAST(ast->unopArg);
    PopIRValue(&value_a);
    value_a=MoveToReg(value_a,0);
    if(DerefedType(value_a->type2)->type==HTT_ARRAY_TYPE)
        PushIRReg(DerefedType(value_a->type2),value_a->reg);
    else
        PushIRIndirX(DerefedType(value_a->type2),value_a->reg,0);
    Free(value_a);
    break;
    case AST_ADDROF:
    CompileAST(ast->unopArg,TRUE,TRUE);
    if(Compiler.stack->type2->type==HTT_ARRAY_TYPE) {
        if(Compiler.stack->type!=IRV_REG) {
            "STRANGE ADDROF\n";
            throw('Compile');
        }
        Compiler.stack->type2++;
        break;
    }
    if(Compiler.stack->type!=IRV_INDIRX) {
        "NEEDS INDIRX\n";
        throw('Compile');
    }
    PopIRValue(&value_a);
    PushIRReg(ast->type2,idx=value_a->indirx.reg);
    if(value_a->indirx.offset)
        jit_addi(Compiler.jit,jit_R(idx),jit_R(idx),value_a->indirx.offset);
    Free(value_a);
    break;
    case AST_SIZEOF:
    PushIRInt(ast->type2->sz);
    break;
    case AST_IMPLICIT_TC:
    break;
    case AST_EXPLICIT_TC:
    CompileAST(ast->typecast.base,pass_by_ref);
    if(IsF64(Compiler.stack->type2)) {
        if(IsF64(ast->typecast.to_type))
            Compiler.stack->type2=ast->typecast.to_type;
        else if(IsInteger(ast->typecast.to_type)) {
            if(FoldIRF64) {
                PopIRValue(&value_a);
                PushIRInt(value_a->flt(I64));
            } else {
                PopIRValue(&value_a);
                value_a=MoveToFReg(value_a,0);
                LoadSymPtrIntoReg("Bit4BitF64ToU64",0);
                PushIRReg(ast->typecast.to_type,idx=GetFreeIReg);
                jit_prepare(Compiler.jit);
                jit_fputargr(Compiler.jit,jit_FR(value_a->reg),8);
                jit_callr(Compiler.jit,jit_R(0));
                jit_retval(Compiler.jit,jit_R(idx));
            }
            Free(value_a);
        }
    } else if(IsInteger(Compiler.stack->type2)) {
        if(IsF64(ast->typecast.to_type)) {
            if(FoldIRInt) {
                PopIRValue(&value_a);
                PushIRFlt(value_a->int(F64));
            } else {
                PopIRValue(&value_a);
                value_a=MoveToReg(value_a,0);
                LoadSymPtrIntoReg("Bit4BitU64ToF64",0);
                PushIRFReg(ast->typecast.to_type,idx=GetFreeFReg);
                jit_prepare(Compiler.jit);
                jit_putargr(Compiler.jit,jit_R(value_a->reg));
                jit_callr(Compiler.jit,jit_R(0));
                jit_fretval(Compiler.jit,jit_FR(idx),8);
            }
            Free(value_a);
        } else if(IsInteger(ast->typecast.to_type)) {
            Compiler.stack->type2=ast->typecast.to_type;
        }
    }
    break;
    case AST_FUNC_CALL:
    CompileAST(ast->func_call.base,,TRUE);
    value_a=IRValueClone(Compiler.stack);
    if(IsPtrType(value_a->type2)) {
        type1=DerefedType(value_a->type2);
    } else {
        if(value_a->type==IRV_INDIRX) {
            value_a->reg=value_a->indirx.reg;
            value_a->type=IRV_REG;
        }
        type1=value_a->type2;
    }
    cnt=MSize(type1->arg_types)/sizeof(CType*);
    for(idx=0;idx!=cnt;idx++) {
        if(MSize(ast->func_call.args)/sizeof(AST*)>idx&&ast->func_call.args[idx]) {
            CompileAST(ast->func_call.args[idx]);
            PopIRValue(&fargs[idx]);
            if(IsF64(type1->arg_types[idx])) {
                fargs[idx]=MoveToFReg(fargs[idx],0);
            }else
                fargs[idx]=MoveToReg(fargs[idx],0);
            fargs[idx].last=Compiler.stack;
            Compiler.stack=fargs[idx];
        } else {
            if(type1->arg_values[idx]) {
                if(type1->arg_values[idx]==&Lastclass) {
                    if(ast->func_call.args[idx-1])
                        type2=ast->func_call.args[idx-1]->type2;
                    else
                        type2=type1->arg_types[idx-1];
                    lc_type:
                    switch(type2->type) {
                        case HTT_ARRAY_TYPE:
                        type2=DerefedType(type2);
                        goto lc_type;
                        case HTT_FUNC_TYPE:
                        type2=type2->ret_type;
                        goto lc_type;
                        default:
                        if(type2->star_cnt) {
                            type2-=type2->star_cnt;
                            goto lc_type;
                        }
                    }
                    PushIRString(type2->str,MSize(type2->str));
                    fargs[idx]=Compiler.stack;
                } else if(!Compiler.aot_mode) {
                    if(IsF64(type1->arg_types[idx])) {
                        PushIRFReg(GetType("F64"),GetFreeFReg);
                        jit_fmovi(Compiler.jit,jit_FR(Compiler.stack->reg),type1->arg_values[idx][0](F64));
                        fargs[idx]=Compiler.stack;
                    } else {
                        PushIRReg(type1->arg_types[idx],GetFreeIReg);
                        jit_movi(Compiler.jit,jit_R(Compiler.stack->reg),type1->arg_values[idx][0]);
                        fargs[idx]=Compiler.stack;
                    }
                } else if(Compiler.aot_mode) {
                    if(IsF64(type1->arg_types[idx])) {
                        PushIRFReg(GetType("F64"),GetFreeFReg);
                        LoadSymPtrIntoReg(type1->aot_arg_names[idx],0);
                        jit_fldr(Compiler.jit,jit_FR(Compiler.stack->reg),jit_R(0),8);
                        fargs[idx]=Compiler.stack;
                    } else {
                        PushIRReg(type1->arg_types[idx],GetFreeIReg);
                        LoadSymPtrIntoReg(type1->aot_arg_names[idx],0);
                        jit_ldr(Compiler.jit,jit_R(Compiler.stack->reg),jit_R(0),8);
                        fargs[idx]=Compiler.stack;
                    }
                }
            } else {
                "???\n";
                throw('Compile');
            }
        }
    }
    cnt=MSize(type1->arg_types)/sizeof(CType*);
    for(;idx<cnt;idx++) {
        if(type1->arg_values[idx]==&Lastclass) {
            if(ast->func_call.args[idx-1])
                type2=ast->func_call.args[idx-1]->type2;
            else
                type2=type1->arg_types[idx-1];
            lc_type2:
            switch(type2->type) {
                case HTT_ARRAY_TYPE:
                type2=DerefedType(type2);
                goto lc_type2;
                case HTT_FUNC_TYPE:
                type2=type2->ret_type;
                goto lc_type2;
                default:
                if(type2->star_cnt) {
                    type2-=type2->star_cnt;
                    goto lc_type2;
                }
                }
            PushIRString(type2->str,MSize(type2->str));
        } else if(!Compiler.aot_mode) {
            if(IsF64(type1->arg_types[idx])) {
                PushIRFReg(type1->arg_types[idx],GetFreeFReg);
                jit_fmovi(Compiler.jit,jit_FR(Compiler.stack->reg),type1->arg_values[idx][0](F64));;
            } else {
                PushIRReg(type1->arg_types[idx],GetFreeIReg);
                jit_movi(Compiler.jit,jit_R(Compiler.stack->reg),type1->arg_values[idx][0]);
            }
        } else if(Compiler.aot_mode) {
            if(IsF64(type1->arg_types[idx])) {
                PushIRFReg(GetType("F64"),GetFreeFReg);
                LoadSymPtrIntoReg(type1->aot_arg_names[idx],0);
                jit_fldr(Compiler.jit,jit_FR(Compiler.stack->reg),jit_R(0),8);
                fargs[idx]=Compiler.stack;
            } else {
                PushIRReg(type1->arg_types[idx],GetFreeIReg);
                LoadSymPtrIntoReg(type1->aot_arg_names[idx],0);
                jit_ldr(Compiler.jit,jit_R(Compiler.stack->reg),jit_R(0),8);
                fargs[idx]=Compiler.stack;
            }
        }
        fargs[idx]=Compiler.stack;
    }
    if(type1->has_vargs) {
        offset=0;
        cnt=MSize(ast->func_call.args)/sizeof(AST*);
        //Look here at the meta sponge,he has something youll want to hear
        //   _______
        //  /^     ^\
        // | *-[w]-* | <lo is used to point to the register containing argv>
        // |         |
        // |_/     \-|
        //  \_______/
        //
        lo=jit_allocai(Compiler.jit,8*(cnt-idx));
        for(;idx!=cnt;idx++,++offset) {
            CompileAST(ast->func_call.args[idx]);
            PopIRValue(&fargs[idx]);
            if(IsF64(ast->func_call.args[idx]->type2)) {
                fargs[idx]=MoveToFReg(fargs[idx],0);
                jit_fstxi(Compiler.jit,lo+offset*8,R_FP,jit_FR(fargs[idx]->reg),8);
            } else {
                fargs[idx]=MoveToReg(fargs[idx],0);
                jit_stxi(Compiler.jit,lo+offset*8,R_FP,jit_R(fargs[idx]->reg),8);
            }
        }
        //Look at metasponge above
        jit_addi(Compiler.jit,jit_R(offset=GetFreeIReg),R_FP,lo);
        lo=offset;
    }
    jit_prepare(Compiler.jit);
    cnt=MSize(type1->arg_types)/sizeof(CType*);
    for(idx=0;idx<cnt;idx++) {
        if(fargs[idx].type==IRV_FREG) {
            jit_fputargr(Compiler.jit,jit_FR(fargs[idx]->reg),8);
        } else {
            jit_putargr(Compiler.jit,jit_R(fargs[idx]->reg));
        }
    }
    if(type1->has_vargs) {
        jit_putargi(Compiler.jit,MSize(ast->func_call.args)/sizeof(AST*)-cnt);
        jit_putargr(Compiler.jit,jit_R(lo));
    }
    idx=cnt;
    while(--idx>=0)
        PopIRValue;
    PopIRValue;
    value_a=MoveToReg(value_a,0);
    jit_callr(Compiler.jit,jit_R(value_a->reg));
    Free(value_a);
    if(IsF64(type1(CType*)->ret_type)) {
        PushIRFReg(GetType("F64"),GetFreeFReg);
        jit_fretval(Compiler.jit,jit_FR(Compiler->stack->reg),8);
    } else if(IsInteger(type1->ret_type)) {
        PushIRReg(type1->ret_type,GetFreeIReg);
        jit_retval(Compiler.jit,jit_R(Compiler->stack->reg));
    }
    Compiler.stack->type2=type1->ret_type;
    break;
    case AST_IF:
    CompileAST(ast->if_stmt->cond);
    PopIRValue(&value_a);
    value_a=MoveToReg(value_a,0);
    op1=jit_beqi(Compiler.jit,NULL,jit_R(value_a->reg),0);
    Free(value_a);
    if(ast->if_stmt->body) {
        CompileAstWithBreakpoint(ast->if_stmt->body);
        PopIRValue;
    }
    if(ast->if_stmt->el) {
        op2=jit_jmpi(Compiler.jit,NULL);
        jit_patch(Compiler.jit,op1);
        CompileAstWithBreakpoint(ast->if_stmt->el);
        PopIRValue;
        jit_patch(Compiler.jit,op2);
    } else {
        jit_patch(Compiler.jit,op1);
    }
    PushIRInt(0);
    break;
    case AST_FOR:
    if(ast->for_stmt->init)
        CompileAstWithBreakpoint(ast->for_stmt->init),PopIRValue;
    op1=jit_get_label(Compiler.jit);
    if(ast->for_stmt->cond) {
        CompileAstWithBreakpoint(ast->for_stmt->cond);
        PopIRValue(&value_a);
        value_a=MoveToReg(value_a,0);
        op2=jit_beqi(Compiler.jit,NULL,jit_R(value_a->reg),0);
    } else {
        //Nothing to do
    }
    Free(value_a);
    breaks=MAlloc(sizeof(*breaks));
    breaks->breaks=VectorNew;
    breaks->last=Compiler.cur_breaks;
    Compiler.cur_breaks=breaks;
    CompileAST(ast->for_stmt->body);
    Compiler.cur_breaks=breaks->last;
    PopIRValue;
    if(ast->for_stmt->inc)
        CompileAST(ast->for_stmt->inc),PopIRValue;
    jit_jmpi(Compiler.jit,op1);
    if(ast->for_stmt->cond) {
        jit_patch(Compiler.jit,op2);
    }
    while(breaks->breaks->size) {
        op3=VectorPop(breaks->breaks);
        jit_patch(Compiler.jit,op3);
    }
    VectorFree(breaks->breaks);
    Free(breaks);
    PushIRInt(0);
    break;
    case AST_DO:
    op1=jit_get_label(Compiler.jit);
    breaks=MAlloc(sizeof(*breaks));
    breaks->breaks=VectorNew;
    breaks->last=Compiler.cur_breaks;
    Compiler.cur_breaks=breaks;
    CompileAstWithBreakpoint(ast->do_stmt.body);
    PopIRValue;
    Compiler->cur_breaks=breaks->last;
    CompileAstWithBreakpoint(ast->do_stmt.cond);
    PopIRValue(&value_a);
    value_a=MoveToReg(value_a,0);
    jit_bnei(Compiler.jit,op1,jit_R(value_a->reg),0);
    Free(value_a);
    while(breaks->breaks->size) {
        op3=VectorPop(breaks->breaks);
        jit_patch(Compiler.jit,op3);
    }
    Compiler.cur_breaks=breaks->last;
    VectorFree(breaks->breaks);
    Free(breaks);
    PushIRInt(0);
    break;
    case AST_WHILE:
    op1=jit_get_label(Compiler.jit);
    CompileAstWithBreakpoint(ast->while_stmt.cond);
    PopIRValue(&value_a);
    value_a=BoolifyValue(value_a);
    op2=jit_beqi(Compiler.jit,NULL,jit_R(value_a->reg),0);
    Free(value_a);
    breaks=MAlloc(sizeof(*breaks));
    breaks->breaks=VectorNew;
    breaks->last=Compiler.cur_breaks;
    Compiler.cur_breaks=breaks;
    CompileAstWithBreakpoint(ast->while_stmt.body);
    PopIRValue;
    jit_jmpi(Compiler.jit,op1);
    Compiler.cur_breaks=breaks->last;
    jit_patch(Compiler.jit,op2);
    while(breaks->breaks->size) {
        op3=VectorPop(breaks->breaks);
        jit_patch(Compiler.jit,op3);
    }
    VectorFree(breaks->breaks);
    Free(breaks);
    PushIRInt(0);
    break;
    case AST_META_DATA:
    break;
    case AST_SWITCH:
    CompileAstWithBreakpoint(ast->switch_stmt.cond); //Conditon
    value_a=IRValueClone(Compiler.stack);
    value_a=MoveToReg(value_a,0);
    value_a->last=Compiler.stack;
    Compiler.stack=value_a;
    value_c=IRValueClone(value_a);
    //Make a backup register for our condition
    PushIRReg(HashFind("I64i",Compiler.globals),GetFreeIReg); //subswit
    Assign(Compiler.stack,value_a);
    value_a=IRValueClone(Compiler.stack);
    swit_table=MAlloc(sizeof(*swit_table));
    ScanForCases(ast->switch_stmt.body,&lo,&hi,&has_dft);
    table=__CreateCaseTable(ast->switch_stmt.body);
    swit_table->last=Compiler.swit_table;
    swit_table->lo=lo,swit_table->hi=hi;
    Compiler.swit_table=swit_table;
    breaks=MAlloc(sizeof(*breaks));
    breaks->breaks=VectorNew;
    breaks->last=Compiler.cur_breaks;
    Compiler.cur_breaks=breaks;
    //
    op1=jit_blti(Compiler.jit,NULL,jit_R(value_a->reg),lo);
    op2=jit_bgti(Compiler.jit,NULL,jit_R(value_a->reg),hi);
    swit_table->dft1=op1,swit_table->dft2=op2;
    swit_table->subswit_var_offset=jit_allocai(Compiler.jit,8);
    jit_movi(Compiler.jit,jit_R(0),0);
    jit_stxi(Compiler.jit,swit_table->subswit_var_offset,R_FP,jit_R(0),8);
    swit_table->enter_point=jit_get_label(Compiler.jit);
    swit_table->table=MAlloc(sizeof(U0*)*(hi-lo+1));
    swit_table->ast_table=table;
    Assign(value_a,value_c);
    jit_ref_data(Compiler.jit,jit_R(GetFreeIReg),op3);
    jit_subi(Compiler.jit,jit_R(value_a->reg),jit_R(value_a->reg),lo);
    //
    op4=table[idx=hi-lo];
    for(idx;idx>=0;) {
        idx2=idx;
        while(op4==table[idx]&&idx>=0) {
            --idx;
        }
        op3=jit_bgti(Compiler.jit,NULL,jit_R(value_a->reg),idx);
        swit_table->table[idx2]=op3;
        if(idx!=-1) op4=table[idx];
    }
    //
    CompileAstWithBreakpoint(ast->switch_stmt.body);

    if(!has_dft) {
        jit_patch(Compiler.jit,op1);
        jit_patch(Compiler.jit,op2);
        idx=hi-lo+1;
        while(--idx>=0) {
            if((!table[idx])&&swit_table->table[idx])
                jit_patch(Compiler.jit,swit_table->table[idx]);
            else if(table[idx]&&table[idx]->type==AST_DFT&&swit_table->table[idx])
                jit_patch(Compiler.jit,swit_table->table[idx]);
        }
    }
    Free(table);
    PopIRValue; //Body
    PopIRValue; //Condition clone
    PopIRValue; //Condition
    PopIRValue;
    Compiler.cur_breaks=breaks->last;
    Compiler.swit_table=swit_table->last;
    Free(swit_table->table);
    Free(swit_table);
    while(breaks->breaks->size) {
        op1=VectorPop(breaks->breaks);
        jit_patch(Compiler.jit,op1);
    }
    VectorFree(breaks->breaks);
    Free(breaks),Free(value_a),Free(value_c);
    PushIRInt(0);
    break;
    case AST_SUBSWIT:
    if(!Compiler.swit_table) {
        "STRAY SUBSIW;\n";
        throw('Compile');
    }
    breaks=MAlloc(sizeof(*breaks));
    breaks->breaks=VectorNew;
    breaks->last=Compiler.cur_breaks;
    breaks->start_code_label=jit_get_label(Compiler.jit);
    //
    Compiler.cur_breaks=breaks;
    jit_movi(Compiler.jit,jit_R(0),1);
    jit_stxi(Compiler.jit,Compiler.swit_table->subswit_var_offset,R_FP,jit_R(0),8);
    if(ast->sub_swit.start_code) {
        CompileAstWithBreakpoint(ast->sub_swit.start_code);
        PopIRValue;
    }
    jit_jmpi(Compiler.jit,Compiler.swit_table->enter_point);
    if(ast->sub_swit.body) {
        CompileAstWithBreakpoint(ast->sub_swit.body);
        PopIRValue;
    }
    Compiler.cur_breaks=breaks->last;
    while(breaks->breaks->size) {
        op1=VectorPop(breaks->breaks);
        jit_patch(Compiler.jit,op1);
    }
    VectorFree(breaks->breaks);
    Free(breaks);
    PushIRInt(0);
    break;
    case AST_BREAK:
    VectorPush(Compiler.cur_breaks->breaks,jit_jmpi(Compiler.jit,NULL));
    PushIRInt(0);
    break;
    case AST_DFT:
    jit_patch(Compiler.jit,Compiler.swit_table->dft1);
    jit_patch(Compiler.jit,Compiler.swit_table->dft2);
    cnt=Compiler.swit_table->hi-Compiler.swit_table->lo+1;
    while(--cnt>=0) {
        if(Compiler.swit_table->ast_table[cnt]==ast)
            if(Compiler.swit_table->table[cnt])
                jit_patch(Compiler.jit,Compiler.swit_table->table[cnt]);
    }
    //Check if in subswit
    if(Compiler.cur_breaks->start_code_label) {
        jit_ldxi(Compiler.jit,jit_R(0),R_FP,Compiler.swit_table->subswit_var_offset,8);
        jit_beqi(Compiler.jit,Compiler.cur_breaks->start_code_label,jit_R(0),0);
    }
    PushIRInt(0);
    break;
    case AST_CASE:
    lo=ast->cs._li;
    hi=ast->cs._hi;
    for(;lo<=hi;lo++)
        if(Compiler.swit_table->table[lo-Compiler.swit_table->lo])
            jit_patch(Compiler.jit,Compiler.swit_table->table[lo-Compiler.swit_table->lo]);
    //Check if in subswit
    if(Compiler.cur_breaks->start_code_label) {
        jit_ldxi(Compiler.jit,jit_R(0),R_FP,Compiler.swit_table->subswit_var_offset,8);
        jit_beqi(Compiler.jit,Compiler.cur_breaks->start_code_label,jit_R(0),0);
    }
    PushIRInt(0);
    break;
    case AST_RETURN:
    if(!Compiler.ret_type) {
        "NO REUTRN TYPE;\n";
        throw('Compile');
    }
    if(IsInteger(Compiler.ret_type)) {
        if(!ast->unopArg) {
            #if IsWindows
            if(Compiler.debug_mode) {
                jit_prepare(Compiler.jit);
                jit_call(Compiler.jit,&WindowsLeaveFunc);
            }
            #endif
            jit_reti(Compiler.jit,0);
            PushIRInt(0);
            break;
        }
        CompileAST(ast->unopArg);
        PopIRValue(&value_a);
        value_a=MoveToReg(value_a,0);
        #if IsWindows
        if(Compiler.debug_mode) {
            jit_prepare(Compiler.jit);
            jit_call(Compiler.jit,&WindowsLeaveFunc);
        }
        #endif
        jit_retr(Compiler.jit,jit_R(value_a->reg));
    } else if(IsF64(Compiler.ret_type)) {
        if(!ast->unopArg) {
            #if IsWindows
            if(Compiler.debug_mode) {
                jit_prepare(Compiler.jit);
                jit_call(Compiler.jit,&WindowsLeaveFunc);
            }
            #endif
            jit_freti(Compiler.jit,0.0,8);
            PushIRInt(0);
            break;
        }
        CompileAstWithBreakpoint(ast->unopArg);
        PopIRValue(&value_a);
        value_a=MoveToFReg(value_a,0);
        #if IsWindows
        if(Compiler.debug_mode) {
            jit_prepare(Compiler.jit);
            jit_call(Compiler.jit,&WindowsLeaveFunc);
        }
        #endif
        jit_fretr(Compiler.jit,jit_FR(value_a->reg),8);
    }
    PushIRInt(0);
    break;
    case AST_TYPE:
    //What do you want me to do
    PushIRInt(0);
    break;
    case AST_GOTO:
    lab=HashFind(ast->name,Compiler.labels);
    if(!lab) {
        "MISSING LABEL\n";
        throw('Compile');
    }
    if(lab->type!=HTT_LABEL) {
        "JUMP TO NONALBEL\n";
        throw('Compile');
    }
    if(lab->op) {
        jit_jmpi(Compiler.jit,lab->op);
    } else {
        VectorPush(lab->refs,jit_jmpi(Compiler.jit,NULL));
    }
    PushIRInt(0);
    break;
    case AST_LABEL:
    lab=HashFind(ast->name,Compiler.labels);
    if(!lab) {
        "WHATY LABLE\n";
        throw('Compile');
    }
    if(lab->type!=HTT_LABEL||lab->op) {
        "NONO_ALBLE\n";
        throw('Compile');
    }
    jit_dump_ptr(Compiler.jit,&lab->ptr);
    while(lab->refs->size) {
        op1=VectorPop(lab->refs);
        jit_patch(Compiler.jit,op1);
    }
    lab->op=jit_get_label(Compiler.jit);
    PushIRInt(0);
    break;
    case AST_TRY:
    //If we are debugging on windows,make a spot to store our call_stack depth,because I am sick of trying to make a SEH compliant thing
    #if IsWindows
    if(Compiler.debug_mode) {
        lo=jit_allocai(Compiler.jit,8);
        jit_prepare(Compiler.jit);
        jit_call(Compiler.jit,&WindowsCallStackSize);
        jit_retval(Compiler.jit,jit_R(0));
        jit_stxi(Compiler.jit,lo,R_FP,jit_R(0),8);
    }
    #endif
    LoadSymPtrIntoReg("EnterTry",0);
    jit_prepare(Compiler.jit);
    jit_callr(Compiler.jit,jit_R(0));
    jit_retval(Compiler.jit,jit_R(0));
    LoadSymPtrIntoReg("HCSetJmp",1);
    jit_prepare(Compiler.jit);
    jit_putargr(Compiler.jit,jit_R(0));
    jit_putargi(Compiler.jit,1);
    jit_callr(Compiler.jit,jit_R(1));
    jit_retval(Compiler.jit,jit_R(0));
    op1=jit_bnei(Compiler.jit,NULL,jit_R(0),0);
    CompileAstWithBreakpoint(ast->try_block.body);
    PopIRValue;
    LoadSymPtrIntoReg("PopTryFrame",0);
    jit_prepare(Compiler.jit);
    jit_callr(Compiler.jit,jit_R(0));
    op2=jit_jmpi(Compiler.jit,NULL);
    jit_patch(Compiler.jit,op1);
    //If debugging on windows,restore call stack size
    #if IsWindows
    if(Compiler.debug_mode) {
        jit_ldxi(Compiler.jit,jit_R(0),R_FP,lo,8);
        jit_prepare(Compiler.jit);
        jit_putargr(Compiler.jit,jit_R(0));
        jit_call(Compiler.jit,&WindowsSetCallStackSize);
    }
    #endif
    CompileAstWithBreakpoint(ast->try_block.cat);
    PopIRValue;
    LoadSymPtrIntoReg("ExitCatch",0);
    jit_prepare(Compiler.jit);
    jit_callr(Compiler.jit,jit_R(0));
    jit_patch(Compiler.jit,op2);
    PushIRInt(0);
    break;
    case AST_LASTCLASS:
    //Is symbolic
    break;
    case AST_RANGE:
    to_patch=VectorNew;
    PushIRReg(GetType("I64i"),hi=GetFreeIReg);
    jit_movi(Compiler.jit,jit_R(hi),1);
    value_c=Compiler.stack;
    cnt=MSize(ast->range.values)/sizeof(AST*);
    CompileAST(ast->range.values[0]);
    op1=NULL;
    for(idx=1;idx<cnt;idx++) {
        if(op1) jit_patch(Compiler.jit,op1);
        CompileAST(ast->range.values[idx]);
        if(IsF64(Compiler.stack->last->type2)||IsF64(Compiler.stack->type2)) {
            value_a=MoveToFReg(IRValueClone(Compiler.stack->last),0);
            value_b=MoveToFReg(IRValueClone(Compiler.stack),1);
            PopIRValue,PopIRValue;
            switch(ast->range.types[idx-1]) {
                case AST_LT:
                op1=jit_fbltr(Compiler.jit,NULL,jit_FR(value_a->reg),jit_FR(value_b->reg));
                break;
                case AST_GT:
                op1=jit_fbgtr(Compiler.jit,NULL,jit_FR(value_a->reg),jit_FR(value_b->reg));
                break;
                case AST_LE:
                op1=jit_fbler(Compiler.jit,NULL,jit_FR(value_a->reg),jit_FR(value_b->reg));
                break;
                case AST_GE:
                op1=jit_fbger(Compiler.jit,NULL,jit_FR(value_a->reg),jit_FR(value_b->reg));
                break;
            }
            Free(value_a);
            value_b->last=Compiler.stack;
            Compiler.stack=value_b;
        } else if(!IsSigned(Compiler.stack->last->type2)||!IsSigned(Compiler.stack->type2)) {
            //Types below 64bits are promoted so treat them as 64bit signed if small
            if((Compiler.stack->last->type2->sz==8&&!IsSigned(Compiler.stack->last->type2))||(Compiler.stack->type2->sz==8&&!IsSigned(Compiler.stack->type2)))
                ;
            else
                goto cmp_signed;
            value_a=MoveToReg(IRValueClone(Compiler.stack.last),0);
            value_b=MoveToReg(IRValueClone(Compiler.stack),1);
            PopIRValue,PopIRValue;
            switch(ast->range.types[idx-1]) {
                case AST_LT:
                op1=jit_bltr_u(Compiler.jit,NULL,jit_R(value_a->reg),jit_R(value_b->reg));
                break;
                case AST_GT:
                op1=jit_bgtr_u(Compiler.jit,NULL,jit_R(value_a->reg),jit_R(value_b->reg));
                break;
                case AST_LE:
                op1=jit_bler_u(Compiler.jit,NULL,jit_R(value_a->reg),jit_R(value_b->reg));
                break;
                case AST_GE:
                op1=jit_bger_u(Compiler.jit,NULL,jit_R(value_a->reg),jit_R(value_b->reg));
                break;
            }
            Free(value_a);
            value_b->last=Compiler.stack;
            Compiler.stack=value_b;
        } else {
            cmp_signed:
            value_a=MoveToReg(IRValueClone(Compiler.stack.last),0);
            value_b=MoveToReg(IRValueClone(Compiler.stack),1);
            PopIRValue,PopIRValue;
            switch(ast->range.types[idx-1]) {
                case AST_LT:
                op1=jit_bltr(Compiler.jit,NULL,jit_R(value_a->reg),jit_R(value_b->reg));
                break;
                case AST_GT:
                op1=jit_bgtr(Compiler.jit,NULL,jit_R(value_a->reg),jit_R(value_b->reg));
                break;
                case AST_LE:
                op1=jit_bler(Compiler.jit,NULL,jit_R(value_a->reg),jit_R(value_b->reg));
                break;
                case AST_GE:
                op1=jit_bger(Compiler.jit,NULL,jit_R(value_a->reg),jit_R(value_b->reg));
                break;
            }
            Free(value_a);
            value_b->last=Compiler.stack;
            Compiler.stack=value_b;
        }
        jit_movi(Compiler.jit,jit_R(hi),0);
        VectorPush(to_patch,jit_jmpi(Compiler.jit,NULL));
    }
    jit_patch(Compiler.jit,op1);
    while(to_patch->size) {
        op2=VectorPop(to_patch);
        jit_patch(Compiler.jit,op2);
    }
    PopIRValue;
    VectorFree(to_patch);
    break;
    case AST_ASSIGN_MOD:
    CompileAST(ast->binop.a,TRUE);
    value_a=IRValueClone(Compiler.stack);
    CompileAST(ast->binop.b);
    value_b=IRValueClone(Compiler.stack);
    if(IsF64(value_a->type2)) {
        value_b=MoveToFReg(value_b,1);
        value_c=MoveToFReg(IRValueClone(value_a),0);
        Assign(value_c,value_a);
        LoadSymPtrIntoReg("FMod",0);
        jit_prepare(Compiler.jit);
        jit_fputargr(Compiler.jit,jit_FR(value_c->reg),8);
        jit_fputargr(Compiler.jit,jit_FR(value_b->reg),8);
        jit_callr(Compiler.jit,jit_R(0))    ;
        jit_fretval(Compiler.jit,jit_FR(value_b->reg),8);
        Assign(value_a,value_b);
    } else {
        value_c=MoveToReg(IRValueClone(value_a),0);
        if(FoldIRInt) {
            jit_modi(Compiler.jit,jit_R(value_c->reg),jit_R(value_c->reg),value_b->int);
            Assign(value_a,value_c);
            PopIRValue,PopIRValue;
            Free(value_b),Free(value_c);
            goto amod_end;
        }
        value_b=MoveToFReg(value_b,1);
        jit_modr(Compiler.jit,jit_R(value_b->reg),jit_R(value_c->reg),jit_R(value_b->reg));
        Assign(value_a,value_b);
    }
    PopIRValue,PopIRValue;
    Free(value_c),Free(value_b);
    amod_end:
    value_a->last=Compiler.stack;
    Compiler.stack=value_a;
    goto asn_end;
    case AST_NAME:
    if(Compiler.in_func) {
        var=HashFind(ast->name,Compiler.locals);
        if(!var) goto glob_var;
        if(var->type!=HTT_VAR) {
            "NOT A VAR\n";
            throw('Compile');
        }
        if(pass_by_ref) {
            if(var->is_extern_ptr) {
                PushIRIndirX(var->hc_type,idx=GetFreeIReg,0);
                jit_movi(Compiler.jit,jit_R(idx),var->extern_ptr);
            } else if(var->is_static&&var->hc_type->type!=HTT_ARRAY_TYPE) {
                PushIRIndirX(var->hc_type,idx=GetFreeIReg,0);
                VectorPush(var->static_refs,jit_ref_data(Compiler.jit,jit_R(idx),NULL));
            } else if(var->is_static&&var->hc_type->type==HTT_ARRAY_TYPE) {
                PushIRReg(var->hc_type,idx=GetFreeIReg);
                VectorPush(var->static_refs,jit_ref_data(Compiler.jit,jit_R(idx),NULL));
            } else if(var->isReg) {
                if(IsF64(var->hc_type))
                    PushIRFReg(var->hc_type,var->reg);
                else
                    PushIRReg(var->hc_type,var->reg);
            } else if(var->hc_type->type!=HTT_ARRAY_TYPE) {
                PushIRIndirX(var->hc_type,idx=GetFreeIReg,var->frame_off);
                jit_movr(Compiler.jit,jit_R(idx),R_FP);
            } else {
                PushIRReg(var->hc_type,idx=GetFreeIReg);
                jit_addi(Compiler.jit,jit_R(idx),R_FP,var->frame_off);
            }
            break;
        } else {
            if(var->is_extern_ptr) {
                if(IsF64(var->hc_type)) {
                    PushIRFReg(var->hc_type,GetFreeFReg);
                } else {
                    PushIRReg(var->hc_type,GetFreeIReg);
                }
                PushIRIndirX(var->hc_type,idx=GetFreeIReg,0);
                jit_movi(Compiler.jit,jit_R(idx),var->extern_ptr);
            } else if(var->is_static) {
                if(IsF64(var->hc_type)) {
                    PushIRFReg(var->hc_type,GetFreeFReg);
                } else {
                    PushIRReg(var->hc_type,GetFreeIReg);
                }
                if(var->hc_type->type!=HTT_ARRAY_TYPE)
                    PushIRIndirX(var->hc_type,idx=GetFreeIReg,0);
                else
                    PushIRReg(var->hc_type,idx=GetFreeIReg);
                VectorPush(var->static_refs,jit_ref_data(Compiler.jit,jit_R(idx),NULL));
            } else if(var->isReg) {
                if(IsF64(var->hc_type)) {
                    PushIRFReg(var->hc_type,GetFreeFReg);
                    PushIRFReg(var->hc_type,var->reg);
                } else {
                    PushIRReg(var->hc_type,GetFreeIReg);
                    PushIRReg(var->hc_type,var->reg);
                }
            } else if(var->hc_type->type!=HTT_ARRAY_TYPE) {
                if(IsF64(var->hc_type))
                    PushIRFReg(var->hc_type,GetFreeFReg);
                else if(IsInteger(var->hc_type)||IsPtrType(var->hc_type))
                    PushIRReg(var->hc_type,GetFreeIReg);
                else {
                    PushIRIndirX(var->hc_type,idx=GetFreeIReg,var->frame_off);
                    jit_movr(Compiler.jit,jit_R(idx),R_FP);
                    break;
                }
                PushIRIndirX(var->hc_type,idx=GetFreeIReg,var->frame_off);
                jit_movr(Compiler.jit,jit_R(idx),R_FP);
            } else {
                PushIRReg(var->hc_type,idx=GetFreeIReg);
                jit_addi(Compiler.jit,jit_R(idx),R_FP,var->frame_off);
                break;
            }
            Assign(Compiler.stack->last,Compiler.stack);
            PopIRValue;
            break;
        }
    }
    glob_var:
    func=var=HashFind(ast->name,Compiler.globals);
    if(var&&var->type!=HTT_VAR&&var->type!=HTT_FUNC) {
        "NOYT FUNC OR VAR\n";
        throw('Compile');
    }
    if(var&&var->type==HTT_FUNC) {
        if(dont_call_lone_name) {
            LoadSymPtrIntoReg(func->str,idx=GetFreeIReg);
            PushIRIndirX(func->hc_type,idx,0);
        } else {
            type1=func->hc_type;
            cnt=MSize(type1->arg_types)/sizeof(AST*);
            while(--cnt>=0) {
                if(!type1->arg_values[cnt]) {
                        "Expected dft\n";
                    throw('Compile');
                }
                if(!Compiler.aot_mode) {
                    if(IsF64(type1->arg_types[cnt]))
                        PushIRFlt(type1->arg_values[cnt][0](F64));
                    else
                        PushIRInt(type1->arg_values[cnt][0](I64));
                } else {
                    if(IsF64(type1->arg_types[cnt])) {
                        PushIRFReg(GetType("F64"),GetFreeFReg);
                        LoadSymPtrIntoReg(type1->aot_arg_names[cnt],0);
                        jit_fldr(Compiler.jit,jit_FR(Compiler.stack->reg),jit_R(0),8);
                    } else {
                        PushIRReg(type1->arg_types[cnt],GetFreeIReg);
                        LoadSymPtrIntoReg(type1->aot_arg_names[cnt],0);
                        jit_ldr(Compiler.jit,jit_R(Compiler.stack->reg),jit_R(0),8);
                    }
                }
            }
            cnt=MSize(type1->arg_types)/sizeof(AST*);
            LoadSymPtrIntoReg(func->str,0);
            jit_prepare(Compiler.jit);
            cnt=MSize(type1->arg_types)/sizeof(AST*);
            while(--cnt>=0) {
                PopIRValue(&value_a);
                switch(value_a->type) {
                case IRV_INT:
                    jit_putargi(Compiler.jit,value_a->int);
                    break;
                case IRV_FLOAT:
                    jit_fputargi(Compiler.jit,value_a->flt,8);
                    break;
                case IRV_REG:
                    jit_putargr(Compiler.jit,jit_R(value_a->reg));
                    break;
                case IRV_FREG:
                    jit_fputargr(Compiler.jit,jit_FR(value_a->reg),8);
                    break;
                }
                Free(value_a);
            }
            if(type1->has_vargs) {
                jit_putargi(Compiler.jit,0);
                jit_putargi(Compiler.jit,0);
            }
            jit_callr(Compiler.jit,jit_R(0));
            if(IsF64(func->hc_type->ret_type)) {
                PushIRFReg(func->hc_type->ret_type,idx=GetFreeFReg);
                jit_fretval(Compiler.jit,jit_FR(idx),8);
            } else {
                PushIRReg(func->hc_type->ret_type,idx=GetFreeIReg);
                jit_retval(Compiler.jit,jit_R(idx));
            }
        }
    } else if(var&&var->type==HTT_VAR) {
        LoadSymPtrIntoReg(var->str,idx=GetFreeIReg);
        if(var->hc_type->type==HTT_ARRAY_TYPE) {
            PushIRReg(var->hc_type,idx);
            break;
        }
        PushIRIndirX(var->hc_type,idx,0);
        if(IsF64(var->hc_type)||IsInteger(var->hc_type)||IsPtrType(var->hc_type))
            if(!IsF64(var->hc_type)) {
                if(!pass_by_ref) {
                    PopIRValue(&value_a);
                    PushIRReg(var->hc_type,idx);
                    Assign(Compiler.stack,value_a);
                    Free(value_a);
                }
            } else if(!pass_by_ref) {
                PopIRValue(&value_a);
                PushIRFReg(var->hc_type,GetFreeFReg);
                Assign(Compiler.stack,value_a);
                Free(value_a);
            }
    } else {
        //May be an unresolved label
        PushIRReg(GetType("I64"),idx=GetFreeIReg);
        LoadSymPtrIntoReg(ast->name,idx);
    }
    break;
    }
}
U0 CompileAST(AST *a,Bool pass_by_ref=FALSE,Bool dont_call_lone_name=FALSE) {
    if(Compiler.error_count) {
    	PushIRInt(0);
    	return;
    }
    loop:
    CIRValue *to_validate=Compiler.stack;
    __CompileAST(a,pass_by_ref,dont_call_lone_name);
    if(a->type==AST_OPCODE) {
        if(a->next&&a->next->type==AST_OPCODE) {
        } else {
            jit_end_asm_blk(Compiler.jit);
        }
    } else if(a->type!=AST_OPCODE&&a->next&&a->next->type==AST_OPCODE) {
        //If is a group of opcodes,taint at first
        //taint at enter
        jit_taint_label(Compiler.jit,jit_get_label(Compiler.jit));
    }
    if(to_validate!=Compiler.stack->last) {
        "Fail @%d\n",a->type;
        throw('Compile');
    }
    if(a->next) {
        PopIRValue;
        a=a->next;
        goto loop;
    }
}
//See jitlib.h
class _CBreakpoint {
    U8 enabled;
    U0 *jit_op;
    CFunction *f;
    U8 path[256];
    _CBreakpoint *prev,*next;
};
class CBreakpoints:CHash {
    _CBreakpoint *bps;
};
class jit_debugger_regs {
    //Prefix with "_" to allow bootstrap compiler to not recognize them as registers
    I64 _RAX;
    I64 _RBX;
    I64 _RCX;
    I64 _RDX;
    I64 _RSI;
    I64 _RDI;
    I64 _RSP;
    I64 _RBP;
    I64 _R8;
    I64 _R9;
    I64 _R10;
    I64 _R11;
    I64 _R12;
    I64 _R13;
    I64 _R15;
    F64 _XMM0;
    F64 _XMM1;
    F64 _XMM2;
    F64 _XMM3;
    F64 _XMM4;
    F64 _XMM5;
    F64 _XMM6;
    F64 _XMM7;
    I64 _R14;
};
class jit_debugger_ctrl {
    #define JIT_DBG_STEP 1
    #define JIT_DBG_NEXT 2
    #define JIT_DBG_FIN 3
    I64 code;
    U0 *prev_stack_ptr;
};
class CDebugger {
    jit_debugger_ctrl ctrl;
    _CBreakpoint *cur_bp;
    #if IsWindows
    //Im am sorry,I actually tried to get SEH working on windows but im flabergasted
    //So we will stick to a explicit call stack,it goes like this [func1,frame1,func2,func2,...]
    CVector *call_stack;
    #endif
} _Debugger;
_Debugger.call_stack=VectorNew;
CFunction *GetFunctionByPtr(U0 *ptr) {
    I64 idx=Compiler.globals->mask+1;
    U8 *bptr;
    CFunction *func;
    while(--idx>=0) {
        for(func=Compiler.globals->body[idx];func;func=func->next) {
            if(func->type==HTT_FUNC) {
                if(func->jit) {
                    bptr=func->func_ptr;
                    if(bptr<=ptr<=bptr+jit_bin_size(func->jit))
                        return func;
                } else if(func->is_aot) {
                    bptr=func->func_ptr;
                    if(bptr<=ptr<=bptr+func->aot_bin_size)
                        return func;
                }
            }
        }
    }
    return NULL;
}
_CBreakpoint *GetBreakpointByPtr(U0 *ptr) {
    CFunction *func=GetFunctionByPtr(ptr);
    if((!func)||func->is_aot) return NULL;
    return jit_get_breakpoint_by_ptr(func->jit,ptr);
}
U8 *DbgSkipWhitespace(U8 *str) {
    while(*str&&Bt(char_bmp_white_space,*str))
        str++;
    return str;
}
U0 ListLines(U8 *fn,I64 s,I64 e) {
    I64 cnt,diff,os,oe;
    os=s--,oe=e--;
    U8 *ftext=FileRead(fn,&cnt);
    if(!ftext) goto unable;
    U8 *start_ptr=ftext,*end_ptr=ftext,*buf;
    while(s>0) {
        start_ptr=StrFirstOcc(start_ptr,"\n");
        if(!start_ptr) {
            unable:;
            "Unable to list lines.\n";
            goto en;
        }
        start_ptr++;
        s--;
    }
    diff=oe-os+1;
    end_ptr=start_ptr;
    while(--diff>=0) {
        end_ptr=StrFirstOcc(start_ptr,"\n");
        if(!end_ptr) end_ptr=start_ptr+StrLen(start_ptr);
        buf=StrNCpy(MAlloc(end_ptr-start_ptr+1),start_ptr,end_ptr-start_ptr);
        " [%d]: %s\n",os++,buf;
        Free(buf);
        if(!*end_ptr) {
            break;
        } else end_ptr++;
        start_ptr=end_ptr;
    }
    en:
    Free(ftext);
}
U0 Backtrace(I64 hl_at,I64 trim=0) {
    U0 *backtrace[0x100];
    try {
        I64 idx,stk_sz=jit_FunctionParents(backtrace,0x100);
    } catch {
        Fs->catch_except=TRUE;
        "CORRUPT STACK\n";
        return;
    }
    CFunction *func;
    _CBreakpoint *bp;
    U8 chr;
    #if !IsWindows
    trim++;
    #endif
    for(idx=trim;idx<stk_sz;idx++) {
        if(hl_at==idx-trim) chr='*';
        else chr=' ';
        func=GetFunctionByPtr(backtrace[idx]);
        if(func) {
            if(bp=GetBreakpointByPtr(backtrace[idx]))
                "[%d] %c %s %s\n",idx-trim,chr,func->str,bp->path;
            else
                "[%d] %c %s ???\n",idx-trim,chr,func->str;
        } else
            "[%d] %c ???\n",idx-trim,chr;
        next:
    }
}
U0 DebugPrint(U0 *ptr,CType *type) {
    loop:
    if(type->type==HTT_UNION&&type->base) {
        type=type->base;
        goto loop;
    }
    I64 idx,scale;
    CType *deref;
    CTypeMember *mem;
    switch(type->type) {
        case HTT_TYPE:
        if(IsF64(type)) {
            "%n",ptr(F64*)[0];
        } else if(IsPtrType(type)) {
            "%x",ptr(U0**)[0];
        } else {
            switch(type->sz) {
                case 1:
                "%d",ptr(I8*)[0];
                break;
                case 2:
                "%d",ptr(I16*)[0];
                break;
                case 4:
                "%d",ptr(I32*)[0];
                break;
                case 8:
                "%d",ptr(I64*)[0];
            }
        }
        break;
        case HTT_ARRAY_TYPE:
        "{";
        scale=(deref=DerefedType(type))->sz;
        for(idx=0;idx!=type->dim;idx++) {
            DebugPrint(ptr(U8*)+scale*idx,deref);
            ",";
        }
        "}";
        break;
        case HTT_FUNC_TYPE:
        //Should never reach here
        "%s",type->str;
        break;
        case HTT_CLASS:
        case HTT_UNION:
        "{";
        for(mem=type->first_member;mem;mem=mem->next) {
            "%s:",mem->name;
            DebugPrint(ptr(U8*)+mem->offset,mem->type);
            ",";
        }
        "}";
        break;
    }
}
U0 DebuggerRoutine(jit_debugger_regs *regs,_CBreakpoint *bp,U8 *caller_ptr) {
    "Welcome to debugger land.\n";
    U0 *backtrace[0x100];
    U0 *base_ptrs[0x100];
    static U8 list_fn[256];
    static U8 last_command[1024];
    static I64 list_ln;
    Bool leave=FALSE;
    CCompilerState *cstate;
    I64 stk_sz,stk_pos,com_len,bp_ln,idx,cidx;
    U8 *command,*o_command,*fn_end,*fmt,*bp_fn;
    CFunction *cur_func;
    CVariable *local_var,*new_var;
    CType *expr_type;
    CCmpCtrl *cc;
    AST *debug_expr;
    CIRValue *debug_value;
    _CBreakpoint *_bp;
    CBreakpoints *bps;
    U0 (*fptr)();
    stk_sz=jit_FunctionParents(backtrace,0x100);
    jit_ParentFramePtrs(base_ptrs,0x100);
    if(caller_ptr&&!IsWindows) {
        for(cidx=0;backtrace[cidx];cidx++)
            if(backtrace[cidx]==caller_ptr)
                break;
    } else
#if IsWindows
        cidx=0;
#else
        cidx=2;
#endif
    for(idx=cidx;idx<stk_sz;idx++) {
        backtrace[idx-cidx]=backtrace[idx];
        base_ptrs[idx-cidx]=base_ptrs[idx];
    }
    stk_sz-=cidx,stk_pos=0;
    _Debugger.cur_bp=bp;
    if(bp) {
        fmt=list_fn;
        StrScan(bp->path,"%s,%d",&fmt,&list_ln);
        ListLines(list_fn,list_ln,list_ln);
    }
    loop:
    if(leave) return;
    o_command=command=GetStr("BSAT>>> ");
    if(!StrLen(command)) {
        Free(command);
        o_command=command=StrNew(last_command);
    }
    command=DbgSkipWhitespace(command);
    com_len=0;
    while(*command&&Bt(char_bmp_word,command[com_len]))
        com_len++;
    if(!StrNCmp(command,"help",com_len)&&com_len==4) {
        help:
        "help\t--Display this message.\n"
        "up\t--Go up the call stack.\n"
        "down\t--Go down the call stack.\n"
        "list\t--List next lines.\n"
        "list fn:ln\t--List lines at file.\n"
        "list ln\t--List lines in current file.\n"
        "b\t--Set breakpoint at current position.\n"
        "b fn:ln\t--Set breakpoint at file.\n"
        "b ln\t--Set breakpoint at line.\n"
        "d\t--Delete all breakpoints.\n"
        "d num\t--Delete a specific breakpoint.\n"
        "s\t--Step into a function.\n"
        "n\t--Step over a function.\n"
        "f\t--Finish a function.\n"
        "listb\t--List breakpoints\n"
        "bt\t--Backtrace.\n"
        "exit\t--Exit debugger.\n"
        "Press enter for last command.\n"
        ;
    } else if(!StrNCmp(command,"up",com_len)&&com_len==2) {
        if(stk_pos+1>=stk_sz) {
            "At topmost stack item.\n";
            goto next;
        }
        stk_pos++;
        _Debugger.cur_bp=GetBreakpointByPtr(backtrace[stk_pos]);
        if(_Debugger.cur_bp) {
            fmt=list_fn;
            StrScan(_Debugger.cur_bp->path,"%s,%d",&fmt,&list_ln);
            ListLines(list_fn,list_ln,list_ln);
        }
    } else if(!StrNCmp(command,"down",com_len)&&com_len==4) {
        if(stk_pos==0) {
            "At bottomost stack item.\n";
            goto next;
        }
        stk_pos--;
        _Debugger.cur_bp=GetBreakpointByPtr(backtrace[stk_pos]);
        if(_Debugger.cur_bp) {
            fmt=list_fn;
            StrScan(_Debugger.cur_bp->path,"%s,%d",&fmt,&list_ln);
            ListLines(list_fn,list_ln,list_ln);
        }
        #define LIST_LINE_COUNT 5
    } else if(!StrNCmp(command,"list",com_len)&&com_len==4) {
        command=DbgSkipWhitespace(command+StrLen("list"));
        if(StrOcc(command,':')) {
            fn_end=StrLastOcc(command,":");
            StrNCpy(list_fn,command,fn_end-command);
            fn_end++;
            list_ln=Str2I64(fn_end,10);
        } else if(*command) {
            list_ln=Str2I64(fn_end,10);
        }
        if(list_ln-LIST_LINE_COUNT<0)
            list_ln=LIST_LINE_COUNT;
        ListLines(list_fn,list_ln-LIST_LINE_COUNT,list_ln+LIST_LINE_COUNT);
        list_ln+=2*LIST_LINE_COUNT+1;
    } else if(!StrNCmp(command,"b",com_len)&&com_len==1) {
        bp_fn=NULL;
        command=DbgSkipWhitespace(command+StrLen("b"));
        if(StrOcc(command,':')) {
            //b LUMPIS.HC:10
            fn_end=StrLastOcc(command,":");
            bp_fn=StrNCpy(MAlloc(fn_end-command+1),command,fn_end-command);
            fn_end++;
            bp_ln=Str2I64(fn_end,10);
        } else {
            if(_Debugger.cur_bp) {
                /*    ________
                     /|wwewww|\
                     ||0    0||   <== Hairy lumpis
                     ||||oo||||
                     |////\\\\|
                <---/////  \\\\\--->
                */
                bp_fn=MAlloc(256);
                StrScan(_Debugger.cur_bp->path,"%s,%d",&bp_fn,&bp_ln);
            }
            if(*command)
                //b 10
                bp_ln=Str2I64(command,10);
            else
                //b
                ; //Was set at the lumpis
        }
        if(bp_fn) {
            fmt=LexMatchFile(Compiler.cc,bp_fn);
            Free(bp_fn);
        }     else
            fmt=NULL;
        if(fmt) {
            bp_fn=MStrPrint("%s,%d",fmt,bp_ln);
            bps=HashFind(bp_fn,Compiler.breakpoints);
            if(bps) {
                "Match at %s\n",bp_fn;
                for(_bp=bps->bps;_bp;_bp=_bp->next) {
                    _bp->enabled=TRUE;
                }
            }
            Free(bp_fn);
        }
        Free(fmt);
    } else if(!StrNCmp(command,"s",com_len)&&com_len==1) {
        _Debugger.ctrl->code=JIT_DBG_STEP;
        leave=TRUE;
    } else if(!StrNCmp(command,"n",com_len)&&com_len==1) {
        _Debugger.ctrl->code=JIT_DBG_NEXT;
        _Debugger.ctrl->prev_stack_ptr=base_ptrs[0];
        leave=TRUE;
    } else if(!StrNCmp(command,"f",com_len)&&com_len==1) {
        _Debugger.ctrl->code=JIT_DBG_FIN;
        _Debugger.ctrl->prev_stack_ptr=base_ptrs[0];
        leave=TRUE;
    } else if(!StrNCmp(command,"bt",com_len)&&com_len==2) {
        #if !IsWindows
        Backtrace(stk_pos,1+1);
        #else
        Backtrace(stk_pos,0);
        #endif
    } else if(!StrNCmp(command,"p",com_len)&&com_len==1) {
        //TODO ensure doesnt reference optimized out values that arent avialable
        command+=com_len;
        cstate=NewCompilerState;
        Compiler.in_func=TRUE;
        cur_func=GetFunctionByPtr(backtrace[stk_pos]);
        if(cur_func&&_Debugger.cur_bp) {
            idx=cur_func->locals.mask+1;
            while(--idx>=0) {
                for(local_var=cur_func->locals.body[idx];local_var;local_var=local_var->next) {
                    if(local_var->type!=HTT_VAR) goto next2;
                    new_var=MAlloc(sizeof(CVariable));
                    new_var->type=HTT_VAR;
                    new_var->str=StrNew(local_var->str);
                    new_var->hc_type=local_var->hc_type;
                    new_var->is_extern_ptr=TRUE;
                    new_var->static_refs=VectorNew;
                    if(local_var->is_static) {
                        new_var->extern_ptr=local_var->static_ptr;
                    } else if(!local_var->isReg) {
                        new_var->extern_ptr=base_ptrs[stk_pos](U8*)+local_var->frame_off;
                    } else {
                        if(stk_pos==0) {
                            new_var->extern_ptr=jit_debugger_get_reg_ptr(cur_func->jit,regs,_Debugger.cur_bp->jit_op,jit_R(local_var->reg));
                        } else {
                            new_var->extern_ptr=jit_debugger_get_vreg_ptr_from_parent(cur_func->jit,base_ptrs[stk_pos],_Debugger.cur_bp->jit_op,jit_R(local_var->reg));
                        }
                        if(new_var->extern_ptr==NULL)
                            new_var->is_opted_out=TRUE;
                    }
                    HashAdd(new_var,Compiler.locals);
                    next2:
                }
            }
        }
        //TODO free cc
        cc=CmpCtrlNew(NULL);
        LexIncludeStr(cc,NULL,StrNew(command),FALSE);
        Lex(cc);
        debug_expr=ParseExpr(cc);
        try {
            expr_type=AssignTypeToNode(debug_expr);
            CompileAST(debug_expr);
            PopIRValue(&debug_value);
            switch(debug_value->type) {
                case IRV_INT:
                "%d\n",debug_value->int;
                break;
                case IRV_FLOAT:
                "%n\n",debug_value->flt;
                start:
                idx=jit_allocai(Compiler.jit,8);
                case IRV_FREG:
                jit_fstxi(Compiler.jit,idx,R_FP,jit_FR(debug_value->reg),8);
                break;
                case IRV_REG:
                jit_stxi(Compiler.jit,idx,R_FP,jit_R(debug_value->reg),8);
                break;
                end:
                jit_addi(Compiler.jit,jit_R(0),R_FP,idx);
                jit_prepare(Compiler.jit);
                jit_putargr(Compiler.jit,jit_R(0));
                jit_putargi(Compiler.jit,expr_type);
                jit_call(Compiler.jit,&DebugPrint);
                break;
                case IRV_STR:
                "%s\n",debug_value->int;
                break;
                case IRV_INDIRX:
                jit_addi(Compiler.jit,jit_R(debug_value->indirx.reg),jit_R(debug_value->indirx.reg),debug_value->indirx.offset);
                jit_prepare(Compiler.jit);
                jit_putargr(Compiler.jit,jit_R(debug_value->indirx.reg));
                jit_putargi(Compiler.jit,expr_type);
                jit_call(Compiler.jit,&DebugPrint);
            }
            Free(debug_value);
            #if IsWindows
            if(Compiler.debug_mode) {
                jit_prepare(Compiler.jit);
                jit_call(Compiler.jit,&WindowsLeaveFunc);
            }
            #endif
            jit_reti(Compiler.jit,1);
            cur_func=CompilerEmitCode;
            if(cur_func) {
                fptr=cur_func->func_ptr;
  		            "=";
                #ifdef SIGNAL
                ArmSignals;
                #endif
  		            (*fptr)();
  		            "\n";
  		            FreeFunction(cur_func);
            }
        } catch {
            Fs->catch_except=TRUE;
        }
        RestoreCompilerState(cstate);
    } else if(!StrNCmp(command,"exit",com_len)&&com_len==4) {
        //Not a valid code so we skip all conditions
        _Debugger.ctrl->code=-1;
        leave=TRUE;
    } else {
        goto help;
    }
    next:
    StrCpy(last_command,o_command);
    Free(o_command);
    goto loop;
}
U0 CompileAstWithBreakpoint(AST *a) {
    if(Compiler.error_count) {
        PushIRInt(0);
        return;
    }
    if(a->type!=AST_OPCODE&&!Compiler.debug_mode) {
        CompileAST(a);
        return;
    }
    re:
    U8 *path_str=MStrPrint("%s,%d",a->fn,a->ln);
    _CBreakpoint *bp=MAlloc(sizeof(_CBreakpoint));
    CBreakpoints *tail;
    StrCpy(bp->path,path_str);
    loop:
    if(tail=HashFind(path_str,Compiler.breakpoints)) {
        if(tail->bps) tail->bps->prev=bp;
        bp->next=tail->bps;
        tail->bps=bp;
    } else {
        tail=MAlloc(sizeof(CBreakpoints));
        tail->str=StrNew(path_str);
        HashAdd(tail,Compiler.breakpoints);
        goto loop;
    }
    jit_breakpoint(Compiler.jit,bp,&DebuggerRoutine,&_Debugger.ctrl);
    __CompileAST(a);
    Free(path_str);
    if(a->next) {
        PopIRValue;
        a=a->next;
        goto re;
    }
}
U0 Debugger() {
    DebuggerRoutine(NULL,NULL,NULL);
}
//For AOT
U0 GlobalCompileASTTo(U8 *to,AST *ast) {
    I64 reg;
    Bool replace=&Compiler!=GlobalCompiler;
    if(replace) {
        CCompiler old=Compiler;
        Compiler=*GlobalCompiler;
    }
    CVariable *var=MAlloc(sizeof(CVariable));
    var->type=HTT_VAR;
    var->str=StrNew(to);
    var->global_ptr=MAlloc(AssignTypeToNode(ast)->sz);
    HashAdd(var,Compiler.globals);
    CompileAST(ast);
    PushIRIndirX(AssignTypeToNode(ast),reg=GetFreeIReg,0);
    LoadSymPtrIntoReg(to,reg);
    Assign(Compiler.stack,Compiler.stack->last);
    PopIRValue,PopIRValue;
    if(replace) {
        *GlobalCompiler=Compiler;
        Compiler=old;
    }
}
U0 FreeAST(AST *d) {
    I64 idx,cnt;
    if(!d) return;
    switch(d->type) {
        start:
    case AST_NOP:
    break;
    case AST_DECL:
    FreeAST(d->decl.name);
    FreeAST(d->decl.dft);
    break;
    case AST_ARRAY_LITERAL:
    idx=MSize(d->arr_lit)/sizeof(AST*);
    while(--idx>=0)
        FreeAST(d->arr_lit[idx]);
    break;
    case AST_MEMBER:
    FreeAST(d->member_acc.base);
    break;
    case AST_PAREN:
    unop:
    FreeAST(d->unopArg);
    break;
    case AST_INT:
    case AST_FLOAT:
    case AST_CHAR:
    break;
    case AST_STRING:
    Free(d->string);
    break;
    case AST_COMMA:
    binop:
    FreeAST(d->binop.a);
    FreeAST(d->binop.b);
    break;
    case AST_ADD:
    goto binop;
    case AST_SUB:
    goto binop;
    case AST_MUL:
    goto binop;
    case AST_DIV:
    goto binop;
    case AST_MOD:
    goto binop;
    case AST_LSHIFT:
    goto binop;
    case AST_RSHIFT:
    goto binop;
    case AST_POW:
    goto binop;
    case AST_BAND:
    goto binop;
    case AST_BXOR:
    goto binop;
    case AST_BOR:
    goto binop;
    case AST_LT:
    case AST_GT:
    case AST_LE:
    case AST_EQ:
    case AST_NE:
    case AST_GE:
    case AST_LAND:
    case AST_LOR:
    case AST_LXOR:
    case AST_ASSIGN:
    case AST_ASSIGN_SHL:
    case AST_ASSIGN_SHR:
    case AST_ASSIGN_MUL:
    case AST_ASSIGN_DIV:
    case AST_ASSIGN_BAND:
    case AST_ASSIGN_BOR:
    case AST_ASSIGN_BXOR:
    case AST_ASSIGN_ADD:
    case AST_ASSIGN_SUB:
    goto binop;
    case AST_POST_INC:
    case AST_POST_DEC:
    goto unop;
    case AST_ARRAY_ACCESS:
    FreeAST(d->arr_acc.base);
    FreeAST(d->arr_acc.index);
    break;
    case AST_PRE_DEC:
    case AST_PRE_INC:
    case AST_POS:
    case AST_NEG:
    case AST_LNOT:
    case AST_BNOT:
    case AST_DERREF:
    case AST_ADDROF:
    case AST_SIZEOF:
    goto unop;
    case AST_IMPLICIT_TC:
    case AST_EXPLICIT_TC:
    FreeAST(d->typecast.base);
    break;
    case AST_FUNC_CALL:
    FreeAST(d->func_call.base);
    idx=MSize(d->func_call.args)/sizeof(AST*);
    while(--idx>=0)
        FreeAST(d->func_call.args[idx]);
    break;
    case AST_IF:
    FreeAST(d->if_stmt.cond);
    FreeAST(d->if_stmt.body);
    FreeAST(d->if_stmt.el);
    break;
    case AST_FOR:
    FreeAST(d->for_stmt.cond);
    FreeAST(d->for_stmt.body);
    FreeAST(d->for_stmt.inc);
    FreeAST(d->for_stmt.init);
    break;
    case AST_DO:
    FreeAST(d->do_stmt.cond);
    FreeAST(d->do_stmt.body);
    break;
    case AST_WHILE:
    FreeAST(d->while_stmt.cond);
    FreeAST(d->while_stmt.body);
    break;
    case AST_META_DATA:
    //TODO
    break;
    case AST_CASE:
    FreeAST(d->cs.low);
    FreeAST(d->cs.high);
    break;
    case AST_SWITCH:
    FreeAST(d->switch_stmt.cond);
    FreeAST(d->switch_stmt.body);
    break;
    case AST_SUBSWIT:
    FreeAST(d->sub_swit.start_code);
    FreeAST(d->sub_swit.body);
    break;
    case AST_BREAK:
    case AST_DFT:
    break;
    case AST_RETURN:
    FreeAST(d->unopArg);
    break;
    case AST_TYPE:
    break;
    case AST_GOTO:
    case AST_LABEL:
    Free(d->name);
    break;
    case AST_TRY:
    FreeAST(d->try_block.body);
    FreeAST(d->try_block.cat);
    break;
    case AST_LASTCLASS:
    break;
    case AST_RANGE:
    cnt=MSize(d->range.values)/sizeof(AST*);
    while(--cnt>=0) {
        FreeAST(d->range.values[cnt]);
    }
    Free(d->range.values),Free(d->range.types);
    break;
    case AST_ASSIGN_MOD:
    goto binop;
    case AST_NAME:
    Free(d->name);
    break;
    case AST_PRINT:
    Free(d->print.text);
    cnt=MSize(d->print.args)/sizeof(AST*);
    while(--cnt>=0) {
        FreeAST(d->print.args[cnt]);
    }
    Free(d->print.args);
    break;
    end:
    Free(d->fn);
    Free(d);
    break;
    }
}
U0 FillInRelocations(CFunction *f) {
    CVariable *var;
    CFunction *func;
    CRelocation *relocs=f->relocs,*last=NULL,*first=NULL,*tmp;
    //"ENTER:\n";
    if(Compiler.aot_mode) first=f->relocs;
    next2:
    for(;relocs;) {
        loop:
        var=func=HashFind(relocs->name,Compiler.globals);
        if(!var) goto next;
        if(func->type==HTT_FUNC) {
            switch(func->link.type&~LINK_PUBLIC) {
                case 0:
                case LINK_IMPORT:
                case LINK_EXTERN:
                if(func->func_ptr) {
                    *relocs->ptr=func->func_ptr;
                    pass:
                    if(!Compiler.aot_mode) {
                        if(last) last->next=relocs->next;
                        tmp=relocs->next;
                        Free(relocs);
                        relocs=tmp;
                    } else
                        relocs=relocs->next;
                    goto next2;
                }
                break;
                case LINK__IMPORT:
                case LINK__EXTERN:
                var=func=HashFind(func->link._link,Compiler.globals);
                goto loop;
            }
        } else if(var->type==HTT_VAR) {
            switch(var->link.type&~LINK_PUBLIC) {
                case 0:
                case LINK_IMPORT:
                case LINK_EXTERN:
                if(var->global_ptr) {
                    *relocs->ptr=var->global_ptr;
                    goto pass;
                }
                break;
                case LINK__IMPORT:
                case LINK__EXTERN:
                var=func=HashFind(var->link._link,Compiler.globals);
                goto loop;
            }
        }
        next:
        if(!first) first=relocs;
        last=relocs;
        relocs=relocs->next;
    }
    f->relocs=first;
}
U0 ApplyAsmPatches(CFunction *f);
CFunction *CompilerEmitCode() {
    if(Compiler. error_count) return NULL;
    CFunction *ret;
    CCompString *str;
    CCompStringRef *str_ref;
    CVariable *var;
    CLabel *label,*new_label;
    I64 idx=Compiler.cur_strings->mask+1;
    U0 *pat;
    U8 *zeros;
    idx=Compiler.locals->mask+1;
    while(--idx>=0) {
        for(var=Compiler.locals->body[idx];var;var=var->next) {
            if(var->type==HTT_VAR&&var->is_static) {
                jit_code_align_fill(Compiler.jit,var->hc_type->align,0);
                while(var->static_refs->size) {
                    pat=VectorPop(var->static_refs);
                    jit_patch(Compiler.jit,pat);
                }
                zeros=MAlloc(var->hc_type->sz);
                jit_data_bytes(Compiler.jit,MSize(zeros),zeros);
                Free(zeros);
            }
        }
    }
    idx=Compiler.labels->mask+1;
    while(--idx>=0) {
        for(label=Compiler.labels->body[idx];label;label=label->next) {
            if(label->is_exported) {
                new_label=MAlloc(sizeof(CLabel));
                *new_label=*label;
                new_label->str=StrNew(label->str);
                HashAdd(new_label,Compiler.globals);
            }
        }
    }
    ret=MAlloc(sizeof(CFunction));
    jit_disable_optimization(Compiler.jit,0xff);
    jit_generate_code(Compiler.jit);
    ret->type=HTT_FUNC;
    ret->jit=Compiler.jit;
    ret->func_ptr=Compiler.func_ptr;
    ret->relocs=Compiler.relocations;
    ret->locals=Compiler.locals;
    ret->labels=Compiler.labels;
    ret->asm_patches=Compiler.asm_patches;
    FillInRelocations(ret);
    ApplyAsmPatches(ret);
    if(Compiler.aot_mode) {
        //jit_dump_ops(Compiler.jit,1);
        ret->aot_strings=Compiler.cur_strings;
        idx=ret->aot_strings->mask+1;
        while(--idx>=0) {
            for(str=ret->aot_strings->body[idx];str;str=str->next) {
                for(str_ref=str->refs;str_ref;str_ref=str_ref->next) {
                    *str_ref->ptr=str->data;
                }
            }
        }
        Compiler.cur_strings=HashTableNew(0x40);
    }
    return ret;
}
F64 EvalASTF64(AST *ast) {
    AST *ret=MAlloc(sizeof(AST));
    F64 v=0.0;
    I64 en=PoopSetGCEnable(0);
    ret->type=AST_RETURN;
    ret->unopArg=ast;
    CCompilerState *state=NewCompilerState;
    Compiler.ret_type=HashFind("F64",Compiler.globals);
    CompileAST(ret);
    CFunction *func=CompilerEmitCode;
    if(func)
        F64(*fptr)()=func->func_ptr;
    else
        fptr=NULL;
    RestoreCompilerState(state);
    if(fptr) v=(*fptr)();
    PoopSetGCEnable(en);
    if(func) FreeFunction(func);
    return v;
}
I64 EvalAST(AST *ast) {
    AST *ret=MAlloc(sizeof(AST));
    I64 v=0,en=PoopSetGCEnable(0);
    ret->type=AST_RETURN;
    ret->unopArg=ast;
    CCompilerState *state=NewCompilerState;
    Compiler.ret_type=HashFind("I64i",Compiler.globals);
    CompileAST(ret);
    CFunction *func=CompilerEmitCode;
    if(func)  {
        I64(*fptr)()=func->func_ptr;
        v=(*fptr)();
    }
    PoopSetGCEnable(en);
    RestoreCompilerState(state);
    if(func) FreeFunction(func);
    return v;
}
CFunction *CompileFunction(AST *at,CCmpCtrl *cc,CType *type,U8 *name,Bool inst_eval=FALSE) {
    try {
        if(Compiler.aot_mode&&name&&HashFind(name,Compiler.globals)&&!IsForward(HashFind(name,Compiler.globals)))
            ParserFail2(at,"Redinition of symbol %s.",name);
    } catch {
        CatchIf('Parser','Lexer');
        return NULL;
    }
    CFunction *ret,*func;
    CVariable *arg;
    CHash *tainted;
    CUnresolvedLabel *unres;
    CLabel *label;
    CCompilerState *state=NewCompilerState;
    Compiler.ret_type=type->ret_type;
    AST *body;
    Compiler.in_func=TRUE;
    I64 idx,cnt,frame_off=0,align,freg,fp;
    cnt=MSize(type->arg_types)/sizeof(CType*);
    for(idx=0;idx!=cnt;idx++) {
        if(type->arg_names[idx]) {
            arg=MAlloc(sizeof(CVariable));
            arg->str=StrNew(type->arg_names[idx]);
            arg->type=HTT_VAR;
            arg->hc_type=type->arg_types[idx];
            if(IsF64(arg->hc_type)||IsInteger(arg->hc_type)||IsPtrType(arg->hc_type))
                arg->isReg=TRUE; //Can be turned false later.
            HashAdd(arg,Compiler.locals);
        }
    }
    if(type->has_vargs) {
        arg=MAlloc(sizeof(CVariable));
        arg->str=StrNew("argc");
        arg->type=HTT_VAR;
        arg->hc_type=HashFind("I64i",Compiler.globals);
        arg->isReg=TRUE;
        HashAdd(arg,Compiler.locals);
        arg=MAlloc(sizeof(CVariable));
        arg->str=StrNew("argv");
        arg->type=HTT_VAR;
        arg->hc_type=GetType("I64i")+1;
        arg->isReg=TRUE;
        HashAdd(arg,Compiler.locals);
    }
    //Create forward decl
    if(name) {
        ret=MAlloc(sizeof(CFunction));
        ret->src_link=MStrPrint("%s,%d",at->fn,at->ln);
        ret->link.type=LINK_EXTERN;
        ret->hc_type=type;
        ret->type=HTT_FUNC;
        ret->str=StrNew(name);
        HashAdd(ret,Compiler.globals);
    }
    if(cc->token!='{') ParserFail(cc,"Expected function body.");
    body=ParseScope(cc,inst_eval);
    Compiler.first_avail_ireg=3;
    Compiler.first_avail_freg=2;
    cnt=Compiler.locals->mask+1;
    for(idx=0;idx!=cnt;idx++) {
        arg=Compiler.locals->body[idx];
        for(;arg;arg=arg->next) {
            if(arg->type==HTT_VAR&&!(arg->link.type&LINK_STATIC)) {
                if(arg->isReg&&!Compiler.force_noregs) {
                    if(IsF64(arg->hc_type))
                        arg->reg=Compiler.first_avail_freg++;
                    else
                        arg->reg=Compiler.first_avail_ireg++;
                } else {
                    arg->isReg=FALSE;
                    align=arg->hc_type->align;
                    frame_off+=(align-(frame_off%align))%align;
                    arg->frame_off=frame_off;
                    frame_off+=arg->hc_type->sz;
                }
            }
        }
    }
    Compiler.frame_off=jit_allocai(Compiler.jit,frame_off);
    cnt=Compiler.locals->mask+1;
    for(idx=0;idx!=cnt;idx++) {
        arg=Compiler.locals->body[idx];
        for(;arg;arg=arg->next) {
            if(arg->type==HTT_VAR&&!(arg->link.type&LINK_STATIC)) {
                if(!arg->isReg){
                    arg->frame_off+=Compiler.frame_off;
                }
            }
        }
    }
    cnt=MSize(type->arg_types)/sizeof(CType*);
    for(idx=0;idx!=cnt;idx++) {
        if(IsF64(type->arg_types[idx])) {
            jit_declare_arg(Compiler.jit,JIT_FLOAT_NUM,8);
        } else if(IsSigned(type->arg_types[idx]))
            jit_declare_arg(Compiler.jit,JIT_SIGNED_NUM,8);
        else
            jit_declare_arg(Compiler.jit,JIT_UNSIGNED_NUM,8);
    }
    cnt=MSize(type->arg_types)/sizeof(CType*);
    for(idx=0;idx!=cnt;idx++) {
        if(type->arg_names[idx]) {
            arg=HashFind(type->arg_names[idx],Compiler.locals);
            if(arg->isReg) {
                if(IsF64(type->arg_types[idx]))
                    jit_getarg(Compiler.jit,jit_FR(arg->reg),idx);
                else
                    jit_getarg(Compiler.jit,jit_R(arg->reg),idx);
            } else {
                if(IsF64(type->arg_types[idx])) {
                    jit_getarg(Compiler.jit,jit_FR(freg=GetFreeFReg),idx);
                    PushIRFReg(HashFind("F64",Compiler.globals),freg);
                } else {
                    jit_getarg(Compiler.jit,jit_R(freg=GetFreeIReg),idx);
                    PushIRReg(HashFind("I64i",Compiler.globals),freg);
                }
                PushIRIndirX(arg->hc_type,fp=GetFreeIReg,arg->frame_off);
                jit_movr(Compiler.jit,jit_R(fp),R_FP);
                Assign(Compiler.stack,Compiler.stack->last);
                PopIRValue,PopIRValue;
            }
        }
    }
    if(type->has_vargs) {
         jit_declare_arg(Compiler.jit,JIT_SIGNED_NUM,8);
        arg=HashFind("argc",Compiler.locals);
        if(arg->isReg) {
            jit_getarg(Compiler.jit,jit_R(arg->reg),idx++);
        } else {
            jit_getarg(Compiler.jit,jit_R(freg=GetFreeIReg),idx++);
            PushIRReg(HashFind("I64i",Compiler.globals),freg);
            PushIRIndirX(arg->hc_type,fp=GetFreeIReg,arg->frame_off);
            jit_movr(Compiler.jit,jit_R(fp),R_FP);
            Assign(Compiler.stack,Compiler.stack->last);
            PopIRValue,PopIRValue;
        }
         jit_declare_arg(Compiler.jit,JIT_PTR,8);
        arg=HashFind("argv",Compiler.locals);
        if(arg->isReg) {
            jit_getarg(Compiler.jit,jit_R(arg->reg),idx++);
        } else {
            jit_getarg(Compiler.jit,jit_R(freg=GetFreeIReg),idx++);
            PushIRReg(HashFind("I64i",Compiler.globals),freg);
            PushIRIndirX(arg->hc_type,fp=GetFreeIReg,arg->frame_off);
            jit_movr(Compiler.jit,jit_R(fp),R_FP);
            Assign(Compiler.stack,Compiler.stack->last);
            PopIRValue,PopIRValue;
        }
    }

    CompileAstWithBreakpoint(body);
    PopIRValue;
    #if IsWindows
    if(Compiler.debug_mode) {
        jit_prepare(Compiler.jit);
        jit_call(Compiler.jit,&WindowsLeaveFunc);
    }
    #endif
    jit_reti(Compiler.jit,0);
    idx=Compiler.asm_tainted_label_names.mask+1;
    while(--idx>=0) {
        for(tainted=Compiler.asm_tainted_label_names.body[idx];tainted;tainted=tainted->next) {
            if(label=HashFind(tainted->str,Compiler.labels))
                jit_taint_label(Compiler.jit,label->op);

        }
    }
    idx=Compiler.unresolved_labels.mask+1;
    while(--idx>=0) {
        for(unres=Compiler.unresolved_labels.body[idx];unres;unres=unres->next) {
            if((label=HashFind(unres->str,Compiler.labels))&&label->type==HTT_LABEL);
            else
                try {
                    ParserFail2(unres->stmt,"Unresolved label %s.",unres->str);
                } catch {
                    CatchIf('Parser','Lexer');
                }
        }
    }
    FreeAST(body);
    ret=CompilerEmitCode;
    if(!ret) {
        ret=HashFind(name,Compiler.globals);
        if(ret) {
            ret->hc_type=type;
            Free(ret->src_link);
            ret->src_link=MStrPrint("%s,%d",at->fn,at->ln);
        }
        RestoreCompilerState(state);
        return NULL;
    }
    ret->hc_type=type;
    if(name) {
        ret->str=StrNew(name);
        HashAdd(ret,Compiler.globals);
        //Relocations arent deleted in aot_mode,so dont fill willy nilly
        if(!Compiler.aot_mode) {
            idx=Compiler.globals->mask+1;
            while(--idx>=0) {
                func=Compiler.globals->body[idx];
                for(;func;func=func->next) {
                        if(func->type==HTT_FUNC)
                        FillInRelocations(func);
                }
            }
        }
    }
    ret->locals=Compiler.locals;
    RestoreCompilerState(state);
    return ret;
}
#if IsWindows
I64 WindowsSetCallStackSize(I64 sz) {
    return _Debugger.call_stack->size=2*sz;
}
I64 WindowsCallStackSize() {
    return _Debugger.call_stack->size/2;
}
I64 jit_FunctionParents(U0 **ptrs,I64 max) {
    I64 top=_Debugger.call_stack->size-2,cnt=0;
    for(;top>=0<max;top-=2)
        ptrs[--max,cnt++]=_Debugger.call_stack->data[top];
    return cnt;
}
I64 jit_ParentFramePtrs(U0 **ptrs,I64 max) {
    I64 top=_Debugger.call_stack->size-2,cnt=0;
    for(;top>=0<max;top-=2)
        ptrs[--max,cnt++]=_Debugger.call_stack->data[top+1];
    return cnt;
}
U0 WindowsEnterFunc(U0 *func,U0 *frame) {
    VectorPush(_Debugger.call_stack,func);
    VectorPush(_Debugger.call_stack,frame);
}
U0 WindowsLeaveFunc() {
    VectorPop(_Debugger.call_stack),VectorPop(_Debugger.call_stack);
}
#endif
AST *CompileX64Mode(AST *a,CReg **base,CReg **index,I64 *scale,CVector *offsets,Bool reg_only=FALSE) {
    AST *ret=NULL;
    AST *binop_a,*binop_b;
    I64 cnt,s;
    CReg *reg;
    try {
        if(reg_only) goto rchk;
        switch(a) {
            //SIB
            case AST_ADD:
            binop_a=CompileX64Mode(a->binop.a,base,index,scale,offsets);
            binop_b=CompileX64Mode(a->binop.b,base,index,scale,offsets);
            for(cnt=0;cnt!=2;cnt++) {
                if(binop_a&&binop_a->x64_mode->type==X64O_REG) {
                    if(binop_b&&binop_b->x64_mode->type==X64O_REG) {
                        if(*base||*index) {
                            ParserFail2(a,"Only one base and index register allowed.");
                        }
                        *base=binop_b->x64_mode->reg;
                        *index=binop_a->x64_mode->reg;
                        *scale=1;
                        FreeAST(binop_a),FreeAST(binop_b);
                        break;
                    }
                    if(!*base) *base=binop_a->x64_mode->reg;
                    if(!*index) *index=binop_a->x64_mode->reg;
                    FreeAST(binop_a);
                    VectorPush(offsets,binop_b);
                    break;
                }
                SwapI64(&binop_a,&binop_b);
            }
            break;
            case AST_MUL:
            binop_a=CompileX64Mode(a->binop.a,base,index,scale,offsets,TRUE);
            binop_b=CompileX64Mode(a->binop.b,base,index,scale,offsets,TRUE);
            for(cnt=0;cnt!=2;cnt++) {
                if(binop_a&&binop_a->x64_mode->type==X64O_REG) {
                    switch(s=EvalAST(binop_b)) {
                        case 1:
                        case 2:
                        case 4:
                        case 8:
                        if(!*index) index=binop_a->x64_mode->reg;
                        else ParserFail2(a,"Only 1 SIB Index allowed.");
                        *scale=s;
                        return;
                        default:
                        ParserFail2(binop_b,"SIB offset must be 1,2,4 or 8(got %d).",s);
                    }
                }
                SwapI64(&binop_a,&binop_b);
            }
            VectorPush(offsets,a);
            break;
            case AST_NAME:
            rchk:
            if(a->type==AST_NAME&&(reg=HashFind(a->name,regs))) {
                ret=MAlloc(sizeof(AST));
                ret->type=AST_X64_MODE;
                ret->x64_mode=X64ModeReg(reg);
                return ret;
            } else if(reg_only)
                return NULL;
            default:
            VectorPush(offsets,a);
        }
    } catch {
        CatchIf('Parser','Lexer');
    }
    return NULL;
}
AST *ParseX64Mode(CCmpCtrl *cc) {
    I64 scale=0,r;
    CCompilerState *cstate;
    CFunction *patcher;
    CReg *seg=NULL,*base=NULL,*index=NULL;
    AST *tmp,*tmp2,*offset,*ret,*sum;
    CVector *offsets;
    CIRValue *ir_value;
    CType *addr_type=NULL;
    Bool is_indirect=FALSE;
    addr_type=ParseType(NULL,cc,NULL);
    tmp=ParseExprNoComma(cc); //Dont free as may contain the offset we need,leave it to the gc
    offsets=VectorNew;
    if(!tmp) goto sib;
    Compiler.asm_mode=TRUE;
    tmp2=CompileX64Mode(tmp,&base,&index,&scale,offsets,TRUE);
    loop:
    if(tmp2&&tmp2->type==AST_X64_MODE) {
        //Passed only_reg above
        if(!seg&&cc->token==':') {
            seg=    tmp2->x64_mode->reg;
            FreeAST(tmp2);
            Lex(cc);
            tmp2=CompileX64Mode(tmp,&base,&index,&scale,offsets,TRUE);
            goto loop;
        } else {
            base=tmp2->x64_mode->reg;
        }
    } else {
        FreeAST(tmp2);
        Compiler.no_array_access_mode=TRUE;
        //only_reg not passed
        tmp2=CompileX64Mode(tmp,&base,&index,&scale,offsets);
        Compiler.no_array_access_mode=FALSE;
        FreeAST(tmp2);
        tmp2=NULL;
        sib:
        if(cc->token=='[') {
            is_indirect=TRUE;
            Lex(cc);
            tmp=ParseExpr(cc);
            if(!tmp) ParserFail(cc,"Expected an SIB expression with an optional offset.");
            tmp2=CompileX64Mode(tmp,&base,&index,&scale,offsets);
            if(cc->token!=']') ParserFail(cc,"Expected a ']'.");
            Lex(cc);
            goto gotsib;
        }
        if(!tmp) return NULL;
    }
    if(cc->token=='[')
        goto sib;
    if(tmp2)
        return tmp2;
    gotsib:
    if(addr_type&&!is_indirect)
        ParserWarn(cc,"Got an address-type for non-indirect.");
    FreeAST(tmp2);
    ret=MAlloc(sizeof(AST));
    ret->type=AST_X64_MODE;
    ret->x64_mode_offset=NULL;
    while(offsets->size) {
        offset=VectorPop(offsets);
        if(!ret->x64_mode_offset)
            ret->x64_mode_offset=offset;
        else {
            tmp=MAlloc(sizeof(AST));
            tmp->type=AST_ADD;
            tmp->binop.a=ret->x64_mode_offset;
            tmp->binop.b=offset;
            ret->x64_mode_offset=tmp;
        }
    }
    if(!is_indirect) {
        if(base)
            ret->x64_mode=X64ModeReg(base);
        else
            //See x64_mode_offsets;
            ret->x64_mode=X64ModeImm(0x1122334455); //Will be trimmed down to fit in a lesser-sized value dft is 64bit
    } else {
        if(addr_type)
            r=addr_type->type;
        else
            r=0;
        if(!base&&!index)
            base=HashFind("RIP",regs);
        ret->x64_mode=X64ModeMem(seg,scale,index,base,0x11223344,r);
    }
    VectorFree(offsets);
    Compiler.asm_mode=FALSE;
    return ret;
}
U0 ApplyAsmPatches(CFunction *f) {
    CAsmPatcher *patcher=f->asm_patches,*next;
    I64 value;
    I64 (*fptr)();
    for(;patcher;) {
        PatchAsmPatcher(f,patcher);
        value=(*(fptr=patcher->func_ptr))();
        if(patcher->is_rel)
            value-=patcher->ptr(I64)+patcher->width;
        switch(patcher->width) {
            case 1:
            patcher->ptr(U8*)[0]=value;
            break;
            case 2:
            patcher->ptr(U16*)[0]=value;
            break;
            case 4:
            patcher->ptr(U32*)[0]=value;
            break;
            case 8:
            patcher->ptr(U64*)[0]=value;
            break;
        }
        next=patcher->next_patch;
        FreeFunction(patcher);
        patcher=next;
    }
    if(!Compiler.aot_mode);
    f->asm_patches=NULL;
}
//Name Compiler.label_scope
#define LOCAL_LAB_FMT "%s.%d"
U0 PatchAsmPatcher(CFunction *parent,CAsmPatcher *patcher ) {
    I64 idx,__local_scope;
    U8 *local_lab_name=MAlloc(STR_LEN);
    CRelocation *reloc;
    CVariable *nvar,*var;
    CLabel *lab;
    CHashTable *old_globs=Compiler.globals;
    Compiler.globals=HashTableNew(0x10);
    idx=parent->locals.mask+1;
    while(--idx>=0) {
        for(var=parent->locals->body[idx];var;var=var->next) {
            if(var->type==HTT_VAR) {
                nvar=MAlloc(sizeof(CVariable));
                nvar->str=StrNew(var->str);
                nvar->type=HTT_VAR;
                nvar->is_static=TRUE;
                nvar->static_refs=VectorNew;
                nvar->is_extern_ptr=TRUE;
                if(var->is_static) {
                    nvar->static_ptr=var->static_ptr;
                } if(!var->isReg) {
                    nvar->static_ptr=var->frame_off;
                }
                HashAdd(nvar,Compiler.globals);
            }
        }
    }
    idx=parent->labels->mask+1;
    while(--idx>=0) {
        for(lab=parent->labels->body[idx];lab;lab=lab->next) {
            if(lab->is_local) {
                StrScan(lab->str,LOCAL_LAB_FMT,&local_lab_name,&__local_scope);
                if(patcher->label_scope!=__local_scope)
                    goto next;
            } else
                StrCpy(local_lab_name,lab->str);
            nvar=MAlloc(sizeof(CVariable));
            nvar->str=StrNew(local_lab_name);
            nvar->type=HTT_VAR;
            nvar->is_static=TRUE;
            nvar->static_ptr=lab->ptr;
            nvar->is_extern_ptr=TRUE;
            nvar->hc_type=GetType("U8i")+1;
            nvar->static_refs=VectorNew;
            HashAdd(nvar,Compiler.globals);
            next:
        }
    }
    FillInRelocations(patcher);
    Free(local_lab_name);
    HashTableDel(Compiler.globals,&HTT_LocalDestroy);
    Compiler.globals=old_globs;
    FillInRelocations(patcher);
    for(reloc=patcher->relocs;reloc;reloc=reloc->next) {
        if(!HashFind(reloc->name,Compiler.globals)) {
            //ParserFail2(cc,"Function '%s' has undefined reference to non-existant label '%s'.",f->str,reloc->name);
        }
    }
}
U0 CompileAsmConst(AST *mode,I64 width,Bool is_rel=FALSE,I64 local_scope=-2) {
    AST *offset;
    Bool old_asm_mode=Compiler.asm_mode;
    CFunction *func;
    CCompilerState *cstate;
    CHashTable *old_locs=Compiler.locals;
    I64 r,idx,*value,cnt=0,__local_scope=-1;
    CVariable *var,*nvar;
    CAsmPatcher *patcher;
    CLabel *lab;
    CIRValue *ir_value;
    U8 *local_lab_name=MAlloc(128);
    I64 (*fptr)();
    if(1) {
        patcher=MAlloc(sizeof(CAsmPatcher));
        jit_dump_ptr(Compiler.jit,&patcher->ptr);
        switch(width) {
            case 1:
            jit_data_byte(Compiler.jit,0);
            break;
            case 2:
            jit_data_word(Compiler.jit,0);
            break;
            case 4:
            jit_data_dword(Compiler.jit,0);
            break;
            case 8:
            jit_data_qword(Compiler.jit,0);
            break;
        }
        cstate=NewCompilerState;
        PushIRReg(GetType("I64i"),r=GetFreeIReg);
        try {
            old_asm_mode=Compiler.asm_mode;
            Compiler.asm_mode=TRUE;
            AssignTypeToNode(mode);
            CompileAST(mode);
        } catch {
            Compiler.asm_mode=old_asm_mode;
        }
        Compiler.asm_mode=old_asm_mode;
        Assign(Compiler.stack->last,Compiler.stack);
        #if IsWindows
        if(Compiler.debug_mode) {
            jit_prepare(Compiler.jit);
            jit_call(Compiler.jit,&WindowsLeaveFunc);
        }
        #endif
        jit_retr(Compiler.jit,jit_R(r));
        PopIRValue,PopIRValue;
        *patcher=*(func=CompilerEmitCode);
        patcher->width=width;
        patcher->is_rel=is_rel;
        patcher->label_scope=local_scope;
        //Free pointer not contents
        Free(func);
        FillInRelocations(patcher);
        RestoreCompilerState(cstate);
    }
    patcher->next_patch=Compiler.asm_patches;
    Compiler.asm_patches=patcher;
    Free(local_lab_name);
}
AST *ParseOpcode(CCmpCtrl *cc) {
    I64 acnt,idx;
    AST     *ret,**args;
    Bool asm_mode2=Compiler.asm_mode;
    if(cc->token!=TK_IDENT) return NULL;
    if(!HashFind(cc->cur_str,opcodes)) return NULL;
    ret=LSP(cc,MAlloc(sizeof(AST)));
    COpcode *opc=HashFind(cc->cur_str,opcodes);
    acnt=opc->arg_count;
    args=MAlloc(sizeof(AST*)*acnt);
    Lex(cc);
    Compiler.asm_mode=TRUE;
    for(idx=0;idx!=acnt;idx++) {
        if(!(args[idx]=ParseX64Mode(cc)))
            ParserFail(cc,"Expected addressing mode.");
        if(idx+1<acnt)
            if(cc->token!=',')
                ParserFail(cc,"Expected addressing mode.");
            else
                Lex(cc);
    }
    Compiler.asm_mode=asm_mode2;
    ret->type=AST_OPCODE;
    ret->opcode.opcode=opc;
    ret->opcode.args=args;
    ret->label_scope=Compiler.label_scope;
    return ret;
}
AST *ParseAsmBlock(CCmpCtrl *cc) {
    AST *first=NULL,*last=NULL,*cur,**arr,*arr_first,*arr_cur,*arr_last;
    U8 *name,*new_name;
    I64 cnt;
    CLabel *label;
    if(cc->token!=TK_IDENT) return NULL;
    if(StrCmp(cc->cur_str,"asm")) return NULL;
    //Compute possible labels later.
    Compiler.asm_mode=TRUE;
    Lex(cc);
    if(cc->token!='{') ParserFail(cc,"Expected '{'.");
    Lex(cc);
    loop:
    if(cc->token==';') {
        Lex(cc);
        goto loop;
    } if(cc->token=='}') {
        Lex(cc);
        Compiler.asm_mode=FALSE;
        if(!first) {
            first=MAlloc(sizeof(AST));
            first->type=AST_NOP;
        }
        return first;
    }
    if(cc->token==TK_IDENT) {
        if(cur=ParseOpcode(cc)) {
            goto next;
            next:
            cur->label_scope=Compiler.label_scope;
            if(!first) first=cur;
            if(last) last->next=cur;
            last=cur;
            goto loop;
        }
        if(!StrCmp("DU8",cc->cur_str)) {
            cur=LSP(cc,MAlloc(sizeof(AST)));
            cur->type=AST_DU8;
            data_parse:
            Lex(cc);
            cnt=0;
            arr_first=NULL;
            dloop:
            if(cc->token==';') {
                arr=MAlloc(sizeof(AST*)*cnt);
                for(cnt=0;arr_first;arr_first=arr_first->next,cnt++) {
                    arr[cnt]=arr_first;
                    if(cnt) arr[cnt]->next=NULL;
                }
                cur->arr_lit=arr;
                goto next;
            } else {
                arr_cur=ParseExprNoComma(cc);
                if(!arr_cur) ParserFail(cc,"Expected expression.");
                if(!arr_first) arr_first=arr_cur;
                if(arr_last) arr_last->next=arr_cur;
                arr_last=arr_cur;
                cnt++;
                if(cc->token==',')
                    Lex(cc);
                else if(cc->token!=';')
                    ParserFail(cc,"Expected ','");
                goto dloop;
            }
        } else if(!StrCmp("DU16",cc->cur_str)) {
            cur=MAlloc(sizeof(AST));
            cur->type=AST_DU16;
            goto data_parse;
        } else if(!StrCmp("DU32",cc->cur_str)) {
            cur=MAlloc(sizeof(AST));
            cur->type=AST_DU32;
            goto data_parse;
        } else if(!StrCmp("DU64",cc->cur_str)) {
            cur=MAlloc(sizeof(AST));
            cur->type=AST_DU64;
            goto data_parse;
        } else if(!StrCmp("USE16",cc->cur_str)) {
            ParserFail(cc,"USE16 not implemented.");
        } else if(!StrCmp("USE32",cc->cur_str)) {
            ParserFail(cc,"USE32 not implemented.");
        } else if(!StrCmp("USE64",cc->cur_str)) {
            Lex(cc);
            goto loop;
        } else if(!StrCmp("IMPORT",cc->cur_str)) {
            Lex(cc);
            iloop:
            if(cc->token==';') {
                Lex(cc);
            } else if(cc->token==TK_IDENT) {
                cur=MAlloc(sizeof(AST));
                LSP(cc,cur);
                cur->type=AST_IMPORT;
                cur->name=StrNew(cc->cur_str);
                if(!first) first=cur;
                if(last) last->next=cur;
                last=cur;
                Lex(cc);
                if(cc->token!=';'&&cc->token!=',')
                    ;
                else
                    ParserFail(cc,"Expected ';'.");
                goto iloop;
            }
        } else if(!StrCmp("LIST",cc->cur_str)) {
            //TODO
            Lex(cc);
            goto loop;
        } else if(!StrCmp("NOLIST",cc->cur_str)) {
            //TODO
            Lex(cc);
            goto loop;
        } else if(!StrCmp("ALIGN",cc->cur_str)) {
            cur=LSP(cc,MAlloc(sizeof(AST)));
            cur->type=AST_ALIGN;
            Lex(cc);
            arr_cur=ParseExprNoComma(cc);
            if(!arr_cur) ParserFail(cc,"Expected expression.");
            cur->align.to=EvalAST(arr_cur);
            FreeAST(arr_cur);
            if(cc->token!=',') ParserFail(cc,"Expected ','.");
            arr_cur=ParseExprNoComma(cc);
            if(!arr_cur) ParserFail(cc,"Expected expression.");
            cur->align.fill=EvalAST(arr_cur);
            FreeAST(arr_cur);
            goto next;
        } else if(!StrCmp("BINFILE",cc->cur_str)) {
            if(cc->token!=TK_STR) ParserFail(cc,"Expected");
            cur=LSP(cc,MAlloc(sizeof(AST)));
            cur->type=AST_BINFILE;
            cur->name=StrNew(cc->cur_str);
            Lex(cc);
            goto next;
        }

        name=StrNew(cc->cur_str);
        LSP(cc,cur=MAlloc(sizeof(AST)));
        Lex(cc);
        switch(cc->token) {
            start:
            cur->name=name;
            label=MAlloc(sizeof(CLabel));
            label->type=HTT_LABEL;
            label->local_scope=Compiler.label_scope;
            label->refs=VectorNew;
            case TK_DBL_COLON:
            label->is_exported=TRUE;
            Compiler.label_scope++;
            break;
            case ':':
            if(!StrNCmp(name,"@@",2)) {
                cur->type=AST_LOCAL_LABEL;
                label->is_local=TRUE;
                new_name=MStrPrint(LOCAL_LAB_FMT,name,Compiler.label_scope);
                Free(name);
                name=new_name;
                cur->name=name;
            } else {
                cur->type=AST_LABEL;
                Compiler.label_scope++;
            }
            end:
            label->str=StrNew(name);
            HashAdd(label,Compiler.labels);
            Lex(cc);
            goto next;
            default:
            ParserFail(cc,"Expected label ending.");
        }
    }
    goto loop;
}
U0 __LoadBuiltin(U8 *name,U0 *fptr,I64 cnt) {
    CFunction *func=MAlloc(sizeof(CFunction)),*old;
    CVariable *oldv;
    oldv=old=HashFind(name,Compiler.globals);
    func->type=HTT_FUNC;
    if(old) {
      if(old->type==HTT_FUNC)
        func->hc_type=old->hc_type;
      else if(oldv->type=HTT_VAR) {
        oldv->global_ptr=fptr;
        oldv->link.type=0;
        Free(func);
        return;
      }
    } else
        func->hc_type=GetType("U0");
    func->func_ptr=fptr;
    func->is_aot=TRUE;
    func->aot_bin_size=cnt;
    func->str=StrNew(name);
    HashAdd(func,Compiler.globals);
}
U0 LoadBuiltins() {
    CCmpCtrl *cc=CmpCtrlNew(NULL);
    LexIncludeStr(cc,NULL,__GetBuiltinMacrosText,FALSE);
    Lex(cc);
    CCompilerState *state=NewCompilerState;
    AST *s;
    while(s=ParseStmt(cc)) {
        FreeAST(s);
    }
    Lex(cc);
    RestoreCompilerState(state);
    ForeachFuncInTable(&__LoadBuiltin);
}
#include "PARSER.HH"
U0 REPL(U8 *init=NULL,I64 flags=PF_DEBUGGER,U8 *body=NULL) {
    CCmpCtrl *cc=CmpCtrlNew(NULL);
    if(init)
        LexIncludeStr(cc,NULL,StrNew(init),FALSE);
    Fs->last_cc=Compiler.cc=cc;
    AST *ast;
    Compiler.debug_mode=FALSE;
    if(flags&PF_BOUNDS_CHECK) {
        "Bounds-Checking (and debugger) enabled.\n";
        Compiler.bounds_check_mode=TRUE;
        Compiler.debug_mode=TRUE;
    } else if(flags&PF_DEBUGGER)  {
        "Debugger enabled.\n";
        Compiler.debug_mode=TRUE;

    }
    //Load Forward declarations before assigning values
    Lex(cc);
    try {
        while(ast=ParseStmt(cc)) {
            EvalAST(ast);
            FreeAST(ast);
        }
    } catch {
        Fs->catch_except=TRUE;
    }
    LoadBuiltins;
    if(body) {
        LexIncludeStr(cc,NULL,StrNew(body),FALSE);
    }
    for(;;) {
        try {
            LexSetReplMode(Compiler.cc,TRUE);
            Lex(cc);
            Compiler.error_count=0;
            ast=ParseStmt(cc,TRUE);
            if(ast) {
                EvalAST(ast);
                FreeAST(ast);
            }
        } catch {
            Fs->catch_except=TRUE;
            "Caught a %c.\n",Fs->except_ch;
            #ifdef SIGNAL
            ArmSignals;
            #endif
        }
    }
}
//REPL;
U8 *WriteIfPresent(U8 *ptr,U8 *inp,I64 sz,I64 *written) {
    if(written) *written+=sz;
    if(!ptr) return NULL;
    MemNCpy(ptr,inp,sz);
    return ptr+sz;
}
class CBinPatch {
    I64 offset;
    U8 name[64];
};
class CBinLabel {
    I64 offset,type;
    #define BIN_LABEL_LOCAL 0
    #define BIN_LABEL_NORMAL 1
    #define BIN_LABEL_EXPORTED 2
    U8 name[64]    ;
};
class CBinStatic {
    I64 size;
    U8 name[64];
};
class CBinString {
    I64 offset,len;
    //Data appended here
} ;
/**
 * CBinAsmPatch {
 *     I64 is_rel;
 *     I64 rel_offset;
 *     I64 width;
 *     I64 offset;
 *     I64 label_context
 * };
 */
class CBinAsmPatch {
    I64 is_rel;
    I64 rel_offset;
    I64 width;
    I64 offset;
    I64 labelContext;
};
/**
 * BinFunc:
 *          U8 ident=1;
 *          U8 name[];
 *          i64 patch_count;
 *          CBinPatch patches[];
 *          I64 string_count
 *          CBinString strings[];
 *          I64 static_count;
 *          CBinStatic statics;
 *          I64 label_count
 *          CBinLabel labels;
 *          I64 binsize;
 *          U8 func_bytesc[];
 *          I64 asmPatches;
 *          class {CBinAsmPatch info;BinFunc applyer;} patches[];
 */
CHashTable *AOTStrings=HashTableNew(0x100);
U8 *LoadFunction(U8 *from,Bool verbose=FALSE,CFunction **dump_to=NULL) {
    U8 *ptr;
    I64 cnt,off,len,pcnt;
    CBinPatch *patches;
    CBinString **strings,*string;
    CHashTable *statics,*labels;
    CVariable *static_var;
    CFunction *func,*tfunc;
    CRelocation *reloc;
    CBinLabel blabel;
    CCompString *comp_string;
    CLabel *label;
    CBinAsmPatch basm_patch;
    CAsmPatcher *patcher;
    if(*from!=1) return from;
    func=MAlloc(sizeof(CFunction));
    statics=HashTableNew(0x10);
    labels=HashTableNew(0x20);
    for(ptr=++from;*ptr;ptr++);
    U8 name[256];
    StrNCpy(name,from,ptr-from+1);
    from=ptr+1;
    if(verbose)
        "FUNC:%s\n",name;
    //Patches
    cnt=*(from(I64*));
    pcnt=cnt;
    from+=8;
    patches=MAlloc(cnt*sizeof(CBinPatch));
    while(--cnt>=0) {
        patches[cnt]->offset=from(I64*)[0];
        from+=8;
        StrCpy(patches[cnt]->name,from);
        from+=StrLen(patches[cnt]->name)+1;
        if(verbose)
            "PATCH:%s(%d)\n",patches[cnt]->name,patches[cnt]->offset;
    }
    //Strings
    cnt=*(from(I64*));
    from+=8;
    strings=MAlloc(sizeof(CBinString*)*cnt);
    while(--cnt>=0) {
        off=from(I64*)[0];
        from+=8;
        len=from(I64*)[0];
        from+=8;
        string=MAlloc(sizeof(CBinString)+len);
        string->offset=off;
        string->len=len;
        StrNCpy(string+1,from,len);
        strings[cnt]=string;
        from+=len;
        if(verbose)
            "STRING:%s(%d)\n",string+1,string->offset;
    }
    //Statics
    cnt=from(I64*)[0];
    from+=8;
    while(--cnt>=0) {
        static_var=MAlloc(sizeof(CVariable));
        static_var->str=StrNew(from(CBinStatic*)->name);
        static_var->global_ptr=MAlloc(from(CBinStatic*)->size);
        from+=sizeof(CBinStatic);
    }
    //Labels
    cnt=from(I64*)[0];
    from+=8;
    while(--cnt>=0) {
        blabel.offset=from(I64*)[0];
        from+=8;
        blabel.type=from(I64*)[0];
        from+=8;
        label=MAlloc(sizeof(CLabel));
        label->type=HTT_LABEL;
        label->str=StrNew(from);
        from+=StrLen(from)+1;
        switch(blabel.type) {
            case BIN_LABEL_LOCAL:
            label->is_local=TRUE;
            break;
            case BIN_LABEL_NORMAL:
            break;
            case BIN_LABEL_EXPORTED:
            label->is_exported=TRUE;
            break;
        }
        HashAdd(label,labels);
        if(verbose)
            "LABEL:%s(%d)\n",label->str,blabel.offset;
    }
    //Binary
    cnt=from(I64*)[0];
    from+=8;
    func->str=StrNew(name);
    func->is_aot=TRUE;
    func->aot_bin_size=cnt;
    func->func_ptr=MAlloc(cnt);
    MemCpy(func->func_ptr,from,cnt);
    from+=cnt;
    while(--pcnt>=0) {
        if(static_var=HashFind(patches[pcnt]->name,statics)) {
            (func->func_ptr(U8*)+patches[pcnt]->offset)(U0**)[0]=static_var->global_ptr;
        } else {
            reloc=MAlloc(sizeof(CRelocation));
            StrCpy(reloc->name,patches[pcnt]->name);
            reloc->ptr=func->func_ptr(U8*)+patches[pcnt]->offset;
            reloc->next=func->relocs;
            func->relocs=reloc;
        }
    }
    Free(patches);
    func->locals=statics;
    func->type=HTT_FUNC;
    func->labels=labels;
    //Asm patches
    cnt=from(I64*)[0];
    from+=8;
    while(--cnt>=0) {
        basm_patch=from(CBinAsmPatch*)[0];
        from+=sizeof(basm_patch);
        patcher=MAlloc(sizeof(CAsmPatcher));
        from=LoadFunction(from,verbose,&tfunc);
        *patcher=*tfunc;
        patcher->width=basm_patch.width;
        patcher->is_rel=basm_patch.is_rel;
        patcher->ptr=basm_patch.offset+func->func_ptr;
        patcher->label_scope=basm_patch.labelContext;
        Free(tfunc);
        if(func->asm_patches)
            func->asm_patches->next_patch=patcher;
        func->asm_patches=patcher;
    }
    ApplyAsmPatches(func);
    //
    cnt=MSize(strings)/sizeof(CBinString*);
    while(--cnt>=0) {
        CCompString *cs=MAlloc(sizeof(CCompString));
        cs->str=HexifyString(strings[cnt]+1,strings[cnt]->len);
        cs->data=MemCpy(MAlloc(strings[cnt]->len),strings[cnt]+1,strings[cnt]->len);
        (func->func_ptr(U8*)+strings[cnt]->offset)(U8**)[0]=cs->data;
        HashAdd(cs,AOTStrings);
        Free(strings[cnt]);
    }
    Free(strings),Free(labels);
    if(!dump_to)
        HashAdd(func,Compiler.globals);
    if(dump_to) *dump_to=func;
    return from;
}
I64 SerializeFunc(U8 *to,CFunction *func) {
    I64 size,idx,written=0,tmp,label_context;
    CRelocation *reloc;
    CBinPatch patch1;
    CBinLabel bin_label;
    CBinString bstr;
    CLabel *label;
    CCompString *comp_string;
    CCompStringRef *cs_ref;
    CAsmPatcher *asm_patch;
    CBinAsmPatch asm_bin_patch;
    //ident
    tmp=1;
    to=WriteIfPresent(to,&tmp,1,&written);
    //name
    size=StrLen(func->str);
    to=WriteIfPresent(to,func->str,size+1,&written);
    //patches
    size=0;
    for(reloc=func->relocs;reloc;reloc=reloc->next) {
        size++;
    }
    tmp=size;
    to=WriteIfPresent(to,&tmp,8,&written);
    for(reloc=func->relocs;reloc;reloc=reloc->next) {
        patch1.offset=reloc->ptr(U8*)-func->func_ptr(U8*);
        to=WriteIfPresent(to,&patch1.offset,sizeof(patch1.offset),&written);
        to=WriteIfPresent(to,reloc->name,StrLen(reloc->name)+1,&written);
    }
    //strings
    tmp=0;
    idx=func->aot_strings->mask+1;
    while(--idx>=0) {
        for(comp_string=func->aot_strings->body[idx];comp_string;comp_string=comp_string->next) {
            for(cs_ref=comp_string->refs;cs_ref;cs_ref=cs_ref->next) tmp++;
        }
    }
    to=WriteIfPresent(to,&tmp,8,&written);
    idx=func->aot_strings->mask+1;
    while(--idx>=0) {
        for(comp_string=func->aot_strings->body[idx];comp_string;comp_string=comp_string->next) {
            for(cs_ref=comp_string->refs;cs_ref;cs_ref=cs_ref->next) {
                bstr.offset=cs_ref->ptr(U8*)-func->func_ptr(U8*);
                bstr.len=MSize(comp_string->data);
                to=WriteIfPresent(to,&bstr,8*2,&written);
                to=WriteIfPresent(to,comp_string->data,MSize(comp_string->data),&written);
            }
        }
    }
    //statics(are append at end of function binary)
    tmp=0;
    to=WriteIfPresent(to,&tmp,8,&written);
    //Labels
    idx=func->labels->mask+1,size=0;
    while(--idx>=0) {
        for(label=func->labels->body[idx];label;label=label->next) {
            if(label->type==HTT_LABEL) {
                size++;
            }
        }
    }
    tmp=size;
    to=WriteIfPresent(to,&tmp,8,&written);
    idx=func->labels->mask+1,size=0;
    while(--idx>=0) {
        for(label=func->labels->body[idx];label;label=label->next) {
            if(label->type==HTT_LABEL) {
                if(label->is_local) {
                    bin_label.type=BIN_LABEL_LOCAL;
                } else if(label->is_exported) {
                    bin_label.type=BIN_LABEL_EXPORTED;
                } else
                    bin_label.type=BIN_LABEL_NORMAL;
                bin_label.offset=label->ptr(U8*)-func->func_ptr(U8*);
                to=WriteIfPresent(to,&bin_label,2*sizeof(I64),&written);
                to=WriteIfPresent(to,label->str,StrLen(label->str)+1,&written);
            }
        }
    }
    //Function binary
    if(!func->func_ptr) throw('BinMod');
    size=jit_bin_size(func->jit);
    tmp=size;
    to=WriteIfPresent(to,&tmp,8,&written);
    to=WriteIfPresent(to,func->func_ptr,size,&written);
    //Asm Patches
    tmp=0;
    for(asm_patch=func->asm_patches;asm_patch;asm_patch=asm_patch->next)
        tmp++;
    to=WriteIfPresent(to,&tmp,8,&written);
    for(asm_patch=func->asm_patches;asm_patch;asm_patch=asm_patch->next) {
        asm_bin_patch.is_rel=asm_patch->is_rel;
        asm_bin_patch->rel_offset=asm_patch->width;
        asm_bin_patch->width=asm_patch->width;
        asm_bin_patch->offset=asm_patch->ptr(U8*)-func->func_ptr(U8*);
        asm_bin_patch->labelContext=asm_patch->label_scope;
        to=WriteIfPresent(to,&asm_bin_patch,sizeof(asm_bin_patch),&written);
        written+=SerializeFunc(to,asm_patch);
    }
    return written;
}
/**
 * Values are defined by function @@Main
 * BinVar: U8 ident=0;
 *         U8 is_internal(to module);
 *         U8 name[];
 *         I64 size;
 */
U8 *LoadVar(U8 *from,Bool verbose=FALSE) {
    CVariable *var;
    U8 *name;
    I64 sz;
    if(*from) return from;
    from++,++from;
    name=from;
    //Find NULL terminaotr
    while(*++from);
    from++;
    sz=from(I64*)[0];
    from+=8;
    var=MAlloc(sizeof(CVariable));
    var->global_ptr=MAlloc(sizeof(sz));
    var->hc_type=GetType("U0");
    var->str=StrNew(name);
    var->type=HTT_VAR;
    if(verbose) "VAR:%s(%d)\n",var->str,sz;
    HashAdd(var,Compiler.globals);
    return from;
}
I64 SerializeVar(U8 *to,CVariable *var) {
    I64 tmp=0,written=0;
    to=WriteIfPresent(to,&tmp,1,&written);
    to=WriteIfPresent(to,&tmp,1,&written);
    tmp=StrLen(var->str)+1;
    to=WriteIfPresent(to,var->str,tmp,&written);
    tmp=MSize(var->global_ptr);
    to=WriteIfPresent(to,&tmp,8,&written);
    return written;
}
I64 __CompileBinModule(U8 *to=NULL,U8 *header_text=NULL) {
    I64 written=0,idx=Compiler.globals->mask+1,tmp;
    CVariable *var;
    CFunction *func;
    CHashTable *already_ser=HashTableNew(0x400);
    CHash *ser;
    while(--idx>=0) {
        var=Compiler.globals->body[idx];
        for(;var;var=var->next) {
            if(!HashFind(var,already_ser))
            if(var->type==HTT_VAR&&!IsForward(var)) {
                written+=tmp=SerializeVar(to,var);
                if(to) to+=tmp;
                goto add;
            } else if(var->type==HTT_FUNC&&!IsForward(var)) {
                func=var;
                written+=tmp=SerializeFunc(to,func);
                if(to) to+=tmp;
                add:
                ser=MAlloc(sizeof(CHash));
                ser->str=StrNew(var->str);
                HashAdd(ser,already_ser);
            }
        }
    }
    if(header_text) {
        if(to) *to++=2;
        written++;
        if(to) StrNCpy(to,header_text,StrLen(header_text));
        written+=StrLen(header_text);
    }
    HashTableDel(already_ser);
    return written;
}
U0 LoadBinModule(U8 *fn,Bool verbose=FALSE) {
    CCmpCtrl *cc;
    AST *ast;
    I64 cnt,en;
    CFunction *aa_main=NULL,*func;
    U8 *bin=FileRead(fn,&cnt),*ptr;
    ptr=bin;
    loop:
    switch(*ptr) {
        case 1:
        ptr=LoadFunction(ptr,verbose);
        break;
        case 0:
        ptr=LoadVar(ptr,verbose);
        break;
        case 2:
        ptr++;
        cc=CmpCtrlNew(NULL);
        LexIncludeStr(cc,fn,MemCpy(MAlloc(cnt-(ptr-bin)+1),ptr,cnt-(ptr-bin)),TRUE);
        Lex(cc);
        while(ast=ParseStmt(cc)) {
            EvalAST(ast);
            FreeAST(ast);
        }
        goto en;
        default:
        "GOT A %d\n",*ptr;
        throw('AOTBIN');
    }
    if(ptr!=bin+cnt)
        goto loop;
    en:
    cnt=Compiler.globals->mask+1;
    while(--cnt>=0) {
        func=Compiler.globals->body[cnt];
        for(;func;func=func->next) {
            if(func->type==HTT_FUNC) {
                FillInRelocations(func);
                ApplyAsmPatches(func);
            }
        }
    }
    Free(bin);
    aa_main=HashFind("@@Main",Compiler.globals);
    if(aa_main) {
        en=PoopSetGCEnable(0);
        U0 (*fptr)()=aa_main->func_ptr;
        (*fptr)();
        PoopSetGCEnable(en);
    }
}
U8 *__GetBuiltinMacrosText();
U0 CompileBinModule(U8 *source,U8 *fn="A.BIN",U8 *header=NULL) {
    Bool old_aot=Compiler.aot_mode;
    Compiler.aot_mode=TRUE;
    LoadBuiltins;
    CCompiler *old_glob=GlobalCompiler;
    CCompilerState *cstate=NewCompilerState;
    GlobalCompiler=&Compiler;;
    I64 written;
    AST *stmt;
    U8 *header_text=NULL,*bin;
    if(header)
        header_text=FileRead(header,NULL);
    CCmpCtrl *cc=CmpCtrlNew(NULL);
    LexIncludeStr(cc,NULL,StrNew(source),FALSE);
    Lex(cc);
    while(cc->token) {
        stmt=ParseStmt(cc);
        CompileAST(stmt);
        PopIRValue;
        FreeAST(stmt);
    }
    jit_reti(Compiler.jit,0);
    CFunction *main=CompilerEmitCode;
    main->str=StrNew("@@Main");
    HashAdd(main,Compiler.globals);
    written=__CompileBinModule(NULL,header_text);
    bin=MAlloc(written);
    __CompileBinModule(bin,header_text);
    if(header_text)
        Free(header_text);
    FileWrite(fn,bin,written);
    Free(bin);
    RestoreCompilerState(cstate);
    GlobalCompiler=old_glob;
    Compiler.aot_mode=old_aot;
}

//
// These shenagins are for runtime.c for loading user-defined "builtin" types.
//
CType *LoaderCreateTypeFwd(U8 *name,I64 sz,I64 align) {
    CType *ret;
    I64 idx;
    if(ret=GetType(name))
        return ret;
    ret=MAlloc(sizeof(CType)*(1+STAR_CNT));
    ret->type=HTT_CLASS;
    ret->sz=sz,ret->align=align;
    ret->str=MStrPrint("%s",name);
    for(idx=0;idx!=STAR_CNT;idx++) {
        ret[idx+1].base=&ret[idx];
        ret[idx+1].star_cnt=idx+1;
        ret[idx+1].type=HTT_TYPE;
        ret[idx+1].sz=ret[idx+1].align=8;
        ret[idx+1]->str=MStrPrint("%s*",ret[idx]->str);
    }
    HashAdd(ret,Compiler.globals);
    return ret;
}
CType *LoaderAddMember(CType *dst,CType *t,U8 *name,I64 off) {
    CTypeMember *mem=MAlloc(sizeof(CTypeMember)),*tmp;
    mem->name=StrNew(name);
    mem->offset=off;
    mem->type=t;
    if(!dst->first_member) dst->first_member=mem;
    else {
        for(tmp=dst->first_member;tmp->next;tmp=tmp->next);
        tmp->next=mem;
    }
}
CType *LoaderCreateArrayType(U8 *basen,I64 dim) {
    I64 idx;
    CType *base=GetType(basen),*t=MAlloc(sizeof(CType)*(1+STAR_CNT));
    if(!base) {
        Free(t);
        return NULL;
    }
    t->type=HTT_ARRAY_TYPE;
    t->dim=dim;
    t->base=base;
    t->align=base->align;
    t->sz=dim*base->sz;
    t->str=MStrPrint("%s[%d]",base->str,dim);
    for(idx=0;idx!=STAR_CNT;idx++) {
        t[idx+1].base=&t[idx];
        t[idx+1].star_cnt=idx+1;
        t[idx+1].type=HTT_TYPE;
        t[idx+1].sz=t[idx+1].align=8;
        t[idx+1]->str=MStrPrint("%s*",t[idx]->str);
    }
    HashAdd(t,Compiler.globals);
    return t;
}
/*
try {
    REPL;
} catch {
    Fs->catch_except=TRUE;
};
*/
I64 JIT_Eval(U8 *text,U8 **unused) {
    CCmpCtrl *cc=CmpCtrlNew(NULL);
    AST *expr;
    I64 ret;
    LexIncludeStr(cc,NULL,StrNew(text),FALSE);
    Lex(cc);
    expr=ParseExpr(cc);
    ret=EvalAST(expr);
    FreeAST(expr);
    //TODO free cc
    return ret;
}
#ifdef BOUNDS_CHECK
U0 BtOnOOB(U8 *ptr,I64 sz) {
    if(!WhineOnOutOfBounds(ptr,sz))
        Backtrace(-1,0);
}
U0 CompileBoundsCheck(I64 r,I64 sz,I64 off=0) {
    I64 tmp;
    if(Compiler.bounds_check_mode) {
        if(off) {
            tmp=2; //Unused register typeically used for function calls.
            jit_addi(Compiler.jit,jit_R(tmp),jit_R(r),off);
        } else
            tmp=r;
        jit_prepare(Compiler.jit);
        jit_putargr(Compiler.jit,jit_R(tmp));
        jit_putargi(Compiler.jit,sz);
        jit_call(Compiler.jit,&BtOnOOB);
    }
}
#else
U0 CompileBoundsCheck(I64 r,I64 sz,I64 off=0){}
#endif
#ifdef STAGE1
GC_Collect;
RegisterRuntimeClasses(&GetType,&LoaderCreateTypeFwd,&LoaderAddMember,&LoaderCreateArrayType);
    CompileBinModule("#define BOUNDS_CHECK\n#define SIGNAL\n#define RUNTIME_C\n#include \"PARSER.HC\"#include \"HolyEd/EDITOR.HC\";","REPL.BIN","HCRT/HCRT.HH");
#endif
//See rl.c
I64 __HCCompetions2(U8 *buf,U8 **dst) {
    I64 idx,buf_len=StrLen(buf),cnt=0;
    CHash *item;
    idx=lexer_macros->mask+1;
    while(--idx>=0) {
        for(item=lexer_macros->body[idx];item;item=item->next)
            if(!StrNCmp(buf,item->str,buf_len)) {
                if(dst) *dst++=StrNew(item->str);
                cnt++;
            }
    }
    idx=Compiler.globals->mask+1;
    while(--idx>=0) {
        for(item=Compiler.globals->body[idx];item;item=item->next)
            if(!StrNCmp(buf,item->str,buf_len)) {
                if(dst) *dst++=StrNew(item->str);
                cnt++;
            }
    }
    return cnt;
}
U8 **__HCCompetions(U8 *buf) {
    U8 **ret;
    I64 cnt=__HCCompetions2(buf,NULL);
    if(!cnt) return NULL;
    ret=MAlloc(sizeof(U8 *)*cnt);
    __HCCompetions2(buf,ret);
    return ret;
}
I64 __WriteTags(U8 *write_t=NULL) {
    I64 off=0,idx,ln;
    CHashSrcSym *hash,*tmp2;
    CHashTable *names=HashTableNew(0x100);
    CType *type,*item_type;
    CTypeMember *member;
    CVariable *var;
    CFunction *func;
    U8 fn[1024];
    U8 *tmp,*fn_ptr=fn;
    for(idx=lexer_macros.mask;idx>=0;idx--) {
        for(hash=lexer_macros.body[idx];hash;hash=hash->next) {
            if(hash->type==HTT_DEFINE_STR&&hash->src_link&&!HashFind(hash->str,names)) {
                tmp2=MAlloc(sizeof(CHash));
                tmp2->str=StrNew(hash->str);
                HashAdd(tmp2,names);
                StrScan(hash->src_link,"%s,%d",&fn_ptr,&ln);
                tmp=MStrPrint("%s\t%s\t%d;\tkind:d\tdisplay:%s:Macro\n",hash->str,fn,ln,hash->str);
                off+=StrLen(tmp);
                if(write_t) {
                    StrCpy(write_t,tmp);
                    write_t+=StrLen(tmp);
                }
                Free(tmp);
            }
        }
    }
    for(idx=Compiler.globals.mask;idx>=0;idx--) {
        for(hash=Compiler.globals.body[idx];hash;hash=hash->next) {
            U8 *kind="?";
            func=var=NULL;
            switch(hash->type) {
            start:
            case HTT_VAR:
                kind="v";
                var=hash;
                break;
            case HTT_FUNC:
                kind="f";
                func=hash;
                break;
            case HTT_CLASS:
                kind="c";
                type=hash;
                goto mems;
            case HTT_UNION:
                kind="u";
                type=hash;
                mems:;
                for(member=type->first_member;member;member=member->next) {
                    if(member->src_link) {
                        StrScan(member->src_link,"%s,%d",&fn_ptr,&ln);
                        tmp=MStrPrint("%s\t%s\t%d;\tkind:m\tdisplay:%s.%s\n",member->name,fn,ln,hash->str,member->name);
                        off+=StrLen(tmp);
                        if(write_t) {
                            StrCpy(write_t,tmp);
                            write_t+=StrLen(tmp);
                        }
                        Free(tmp);
                    }
                }
            end:
                if(hash->src_link&&!HashFind(hash->str,names)) {
                    tmp2=MAlloc(sizeof(CHash));
                    tmp2->str=StrNew(hash->str);
                    HashAdd(tmp2,names);
                    StrScan(hash->src_link,"%s,%d",&fn_ptr,&ln);
                    if(var)
                        tmp=MStrPrint("%s\t%s\t%d;\tkind:%s\tdisplay:%s:%s\n",hash->str,fn,ln,kind,hash->str,var->hc_type->str);
                    else if(func)
                        tmp=MStrPrint("%s\t%s\t%d;\tkind:%s\tdisplay:%s:%s\n",hash->str,fn,ln,kind,hash->str,func->hc_type->str);
                    else
                        tmp=MStrPrint("%s\t%s\t%d;\tkind:%s\n",hash->str,fn,ln,kind);
                    off+=StrLen(tmp);
                    if(write_t) {
                        StrCpy(write_t,tmp);
                        write_t+=StrLen(tmp);
                    }
                    Free(tmp);
                }
            }
        }
    }
    HashTableDel(names);
    return off;
}
U0 WriteTagsAndErrsToFile(U8 *src,U8 *tags=NULL,U8 *errs=NULL) {
    CCmpCtrl *cc=CmpCtrlNew(NULL),*old_cc=Compiler.cc;
    LexIncludeStr(cc,NULL,StrNew(src),FALSE);
    CCompilerState *state;
    AST *ast;
    I64 cnt;
    U8 *ftext,*dir=DirCur;
    Error_file_text=StrNew("");
    state=NewCompilerState;
    Compiler.cc=cc;
    LexSetReplMode(cc,FALSE);
    //TODO add explicit flags;Force no compile
    Compiler.error_count=1;
    try {
        for(Lex(cc);;) {
            ast=ParseStmt(cc);
            if(!ast) break;
            FreeAST(ast);
        }
    } catch {
        Fs->catch_except=TRUE;
    }
    Cd(dir);
    Free(dir);
    Compiler.cc=old_cc;
    if(tags) {
        cnt=__WriteTags;
        ftext=MAlloc(cnt+1);
        __WriteTags(ftext);
        FileWrite(tags,ftext,cnt);
        Free(ftext);
    }
    if(errs) {
        FileWrite(errs,Error_file_text,StrLen(Error_file_text));
    }
    RestoreCompilerState(state);
    Free(Error_file_text);
    Error_file_text=NULL;
}
Bool ParseAndRun(CCmpCtrl *cc) {
    AST *t=ParseStmt(cc);
    if(t) {
        EvalAST(t);
        FreeAST(t);
        return TRUE;
    }
    return FALSE;
}


