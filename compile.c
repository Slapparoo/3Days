#include "3d.h"
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <setjmp.h>
#ifdef TARGET_WIN32
#include <memoryapi.h>
#endif
COldFuncState CreateCompilerState() {
    COldFuncState old;
    old.asmPatches=Compiler.asmPatches;
    old.locals=Compiler.locals;
    old.returnType=Compiler.returnType;
    old.inFunction=Compiler.inFunction;
    old.errorFlag=Compiler.errorFlag;
    old.debugMode=Compiler.debugMode;
    old.JIT=Compiler.JIT;
    old.frameOffset=Compiler.frameOffset;
    old.tmpIntRegStart=Compiler.tmpIntRegStart;
    old.tmpFltRegStart=Compiler.tmpFltRegStart;
    old.valueStack=Compiler.valueStack;
    old.breakops=Compiler.breakops;
    old.labels=Compiler.labels;
    old.labelRefs=Compiler.labelRefs;
    old.labelPtrs=Compiler.labelPtrs;
    return old;
}
void RestoreCompilerState(COldFuncState old) {
    Compiler.locals=old.locals;
    Compiler.returnType=old.returnType;
    Compiler.inFunction=old.inFunction;
    Compiler.errorFlag=old.errorFlag;
    Compiler.debugMode=old.debugMode;
    Compiler.JIT=old.JIT;
    Compiler.frameOffset=old.frameOffset;
    Compiler.tmpIntRegStart=old.tmpIntRegStart;
    Compiler.tmpFltRegStart=old.tmpFltRegStart;
    Compiler.valueStack=old.valueStack;
    Compiler.breakops=old.breakops;
    Compiler.labels=old.labels;
    Compiler.labelRefs=old.labelRefs;
    Compiler.labelPtrs=old.labelPtrs;
    Compiler.asmPatches=old.asmPatches;
    
}
static int IsControlNode(AST *node) {
    switch(node->type) {
    case AST_IF:
    case AST_FOR:
    case AST_DO:
    case AST_WHILE:
    case AST_SWITCH:
        return 1;
    default:
        return 0;
    }
}
static void CompileBodyWithBreakpoints(AST *body) {
    if(!IsControlNode(body)&&body->type!=AST_STMT_GROUP)
        CompileBreakpoint(body);
    __CompileAST(body);
}
static int GetIntTmpReg();
static void GrabXBytes(long dstreg,long ptrreg,long size);
static int IsCompositeType(CType *_t) {
    CType *t=BaseType(_t);
    return t->type==TYPE_CLASS||t->type==TYPE_UNION;
}

static void PutXBytes(long srcreg,long ptrreg,long size);
static jit_value MoveValueToIntRegIfNeeded(CValue v,int r);
static jit_value MoveValueToFltRegIfNeeded(CValue v,int r);
static double F64And(double a,double b) {
    return (*(uint64_t*)&a)&(*(uint64_t*)&b);
}
static double F64Or(double a,double b) {
    return (*(uint64_t*)&a)&(*(uint64_t*)&b);
}
static double F64Xor(double a,double b) {
    return (*(uint64_t*)&a)&(*(uint64_t*)&b);
}
static double F64Shl(double a,int64_t b) {
    return (*(uint64_t*)&a)<<b;
}
static double F64Shr(double a,int64_t b) {
    return (*(uint64_t*)&a)>>b;
}
static char *__ArgsToStr(CType *f) {
    vec_char_t ret;
    vec_init(&ret);
    vec_push(&ret, '(');
    CType *t;
    int i;
    vec_foreach(&f->func.arguments, t, i) {
        char *ts=TypeToString(t);
        vec_pusharr(&ret, ts, strlen(ts));
        if(i+1!=f->func.arguments.length) vec_push(&ret, ',');
        TD_FREE(ts);
    }
    vec_push(&ret, ')');
    vec_push(&ret, '\x00');
    return ret.data;
}
char *TypeToString(CType *t) {
    char buffer[1024];
    switch(t->type) {
    case TYPE_ARRAY_REF:
    case TYPE_ARRAY: {
        char *bt=TypeToString(t->array.base);
        if(-1==t->array.dim)
            sprintf(buffer, "%s[]",bt);
        else
            sprintf(buffer, "%s[%ld]",bt,t->array.dim);
        TD_FREE(bt);
        goto ret;
    }
    case TYPE_BOOL:
        return strdup("Bool");
    case TYPE_CLASS: {
        if(t->cls.name)
            return strdup(t->cls.name);
        else
            sprintf(buffer, "<Anon-class:%p>", t);
        goto ret;
    }
    case TYPE_F64:
        return strdup("F64");
    case TYPE_FUNC: {
        char *rtype=TypeToString(t->func.ret);
        char *args=__ArgsToStr(t);
        sprintf(buffer, "%s%s", rtype,args);
        TD_FREE(rtype),TD_FREE(args);
        goto ret;
    }
    case TYPE_I16:
        return strdup("I16i");
    case TYPE_I32:
        return strdup("I32i");
    case TYPE_I64:
        return strdup("I64i");
    case TYPE_I8:
        return strdup("I8i");
    case TYPE_PTR: {
        vec_char_t ptrs;
        vec_init(&ptrs);
        while(t->type==TYPE_PTR) {
            t=t->ptr;
            vec_push(&ptrs,'*');
        }
        vec_push(&ptrs, 0);
        if(t->type==TYPE_FUNC) {
            char *rtype=TypeToString(t->func.ret);
            char *args=__ArgsToStr(t);
            sprintf(buffer, "%s(%s)%s", rtype,ptrs.data,args);
            TD_FREE(rtype),TD_FREE(args);
        } else {
            char *bt=TypeToString(t);
            sprintf(buffer, "%s%s",bt,ptrs.data);
            TD_FREE(bt);
        }
        vec_deinit(&ptrs);
        goto ret;
    }
    case TYPE_U0:
        return strdup("U0");
    case TYPE_U16:
        return strdup("U16i");
    case TYPE_U32:
        return strdup("U32i");
    case TYPE_U64:
        return strdup("U64i");
    case TYPE_U8:
        return strdup("U8i");
    case TYPE_UNION: {
        if(t->un.name)
            return strdup(t->un.name);
        else
            sprintf(buffer, "<Anon-union:%p>", t);
        goto ret;
    }
    }
ret:
    return strdup(buffer);
}
static CType *ConvertToArrayRefs(CType *t) {
    if(t->type==TYPE_ARRAY) {
        CType *r=TD_MALLOC(sizeof(CType));
        r->refCnt=1;
        r->type=TYPE_ARRAY_REF;
        r->array.base=ConvertToArrayRefs(t->array.base);
        r->array.dim=t->array.dim;
        return r;
    }
    return t;
}
int TypeSize(CType *_t) {
    CType *t=BaseType(_t);
    switch(t->type) {
    case TYPE_U0:
        return 0;
    case TYPE_I8:
        return 1;
    case TYPE_U8:
        return 1;
    case TYPE_I16:
        return 2;
    case TYPE_U16:
        return 2;
    case TYPE_I32:
        return 4;
    case TYPE_U32:
        return 4;
    case TYPE_I64:
        return 8;
    case TYPE_U64:
        return 8;
    case TYPE_F64:
        return 8;
    case TYPE_PTR:
        return sizeof(void*);
    case TYPE_FUNC:
        return sizeof(void*);
    case TYPE_BOOL:
        return 1;
    case TYPE_ARRAY_REF:
    case TYPE_ARRAY:
        if(t->array.dim==-1)
            return sizeof(void*);
        return TypeSize(t->array.base)*t->array.dim;
    case TYPE_CLASS:
        return t->cls.size;
    case TYPE_UNION:
        return t->un.size;
    default:
        abort();
    }
}
int TypeAlign(CType *_t) {
    CType *t=BaseType(_t);
    switch(t->type) {
    case TYPE_U0:
        return 1;
    case TYPE_I8:
        return 1;
    case TYPE_U8:
        return 1;
    case TYPE_I16:
        return 2;
    case TYPE_U16:
        return 2;
    case TYPE_I32:
        return 4;
    case TYPE_U32:
        return 4;
    case TYPE_I64:
        return 8;
    case TYPE_U64:
        return 8;
    case TYPE_F64:
        return 8;
    case TYPE_PTR:
        return sizeof(void*);
    case TYPE_FUNC:
        return sizeof(void*);
    case TYPE_BOOL:
        return 1;
    case TYPE_ARRAY_REF:
    case TYPE_ARRAY:
        return TypeSize(t->array.base);
    case TYPE_CLASS:
        return t->cls.align;
    case TYPE_UNION:
        return t->un.align;
    default:
        abort();
    }
}
void RaiseError(AST *at,...) {
    va_list list;
    va_start(list, at);
    char *fmt=va_arg(list,char *);
    char buffer[1024];
    if(!Compiler.errorsFile)
      sprintf(buffer,"%s:%ld:%ld: \e[31mERROR\e[0m %s\n",at->fn,at->ln,at->col,fmt);
    else
      sprintf(buffer,"%s:%ld:%ld: ERROR %s\n",at->fn,at->ln,at->col,fmt);
    if(!Compiler.silentMode||Compiler.errorsFile) vfprintf((Compiler.errorsFile)?Compiler.errorsFile:stderr,buffer,list);
    va_end(list);
    Compiler.errorFlag=1;
    Compiler.JIT=NULL;
}
void RaiseWarning(AST *at,...) {
    va_list list;
    va_start(list, at);
    char *fmt=va_arg(list,char *);
    char buffer[1024];
    if(!Compiler.errorsFile)
      sprintf(buffer,"%s:%ld:%ld: \e[33mWARNING\e[0m %s\n",at->fn,at->ln,at->col,fmt);
    else
      sprintf(buffer,"%s:%ld:%ld: WARNING %s\n",at->fn,at->ln,at->col,fmt);
    if(!Compiler.silentMode||Compiler.errorsFile) vfprintf((Compiler.errorsFile)?Compiler.errorsFile:stderr,buffer,list);
    va_end(list);
}
int IsBinop(AST *node) {
    switch(node->type) {
    case AST_COMMA:
    case AST_SHL:
    case AST_SHR:
    case AST_POW:
    //
    case AST_MUL:
    case AST_DIV:
    case AST_MOD:
    //
    case AST_BAND:
    case AST_BXOR:
    case AST_BOR:
    //
    case AST_ADD:
    case AST_SUB:
    //
    case AST_LT:
    case AST_GT:
    case AST_LE:
    case AST_GE:
    //
    case AST_EQ:
    case AST_NE:
    //
    case AST_LAND:
    case AST_LXOR:
    case AST_LOR:
    //
    case AST_ASSIGN:
    case AST_ASSIGN_SHL:
    case AST_ASSIGN_SHR:
    case AST_ASSIGN_MUL:
    case AST_ASSIGN_MOD:
    case AST_ASSIGN_DIV:
    case AST_ASSIGN_BAND:
    case AST_ASSIGN_BOR:
    case AST_ASSIGN_BXOR:
    case AST_ASSIGN_ADD:
    case AST_ASSIGN_SUB:
        return 1;
    }
    return 0;
}
int IsUnop(AST *node) {
    switch(node->type) {
    case AST_PAREN:
    case AST_POST_INC:
    case AST_POST_DEC:
    //
    case AST_PRE_INC:
    case AST_PRE_DEC:
    //
    case AST_POS:
    case AST_NEG:
    //
    case AST_LNOT:
    case AST_BNOT:
    //
    case AST_DERREF:
    case AST_ADDROF:
        return 1;
    }
    return 0;
}
AST *CreateBinop(AST *a,AST *b,int type) {
    AST *r=TD_CALLOC(1, sizeof(AST));
    r->refCnt=1;
    r->type=type;
    r->binop.a=a,r->binop.b=b;
    return r;
}
CVariable *CloneVariable(CVariable *var) {
    var->refCount++;
    return var;
}
AST *CreateImplicitTypecast(AST *a,AST *to_type) {
    AST *r=TD_CALLOC(1,sizeof(AST));
    r->refCnt=1;
    r->type=AST_IMPLICIT_TYPECAST;
    r->typecast.base=a;
    r->typecast.toType=to_type;
    //TODO
    return r;
}
AST *CreateExplicitTypecast(AST *a,AST *to_type) {
    AST *r=TD_CALLOC(1,sizeof(AST));
    r->type=AST_EXPLICIT_TYPECAST;
    r->typecast.base=a;
    r->typecast.toType=to_type;
    r->refCnt=1;
    //TODO
    return r;
}
AST *CreateMemberAccess(AST *a,AST *member) {
    AST *r =TD_CALLOC(1,sizeof(AST));
    r->type=AST_MEMBER_ACCESS;
    r->refCnt=1;
    r->memberAccess.a=a;
    r->memberAccess.member=strdup(member->name);
    //TODO
    return r;
}
AST *CreateArrayAccess(AST *a,AST *sub) {
    AST *r=TD_CALLOC(1,sizeof(AST));
    r->type=AST_ARRAY_ACCESS;
    r->arrayAccess.array=a;
    r->arrayAccess.subscript=sub;
    //TODO
    return r;
}
AST *CreateSizeof(AST *a) {
    AST *r=TD_CALLOC(1,sizeof(AST));
    r->type=AST_SIZEOF;
    r->refCnt=1;
    r->sizeofExp=a;
    return r;
}
AST *CreateUnop(AST *a,int type) {
    AST *r=TD_CALLOC(1,sizeof(AST));
    r->type=type;
    r->unopArg=a;
    r->refCnt=1;
    return r;
}
AST *AppendToStmts(AST *a,AST *b) {
    AST *r=TD_CALLOC(1,sizeof(AST));
    r->refCnt=1;
    r->type=AST_STMT_GROUP;
    vec_init(&r->stmts);
    if(!a) a=r;
    if(a->type==AST_STMT_GROUP) {
        int i;
        AST *stmt;
        vec_foreach(&a->stmts, stmt, i) {
            vec_push(&r->stmts,stmt);
        }
    } else
        vec_push(&r->stmts,a);
    if(b->type==AST_STMT_GROUP) {
        int i;
        AST *stmt;
        vec_foreach(&b->stmts, stmt, i) {
            vec_push(&r->stmts,stmt);
        }
    } else
        vec_push(&r->stmts,b);
    return r;
}
AST *AppendToRange(AST *a,AST *b,int type) {
    AST *r=TD_CALLOC(1,sizeof(AST));
    r->refCnt=1;
    r->type=AST_RANGE;
    vec_init(&r->range.ops);
    vec_init(&r->range.operands);
    if(a->type==AST_RANGE) {
        vec_pusharr(&r->range.ops, a->range.ops.data, a->range.ops.length);
        vec_pusharr(&r->range.operands, a->range.operands.data, a->range.operands.length);
        int cnt=a->range.operands.length;
        while(--cnt>=0) r->range.operands.data[cnt]=r->range.operands.data[cnt];
    } else {
        vec_push(&r->range.operands,a);
    }
    vec_push(&r->range.ops, type);
    vec_push(&r->range.operands, b);
    return r;
}
AST *CreateFuncCall(AST *func,AST *args) {
    AST *r=TD_CALLOC(1,sizeof(AST));
    r->type=AST_FUNC_CALL;
    r->funcCall.args=args;
    r->funcCall.func=func;
    //TODO
    return r;
}
int IsArith(CType *t) {
    return IsI64(t)|IsU64(t)|IsF64(t)|IsPtr(t);
}
AST *CreateIf(AST *cond,AST *body,AST *el) {
    AST *r=TD_CALLOC(1,sizeof(AST));
    r->refCnt=1;
    r->type=AST_IF;
    r->ifStmt.cond=cond;
    r->ifStmt.body=body;
    r->ifStmt.elseBody=el;
    return r;
}
AST *CreateWhile(AST *cond,AST *body) {
    AST *r=TD_CALLOC(1,sizeof(AST));
    r->refCnt=1;
    r->type=AST_WHILE;
    r->whileStmt.cond=cond;
    r->whileStmt.body=body;
    return r;
}
AST *CreateFor(AST *init,AST *cond,AST *next,AST*body) {
    AST *r=TD_CALLOC(1,sizeof(AST));
    r->refCnt=1;
    r->type=AST_FOR;
    r->forStmt.cond=cond;
    r->forStmt.body=body;
    r->forStmt.init=init;
    r->forStmt.next=next;
    return r;
}
char *GetLabelReadableName(AST *t) {
  if(t->type==AST_LOCAL_LABEL) {
    long discard;
    char name[1024];
    sscanf(t->name,LOCAL_LAB_FMT, &discard,name);
    return strdup(name);
  }
  return strdup(t->name);
}
AST *CreateLabel(AST *name) {
    AST *r=TD_CALLOC(1,sizeof(AST));
    r->refCnt=1;
    r->type=AST_LABEL;
    r->labelNode=name;
    Compiler.lastLabel=r;
    Compiler.globalLabelCount++;
    return r;
}
AST *CreateTry(AST *try,AST *catch)  {
    AST *r=TD_CALLOC(1,sizeof(AST));
    r->refCnt=1;
    r->type=AST_TRY;
    r->try.try=try;
    r->try.catch=catch;
    return r;
}
AST *CreateGoto(AST *name) {
    AST *r=TD_CALLOC(1,sizeof(AST));
    r->refCnt=1;
    r->type=AST_GOTO;
    r->labelNode=name;
    return r;
}
AST *CreateDo(AST *cond,AST *body) {
    AST *r=TD_CALLOC(1,sizeof(AST));
    r->refCnt=1;
    r->type=AST_DO;
    r->doStmt.cond=cond;
    r->doStmt.body=body;
    return r;
}
CType *CreateArrayType(CType * base,AST *dim) {
    CType *r=TD_CALLOC(1,sizeof(CType));
    r->refCnt=1;
    r->array.base=base;
    if(dim)
      r->array.dim=EvaluateInt(dim,0);
    else
        r->array.dim=-1;
    r->type=TYPE_ARRAY;
    return r;
}
CType *CreatePtrType(CType * type) {
    CType *r=TD_CALLOC(1,sizeof(CType));
    r->refCnt=1;
    r->type=TYPE_PTR;
    r->ptr=type;
    return r;
}
CType *CreatePrimType(int type) {
    CType *r=TD_CALLOC(1,sizeof(CType));
    r->refCnt=1;
    r->type=type;
    return r;
}
AST *AppendToMetaData(AST *first,AST *name,AST * value) {
    CMetaData meta;
    meta.name=name;
    meta.value=value;
    if(!first) {
        first=TD_CALLOC(1,sizeof(AST));
        first->type=AST_METADATA;
        vec_init(&first->metaData);
        first->refCnt=0; //Will be inc'd later
    }
    first=first;
    vec_push(&first->metaData,meta);
    return first;
}
vec_AST_t CommaToVec(AST* comma) {
    vec_AST_t ret;
    vec_init(&ret);
    if(!comma) return ret;
loop:
    if(!comma) {
        vec_push(&ret, comma);
    } else if(comma->type==AST_COMMA) {
        vec_push(&ret, comma->binop.b);
        comma=comma->binop.a;
        goto loop;
    } else {
        vec_push(&ret, comma);
    }
    vec_reverse(&ret);
    return ret;
}
int IsF64(CType *type) {
    CType *bt=BaseType(type);
    int ret=bt->type==TYPE_F64;
    return ret;
}
int IsPtr(CType *type) {
    CType *bt =BaseType(type);
    int ret=bt->type==TYPE_PTR||bt->type==TYPE_ARRAY||bt->type==TYPE_ARRAY_REF;
    return ret;
}
int IsU64(CType *type) {
loop:
    switch(type->type) {
    case TYPE_U64:
        return 1;
    case TYPE_UNION:
        type=type->un.baseType;
        if(type) goto loop;
    }
    return 0;
}
/**
 * Promotes to I64
 */
int IsI64(CType *type) {
loop:
    switch(type->type) {
    case TYPE_BOOL:
    case TYPE_U8:
    case TYPE_I8:
    case TYPE_U16:
    case TYPE_I16:
    case TYPE_U32:
    case TYPE_I32:
    case TYPE_I64:
        return 1;
    case TYPE_UNION:
        type=type->un.baseType;
        if(type) goto loop;
    }
    return 0;
}
AST *CreateString(char *txt) {
    AST *r=TD_CALLOC(1,sizeof(AST));
    r->type=AST_STRING;
    r->string=strdup(txt);
    r->refCnt=1;
    return r;
}
static void ReleaseMembers(vec_CMember_t *mems) {
    int iter;
    CMember *mem;
    vec_foreach_ptr(mems, mem, iter) {
        TD_FREE(mem->name);
        CMetaData *meta;
        int iter2;
        vec_foreach_ptr(&mem->meta,meta,iter2) {
            ReleaseAST(meta->name);
            ReleaseAST(meta->value);
        }
    }
    vec_deinit(mems);
}
void ReleaseType(CType *t) {
    if(!t) return;
     {
        switch(t->type) {
        case TYPE_ARRAY:
        case TYPE_ARRAY_REF:
            ReleaseType(t->array.base);
            break;
        case TYPE_BOOL:
        case TYPE_CLASS:
            ReleaseType(t->cls.inheritsFrom);
            ReleaseMembers(&t->cls.members);
            break;
        case TYPE_F64:
            break;
        case TYPE_FUNC:
            ;
            int iter;
            CType *arg;
            vec_foreach(&t->func.arguments, arg, iter) ReleaseType(arg);
            vec_deinit(&t->func.arguments);
            ReleaseType(t->func.ret);
            AST *dft;
            vec_foreach(&t->func.dftArgs, dft, iter) ReleaseAST(dft);
            vec_deinit(&t->func.dftArgs);
            char *n;
            vec_foreach(&t->func.names,n,iter) TD_FREE(n);
            vec_deinit(&t->func.names);
            break;
        case TYPE_I16:
        case TYPE_I32:
        case TYPE_I64:
        case TYPE_I8:
            break;
        case TYPE_PTR:
            ReleaseType(t->ptr);
            break;
        case TYPE_U0:
        case TYPE_U16:
        case TYPE_U32:
        case TYPE_U64:
        case TYPE_U8:
            break;
        case TYPE_UNION:
            ReleaseMembers(&t->un.members);
            ReleaseType(t->un.inheritsFrom);
            ReleaseType(t->un.baseType);
            break;
        }
        TD_FREE(t);
    }
}
void ReleaseAST(AST *t) {
#define TD_FREE_ARR(arr) {int iter;AST *node; vec_foreach(arr,node,iter) {ReleaseAST(node);}}
    if(!t) return;
    switch(t->type) {
    case AST_PRINT_STRING:
        TD_FREE(t->string);
        break;
    case AST_TYPE:
        //Types float around the compiler so dont
        //ReleaseType(t->type2);
        break;
    case AST_SUBSWITCH:
        ReleaseAST(t->subswitch.body);
        ReleaseAST(t->subswitch.startCode);
        break;
    case AST_SWITCH:
        ReleaseAST(t->switStmt.body);
        ReleaseAST(t->switStmt.cond);
        break;
    case AST_CASE:
        break;
    case AST_DECL:
        ReleaseAST(t->decl.dft);
        TD_FREE(t->decl.name);
        break;
    case AST_VAR:
        ReleaseVariable(t->var);
        break;
    case AST_METADATA: {
        int iter;
        CMetaData meta;
        vec_foreach(&t->metaData, meta, iter) {
            TD_FREE(meta.name);
            ReleaseAST(meta.value);
        }
        break;
    }
    case AST_WHILE:
        ReleaseAST(t->whileStmt.body);
        ReleaseAST(t->whileStmt.cond);
        break;
    case AST_DO:
        ReleaseAST(t->doStmt.body);
        ReleaseAST(t->doStmt.cond);
        break;
    case AST_FOR:
        ReleaseAST(t->forStmt.body);
        ReleaseAST(t->forStmt.init);
        ReleaseAST(t->forStmt.cond);
        ReleaseAST(t->forStmt.next);
        break;
    case AST_IF:
        ReleaseAST(t->ifStmt.body);
        ReleaseAST(t->ifStmt.cond);
        ReleaseAST(t->ifStmt.elseBody);
        break;
    case AST_FUNC_CALL:
        ReleaseAST(t->funcCall.func);
        ReleaseAST(t->funcCall.args);
        break;
    case AST_IMPLICIT_TYPECAST:
    case AST_EXPLICIT_TYPECAST:
        ReleaseAST(t->typecast.base);
        ReleaseAST(t->typecast.toType);
        break;
    case AST_STRING:
        TD_FREE(t->string);
        break;
    case AST_MEMBER_ACCESS:
        ReleaseAST(t->memberAccess.a);
        TD_FREE(t->memberAccess.member);
        break;
    case AST_STMT_GROUP:
        TD_FREE_ARR(&t->stmts);
        break;
    case __AST_DECL_TAILS: {
        CDeclTail tail;
        int iter;
        vec_foreach(&t->declTail, tail, iter) {
            ReleaseAST(tail.dft);
            //basetype is in finalType
            //ReleaseAST(tail.basetype);
            ReleaseAST(tail.name);
        }
        vec_deinit(&t->declTail);
    }
    break;
    case AST_ARRAY_ACCESS:
        ReleaseAST(t->arrayAccess.array);
        ReleaseAST(t->arrayAccess.subscript);
        break;
    default:
        if(IsUnop(t)) {
            ReleaseAST(t->unopArg);
        } else if(IsBinop(t)) {
            ReleaseAST(t->binop.a);
            ReleaseAST(t->binop.b);
        }
    }
    TD_FREE(t);
}
CType *DerrefedType(CType *type) {
    CType *bt =BaseType(type);
    if(type->type==TYPE_PTR) {
        return bt->ptr;
    } else if(bt->type==TYPE_ARRAY||bt->type==TYPE_ARRAY_REF) {
        return bt->array.base;
    }
    return CreatePrimType(TYPE_I64);
}
int IsModifyAssign(AST *node) {
    switch(node->type) {
    case AST_ASSIGN:
    case AST_ASSIGN_SHL:
    case AST_ASSIGN_SHR:
    case AST_ASSIGN_MUL:
    case AST_ASSIGN_MOD:
    case AST_ASSIGN_DIV:
    case AST_ASSIGN_BAND:
    case AST_ASSIGN_BOR:
    case AST_ASSIGN_BXOR:
    case AST_ASSIGN_ADD:
    case AST_ASSIGN_SUB:
        return 1;
    }
    return 0;
}
double EvaluateF64(AST *exp) {
    AST *ret =CreateReturn(exp);
    COldFuncState old=CreateCompilerState();
    //Dissable breakpoints
    vec_CVariable_t args;
    vec_init(&args);
    Compiler.returnType=CreatePrimType(TYPE_F64);
    CFunction *f=CompileAST(NULL, ret, args);
    ReleaseType(Compiler.returnType);
    GC_Disable();
    #ifndef TARGET_WIN32
    signal(SIGINT,SignalHandler);
    double ret2=((double(*)())f->funcptr)();
    signal(SIGINT,SIG_IGN);
    #else
    double ret2=((double(*)())f->funcptr)();
    #endif
    GC_Enable();
    ReleaseFunction(f);
    RestoreCompilerState(old);
    return ret2;
}
int64_t EvaluateInt(AST *exp,int flags) {
    AST *ret =CreateReturn(exp);
    map_CVariable_t oldlocs=Compiler.locals;
    COldFuncState old=CreateCompilerState();
    //Dissable breakpoints
    map_CVariable_t *locals=NULL;
    vec_CVariable_t args;
    vec_init(&args);
    if(flags&EVAL_INT_F_PRESERVE_LOCALS)
      locals=&Compiler.locals;
    Compiler.returnType=CreatePrimType(TYPE_I64);
    CFunction *f=CompileAST(locals, ret, args);
    ReleaseType(Compiler.returnType);
    vec_deinit(&args);
    GC_Disable();
    #ifndef TARGET_WIN32
    signal(SIGINT,SignalHandler);
    int64_t ret2=((int64_t(*)())f->funcptr)();
    signal(SIGINT,SIG_IGN);
    #else
    int64_t ret2=((int64_t(*)())f->funcptr)();
    #endif
    GC_Enable();
    ReleaseFunction(f);
    RestoreCompilerState(old);
    return ret2;
}
AST *CreateBreak() {
    AST* r=TD_CALLOC(1,sizeof(AST));
    r->refCnt=1;
    r->type=AST_BREAK;
    return r;
}
AST *CreateCase(AST *low,AST *high) {
    AST* r=TD_CALLOC(1,sizeof(AST));
    r->refCnt=1;
    r->type=AST_CASE;
    r->cs.isUndef=!low;
    if(low)
      r->cs.low=EvaluateInt(low,0);
    else
        r->cs.low=1;
    if(high)
      r->cs.high=EvaluateInt(high,0);
    else
        r->cs.high=r->cs.low;
    return r;
}
AST *CreateDefault() {
    AST* r=TD_CALLOC(1,sizeof(AST));
    r->refCnt=1;
    r->type=AST_DEFAULT;
    return r;
}
/**
 * Convert cases to ints first
 */
static int CaseCmp(const void *a,const void *b) {
    const AST * const *A=a,* const *B=b;
    if(A[0]->cs.low>B[0]->cs.low)
        return 1;
    else if(A[0]->cs.low<B[0]->cs.low)
        return -1;
    return 0;
}
vec_AST_t FindCasesInSwitch(AST *swit) {
    vec_AST_t ret;
    vec_init(&ret);
    AST *body=(swit->type==AST_SWITCH)?swit->switStmt.body:swit->subswitch.body;
    if(body->type==AST_STMT_GROUP) {
        int i;
        AST *item;
        vec_foreach(&body->stmts, item, i) {
            if(item->type==AST_CASE) {
                vec_push(&ret, item);
                item->cs.owner=swit;
            } else if(item->type==AST_SUBSWITCH) {
                vec_AST_t subs=FindCasesInSwitch(item);
                vec_pusharr(&ret, subs.data, subs.length);
                vec_deinit(&subs);
            }
        }
    }
    return ret;
}
AST *CreateSwitch(AST* cond,AST *body) {
    AST* r=TD_CALLOC(1,sizeof(AST));
    r->refCnt=1;
    r->type=AST_SWITCH;
    r->switStmt.cond=cond;
    r->switStmt.body=body;
    return r;
}
AST *CreateNop() {
    AST* r=TD_CALLOC(1,sizeof(AST));
    r->refCnt=1;
    r->type=AST_NOP;
    return r;
}
AST *CreatePrint(char *prn) {
    AST* r=TD_CALLOC(1,sizeof(AST));
    r->type=AST_PRINT_STRING;
    r->refCnt=1;
    r->string=strdup(prn);
    return r;
}
AST *CreateSubswitch(AST *body) {
    AST* r=TD_CALLOC(1,sizeof(AST));
    r->refCnt=1;
    r->type=AST_SUBSWITCH;
    //Find code up until first case or default
    if(body->type==AST_STMT_GROUP) {
        AST *stmt;
        int iter;
        vec_foreach(&body->stmts, stmt, iter) {
            if(stmt->type==AST_CASE||stmt->type==AST_DEFAULT) {
                AST *pre=TD_CALLOC(1,sizeof(AST));
                pre->refCnt=1;
                pre->type=AST_STMT_GROUP;
                vec_init(&pre->stmts);
                vec_pusharr(&pre->stmts,body->stmts.data,iter);
                r->subswitch.startCode=pre;
                //
                AST *body2=TD_CALLOC(1,sizeof(AST));
                body2->refCnt=1;
                body2->type=AST_STMT_GROUP;
                vec_init(&body2->stmts);
                vec_pusharr(&body2->stmts,&body->stmts.data[iter],body->stmts.length-iter);
                r->subswitch.body=body2;
                return r;
            }
        }
        goto nocases;
    } else {
nocases:
        r->subswitch.startCode=body;
        r->subswitch.body=NULL;
    }
    return r;
}
static void FillInLastclasses(AST *at,CType *ftype,vec_AST_t *args) {
    while(args->length<ftype->func.arguments.length)
        vec_push(args, NULL);
    AST *var;
    int iter;
    vec_foreach(args, var, iter) {
        if(var) continue;
        if(!ftype->func.dftArgs.data[iter]) continue;
        if(ftype->func.dftArgs.data[iter]->type!=AST_LASTCLASS) continue;
        if(iter==0) {
            RaiseError(at,"Can't get lastclass(lastclass appears as first argument).");
            continue;
        }
        //Last class points to class of last type excluding pointers and arrays
        CType *_lastclass =NULL;
        if(!args->data[iter-1])
          _lastclass=ftype->func.arguments.data[iter-1];
        else
            _lastclass=AssignTypeToNode(args->data[iter-1]);
        CType *lastclass=_lastclass;
lcloop:
        switch(lastclass->type) {
        case TYPE_PTR:
            lastclass=lastclass->ptr;
            goto lcloop;
        case TYPE_ARRAY_REF:
        case TYPE_ARRAY:
            lastclass=lastclass->array.base;
            goto lcloop;
        }
        char *ts=TypeToString(lastclass);
        args->data[iter]=CreateString(ts);
        TD_FREE(ts);
    }
}
/**
 * CreateFuncType(CType *ret,AST(__AST_DECL_TAILS));
 */
CType *CreateFuncType(CType *ret,AST *_args,int hasvargs) {
    CType *r=TD_CALLOC(1,sizeof(CType));
    r->refCnt=1;
    r->type=TYPE_FUNC;
    r->func.ret=ret;
    r->func.hasvargs=hasvargs;
    if(_args) {
        assert(_args->type==__AST_DECL_TAILS);
        int iter;
        CDeclTail decl;
        vec_foreach(&_args->declTail,decl,iter) {
            vec_push(&r->func.names,strdup(decl.name->name));
            if(decl.dft) {
                if(decl.dft->type==AST_LASTCLASS) {
                    vec_push(&r->func.dftArgs,decl.dft);
                } else {
                    CVariable *dftvar AF_VAR=TD_MALLOC(sizeof(CVariable));
                    dftvar->refCount=1;
                    dftvar->type=decl.finalType;
                    dftvar->linkage.globalPtr=TD_MALLOC(TypeSize(decl.finalType));
                    dftvar->linkage.type=LINK_NORMAL;
                    dftvar->isGlobal=1;
                    AST *vnode =CreateVarNode(dftvar);
                    AST *asn =CreateBinop(vnode, decl.dft, AST_ASSIGN);
                    SetPosFromOther(asn,decl.name);
                    AST *ret =CreateReturn(asn);
                    vec_CVariable_t empty;
                    vec_init(&empty);
                    COldFuncState olds=CreateCompilerState();
                    Compiler.returnType=decl.finalType;
                    CFunction *stmt=CompileAST(NULL,ret,empty);
                    RestoreCompilerState(olds);
                    vec_deinit(&empty);
		    GC_Disable();
                    #ifndef TARGET_WIN32
                    signal(SIGINT,SignalHandler);
                    stmt->funcptr(0);
                    signal(SIGINT,SIG_IGN);
                    #else
                    stmt->funcptr(0);
                    #endif 
		    GC_Enable();
		    ReleaseFunction(stmt);
                    vec_push(&r->func.dftArgs,vnode);
                }
            } else
                vec_push(&r->func.dftArgs,NULL);
            vec_push(&r->func.arguments,decl.finalType);
        }
    }
    return r;
}
AST *CreateI64(uint64_t i) {
    AST *r=TD_CALLOC(1,sizeof(AST));
    r->integer=i;
    r->refCnt=1;
    r->type=AST_INT;
    return r;
}
AST *CreateF64(double f) {
    AST *r=TD_CALLOC(1,sizeof(AST));
    r->floating=f;
    r->refCnt=1;
    r->type=AST_FLOAT;
    return r;
}
AST *CreateTypeNode(CType *t) {
    AST *r=TD_CALLOC(1,sizeof(AST));
    r->refCnt=1;
    r->type=AST_TYPE;
    r->type2=t;
    return r;
}
AST *CreateVarNode(CVariable *var) {
    AST *r=TD_CALLOC(1,sizeof(AST));
    r->refCnt=1;
    r->var=CloneVariable(var);
    r->type=AST_VAR;
    return r;
}
AST *CreateVarDecl(CType *type,AST *name,AST *dft) {
    AST *r=TD_CALLOC(1,sizeof(AST));
    r->refCnt=1;
    r->type=AST_DECL;
    r->decl.type=type;
    if(name)
        r->decl.name=strdup(name->name);
    else
        r->decl.name=NULL;
    if(dft) {
        //TODO validate type
    }
    r->decl.dft=dft;
    return r;
}
CCompiler Compiler;
void ReleaseVariable(CVariable *var) {
    if(!var) return;
    if(--var->refCount<=0) {
        TD_FREE(var->name);
        TD_FREE(var);
    }
}
CType *CreateMultiLvlPtr(CType *type,int lvl) {
    while(--lvl>=0) type=CreatePtrType(type);
    return type;
}
void AddVariable(AST *name,CType *type) {
    CType *bt=BaseType(type);
    CVariable *var=TD_CALLOC(1,sizeof(CVariable));
    var->isFunc=bt->type==TYPE_FUNC;
    var->name=strdup(name->name);
    var->type=type;
    if(name) {
      var->fn=name->fn;
      var->line=name->ln;
    }
    if(Compiler.inFunction) {
        var->isGlobal=0;
        if(map_get(&Compiler.locals,name->name)) {
            char buffer[1024];
            sprintf(buffer,"Redefinition of %s.",name->name);
            RaiseError(name, buffer);
            ReleaseVariable(var);
        } else {
            map_set(&Compiler.locals,name->name,var);
        }
    } else {
        var->isGlobal=1;
        var->linkage.globalPtr=TD_CALLOC(1,TypeSize(type));
        if(map_get(&Compiler.globals,name->name)) {
            ReleaseVariable(*map_get(&Compiler.globals,name->name));
            map_remove(&Compiler.globals, name->name);
        }
        map_set(&Compiler.globals,name->name,var);
        //Fill in unlinked imports
        int iter;
        CVariable *unlinked;
remimp:
        vec_foreach(&Compiler.unlinkedImportVars, unlinked, iter) {
            if(0==strcmp(unlinked->linkage.importLink,name->name)) {
                unlinked->linkage.type=LINK_EXTERN;
                unlinked->linkage.externVar=var;
                unlinked->linkage.externVar=var;
                vec_push(&Compiler.unlinkedExternVars, unlinked);
                vec_remove(&Compiler.unlinkedImportVars, unlinked);
                goto remimp;
            }
        }
remext:
        vec_foreach(&Compiler.unlinkedExternVars, unlinked, iter) {
            if(0==strcmp(unlinked->linkage.externVar->name,name->name)) {
                if(0==var->isFunc)
                    *unlinked->linkage.dummyPtr=var->linkage.globalPtr;
                ReleaseVariable(unlinked);
                vec_remove(&Compiler.unlinkedExternVars, unlinked);
                goto remext;
            }
        }
    }
}
static void __AddFunctionVar(AST *name,CType *type,CFunction *func) {
    CType *bt=BaseType(type);
    CVariable *var=TD_CALLOC(1,sizeof(CVariable));
    var->isFunc=bt->type==TYPE_FUNC;
    assert(var->isFunc);
    var->name=strdup(name->name);
    var->func=func;
    var->type=type;
    var->isGlobal=1;
    if(name) {
      var->fn=name->fn;
      var->line=name->ln;
    }
    var->linkage.globalPtr=TD_CALLOC(1,TypeSize(type));
    if(map_get(&Compiler.globals,name->name)) {
        ReleaseVariable(*map_get(&Compiler.globals,name->name));
        map_remove(&Compiler.globals, name->name);
    }
    map_set(&Compiler.globals,name->name,var);
    //Fill in unlinked imports
    int iter;
    CVariable *unlinked;
remimp:
    vec_foreach(&Compiler.unlinkedImportVars, unlinked, iter) {
        if(0==strcmp(unlinked->linkage.importLink,name->name)) {
            unlinked->linkage.type=LINK_EXTERN;
            unlinked->linkage.externVar=var;
            unlinked->linkage.externVar=var;
            vec_push(&Compiler.unlinkedExternVars, unlinked);
            vec_remove(&Compiler.unlinkedImportVars, unlinked);
            goto remimp;
        }
    }
remext:
    vec_foreach(&Compiler.unlinkedExternVars, unlinked, iter) {
        if(0==strcmp(unlinked->linkage.externVar->name,name->name)) {
            *unlinked->linkage.dummyPtr=var->func->funcptr;
            ReleaseVariable(unlinked);
            vec_remove(&Compiler.unlinkedExternVars, unlinked);
            goto remext;
        }
    }
}
/**
 * tmp_basetype is the temporary base type which will be replaced
 */
AST *AppendToDecls(AST *appendto,CType *tmp_basetype,CType *type,AST *name,AST *dft,AST *metadata) {
    if(!appendto) {
        appendto=TD_CALLOC(1,sizeof(AST));
        appendto->type=__AST_DECL_TAILS;
        appendto->refCnt=1;
        vec_init(&appendto->declTail);
    } else appendto=appendto;
    CDeclTail ent;
    ent.linkage=NULL;
    ent.basetype=tmp_basetype;
    ent.finalType=type;
    ent.name=name;
    ent.dft=dft;
    ent.metaData=metadata;
    vec_push(&appendto->declTail,ent);
    return appendto;
}
static int TypeReplaceBaseTypeWith(CType **item,CType *basetype,CType *with) {
    switch(item[0]->type) {
    case TYPE_ARRAY_REF:
    case TYPE_ARRAY: {
        return TypeReplaceBaseTypeWith(&item[0]->array.base,basetype,with);
    }
    case TYPE_UNION:
    case TYPE_BOOL:
    case TYPE_CLASS:
    case TYPE_F64:
    case TYPE_I16:
    case TYPE_I32:
    case TYPE_I64:
    case TYPE_I8:
    case TYPE_U0:
    case TYPE_U8:
    case TYPE_U16:
    case TYPE_U32:
    case TYPE_U64: {
        if(*item==basetype) {
            *item=with;
            return 1;
        }
        return 0;
    }
    case TYPE_FUNC:
        return TypeReplaceBaseTypeWith(&item[0]->func.ret,basetype,with);
    case TYPE_PTR:
        return TypeReplaceBaseTypeWith(&item[0]->ptr,basetype,with);
    default:
        abort();
        return 0;
    }
}
void DeclsFillInBasetype(CType *bt,AST *tails) {
    assert(tails->type==__AST_DECL_TAILS);
    int iter;
    CDeclTail *decl;
    vec_foreach_ptr(&tails->declTail, decl, iter) {
        assert(1==TypeReplaceBaseTypeWith(&decl->finalType, decl->basetype, bt));
    }
}
//CALL DeclsFillInBasetype FIRST
void AddDeclsToScope(AST *tails) {
    assert(tails->type==__AST_DECL_TAILS);
    int iter;
    CDeclTail decl;
    vec_foreach(&tails->declTail, decl, iter) {
        if(!decl.linkage) {
            AddVariable(decl.name, decl.finalType);
            continue;
        }
        switch(decl.linkage->linkage.type) {
        case LINK_EXTERN:
EXT:
            Extern(decl.finalType, decl.name, NULL);
            break;
        case LINK__EXTERN:
            Extern(decl.finalType, decl.name, decl.linkage->linkage.importLink);
            break;
        case LINK_IMPORT:
            Import(decl.finalType, decl.name, NULL);
            break;
        case LINK__IMPORT:
            Import(decl.finalType, decl.name, decl.linkage->linkage.importLink);
            break;
        case LINK_NORMAL:
            AddVariable(decl.name, decl.finalType);
            break;
        case LINK_STATIC:
            if(Compiler.inFunction) {
                //Create a global variable but add it to locals
                CVariable *stat=TD_MALLOC(sizeof(CVariable));
                stat->refCount=1;
                stat->isGlobal=1;
                if(decl.name) {
                  stat->fn=decl.name->fn;
                  stat->line=decl.name->ln;
                }
                stat->name=strdup(decl.name->name);
                stat->type=decl.finalType;
                stat->linkage.globalPtr=TD_MALLOC(TypeSize(decl.finalType));
                if(map_get(&Compiler.locals,stat->name)) {
                    RaiseError(decl.name,"Redefinition of variable %s.",stat->name);
                }
                map_set(&Compiler.locals,stat->name,stat);
            }
            break;
        }
    }
}
void PrintAST(AST *n,int tabs) {
    int oldtabs=tabs;
    while(--tabs>=0) printf(" ");
    tabs=oldtabs;
    if(!n) {
        printf("<NULL>");
        return ;
    }
    long iter;
    switch(n->type) {
    case __AST_DECL_TAILS: {
        CDeclTail tail;
        vec_foreach(&n->declTail, tail, iter) {
            printf("(DECL ");
            PrintAST(tail.name,0);
            printf(" = ");
            PrintAST(tail.dft,0);
            printf(")");
        }
        break;
    }
    case AST_PRINT_STRING:
        printf("(PRINT:%s)\n",n->string);
        break;
    //
    case AST_NOP:
        printf("NOP\n");
        break;
    case AST_STMT_GROUP: {
        printf("{\n");
        AST *stmt;
        vec_foreach(&n->stmts, stmt, iter) {
            PrintAST(stmt, tabs+1);
        }
        while(--tabs>=0) printf(" ");
        printf("}\n");
        break;
    }
    //
    case AST_NAME: {
        printf("NAME: %s",n->name);
        break;
    }
    case AST_MEMBER_ACCESS: {
        puts("(");
        PrintAST(n->memberAccess.a, 0);
        printf(".");
        puts(n->memberAccess.member);
        puts(")");
        break;
    }
    //
    case AST_PAREN: {
        printf("(");
        PrintAST(n,0);
        printf(")");
        break;
    }
    case AST_INT:
        printf("%lu",n->integer);
        break;
    case AST_FLOAT:
        printf("%lf",n->floating);
        break;
    case AST_CHAR: {
        uint64_t chr=n->chr;
        printf("'");
        int i=8;
        while(--i>=0) {
            uint8_t shifted=0xff&(chr>>(8*i));
            if(shifted) printf("%c",shifted);
        }
        printf("'");
        break;
    }
    case AST_STRING: {
        char buffer[strlen(n->string)*2+1];
        unescapeString((uint8_t*)n->string,buffer);
        printf("\"");
        puts(buffer);
        printf("\"");
        break;
    }
    //
    case AST_COMMA:
#define PRN_BINOP(txt) \
        printf("("); \
        PrintAST(n->binop.a, 0); \
        puts(txt); \
        PrintAST(n->binop.b, 0);\
        printf(")");
        PRN_BINOP(",");
        break;
    //
    case AST_SHL:
        PRN_BINOP("<<");
        break;
    case AST_SHR:
        PRN_BINOP(">>");
        break;
    case AST_POW:
        PRN_BINOP("`");
        break;
    //
    case AST_MUL:
        PRN_BINOP("*");
        break;
    case AST_DIV:
        PRN_BINOP("/");
        break;
    case AST_MOD:
        PRN_BINOP("%");
        break;
    //
    case AST_BAND:
        PRN_BINOP("&");
        break;
    case AST_BXOR:
        PRN_BINOP("^");
        break;
    case AST_BOR:
        PRN_BINOP("|");
        break;
    //
    case AST_ADD:
        PRN_BINOP("+");
        break;
    case AST_SUB:
        PRN_BINOP("-");
        break;
    //
    case AST_LT:
        PRN_BINOP("<");
        break;
    case AST_GE:
        PRN_BINOP(">=");
        break;
    case AST_GT:
        PRN_BINOP(">");
        break;
    case AST_RANGE: {
        printf("(");
        int op,iter;
        vec_foreach(&n->range.ops, op, iter) {
            PrintAST(n->range.operands.data[iter],0);
            switch(op) {
            case AST_LT:
                printf("<");
                break;
            case AST_GT:
                printf(">");
                break;
            case AST_LE:
                printf("<=");
                break;
            case AST_GE:
                printf(">=");
                break;
            default:
                abort();
            }
            PrintAST(n->range.operands.data[n->range.ops.length],0);
        }
        printf(")");
        break;
    }
    //
    case AST_EQ:
        PRN_BINOP("==");
        break;
    case AST_NE:
        PRN_BINOP("!=");
        break;
    //
    case AST_LAND:
        PRN_BINOP("&&");
        break;
    case AST_LXOR:
        PRN_BINOP("^^");
        break;
    case AST_LOR:
        PRN_BINOP("||");
        break;
    //
    case AST_ASSIGN:
        PRN_BINOP("=");
        break;
    case AST_ASSIGN_SHL:
        PRN_BINOP("<<=");
        break;
    case AST_ASSIGN_SHR:
        PRN_BINOP(">>=");
        break;
    case AST_ASSIGN_MUL:
        PRN_BINOP("*=");
        break;
    case AST_ASSIGN_MOD:
        PRN_BINOP("%=");
        break;
    case AST_ASSIGN_DIV:
        PRN_BINOP("/=");
        break;
    case AST_ASSIGN_BAND:
        PRN_BINOP("&=");
        break;
    case AST_ASSIGN_BOR:
        PRN_BINOP("|=");
        break;
    case AST_ASSIGN_BXOR:
        PRN_BINOP("^=");
        break;
    case AST_ASSIGN_ADD:
        PRN_BINOP("+=");
        break;
    case AST_ASSIGN_SUB:
        PRN_BINOP("-=");
        break;
    //
    case AST_POST_INC:
        printf("(");
        PrintAST(n->unopArg,0);
        printf("++)");
        break;
    case AST_POST_DEC:
        printf("(");
        PrintAST(n->unopArg,0);
        printf("--)");
        break;
    //
    case AST_ARRAY_ACCESS:
        printf("(");
        PrintAST(n->arrayAccess.array,0);
        printf("[");
        PrintAST(n->arrayAccess.subscript,0);
        printf("])");
        break;
    //
    case AST_PRE_INC:
        printf("(++");
        PrintAST(n->unopArg,0);
        printf(")");
        break;
    case AST_PRE_DEC:
        printf("(--");
        PrintAST(n->unopArg,0);
        printf(")");
        break;
    //
    case AST_POS:
#define PRN_UNOP(op) \
    printf("("op); \
    PrintAST(n->unopArg,0); \
    printf(")");
        PRN_UNOP("+");
        break;
    case AST_NEG:
        PRN_UNOP("-");
        break;
    //
    case AST_LNOT:
        PRN_UNOP("!");
        break;
    case AST_BNOT:
        PRN_UNOP("~");
        break;
    //
    case AST_DERREF:
        PRN_UNOP("*");
        break;
    case AST_ADDROF:
        PRN_UNOP("&");
        break;
    //
    case AST_SIZEOF:
        PRN_UNOP("sizeof");
        break;
    //
    case AST_TOKEN:
        break;
    //
    case AST_IMPLICIT_TYPECAST:
        printf("(IMP-TYPECAST ");
        PrintAST(n->typecast.base,0);
        printf(" -> ");
        PrintAST(n->typecast.toType,0);
        printf(")");
        break;
    case AST_EXPLICIT_TYPECAST:
        printf("(EXP-TYPECAST ");
        PrintAST(n->typecast.base,0);
        printf(" -> ");
        PrintAST(n->typecast.toType,0);
        printf(")");
        break;
    //
    case AST_FUNC_CALL: {
        printf("(FCALL ");
        PrintAST(n->funcCall.func,0);
        printf("(");
        long iter;
        AST *arg;
        vec_AST_t unpacked=CommaToVec(n->funcCall.args);
        vec_foreach(&unpacked, arg, iter) {
            PrintAST(arg,0);
            printf(",");
        }
        vec_deinit(&unpacked);
        printf("))");
        break;
    }
    //
    case AST_IF:
        printf("(IF ");
        PrintAST(n->ifStmt.cond,0);
        printf(")\n");
        PrintAST(n->ifStmt.body,0);
        if(n->ifStmt.elseBody)
            PrintAST(n->ifStmt.elseBody,tabs+1);
        break;
    case AST_FOR:
        printf("(FOR ");
        PrintAST(n->forStmt.init,0);
        printf(";");
        PrintAST(n->forStmt.cond,0);
        printf(";");
        PrintAST(n->forStmt.next,0);
        printf(")\n");
        PrintAST(n->forStmt.body,tabs+1);
        if(n->ifStmt.elseBody) {
            printf("ELSE");
            PrintAST(n->ifStmt.elseBody,tabs+1);
        }
        break;
    case AST_DO:
        printf("DO\n");
        PrintAST(n->doStmt.body,tabs+1);
        while(--tabs>=0) printf(" ");
        printf("(WHILE ");
        PrintAST(n->doStmt.cond, 0);
        printf(")");
        break;
    case AST_WHILE:
        printf("(WHILE\n");
        PrintAST(n->whileStmt.cond,0);
        printf(")");
        PrintAST(n->whileStmt.body, tabs+1);
        break;
    //
    case AST_METADATA: {
        int iter;
        CMetaData meta;
        printf("(META ");
        vec_foreach(&n->metaData, meta, iter) {
            printf("(");
            printf("%s",meta.name);
            puts(" = ");
            PrintAST(meta.value, 0);
            printf(")");
        }
        printf(")");
        break;
    }
    //
    case AST_DECL:
        printf("(DECL ");
        printf("%s",n->decl.name);
        if(n->decl.dft) {
            puts(" = ");
            PrintAST(n->decl.dft, 0);
        }
        printf(")");
        break;
    case AST_VAR:
        printf("(%s ",(n->var->isGlobal)?"GLOBAL":"LOCAL");
        printf("%s)",n->var->name);
        break;
    //
    case AST_KEYWORD:
        break;
    //
    case AST_CASE:
        puts("(CASE");
        if(n->cs.low) {
            printf(" [%ld",n->cs.low);
            puts(",");
        }
        if(n->cs.low) {
            printf("%ld",n->cs.high);
            puts("]");
        }
        puts(")");
        break;
    case AST_SWITCH:
        puts("(SWITCH ");
        PrintAST(n->switStmt.cond,0);
        puts(")");
        PrintAST(n->switStmt.body,tabs+1);
        break;
    case AST_SUBSWITCH:
        puts("(SUB SWITCH)");
        PrintAST(n->subswitch.startCode,tabs+1);
        PrintAST(n->subswitch.body,tabs+1);
        break;
    case AST_BREAK:
        puts("(BREAK)");
        break;
    case AST_DEFAULT:
        puts("(DEFAULT)");
        break;
    //
    case AST_TYPE: {
        puts("(TYPE ");
        char *ts=TypeToString(n->type2);
        puts(ts);
        TD_FREE(ts);
        puts(")");
        break;
    }
    }
}
CType *FinalizeClass(CType *class) {
    if(class->type!=TYPE_CLASS) return class;
    long pad=(TypeAlign(class)-(TypeSize(class)%TypeAlign(class)))%TypeAlign(class);
    class->cls.size+=pad;
    return class;
}
/**
 * appendto is a AST_TYPE with TYPE_CLASS/TYPE_UNION
 * member can be a DECL_TAIL or an anoynumus class/union(AST_TYPE)
 * member can also be a AST_METADATA
 */
AST *AppendToTypeMembers (AST *appendto,AST* member) {
    assert(appendto->type==AST_TYPE);
    CType *t=appendto->type2;
    long base=TypeSize(t);
    vec_CMember_t *members=(t->type==TYPE_CLASS)?
                           &appendto->type2->cls.members:
                           &appendto->type2->un.members;
    if(member->type==AST_DECL) {
        CMember mem;
        mem.name=strdup(member->decl.name);
        mem.type=member->decl.type;
        mem.fn=member->fn;
        mem.line=member->ln;
        vec_init(&mem.meta);
        if(t->type==TYPE_CLASS) {
            //Update size/align
#define FINALIZE_SA(mem) \
if(t->type==TYPE_UNION) { \
  if(t->un.align<align) t->un.align=align; \
  t->un.size=size; \
  vec_push(&t->un.members,mem); \
}  else { \
  if(t->cls.align<align) t->cls.align=align; \
  t->cls.size=size; \
  vec_push(&t->cls.members,mem); \
}
#define UPDATE_SA(mem,isClassStyle) \
      if(isClassStyle) { \
        int size,align; \
        if(t->type==TYPE_CLASS) { \
          size=TypeSize(t); \
          align=TypeAlign(mem.type); \
          int pad=(align-size%align)%align; \
          mem.offset=size+pad; \
          size+=pad+TypeSize(mem.type); \
        } else if(t->type==TYPE_UNION) { \
          size=TypeSize(mem.type)+mem.offset; \
          align=TypeAlign(mem.type); \
          int pad=(align-size%align)%align; \
          size+=pad; \
        } \
        FINALIZE_SA(mem); \
      } else { \
        int size; \
        if(t->type==TYPE_UNION) { \
          size=t->un.size; \
          size=(TypeSize(mem.type)>size)?TypeSize(mem.type):size; \
        } else if(t->type==TYPE_CLASS) { \
          size=t->cls.size; \
          size=(TypeSize(mem.type)+base>size)?TypeSize(mem.type)+base:size; \
        } \
        mem.offset=(t->type==TYPE_UNION)?0:base; \
        long align=TypeAlign(mem.type); \
        FINALIZE_SA(mem); \
      }
            UPDATE_SA(mem,1); //1 is CLASS style
        } else {
            UPDATE_SA(mem,0); //0 is UNION style
        }
    } else if(member->type==__AST_DECL_TAILS) {
        int iter;
        CDeclTail tail;
        vec_foreach(&member->declTail, tail, iter) {
            AST *decl =TD_CALLOC(1,sizeof(AST));
            decl->refCnt=1;
            decl->type=AST_DECL;
            decl->decl.name=strdup(tail.name->name);
            decl->decl.type=tail.finalType;
            decl->decl.dft=tail.dft;
            decl->fn=tail.name->fn;
            decl->ln=tail.name->ln;
            AppendToTypeMembers(appendto,decl);
            if(tail.metaData)
                AppendToTypeMembers(appendto,tail.metaData);
        }
    } else if(member->type==AST_METADATA) {
        vec_extend(&vec_last(members).meta,&member->metaData);
    } else if(member->type==AST_TYPE) {
        CType *t2=member->type2;
        assert(t2->type==TYPE_CLASS||t2->type==TYPE_UNION);
        vec_CMember_t *members2;
        if(t2->type==TYPE_CLASS) {
            members2=&t2->cls.members;
        } else if(t2->type==TYPE_UNION) {
            members2=&t2->cls.members;
        }
        //Padd to member->type2 pad;
        if(t->type==TYPE_CLASS) {
            long size=t->cls.size;
            long align=TypeAlign(t2);
            int pad=(align-size%align)%align;
            t->cls.size+=pad;
            if(TypeAlign(t)<align) t->cls.align=align;
            base=t->cls.size;
        }
        CMember member;
        long iter;
        vec_foreach(members2, member, iter) {
            CMember clone=member;
            clone.type=member.type;
            clone.name=strdup(member.name);
            UPDATE_SA(clone,t2->type==TYPE_CLASS);
        }
    } else {
        abort();
    }
    return appendto;
}
CType *BaseType(CType *bt) {
    if(bt->type==TYPE_UNION)
        if(bt->un.baseType)
            return BaseType(bt->un.baseType);
    CType *ret=ResolveType(bt);
    return ret;
}
int IsIntegerType(CType *t) {
    CType *bt =BaseType(t);
    switch(bt->type) {
    case TYPE_BOOL:
    case TYPE_I8:
    case TYPE_U8:
    case TYPE_I16:
    case TYPE_U16:
    case TYPE_I32:
    case TYPE_U32:
    case TYPE_I64:
    case TYPE_U64:
    case TYPE_PTR:
    case TYPE_ARRAY_REF:
        return 1;
    }
    return 0;
}
static void CompileLargeTransfer(jit_value dst,jit_value src,CType *type) {
    long size=TypeSize(type);
    jit_prepare(Compiler.JIT);
    jit_putargr(Compiler.JIT, dst);
    jit_putargr(Compiler.JIT, src);
    jit_putargi(Compiler.JIT, size);
    jit_call(Compiler.JIT, &memcpy);
}
CVariable *GetVariable(char *name) {
    CVariable **var;
    if(var=(map_get(&Compiler.locals,name))) {
        return CloneVariable(*var);
    } else if(var=(map_get(&Compiler.globals,name)))
        return CloneVariable(*var);
    return NULL;
}
static CVariable *CreateTmpVarAtFrameOff(long foff,CType *t) {
    CVariable *tmp=TD_CALLOC(1,sizeof(CVariable));
    tmp->refCount=1;
    tmp->name=NULL;
    tmp->isNoreg=1;
    tmp->type=t;
    tmp->frameOffset=foff-Compiler.frameOffset;
    return tmp;
}
jit_value MoveGlobalPtrToReg(CVariable *var,int r) {
    switch(var->linkage.type) {
    case LINK_NORMAL:
        if(var->isFunc)
            jit_movi(Compiler.JIT,R(r),var->func->funcptr);
        else
            jit_movi(Compiler.JIT,R(r),var->linkage.globalPtr);
        return R(r);
        break;
    case LINK__EXTERN:
    case LINK_EXTERN:
        return MoveGlobalPtrToReg(var->linkage.externVar, r);
        break;
    case LINK_IMPORT:
    case LINK__IMPORT:
        ;
        CVariable **g=map_get(&Compiler.globals,var->linkage.importLink);
        if(!g) {
indir:
            jit_ldi(Compiler.JIT, R(r), var->linkage.dummyPtr, sizeof(void*));
            return R(r);
        } else {
            /**
             * A forward declaration pointing to self will be resolved when it is defined,so dont infinitey recur here looking for a definition
             */
            if(0!=strcmp(var->linkage.importLink,var->name))
                return MoveGlobalPtrToReg(*g, r);
            goto indir;
        }
        break;
    default:
        abort();
    }
}
void *GetGlobalPtr(CVariable *var) {
    switch(var->linkage.type) {
    case LINK_NORMAL:
        if(var->isFunc)
	  return var->func->funcptr;
        else
	  return var->linkage.globalPtr;
        break;
    case LINK__EXTERN:
    case LINK_EXTERN:
      return GetGlobalPtr(var->linkage.externVar);
        break;
    case LINK_IMPORT:
    case LINK__IMPORT:
        ;
        CVariable **g=map_get(&Compiler.globals,var->linkage.importLink);
        if(!g) {
indir:
            return var->linkage.dummyPtr;
        } else {
            /**
             * A forward declaration pointing to self will be resolved when it is defined,so dont infinitey recur here looking for a definition
             */
	  if(0!=strcmp(var->linkage.importLink,var->name))
                return GetGlobalPtr(*g);
            goto indir;
        }
        break;
    default:
        abort();
    }
}
void __CompileAssignInt2V(CVariable *var,uint64_t i) {
    CType *bt =BaseType(var->type);
    int tsz=TypeSize(bt);
    if(bt->type==TYPE_F64) {
        if(IsVarRegCanidate(var)) {
            jit_fmovi(Compiler.JIT, FR(var->reg), i);
        } else if(!IsVarRegCanidate(var)&&var->isGlobal) {
            jit_fmovi(Compiler.JIT, FR(0), (double)i);
            jit_fstr(Compiler.JIT, MoveGlobalPtrToReg(var,0),FR(0),8);
        } else if(!IsVarRegCanidate(var)&&!var->isGlobal) {
            jit_fmovi(Compiler.JIT, FR(0),(double)i);
            jit_fstxi(Compiler.JIT, Compiler.frameOffset+var->frameOffset,R_FP,FR(0),8);
        }
    } else if(IsIntegerType(bt)||IsPtrType(bt)) {
        //Is Integer
        if(IsVarRegCanidate(var)) {
            jit_movi(Compiler.JIT,R(var->reg),i);
        } else if(!IsVarRegCanidate(var)&&!var->isGlobal) {
            jit_movi(Compiler.JIT, R(0), i);
            jit_stxi(Compiler.JIT, Compiler.frameOffset+var->frameOffset,R_FP,R(0),TypeSize(var->type));
        } else if(!IsVarRegCanidate(var)&&var->isGlobal) {
            jit_movi(Compiler.JIT, R(0), i);
            jit_str(Compiler.JIT, MoveGlobalPtrToReg(var,1),R(0),TypeSize(var->type));
        }
    }
}
void __CompileAssignFlt2V(CVariable *var,double f) {
    CType *bt =BaseType(var->type);
    int tsz=TypeSize(bt);
    if(bt->type==TYPE_F64) {
        if(IsVarRegCanidate(var)) {
            jit_fmovi(Compiler.JIT, FR(var->reg), f);
        } else if(!IsVarRegCanidate(var)&&!var->isGlobal) {
            jit_fmovi(Compiler.JIT, FR(0), f);
            jit_fstxi(Compiler.JIT,Compiler.frameOffset+var->frameOffset,R_FP,FR(0),tsz);
        } else if(!IsVarRegCanidate(var)&&var->isGlobal) {
            jit_fmovi(Compiler.JIT, FR(0), f);
            jit_fstr(Compiler.JIT,MoveGlobalPtrToReg(var,1),FR(0),tsz);
        }
    } else if(IsIntegerType(bt)||IsPtrType(bt)) { //Is integer
        if(IsVarRegCanidate(var)) {
            jit_movi(Compiler.JIT,R(var->reg),f);
        } else if(!IsVarRegCanidate(var)&&!var->isGlobal) {
            jit_movi(Compiler.JIT, R(0), f);
            jit_stxi(Compiler.JIT,Compiler.frameOffset+var->frameOffset,R_FP,R(0),tsz);
        } else if(!IsVarRegCanidate(var)&&var->isGlobal) {
            jit_movi(Compiler.JIT, R(0), f);
            jit_str(Compiler.JIT, MoveGlobalPtrToReg(var,1), R(0),tsz);
        }
    }
}
static void __CompileAssignV2LV(CVariable *var,CVariable *var2) {
    assert(!var->isGlobal);
    CType *bt =BaseType(var->type),*bt2 =BaseType(var2->type);
    int tsz=TypeSize(bt),t2sz=TypeSize(bt2);
    if(bt->type==TYPE_F64) {
        if(bt2->type==TYPE_F64) {
            if(IsVarRegCanidate(var)&&IsVarRegCanidate(var2)&&!var2->isGlobal)
                jit_fmovr(Compiler.JIT,FR(var->reg), FR(var2->reg));
            else if(IsVarRegCanidate(var)&&!IsVarRegCanidate(var2)&&!var2->isGlobal) {
                jit_fldxi(Compiler.JIT, FR(0), R_FP,Compiler.frameOffset+var2->frameOffset, t2sz);
FR2r: //to reg
                jit_fmovr(Compiler.JIT, FR(var->reg),FR(0));
            } else if(!IsVarRegCanidate(var)&&IsVarRegCanidate(var2)&&!var2->isGlobal) {
                jit_fstxi(Compiler.JIT, Compiler.frameOffset+var->frameOffset, R_FP, FR(var2->reg), tsz);
            } else if(!IsVarRegCanidate(var)&&!IsVarRegCanidate(var2)&&!var2->isGlobal) {
                jit_fldxi(Compiler.JIT, FR(0), R_FP,Compiler.frameOffset+var2->frameOffset, t2sz);
FR2f: //to frame
                jit_fstxi(Compiler.JIT, Compiler.frameOffset+var->frameOffset, R_FP, FR(0), tsz);
            } else if(IsVarRegCanidate(var)&&var2->isGlobal) {
                jit_fldr(Compiler.JIT, FR(0), MoveGlobalPtrToReg(var2,0), t2sz);
                goto FR2r;
            } else if(!IsVarRegCanidate(var)&&var2->isGlobal) {
                jit_fldr(Compiler.JIT, FR(0), MoveGlobalPtrToReg(var2,0), t2sz);
                goto FR2f;
            }
        } else if(IsIntegerType(bt2)) {
            if(IsVarRegCanidate(var2)) {
                jit_extr(Compiler.JIT, FR(0), R(var2->reg));
            } else if(!var2->isGlobal) {
                if(TypeIsSigned(bt2))
                    jit_ldxi(Compiler.JIT,R(0),R_FP,Compiler.frameOffset+var2->frameOffset,t2sz);
                else
                    jit_ldxi_u(Compiler.JIT,R(0),R_FP,Compiler.frameOffset+var2->frameOffset,t2sz);
                jit_extr(Compiler.JIT, FR(0), R(0));
            } else if(var2->isGlobal) {
                if(TypeIsSigned(bt2))
                    jit_ldr(Compiler.JIT,R(0),MoveGlobalPtrToReg(var2,1),t2sz);
                else
                    jit_ldr_u(Compiler.JIT,R(0),MoveGlobalPtrToReg(var2,1),t2sz);
                jit_extr(Compiler.JIT, FR(0), R(0));
            }
            if(IsVarRegCanidate(var)) {
                goto FR2r;
            } else goto FR2f;
        }
    } else if(IsIntegerType(bt)) {
        if(IsIntegerType(bt2)) {
            if(IsVarRegCanidate(var)&&IsVarRegCanidate(var2)) {
                jit_movr(Compiler.JIT,R(var->reg),R(var2->reg));
            } else if(IsVarRegCanidate(var)&&!IsVarRegCanidate(var2)&&!var2->isGlobal) {
                if(TypeIsSigned(bt2))
                    jit_ldxi(Compiler.JIT,R(var->reg),R_FP,Compiler.frameOffset+var2->frameOffset,t2sz);
                else
                    jit_ldxi_u(Compiler.JIT,R(var->reg),R_FP,Compiler.frameOffset+var2->frameOffset,t2sz);
            } else if(!IsVarRegCanidate(var)&&IsVarRegCanidate(var2)) {
                jit_movr(Compiler.JIT,R(0),R(var2->reg));
                goto R2f;
            } else if(!IsVarRegCanidate(var)&&!IsVarRegCanidate(var2)&&!var2->isGlobal) {
                if(TypeIsSigned(bt2))
                    jit_ldxi(Compiler.JIT,R(0),R_FP,Compiler.frameOffset+var2->frameOffset,t2sz);
                else
                    jit_ldxi_u(Compiler.JIT,R(0),R_FP,Compiler.frameOffset+var2->frameOffset,t2sz);
R2f:
                jit_stxi(Compiler.JIT,Compiler.frameOffset+var->frameOffset,R_FP,R(0),tsz);
            } else if(IsVarRegCanidate(var)&&var2->isGlobal) {
                if(TypeIsSigned(bt2))
                    jit_ldr(Compiler.JIT,R(var->reg),MoveGlobalPtrToReg(var2,0),t2sz);
                else
                    jit_ldr_u(Compiler.JIT,R(var->reg),MoveGlobalPtrToReg(var2,0),t2sz);
            } else if(!IsVarRegCanidate(var)&&var2->isGlobal)  {
                if(TypeIsSigned(bt2))
                    jit_ldr(Compiler.JIT,R(0),MoveGlobalPtrToReg(var2,1),t2sz);
                else
                    jit_ldr_u(Compiler.JIT,R(0),MoveGlobalPtrToReg(var2,0),t2sz);
                goto R2f;
            }
        } else if(bt2->type==TYPE_F64) {
            if(IsVarRegCanidate(var)&&IsVarRegCanidate(var2)) {
                jit_truncr(Compiler.JIT, R(var->reg), FR(var2->reg)); //CANN
            } else if(IsVarRegCanidate(var)&&!IsVarRegCanidate(var2)&&!var2->isGlobal) {
                jit_fldxi(Compiler.JIT,FR(0),R_FP,Compiler.frameOffset+var2->frameOffset,t2sz);
                jit_truncr(Compiler.JIT, R(var->reg), FR(0));
            } else if(!IsVarRegCanidate(var)&&!IsVarRegCanidate(var2)&&!var2->isGlobal) {
                jit_fldxi(Compiler.JIT,FR(0),R_FP,Compiler.frameOffset+var2->frameOffset,t2sz);
                goto R2f;
            } else if(!IsVarRegCanidate(var)&&IsVarRegCanidate(var2)) {
                jit_truncr(Compiler.JIT, R(0), FR(var2->reg));
                goto R2f;
            } else if(IsVarRegCanidate(var)&&var2->isGlobal) {
                jit_fldr(Compiler.JIT,FR(0),MoveGlobalPtrToReg(var2,0),t2sz);
                jit_truncr(Compiler.JIT, R(var->reg), FR(0));
            } else if(!IsVarRegCanidate(var)&&var2->isGlobal) {
                jit_fldr(Compiler.JIT,FR(0),MoveGlobalPtrToReg(var2,0),t2sz);
                jit_truncr(Compiler.JIT, R(0), FR(0));
                goto R2f;
            }
        } else if(bt2->type==TYPE_ARRAY||bt2->type==TYPE_ARRAY_REF) {
            CValue arr AF_VALUE=VALUE_VAR(var2);
            if(IsVarRegCanidate(var)) {
                MoveValueToIntRegIfNeeded(arr, var->reg);
            } else if(!IsVarRegCanidate(var)) {
                MoveValueToIntRegIfNeeded(arr, 0);
                jit_stxi(Compiler.JIT, Compiler.frameOffset+var->frameOffset,R_FP, R(0),TypeSize(bt));
            }
        } else if(IsCompositeType(bt2)) {
            CValue val AF_VALUE=VALUE_VAR(var2);
            if(var->isReg) {
                jit_value v=MoveValueToIntRegIfNeeded(val, var->reg);
                jit_movr(Compiler.JIT,R(var->reg),v);
            } else if(var->isNoreg) {
                jit_value v=MoveValueToIntRegIfNeeded(val, 0);
                jit_stxi(Compiler.JIT, Compiler.frameOffset+var->frameOffset, R_FP, v, TypeSize(bt));
            }
        }
    } else if(bt->type==TYPE_CLASS||bt->type==TYPE_UNION) {
        if(var2->isReg) {
            if(var->isReg) {
                jit_movi(Compiler.JIT, R(var->isReg), R(var2->isReg));
            } else {
                jit_addi(Compiler.JIT, R(0), R_FP, Compiler.frameOffset+var2->frameOffset);
                PutXBytes(var2->reg, 0, TypeSize(var2->type));
            }
            return ;
        }
        if(var2->isGlobal)
            MoveGlobalPtrToReg(var2, 0);
        else if(var2->isNoreg)
            jit_addi(Compiler.JIT, R(0), R_FP, Compiler.frameOffset+var2->frameOffset);
        else abort();
        if(var->isNoreg)
            jit_addi(Compiler.JIT, R(1), R_FP, Compiler.frameOffset+var->frameOffset);
        else if(var->isReg) {
            GrabXBytes(var->reg,0,TypeSize(var2->type));
            return;
        }
        jit_prepare(Compiler.JIT);
        jit_putargr(Compiler.JIT, R(1));
        jit_putargr(Compiler.JIT, R(0));
        jit_putargi(Compiler.JIT,TypeSize(var2->type));
        jit_call(Compiler.JIT, memcpy);
    }
end:
    ;
}
static void __CompileAssignV2GV(CVariable *var,CVariable *var2) {
    CType *bt =BaseType(var->type),*bt2 =BaseType(var2->type);
    int tsz=TypeSize(bt),t2sz=TypeSize(bt2);
    assert(var->isGlobal);
    if(bt2->type==TYPE_F64) {
        if(bt->type==TYPE_F64) {
            //var is f64,var2 is f64
            if(IsVarRegCanidate(var2)) {
                jit_fstr(Compiler.JIT, MoveGlobalPtrToReg(var,0), FR(var2->reg),tsz);
            } else if(var2->isGlobal) {
                jit_fldr(Compiler.JIT,FR(0),MoveGlobalPtrToReg(var2,0),t2sz);
                jit_fstr(Compiler.JIT, MoveGlobalPtrToReg(var,0), FR(0), tsz);
            } else { //Is local
                jit_fldxi(Compiler.JIT,FR(0),R_FP,Compiler.frameOffset+var2->frameOffset,t2sz);
                jit_fstr(Compiler.JIT,  MoveGlobalPtrToReg(var,0), FR(0), tsz);
            }
        } else if(IsIntegerType(bt)||IsPtrType(bt)) {
            //var is integer,var2 is f64
            if(IsVarRegCanidate(var2)) {
                jit_truncr(Compiler.JIT, R(0), FR(var2->reg));
                jit_str(Compiler.JIT, MoveGlobalPtrToReg(var,1), R(0), tsz);
            } else if(var2->isGlobal) {
                jit_fldr(Compiler.JIT,FR(0),MoveGlobalPtrToReg(var2,1),t2sz);
                jit_truncr(Compiler.JIT, R(0), FR(0));
                jit_str(Compiler.JIT, MoveGlobalPtrToReg(var,1), R(0), tsz);
            } else {
                //Is local
                jit_fldxi(Compiler.JIT,FR(0),R_FP,Compiler.frameOffset+var2->frameOffset,t2sz);
                jit_truncr(Compiler.JIT,R(0),FR(0));
                jit_str(Compiler.JIT, MoveGlobalPtrToReg(var,1), R(0), tsz);
            }
        }
    } else if(IsIntegerType(bt2)||IsPtrType(bt2)) {
        if(bt->type==TYPE_F64) {
            //var is f64,var2 is integer
            if(IsVarRegCanidate(var2)) {
                jit_extr(Compiler.JIT,FR(0),R(var2->reg));
                jit_fstr(Compiler.JIT,MoveGlobalPtrToReg(var,0),FR(0),tsz);
            } else if(var2->isGlobal) {
                CVariable *tvar AF_VAR=CreateTmpRegVar(GetIntTmpReg(), bt);
                MoveGlobalPtrToReg(tvar, tvar->reg);
                if(bt2->type==TYPE_ARRAY||bt2->type==TYPE_ARRAY) {
                  jit_movr(Compiler.JIT, R(0), R(tvar->reg));
                } else if(TypeIsSigned(bt2))
                    jit_ldr(Compiler.JIT,R(0),R(tvar->reg),t2sz);
                else
                    jit_ldr_u(Compiler.JIT,R(0),R(tvar->reg),t2sz);
                jit_extr(Compiler.JIT,FR(0),R(0));
                jit_fstr(Compiler.JIT,MoveGlobalPtrToReg(var,0),FR(0),tsz);
            } else if(!var2->isGlobal) {
                if(TypeIsSigned(bt2))
                    jit_ldxi(Compiler.JIT,R(0),R_FP,Compiler.frameOffset+var2->frameOffset,t2sz);
                else
                    jit_ldxi_u(Compiler.JIT,R(0),R_FP,Compiler.frameOffset+var2->frameOffset,t2sz);

                jit_extr(Compiler.JIT,FR(0),R(0));
                jit_fstr(Compiler.JIT,MoveGlobalPtrToReg(var,0),FR(0),tsz);
            }
        } else {
            //var is integer,var2 is integer
            if(IsVarRegCanidate(var2)) {
                jit_str(Compiler.JIT,MoveGlobalPtrToReg(var,0),R(var2->reg),tsz);
            } else if(var2->isGlobal) {
                if(bt2->type==TYPE_ARRAY||bt2->type==TYPE_ARRAY_REF) {
                  jit_movr(Compiler.JIT,R(0),MoveGlobalPtrToReg(var2,0));
                } else if(TypeIsSigned(bt2))
                    jit_ldr(Compiler.JIT,R(0),MoveGlobalPtrToReg(var2,0),t2sz);
                else
                    jit_ldr_u(Compiler.JIT,R(0),MoveGlobalPtrToReg(var2,0),t2sz);
                jit_str(Compiler.JIT,MoveGlobalPtrToReg(var,1),R(0),tsz);
            } else if(!var2->isGlobal) {
                if(TypeIsSigned(bt2))
                    jit_ldxi(Compiler.JIT,R(0),R_FP,Compiler.frameOffset+var2->frameOffset,t2sz);
                else
                    jit_ldxi_u(Compiler.JIT,R(0),R_FP,Compiler.frameOffset+var2->frameOffset,t2sz);

                jit_str(Compiler.JIT,MoveGlobalPtrToReg(var,1),R(0),tsz);
            } else if(IsCompositeType(bt2)) {
                CValue val AF_VALUE=VALUE_VAR(var2);
                jit_value v=MoveValueToIntRegIfNeeded(val, 0);
                MoveGlobalPtrToReg(var, 1);
                jit_str(Compiler.JIT, R(1),v, TypeSize(bt));
            }
        }
    } else if(bt->type==TYPE_CLASS||bt->type==TYPE_UNION) {
        MoveGlobalPtrToReg(var, 1);

        if(var2->isGlobal)
            MoveGlobalPtrToReg(var2, 0);
        else if(var2->isNoreg)
            jit_addi(Compiler.JIT, R(0), R_FP, Compiler.frameOffset+var2->frameOffset);
        else if(var2->isReg) {
            PutXBytes(var2->reg, 1, TypeSize(bt2));
            return;
        }
        jit_prepare(Compiler.JIT);
        jit_putargr(Compiler.JIT, R(1));
        jit_putargr(Compiler.JIT, R(0));
        jit_putargi(Compiler.JIT,TypeSize(var2->type));
        jit_call(Compiler.JIT, memcpy);
    }
end:
    ;
}
void __CompileAssignToFrameV(long off,CVariable *var,CType *t) {
    CVariable *tmp AF_VAR=CreateTmpVarAtFrameOff(off, t);
    __CompileAssignV2LV(tmp, var);
}
void CompileAssign(CValue dst,CValue src) {
    if(dst.type==VALUE_VAR) {
        if(src.type==VALUE_VAR) {
            if(!dst.var->isGlobal) {
                __CompileAssignV2LV(dst.var, src.var);
            } else {
                __CompileAssignV2GV(dst.var, src.var);
            }
        } else if(src.type==VALUE_INT) {
            __CompileAssignInt2V(dst.var,src.integer);
        } else if(src.type==VALUE_FLOAT) {
            __CompileAssignFlt2V(dst.var,src.flt);
        } else if(src.type==VALUE_INDIR_VAR) {
            CValue indirv AF_VALUE=VALUE_VAR(src.var);
            jit_value ins=MoveValueToIntRegIfNeeded(indirv,1);
            CType *derefts =DerrefedType(src.var->type);
            long derefss=TypeSize(derefts);
            if(dst.var->isGlobal) {
                if(IsIntegerType(derefts)) {
                    if(TypeIsSigned(derefts))
                        jit_ldr(Compiler.JIT, R(0), ins, derefss);
                    else
                        jit_ldr_u(Compiler.JIT, R(0), ins, derefss);

                    if(IsIntegerType(dst.var->type))  {
                        jit_str(Compiler.JIT, MoveGlobalPtrToReg(dst.var,1), R(0), TypeSize(dst.var->type));
                    } else {
                        jit_truncr(Compiler.JIT, FR(0), R(0));
                        jit_str(Compiler.JIT, MoveGlobalPtrToReg(dst.var,1), FR(0), TypeSize(dst.var->type));
                    }
                } else if(IsF64(derefts)) {
                    jit_fldr(Compiler.JIT,FR(0),R(0),8);
                    if(IsIntegerType(dst.var->type))  {
                        jit_fstr(Compiler.JIT, MoveGlobalPtrToReg(dst.var,0), FR(0), TypeSize(dst.var->type));
                    } else {
                        jit_extr(Compiler.JIT, R(0),FR(0));
                        jit_str(Compiler.JIT, MoveGlobalPtrToReg(dst.var,0), FR(0), TypeSize(dst.var->type));
                    }
                } else {
                    //memcpy
                    MoveGlobalPtrToReg(dst.var,0);
                    jit_prepare(Compiler.JIT);
                    jit_putargr(Compiler.JIT,R(0));
                    jit_putargr(Compiler.JIT,ins);
                    jit_putargi(Compiler.JIT,TypeSize(derefts));
                    jit_call(Compiler.JIT,&memcpy);
                }
            } else if(IsVarRegCanidate(dst.var)) {
                if(IsIntegerType(derefts)) {
                    if(IsIntegerType(dst.var->type)) {
                        if(TypeIsSigned(derefts))
                            jit_ldr(Compiler.JIT, R(dst.var->reg), ins, derefss);
                        else
                            jit_ldr_u(Compiler.JIT, R(dst.var->reg), ins, derefss);
                    } else if(IsF64(dst.var->type)) {
                        if(TypeIsSigned(derefts))
                            jit_ldr(Compiler.JIT, R(0), ins, derefss);
                        else
                            jit_ldr_u(Compiler.JIT, R(0), ins, derefss);
                        jit_extr(Compiler.JIT, FR(dst.var->reg), R(0));
                    } else abort();
                } else if(IsF64(derefts)) {
                    if(IsIntegerType(dst.var->type)) {
                        jit_fldr(Compiler.JIT, FR(0), ins, derefss);
                        jit_truncr(Compiler.JIT, R(dst.var->reg), FR(0));
                    } else if(IsF64(dst.var->type)) {
                        jit_fldr(Compiler.JIT, dst.var->reg,ins, derefss);
                    } else abort();
                } else abort();
            } else {
                if(derefts->type==TYPE_ARRAY|derefts->type==TYPE_ARRAY_REF) {
                    jit_movr(Compiler.JIT, R(0),ins);
                    goto r02f;
                }
                //Is local frame
                if(IsIntegerType(derefts)) {
                    if(TypeIsSigned(derefts))
                        jit_ldr(Compiler.JIT, R(0), ins, derefss);
                    else
                        jit_ldr_u(Compiler.JIT, R(0), ins, derefss);
r02f:
                    ;
                    if(IsIntegerType(dst.var->type))  {
                        jit_stxi(Compiler.JIT, Compiler.frameOffset+dst.var->frameOffset,R_FP, R(0), TypeSize(dst.var->type));
                    } else {
                        jit_truncr(Compiler.JIT, FR(0), R(0));
                        jit_fstxi(Compiler.JIT, Compiler.frameOffset+dst.var->frameOffset,R_FP, FR(0), TypeSize(dst.var->type));
                    }
                } else if(IsF64(derefts)) {
                    jit_fldr(Compiler.JIT,FR(0),R(0),8);
                    if(IsIntegerType(dst.var->type))  {
                        jit_fstxi(Compiler.JIT, Compiler.frameOffset+dst.var->frameOffset,R_FP, FR(0), TypeSize(dst.var->type));
                    } else {
                        jit_extr(Compiler.JIT, R(0),FR(0));
                        jit_stxi(Compiler.JIT,Compiler.frameOffset+ dst.var->frameOffset,R_FP, R(0), TypeSize(dst.var->type));
                    }
                } else {
                    //memcpy
                    jit_addi(Compiler.JIT, R(0), R_FP, Compiler.frameOffset+dst.var->frameOffset);
                    jit_prepare(Compiler.JIT);
                    jit_putargr(Compiler.JIT,R(0));
                    jit_putargr(Compiler.JIT,ins);
                    jit_putargi(Compiler.JIT, TypeSize(derefts));
                    jit_call(Compiler.JIT,&memcpy);
                }
            }
        }
    } else if(dst.type==VALUE_INDIR_VAR) {
        CValue inval AF_VALUE=VALUE_VAR(dst.var);
        jit_value in=MoveValueToIntRegIfNeeded(inval,1);
        CType *deref1 =DerrefedType(dst.var->type);
        if(src.type==VALUE_INT) {
            jit_value i=MoveValueToIntRegIfNeeded(src,0);
            if(IsIntegerType(deref1))
                jit_str(Compiler.JIT, in, i, TypeSize(deref1));
            else if(IsF64(deref1))
                jit_str(Compiler.JIT, in, i, TypeSize(deref1));
        } else if(src.type==VALUE_FLOAT) {
            jit_value f=MoveValueToIntRegIfNeeded(src,0);
            if(IsIntegerType(deref1))
                jit_fstr(Compiler.JIT, in, f, TypeSize(deref1));
            else if(IsF64(deref1))
                jit_fstr(Compiler.JIT, in, f, TypeSize(deref1));
        } else if(src.type==VALUE_INDIR_VAR) {
            CValue sinval AF_VALUE=VALUE_VAR(src.var);
            CType *stype =DerrefedType(src.var->type);
            jit_value in2=MoveValueToIntRegIfNeeded(sinval, 0);
            if(IsF64(stype)) {
                jit_fldr(Compiler.JIT, FR(0), in2, 8);
                jit_fstr(Compiler.JIT,in,FR(0),8);
            } else if(IsIntegerType(stype)) {
                if(TypeIsSigned(stype))
                    jit_ldr(Compiler.JIT, R(0), in2, 8);
                else
                    jit_ldr_u(Compiler.JIT, R(0), in2, 8);
                jit_str(Compiler.JIT,in,R(0), TypeSize(deref1));
            } else {
                //memcpy
                jit_prepare(Compiler.JIT);
                jit_putargr(Compiler.JIT,in);
                jit_putargr(Compiler.JIT,in2);
                jit_putargi(Compiler.JIT, TypeSize(deref1));
                jit_call(Compiler.JIT,&memcpy);
            }
        } else if(src.type==VALUE_VAR) {
            if(IsF64(deref1)) {
                jit_value v=MoveValueToFltRegIfNeeded(src, 0);
                jit_fstr(Compiler.JIT, in, v, 8);
            } else if(IsIntegerType(deref1)) {
                jit_value v=MoveValueToIntRegIfNeeded(src, 0);
                jit_str(Compiler.JIT, in, v, TypeSize(deref1));
            } else if(IsCompositeType(deref1)&&src.var->isGlobal) {
                MoveGlobalPtrToReg(src.var, 1);
                jit_prepare(Compiler.JIT);
                jit_putargr(Compiler.JIT,in);
                jit_putargr(Compiler.JIT,R(1));
                jit_putargi(Compiler.JIT, TypeSize(deref1));
                jit_call(Compiler.JIT,&memcpy);
            } else if(IsCompositeType(deref1)&&src.var->isNoreg) {
                jit_addxi(Compiler.JIT, R(1), R_FP, Compiler.frameOffset+src.var->frameOffset);
                jit_prepare(Compiler.JIT);
                jit_putargr(Compiler.JIT,in);
                jit_putargr(Compiler.JIT,R(1));
                jit_putargi(Compiler.JIT, TypeSize(deref1));
                jit_call(Compiler.JIT,&memcpy);
            } else if(IsCompositeType(deref1)&&src.var->isReg) {
                jit_movi(Compiler.JIT, R(0), in);
                PutXBytes(0,src.var->reg, TypeSize(deref1));
            } else abort();
        }
    }
}
CVariable *CreateTmpRegVar(int v,CType *type) {
    CVariable *var=TD_CALLOC(1,sizeof(CVariable));
    var->reg=v;
    var->type=ConvertToArrayRefs(type);
    var->refCount=1;
    var->name=NULL;
    var->isReg=1;
    return var;
}
int IsPtrType(CType *t) {
    CType *bt =BaseType(t);
    int ret=t->type==TYPE_PTR||t->type==TYPE_ARRAY||t->type==TYPE_ARRAY_REF;
    return ret;
}
int IsCompareOperator(AST *node) {
    switch(node->type) {
    case AST_LT:
    case AST_GT:
    case AST_GE:
    case AST_LE:
    case AST_NE:
    case AST_EQ:
        return 1;
    }
    return 0;
}
/**
 * Will resolve a type with linkage,or if none is provided it will just return the type
 */
CType *ResolveType(CType *type) {
    if(type->type==TYPE_CLASS) {
        CLinkage *link=&type->cls.linkage;
        if(link->type==LINK__EXTERN||link->type==LINK_EXTERN)
            return BaseType(ResolveType(link->externType));
    } else if(type->type==TYPE_UNION) {
        CLinkage *link=&type->un.linkage;
        if(link->type==LINK__EXTERN||link->type==LINK_EXTERN)
            return BaseType(ResolveType(link->externType));
    }
    return type;
}
char *RegisterString(char *string) {
  strloop:
  if(map_get(&Compiler.strings,string)) {
      return *map_get(&Compiler.strings,string);
  } else {
      map_set(&Compiler.strings,string,strdup(string));
      goto strloop;
  }
}
CType *__AssignTypeToNode(AST *node) {
    CType *bt=NULL,*ret=NULL;
    if(node->type==AST_LASTCLASS) {
        CType *u8 =CreatePrimType(TYPE_U8);
        return CreatePtrType(u8);
    } else if(IsUnop(node)) {
        if(node->type==AST_LNOT) {
            ret=CreatePrimType(TYPE_BOOL);
        } else if(node->type==AST_ADDROF) {
            if(node->unopArg->type==AST_NAME) {
              //AVOID IMPLCIT FUNCTION CALL
              CVariable *var =GetVariable(node->unopArg->name);
              if(!var) return CreatePrimType(TYPE_I64);
              return CreatePtrType(var->type);
            }
            CType *t =AssignTypeToNode(node->unopArg);
            if(t->type==TYPE_ARRAY||t->type==TYPE_ARRAY_REF) {
                ret=CreatePtrType(t->array.base);
            } else
                ret=CreatePtrType(t);
            return ret;
        } else if(node->type==AST_DERREF) {
            CType *un =AssignTypeToNode(node->unopArg);
            if(IsPtr(un)) {
                ret=DerrefedType(un);
            } else {
                //Error
                ret=CreatePrimType(TYPE_I64);
            }
            return ret;
        }
        return AssignTypeToNode(node->unopArg);
    } else if(IsBinop(node)) {
        CType *left =AssignTypeToNode(node->binop.a),*right =AssignTypeToNode(node->binop.b);
        if(IsCompareOperator(node)) {
            ret=CreatePrimType(TYPE_BOOL);
        } else if(IsModifyAssign(node)) {
            ret=left;
        } else if(IsF64(left)||IsF64(right))
            return CreatePrimType(TYPE_F64);
        else if(IsPtr(left)||IsPtr(right)) {
            if(node->type==AST_SUB) {
                if(IsPtr(left)&&IsPtr(right)) return CreatePrimType(TYPE_I64);
                /*if(IsPtr(left)||IsPtr(right))*/
                ret=(IsPtr(left))?left:right;
                goto endBinop;
            } else if(node->type==AST_ADD) {
            } else {
            }
            ret=(IsPtr(left))?left:right;
        } else if(IsU64(left)||IsU64(right)) {
            //U64 > I64
            ret=CreatePrimType(TYPE_U64);
        } else if(IsI64(left)||IsI64(right)) {
            ret=CreatePrimType(TYPE_I64);
        } else {
            //Strange arithemetic
            ret=CreatePrimType(TYPE_I64);
        }
endBinop:
        ;
    } else if(node->type==AST_FUNC_CALL) {
        CType *f =NULL;
        if(node->funcCall.func->type!=AST_NAME&&node->funcCall.func->type!=AST_VAR) {
            f=AssignTypeToNode(node->funcCall.func);
        } else if(node->funcCall.func->type==AST_NAME) {
            CVariable *var;
            if(var=GetVariable(node->funcCall.func->name))
                f=var->type;
            else goto uncallable;
        } else if(node->funcCall.func->type==AST_VAR) {
            f=node->funcCall.func->var->type;
        } else goto uncallable;
        bt=BaseType(f);
        CType *ftype=NULL;
        if(bt->type==TYPE_FUNC) {
            ftype=bt;
        } else if(bt->type==TYPE_PTR) {
            if(bt->ptr->type==TYPE_FUNC) {
                ftype=bt->ptr;
            } else {
uncallable:
                ret=CreatePrimType(TYPE_I64);
                goto end;
            }
        } else goto uncallable;
        CType *r=f->func.ret;
        ret=r;
        vec_AST_t args=CommaToVec(node->funcCall.args);
        vec_deinit(&args);
    } else if(node->type==AST_MEMBER_ACCESS) {
      //THis will cause class.member to return an offset
      if(Compiler.addrofFrameoffsetMode) {
	return CreatePrimType(TYPE_I64);
      }
        CType *_a =AssignTypeToNode(node->memberAccess.a);
        CType *a =ResolveType(_a);
        vec_CMember_t *members=NULL;
        if(IsPtrType(a)) {
            CType *ptr=DerrefedType(a);
            a=ptr;
        }
        if(a->type==TYPE_CLASS) {
            members=&a->cls.members;
        } else if(a->type==TYPE_UNION) {
            members=&a->un.members;
        }
        if(members) {
            CMember mem;
            long iter;
            vec_foreach(members, mem, iter) {
                if(0==strcmp(mem.name,node->memberAccess.member)) {
                    ret=mem.type;
                    goto foundmem;
                }
            }
        }
        RaiseError(node,"Couldn't find member %s.",node->memberAccess.member);
        //ERROR
        ret=CreatePrimType(TYPE_I64);
foundmem:
        ;

    } else if(node->type==AST_ARRAY_ACCESS) {
        CType *a =AssignTypeToNode(node->arrayAccess.array);
        bt=BaseType(a);
        if(bt->type==TYPE_PTR) {
            ret=bt->ptr;
        } else if(bt->type==TYPE_ARRAY||bt->type==TYPE_ARRAY_REF) {
            ret=bt->array.base;
        } else {
            ret=CreatePrimType(TYPE_I64);
        }
    } else if(node->type==AST_SIZEOF) {
        return CreatePrimType(TYPE_I64);
    } else if(node->type==AST_INT) {
        return CreatePrimType(TYPE_I64);
    } else if(node->type==AST_FLOAT) {
        return CreatePrimType(TYPE_F64);
    } else if(node->type==AST_CHAR) {
        return CreatePrimType(TYPE_U64);
    } else if(node->type==AST_STRING) {
        return CreatePtrType(CreatePrimType(TYPE_U8));
    }
    else if(node->type==AST_VAR) {
        return node->var->type;
    } else if(node->type==AST_RANGE) {
        return CreatePrimType(TYPE_BOOL);
    } else if(node->type==AST_NAME) {
      //If assmbler is active,check for imports
      if(Assembler.active)
	if(map_get(&Assembler.imports,node->name))
	  return CreatePtrType(CreatePrimType(TYPE_U0));
      CVariable *var;
        if(var=GetVariable(node->name)) {
            if(var->type->type==TYPE_FUNC)
                return var->type->func.ret;
            return var->type;
        } else {
            ;
            return CreatePrimType(TYPE_I64);
        }
    } else if(node->type==AST_EXPLICIT_TYPECAST||node->type==AST_IMPLICIT_TYPECAST) {
        return node->typecast.toType->type2;
    }
end:
    return ret;
}
CType *AssignTypeToNode(AST *node) {
    if(node->type2) return node->type2;
    CType *r=__AssignTypeToNode(node);
    CType *bt =BaseType(r);
    if(bt->type==TYPE_ARRAY) {
        //ONLY immediate lvalues are arrays,else they are automatically converted to TYPE_ARRAY_REF
        if(node->type==AST_NAME||node->type==AST_VAR||node->type==AST_MEMBER_ACCESS) {
            //nothing
        } else {
            CType *r2=ConvertToArrayRefs(r);
            node->type2=r2;
            return r2;
        }
    }
    node->type2=r;
    return r;
}
AST *PtrifyIfNeeded(AST *ast) {
    CType *t =AssignTypeToNode(ast),*bt ;
    AST *ret;
    bt=BaseType(t);
    if(!IsPtrType(bt)) {
        ret=CreateUnop(ast, AST_ADDROF);
        goto end;
    }
    ret=ast;
end:
    return ret;
}
AST *ConvertMembersToPtrs(AST *node) {
    if(IsBinop(node)) {
        node->binop.a=ConvertMembersToPtrs(node->binop.a);
        node->binop.b=ConvertMembersToPtrs(node->binop.b);
    } else if(IsBinop(node->unopArg)) {
        node->unopArg=ConvertMembersToPtrs(node->unopArg);
    } else if(node->type==AST_FUNC_CALL) {
        node->funcCall.func=ConvertMembersToPtrs(node->funcCall.func);
        node->funcCall.func=ConvertMembersToPtrs(node->funcCall.args);
    } else if(node->type==AST_ARRAY_ACCESS) {
        node->arrayAccess.array=ConvertMembersToPtrs(node->arrayAccess.array);
        node->arrayAccess.subscript=ConvertMembersToPtrs(node->arrayAccess.subscript);
    } else if(node->type==AST_MEMBER_ACCESS) {
        CType *_t =AssignTypeToNode(node->memberAccess.a);
        CType *t =ResolveType(_t);
        vec_CMember_t  *members=NULL;
        CType *b;
        if(IsPtrType(t)) {
            b=DerrefedType(t);
getmembers:
            if(b->type==TYPE_CLASS) {
                members=&b->cls.members;
            } else if(b->type==TYPE_UNION) {
                members=&b->un.members;
            }
        } else {
            b=t;
            goto getmembers;
        }
        long offset=0;
        if(members) {
            int iter;
            CMember mem;
            vec_foreach(members, mem, iter) {
                if(0==strcmp(mem.name,node->memberAccess.member)) {
                    offset=mem.offset;
                    break;
                }
            }
        }
        CType *restype =AssignTypeToNode(node),*resptr , *u0=CreatePrimType(TYPE_U8);
        AST *ptnode =CreateTypeNode(resptr=CreatePtrType(restype));
        AST *ptrifed =PtrifyIfNeeded(node->memberAccess.a),*casted,*added,*off2 ,*tnode ;
        AST *u0cast =CreateImplicitTypecast(ptrifed,tnode=CreateTypeNode(u0));
        return CreateUnop(casted=CreateImplicitTypecast(added=CreateBinop(u0cast, off2=CreateI64(offset), AST_ADD),ptnode),AST_DERREF);
    }
    return node;
}
void ReleaseValue(CValue *v) {
    if(v->type==VALUE_STRING) TD_FREE(v->string);
    if(v->type==VALUE_VAR) ReleaseVariable(v->var);
    if(v->type==VALUE_INDIR_VAR) ReleaseVariable(v->var);
}
int TypeIsSigned(CType *t) {
    CType *bt =BaseType(t);
    switch(bt->type) {
    case TYPE_I8:
    case TYPE_I16:
    case TYPE_I32:
    case TYPE_I64:
    case TYPE_F64:
        return 1;
    }
    return 0;
}
static void PutXBytes(long srcreg,long ptrreg,long size) {
    int tmpreg=GetIntTmpReg();
    jit_movr(Compiler.JIT, R(tmpreg), R(srcreg));
    srcreg=tmpreg;
    switch(size) {
    default:
    case 8:
        jit_str(Compiler.JIT, R(ptrreg), R(srcreg), 8);
        break;
    case 7:
#define STR_OR_OP(off,size) \
    jit_stxi(Compiler.JIT,off,R(ptrreg), R(srcreg),size); \
    jit_rshi(Compiler.JIT, R(srcreg), R(srcreg), size*8);
        STR_OR_OP(0,4);
        STR_OR_OP(4,2);
        STR_OR_OP(6,1);
        break;
    case 6:
        STR_OR_OP(0,4);
        STR_OR_OP(4,2);
        break;
    case 5:
        STR_OR_OP(0,4);
        STR_OR_OP(4,1);
        break;
    case 4:
        jit_str(Compiler.JIT, R(ptrreg), R(srcreg), 4);
        break;
    case 3:
        STR_OR_OP(0,2);
        STR_OR_OP(2,1);
        break;
    case 2:
        jit_str(Compiler.JIT, R(ptrreg), R(srcreg), 2);
        break;
    case 1:
        jit_str(Compiler.JIT, R(ptrreg), R(srcreg), 0);
        break;
    case 0:
        break;
    }
}
static void GrabXBytes(long dstreg,long ptrreg,long size) {
    int tmpreg=GetIntTmpReg();
    switch(size) {
    default:
    case 8:
        jit_ldr_u(Compiler.JIT, R(dstreg), R(ptrreg), 8);
        break;
    case 7:
#define OR_OP(off,size) \
    jit_lshi(Compiler.JIT, R(dstreg), R(dstreg), size*8); \
    jit_ldxi_u(Compiler.JIT, R(tmpreg), R(ptrreg),off, size); \
    jit_orr(Compiler.JIT, R(dstreg), R(dstreg), R(tmpreg));
        OR_OP(7-1,1);
        OR_OP(7-1-2,2);
        OR_OP(7-1-2-4,4);
        break;
    case 6:
        OR_OP(6-2,2);
        OR_OP(6-2-4,4);
        break;
    case 5:
        OR_OP(5-1,1);
        OR_OP(5-1-4,4);
        break;
    case 4:
        jit_ldr_u(Compiler.JIT, R(dstreg), R(ptrreg), 4);
        break;
    case 3:
        OR_OP(3-1,1);
        OR_OP(3-1-2,2);
        break;
    case 2:
        jit_ldr_u(Compiler.JIT, R(dstreg), R(ptrreg), 2);
        break;
    case 1:
        jit_ldr_u(Compiler.JIT, R(dstreg), R(ptrreg), 1);
        break;
    case 0:
        jit_movi(Compiler.JIT, R(dstreg), R(ptrreg));
        break;
    }
}
static jit_value MoveValueToIntRegIfNeeded(CValue v,int r) {
    switch(v.type) {
    case VALUE_FLOAT: {
        jit_movi(Compiler.JIT, R(r), v.integer);
        return R(r);
        break;
    }
    case VALUE_INT: {
        jit_movi(Compiler.JIT, R(r), v.integer);
        return R(r);
        break;
    }
    case VALUE_VAR: {
        if(IsF64(v.var->type)) {
            if(v.var->isGlobal) {
                MoveGlobalPtrToReg(v.var, 0);
                jit_fldr(Compiler.JIT, FR(0), R(0), 8);
                jit_truncr(Compiler.JIT, R(r), FR(0));
            } else if(v.var->isNoreg) {
                jit_fldxi(Compiler.JIT, FR(0), R_FP,Compiler.frameOffset+v.var->frameOffset, 8);
                jit_truncr(Compiler.JIT, R(r), FR(0));
            } else if(IsVarRegCanidate(v.var)) {
                jit_truncr(Compiler.JIT, R(r), FR(v.var->reg)); //CAN
            }
            return R(r);
        }
        if(v.var->type->type==TYPE_ARRAY) {
            if(v.var->isGlobal)
                MoveGlobalPtrToReg(v.var,r);
            else
                jit_addi(Compiler.JIT, R(r), R_FP,Compiler.frameOffset+v.var->frameOffset);;
        } else if(v.var->isFunc) {
            return MoveGlobalPtrToReg(v.var,r);
        } else if(v.var->isGlobal&&(IsArith(v.var->type)||IsPtrType(v.var->type))) {
            if(TypeIsSigned(v.var->type))
                jit_ldr(Compiler.JIT, R(r), MoveGlobalPtrToReg(v.var,r),TypeSize(v.var->type));
            else
                jit_ldr_u(Compiler.JIT, R(r), MoveGlobalPtrToReg(v.var,r),TypeSize(v.var->type));
        } else if(v.var->isNoreg&&(IsArith(v.var->type)||IsPtrType(v.var->type))) {
            if(TypeIsSigned(v.var->type))
                jit_ldxi(Compiler.JIT, R(r), R_FP,Compiler.frameOffset+v.var->frameOffset,TypeSize(v.var->type));
            else
                jit_ldxi_u(Compiler.JIT, R(r), R_FP,Compiler.frameOffset+v.var->frameOffset,TypeSize(v.var->type));
        } else if(IsCompositeType(v.var->type)&&v.var->isGlobal) {
            MoveGlobalPtrToReg(v.var, 1);
            GrabXBytes(r, 1, TypeSize(v.var->type));
        } else if(IsCompositeType(v.var->type)&&v.var->isNoreg) {
            //Just get the first xbytes
            jit_addi(Compiler.JIT, R(1), R_FP,v.var->frameOffset+Compiler.frameOffset);
            GrabXBytes(r, 1, TypeSize(v.var->type));
        } else {
            return R(v.var->reg);
        }
        return R(r);
    }
    case VALUE_STRING: {
        char *str;
        jit_movi(Compiler.JIT, R(r), (jit_value)RegisterString(v.string));
        return R(r);
    }
    case VALUE_INDIR_VAR: {
        CValue indir AF_VALUE=VALUE_VAR(v.var);
        jit_value value=MoveValueToIntRegIfNeeded(indir,r);
        CType *deref =DerrefedType(v.var->type);
        //Arrays are flat
        if(deref->type!=TYPE_ARRAY&&deref->type!=TYPE_ARRAY_REF) {
            if(TypeIsSigned(deref))
                jit_ldr(Compiler.JIT, R(r), value, TypeSize(deref));
            else
                jit_ldr_u(Compiler.JIT, R(r), value, TypeSize(deref));
            return R(r);
        } else {
            return value;
        }
    }
    default:
        abort();
        return 0;
    }
}
static jit_value MoveValueToFltRegIfNeeded(CValue v,int r) {
    switch(v.type) {
    case VALUE_FLOAT: {
        jit_fmovi(Compiler.JIT, FR(r), v.flt);
        return FR(r);
        break;
    }
    case VALUE_INT: {
        jit_fmovi(Compiler.JIT, FR(r), v.integer);
        return FR(r);
        break;
    }
    case VALUE_VAR: {
        if(IsF64(v.var->type)) {
            if(v.var->isGlobal) {
                jit_fldr(Compiler.JIT, FR(r), MoveGlobalPtrToReg(v.var,0),TypeSize(v.var->type));
                return FR(r);
            } else if(v.var->isNoreg) {
                jit_fldxi(Compiler.JIT, FR(r), R_FP,Compiler.frameOffset+v.var->frameOffset,TypeSize(v.var->type));
                return FR(r);
            } else {
                return FR(v.var->reg);
            }
        } else {
            jit_extr(Compiler.JIT, FR(r), MoveValueToIntRegIfNeeded(v, 0));
        }
        return FR(r);
    }
    case VALUE_STRING: {
        char *str;
        jit_fmovr(Compiler.JIT, FR(r), (jit_value)RegisterString(v.string));
        return  FR(r);
    }
    case VALUE_INDIR_VAR: {
        CValue varv AF_VALUE=VALUE_VAR(v.var);
        jit_value value=MoveValueToIntRegIfNeeded(varv,1);
        jit_fldr(Compiler.JIT, FR(r), value,8);
        return FR(r);
    }
    default:
        abort();
        return 0;
    }
}

static int GetIntTmpReg() {
    int cur=0,iter;
    CValue v;
    vec_int_t salted;
    vec_init(&salted);
    cur=Compiler.tmpIntRegStart;
    while(--cur>=0)
        vec_push(&salted,cur);

    vec_foreach(&Compiler.valueStack, v, iter) {
loop:
        if(v.type==VALUE_VAR) {
            if(IsVarRegCanidate(v.var)&&!IsF64(v.var->type)) {
                vec_find(&salted, v.var->reg, cur);
                if(cur==-1)
                    vec_push(&salted,v.var->reg);
            }
            if(v.var->isReg&&IsCompositeType(v.var->type)) {
                vec_find(&salted, v.var->reg, cur);
                if(cur==-1)
                    vec_push(&salted,v.var->reg);
            }
        } else if(v.type==VALUE_INDIR_VAR) {
            v=VALUE_VAR(v.var);
            goto loop;
        }
    }
    cur=Compiler.tmpIntRegStart;
    for(;; cur++) {
        vec_find(&salted, cur, iter);
        if(iter==-1) return cur;
    }
}
static int GetFltTmpReg() {
    int cnt=0,iter;
    CValue v;
    vec_int_t salted;
    vec_init(&salted);
    int cur=Compiler.tmpFltRegStart;
    while(--cur>=0)
        vec_push(&salted,cur);

    vec_foreach(&Compiler.valueStack, v, iter) {
loop:
        if(v.type==VALUE_VAR) {
            if(IsVarRegCanidate(v.var)&&IsF64(v.var->type)) {
                vec_find(&salted, v.var->reg, cur);
                if(cur==-1)
                    vec_push(&salted,v.var->reg);
            }
        }
    }
    cur=Compiler.tmpFltRegStart;
    for(;; cur++) {
        vec_find(&salted, cur, iter);
        if(iter==-1) return cur;
    }
}
static uint64_t PowU64(uint64_t x,uint64_t n) {
    if(n==0) return 1;
    uint64_t y=1;
    while(n>1) {
        if(!(n%2)) {
            x=x*x;
            n=n/2;
        } else {
            y=x*y;
            x=x*x;
            n=(n-1)/2;
        }
    }
    return x*y;
}
static int64_t PowI64(int64_t x,int64_t n) {
    if(n<0) return 0;
    if(n==0) return 1;
    int64_t y=1;
    while(n>1) {
        if(!(n%2)) {
            x=x*x;
            n=n/2;
        } else {
            y=x*y;
            x=x*x;
            n=(n-1)/2;
        }
    }
    return x*y;
}
static AST *ValueToAST(CValue v) {
    switch(v.type) {
    case VALUE_FLOAT:
        return CreateF64(v.flt);
    case VALUE_INT:
        return CreateI64(v.integer);
    case VALUE_STRING:
        return CreateString(v.string);
    case VALUE_VAR:
        return CreateVarNode(v.var);
    case VALUE_INDIR_VAR: {
        AST *vn =CreateVarNode(v.var);
        return CreateUnop(vn, AST_DERREF);
    }
    default:
        abort();
    }
}
static double Bit4BitU64ToF64(uint64_t b) {
    return *(double*)&b;
}
static uint64_t Bit4BitF64ToU64(double b) {
    return *(uint64_t*)&b;
}
CValue CloneValue(CValue v) {
    switch(v.type) {
    case VALUE_INT:
    case VALUE_FLOAT:
        return v;
    case VALUE_STRING:
        return VALUE_STRING(v.string);
    case VALUE_VAR:
        return VALUE_VAR(v.var);
    case VALUE_INDIR_VAR:
        return (CValue) {
            .type=VALUE_INDIR_VAR,.var=CloneVariable(v.var)
        };
    default:
        abort();
    }
}
static void TypecastValFromStackIfNeeded(CType *fromtype, CType *totype) {
    CValue v AF_VALUE=vec_pop(&Compiler.valueStack);
    if(IsIntegerType(fromtype)&&IsF64(totype)) {
        CVariable *v2 AF_VAR=CreateTmpRegVar(GetFltTmpReg(), totype);
        CValue ret=VALUE_VAR(v2);
        CompileAssign(ret, v);
        vec_push(&Compiler.valueStack,ret);
        return;
    } else if(IsF64(fromtype)&&IsIntegerType(totype)) {
        CVariable *v2 AF_VAR=CreateTmpRegVar(GetIntTmpReg(), totype);
        CValue ret=VALUE_VAR(v2);
        CompileAssign(ret, v);
        vec_push(&Compiler.valueStack,ret);
        return;
    }
    vec_push(&Compiler.valueStack, CloneValue(v));
}
void __CompileAST(AST *exp);
/**
 * subswitv=0
 * ...
 * case: subswitv=2
 * ...
 * start:
 * subswitv=2
 * scodelab:
 * if(subswitv==0) {subswitv=1,goto start}
 * case : if(subswitv==0) goto scodelab;
 *
 */
static void __CompileSubswitBody(struct jit_label *start,CValue *subswitv,AST *subswit,vec_AST_t *cases,int64_t low,struct jit_op **table,vec_jit_op_t dfts) {
    vec_jit_op_t oldbreaks=Compiler.breakops;
    vec_init(&Compiler.breakops);
    //
    CValue two AF_VALUE=VALUE_INT(2);
    CompileAssign(*subswitv,two);
    jit_label *scodelab=jit_get_label(Compiler.JIT);
    CompileBodyWithBreakpoints(subswit->subswitch.startCode);
    ReleaseValue(&vec_pop(&Compiler.valueStack));
    //
    jit_value r=MoveValueToIntRegIfNeeded(*subswitv, 0);
    struct jit_op *elbranch=jit_bnei(Compiler.JIT, (jit_value)NULL, r, 0);
    CValue one AF_VALUE=VALUE_INT(1);
    CompileAssign(*subswitv,one);
    jit_jmpi(Compiler.JIT, (jit_value)start);
    jit_patch(Compiler.JIT, (jit_value)elbranch);
    //
    AST *node;
    int64_t iter;
    AST *body =NULL;
    if(subswit->subswitch.body->type!=AST_STMT_GROUP)
        subswit->subswitch.body=AppendToStmts(NULL, subswit->subswitch.body);
    vec_foreach(&subswit->subswitch.body->stmts, node, iter) {
        if(node->type==AST_CASE) {
            int iter;
            vec_find(cases,node,iter);
            assert(iter!=-1);
            AST *find=cases->data[iter];
            int64_t lo=find->cs.low-low;
            int64_t hi=find->cs.high-low;
            jit_label *lab=jit_get_label(Compiler.JIT);
            jit_patch(Compiler.JIT,table[lo]);
            //
            jit_value r=MoveValueToIntRegIfNeeded(*subswitv, 0);
            jit_beqi(Compiler.JIT,(jit_value)scodelab,r,0);
        } else if(node->type==AST_DEFAULT) {
            int iter;
            struct jit_op *cs;
            struct jit_label *at=jit_get_label(Compiler.JIT);
            vec_foreach(&dfts, cs, iter)
            jit_patch(Compiler.JIT, at);
            jit_value r=MoveValueToIntRegIfNeeded(*subswitv, 0);
            jit_beqi(Compiler.JIT,(jit_value)scodelab,r,0);
        } else if(node->type==AST_SUBSWITCH) {
            __CompileSubswitBody(start,subswitv,subswit->subswitch.body, cases, low, table, dfts);
        } else {
            CompileBodyWithBreakpoints(node);
            ReleaseValue(&vec_pop(&Compiler.valueStack));
        }
    }
    struct jit_op *br;
    vec_foreach(&Compiler.breakops,br,iter)
    jit_patch(Compiler.JIT,br);
    vec_deinit(&Compiler.breakops);
    Compiler.breakops=oldbreaks;
}
static void __CompileSwitBody(struct jit_label *start,CValue *subswitv,AST *_body, vec_AST_t *cases,int64_t low,struct jit_op **table,vec_jit_op_t dfts) {
    AST *node;
    int64_t iter;
    AST *body ;
    if(_body->type!=AST_STMT_GROUP)
        body=AppendToStmts(NULL,_body);
    else
        body=_body;
    vec_foreach(&body->stmts, node, iter) {
        if(node->type==AST_CASE) {
            int iter;
            vec_find(cases,node,iter);
            assert(iter!=-1);
            AST *find=cases->data[iter];
            int64_t lo=find->cs.low-low;
            int64_t hi=find->cs.high-low;
            jit_label *lab=jit_get_label(Compiler.JIT);
            jit_patch(Compiler.JIT,table[lo]);
        } else if(node->type==AST_DEFAULT) {
            int iter;
            struct jit_op *cs;
            vec_foreach(&dfts, cs, iter)
            jit_patch(Compiler.JIT, cs);
            dfts.length=0;
        } else if(node->type==AST_SUBSWITCH) {
            __CompileSubswitBody(start,subswitv,node, cases, low, table, dfts);
        } else {
            if(!IsControlNode(node))
                CompileBreakpoint(node);
            __CompileAST(node);
            ReleaseValue(&vec_pop(&Compiler.valueStack));
        }
    }
    struct jit_op *cs;
    vec_find(&dfts,NULL,iter);
    if(iter==-1)
        vec_foreach(&dfts, cs, iter)
        jit_patch(Compiler.JIT,  cs);
}
void __CompileCompoundLiteral(CType *type,int ptrreg,AST *comp,int adjust_off) {
    if(!comp) return;
    if(comp->type!=AST_ARRAY_LITERAL) {
        CType *ptrt =CreatePtrType(type);
        CVariable *ptr AF_VAR=CreateTmpRegVar(ptrreg, ptrt);
        CValue indir AF_VALUE=VALUE_INDIR_VAR(ptr);
        __CompileAST(comp);
        CValue src AF_VALUE=vec_pop(&Compiler.valueStack);
        CompileAssign(indir, src);
        if(adjust_off)
          jit_addi(Compiler.JIT,R(ptrreg),R(ptrreg),TypeSize(type));
        return;
    }
    CType *bt =BaseType(type);
    CType *pt =CreatePtrType(type);
    if(bt->type==TYPE_ARRAY) {
        if(bt->array.dim!=-1) {
            if(bt->array.dim<comp->arrayLiteral.length) {
                RaiseWarning(comp, "Array literal dim(%ld) excedes array dim(%ld).",bt->array.dim,comp->arrayLiteral.length);
            }
            int min=(bt->array.dim<comp->arrayLiteral.length)?bt->array.dim:comp->arrayLiteral.length;
            int64_t iter=0;
            CType *t =DerrefedType(type);
            for(; iter!=min; iter++) {
                __CompileCompoundLiteral(t,ptrreg,comp->arrayLiteral.data[iter],1);
            }
            if(bt->array.dim-min)
                jit_addi(Compiler.JIT,R(ptrreg),R(ptrreg),TypeSize(t)*(bt->array.dim-min));
        }
    } else if(bt->type==TYPE_CLASS) {
        CMember mem;
        int iter;
        int curoff=0;
        vec_foreach(&bt->cls.members, mem, iter) {
            if(iter>=comp->arrayLiteral.length) break;
            jit_addi(Compiler.JIT,R(ptrreg),R(ptrreg),mem.offset-curoff);
            __CompileCompoundLiteral(mem.type,ptrreg,comp->arrayLiteral.data[iter],0);
            curoff=mem.offset;
        }
        if(TypeSize(bt)-curoff)
          jit_addi(Compiler.JIT,R(ptrreg),R(ptrreg),TypeSize(bt)-curoff);
    } else {
    }
}
void CompileCompoundLiteral(CVariable *v,CDeclTail decl) {
    assert(decl.dft->type==AST_ARRAY_LITERAL);
    CType *ptype =NULL;
    if(decl.finalType->type==TYPE_ARRAY) {
        ptype=CreatePtrType(decl.finalType->array.base);
    } else {
        ptype=CreatePtrType(decl.finalType);
    }
    int reg;
    CVariable *ptrvar AF_VAR=CreateTmpRegVar(reg=GetIntTmpReg(), ptype);
    CValue ptrv AF_VALUE=VALUE_VAR(ptrvar);
    if(v->isGlobal) {
        MoveGlobalPtrToReg(v,ptrvar->reg);
    } else if(v->isNoreg) {
        jit_addi(Compiler.JIT,R(ptrvar->reg), R_FP,Compiler.frameOffset+v->frameOffset);
    } else {
        fprintf(stderr,"Compound literals act on noreg variables(INTERNAL ERROR).\n");
        abort();
    }
    vec_push(&Compiler.valueStack,ptrv);
    __CompileCompoundLiteral(decl.finalType, reg, decl.dft,1);
    vec_pop(&Compiler.valueStack);
}
int TypeEqual(CType *_a,CType *_b) {
    CType *a =BaseType(_a);
    CType *b =BaseType(_b);
    if(a->type!=b->type) return 0;
    switch(a->type) {
    case TYPE_ARRAY_REF:
    case TYPE_ARRAY:
        return TypeEqual(a->array.base, b->array.base)&&a->array.dim==b->array.dim;
    case TYPE_BOOL:
        return 1;
    case TYPE_CLASS:
        return a==b;
    case TYPE_F64:
        return 1;
    case TYPE_FUNC: {
        if(a->func.arguments.length!=b->func.arguments.length) return 0;
        int iter;
        CType *at,*bt;
        vec_foreach(&a->func.arguments, at, iter) {
            if(!TypeEqual(at, b->func.arguments.data[iter])) return 0;
        }
        return 1;
    }
    case TYPE_I16:
        return 1;
    case TYPE_I32:
        return 1;
    case TYPE_I64:
        return 1;
    case TYPE_I8:
        return 1;
    case TYPE_PTR:
        return TypeEqual(a->ptr, b->ptr);
    case TYPE_U0:
        return 1;
    case TYPE_U16:
        return 1;
    case TYPE_U32:
        return 1;
    case TYPE_U64:
        return 1;
    case TYPE_U8:
        return 1;
    case TYPE_UNION:
        return a==b;
    default:
        abort();
    }
}
static CType *Super(CType *t) {
  loop:
  if(t->type==TYPE_CLASS) {
    if(t->cls.inheritsFrom) {
      t=t->cls.inheritsFrom;
      goto loop;
    }
  } else if(t->type==TYPE_UNION) {
    if(t->un.inheritsFrom) {
      t=t->un.inheritsFrom;
      goto loop;
    }
  }
  return t;
}
int CompositeTypeIsCompatWith(AST *at,CType *to,CType *from) {
    if(TypeEqual(to, from)) return 1;
    CType *tmp=to;
    tmp=Super(tmp);
    from=Super(from);
loop:
if(TypeEqual(tmp,from)) return 1;
testbt:
    if(IsArith(tmp)||IsPtrType(tmp)) {
        if(IsArith(from)||IsPtrType(from)) {
            CType *from2=BaseType(from);
            CType *tmp2=BaseType(tmp);
            WarnOnWeirdPass(at,tmp2,from2);
            return 1;
        }
    }
incompat:
    ;
    char *ts=TypeToString(to),*fs=TypeToString(from);
    RaiseError(at,"Passing %s to incompatable type %s.",ts,fs);
    TD_FREE(ts),TD_FREE(fs);
    return 0;
}
void WarnOnWeirdPass(AST *at,CType *_to,CType *_from) {
    if(IsCompositeType(_to)||IsCompositeType(_from)) {
        CompositeTypeIsCompatWith(at, _to, _from);
        return;
    }
    CType *to =BaseType(_to);
    CType *from =BaseType(_from);
    if(IsPtrType(to)&&!IsPtrType(from)) {
        char *ts=TypeToString(to);
        char *fs=TypeToString(from);
        RaiseWarning(at, "Passing non-pointer %s to pointer %s.",fs,ts);
        TD_FREE(ts),TD_FREE(fs);
    } else if(!IsPtrType(to)&&IsPtrType(from)) {
        char *ts=TypeToString(to);
        char *fs=TypeToString(from);
        RaiseWarning(at, "Passing pointer %s to non-pointer %s.",fs,ts);
        TD_FREE(ts),TD_FREE(fs);
    } else if(IsPtrType(to)&&IsPtrType(from)) {
        CType *_td =DerrefedType(to);
        CType *_fd =DerrefedType(from);
        CType *td =BaseType(_td);
        CType *fd =BaseType(_fd);
        if(td->type!=fd->type) {
            //U0 ptrs can be passed around to any pointer type
            if(td->type==TYPE_U0||fd->type==TYPE_U0) return;

incompatptr:
            ;
            char *ts=TypeToString(to);
            char *fs=TypeToString(from);
            RaiseWarning(at, "Incompatable pointer pass!(%s and %s).",ts,fs);
            TD_FREE(ts),TD_FREE(fs);
        } else if(td->type==TYPE_FUNC) {
            //
        } else if(td->type==TYPE_CLASS||td->type==TYPE_UNION) {
            CompositeTypeIsCompatWith(at, td,fd);
        }
    }
}
static void ___CompileAST(AST *exp) {
    switch (exp->type) {
    case AST_ASM_BLK: {
      Assembler.active=1;
      AST *s;int iter;
      vec_foreach(&exp->stmts,s,iter) {
	__CompileAST(s);
	ReleaseValue(&vec_pop(&Compiler.valueStack));
      }
      vec_push(&Compiler.valueStack,VALUE_INT(0));
      Assembler.active=0;
      break;
    }
    case AST_ASM_OPCODE: {
      AssembleOpcode(exp,exp->asmOpcode.name->name,exp->asmOpcode.operands);
      vec_push(&Compiler.valueStack,VALUE_INT(0));
      break;
    }
    case AST_ASM_ALIGN: {
      int64_t al=EvaluateInt(exp->asmAlign.align,0);
      int64_t f=EvaluateInt(exp->asmAlign.align,0);
        jit_align_fill(Compiler.JIT,al,f);
        vec_push(&Compiler.valueStack,VALUE_INT(0));
        break;
    }
    case AST_ASM_BINFILE: {
        assert(exp->asmBinfile->type==AST_STRING);
        FILE *f=fopen(exp->asmBinfile->string,"rb");
        if(f) {
	  fseek(f,0,SEEK_END);
            long end=ftell(f);
            fseek(f,0,SEEK_SET);
            long start=ftell(f);
            char buffer[end-start];
            fread(buffer,end-start,1,f);
            fclose(f);
            jit_data_bytes(Compiler.JIT,end-start,buffer);
        } else {
            RaiseError(exp->asmBinfile,"Can't open file \"%s\" for reading.",exp->asmBinfile->string);
        }
        vec_push(&Compiler.valueStack,VALUE_INT(0));
        break;
    }
    case AST_ASM_DU16: {
        AST *d;int iter;
        vec_foreach(&exp->duData,d,iter) {
	  int64_t v=(int64_t)EvalLabelExpr(d);
	  jit_data_word(Compiler.JIT,v);
        }
        vec_push(&Compiler.valueStack,VALUE_INT(0));
        break;
    }
    case AST_ASM_DU32: {
        AST *d;int iter;
        vec_foreach(&exp->duData,d,iter) {
	  int64_t v=(int64_t)EvalLabelExpr(d);
	  jit_data_dword(Compiler.JIT,v);
        }
        vec_push(&Compiler.valueStack,VALUE_INT(0));
        break;
    }
    case AST_ASM_DU64: {
        AST *d;int iter;
        vec_foreach(&exp->duData,d,iter) {
	  int64_t v=(int64_t)EvalLabelExpr(d);
	  jit_data_qword(Compiler.JIT,v);
        }
        vec_push(&Compiler.valueStack,VALUE_INT(0));
        break;
    }
    case AST_ASM_DU8: {
      AST *d;int iter;
      vec_foreach(&exp->duData, d, iter) {
	int64_t v=(int64_t)EvalLabelExpr(d);
	jit_data_qword(Compiler.JIT,v);
      }
      vec_push(&Compiler.valueStack,VALUE_INT(0));
      break;
    }
    case AST_ASM_IMPORT: {
        AST *name;
        int iter;
        vec_foreach(&exp->asmImports,name,iter) {
            CVariable **var=map_get(&Compiler.globals,name->name);
            if(var) {
                map_set(&Assembler.imports,name->name,*var);
            } else {
	      RaiseError(name,"Couldn't import global symbol %s.",name->name);
            }
        }
        vec_push(&Compiler.valueStack,VALUE_INT(0));
        break;
    }
    case AST_LASTCLASS: {
        if(!Compiler.lastclass) {
            RaiseError(exp,"Unexpected lastclass.");
            vec_push(&Compiler.valueStack,VALUE_INT(0));
            break;
        }
        break;
    }
    case AST_SIZEOF: {
        CType *type =NULL;
        if(exp->sizeofExp->type==AST_TYPE) {
            type=exp->sizeofExp->type2;
        } else {
            type=AssignTypeToNode(exp->sizeofExp);
        }
        vec_push(&Compiler.valueStack, VALUE_INT(TypeSize(type)));
        break;
    }
    case AST_TRY: {
        CompileTry(exp);
        //Do nothing,but we still put a value on the stack
        vec_push(&Compiler.valueStack, VALUE_INT(0));
        break;
    }
    case AST_TYPE:
        //Do nothing,but we still put a value on the stack
        vec_push(&Compiler.valueStack, VALUE_INT(0));
        break;
    case AST_NAME: {
        CVariable *var;
	//If assembler is active,check for import
	if(Assembler.active) {
	  CVariable **var;
	  if(var=map_get(&Assembler.imports,exp->name)) {
	    vec_push(&Compiler.valueStack,VALUE_INT((int64_t)GetGlobalPtr(*var)));
	    break;
	  }
	}
        if(var=GetVariable(exp->name)) {
            //Account for implicit func calls
            CType *fbt =BaseType(var->type);
            CType *fptr =CreatePtrType(fbt);
            if(var->isFunc) {
                int origstacksz=Compiler.valueStack.length;
                int reg;
                MoveGlobalPtrToReg(var, reg=GetIntTmpReg());
                CVariable *fvar AF_VAR=CreateTmpRegVar(reg, fptr);
                vec_push(&Compiler.valueStack, VALUE_VAR(fvar));
                int fstkpos=Compiler.valueStack.length;
#define POP_STK_TO_ORIG() \
while(Compiler.valueStack.length!=origstacksz) ReleaseValue(&vec_pop(&Compiler.valueStack));
                AST *v;
                int iter;
                vec_foreach(&fbt->func.dftArgs, v, iter) {
                    if(!v) {
                        RaiseError(exp,"Missing defualt argument %ld for implicit function call.",iter);
                        POP_STK_TO_ORIG();
                        vec_push(&Compiler.valueStack, VALUE_INT(0));
                        return;
                    } else if(v->type==AST_LASTCLASS) {
                        if(iter==0) {
                            RaiseError(exp,"lastclass cannot be used as first argument.");
                            vec_push(&Compiler.valueStack, VALUE_INT(0));
                        } else {
                            CType *lastclass=fbt->func.arguments.data[iter-1];
lcloop:
                            switch(lastclass->type) {
                            case TYPE_PTR:
                                lastclass=lastclass->ptr;
                                goto lcloop;
                            case TYPE_ARRAY_REF:
                            case TYPE_ARRAY:
                                lastclass=lastclass->array.base;
                                goto lcloop;
                            }
                            char *ts=TypeToString(lastclass);
                            int r=(IsF64(fbt->func.arguments.data[iter]))?GetFltTmpReg():GetIntTmpReg();
                            CVariable *var AF_VAR=CreateTmpRegVar(r,fbt->func.arguments.data[iter]);
                            CValue val =VALUE_VAR(var);
                            if(IsF64(fbt->func.arguments.data[iter]))
                                MoveValueToFltRegIfNeeded(VALUE_STRING(ts), r);
                            else
                                MoveValueToIntRegIfNeeded(VALUE_STRING(ts), r);
                            vec_push(&Compiler.valueStack, val);
                        }
                    } else {
                        int r=(IsF64(fbt->func.arguments.data[iter]))?GetFltTmpReg():GetIntTmpReg();
                        CVariable *var AF_VAR=CreateTmpRegVar(r,fbt->func.arguments.data[iter]);
                        CValue val=VALUE_VAR(var);
                        __CompileAST(v);
                        CValue src AF_VALUE=vec_pop(&Compiler.valueStack);
                        CompileAssign(val, src);
                        vec_push(&Compiler.valueStack,val);
                    }
                }
                jit_prepare(Compiler.JIT);
                for(iter=fstkpos; iter!=Compiler.valueStack.length; iter++) {
                    int r=Compiler.valueStack.data[iter].var->reg;
                    if(IsF64(fbt->func.arguments.data[iter-fstkpos])) {
                        jit_fputargr(Compiler.JIT, FR(r),8);
                    } else
                        jit_putargr(Compiler.JIT, R(r));
                    ReleaseValue(&Compiler.valueStack.data[iter]);
                }
                vec_truncate(&Compiler.valueStack, fstkpos);
                //Was stuffed in register eariler.
                CValue func AF_VALUE=vec_pop(&Compiler.valueStack);
                jit_callr(Compiler.JIT, R(reg));
                int r;
                if(IsF64(fbt->func.ret)) {
                    jit_fretval(Compiler.JIT, FR(r=GetFltTmpReg()),8);
                } else {
                    jit_retval(Compiler.JIT, R(r=GetIntTmpReg()));
                }
                //
                CVariable *rvar AF_VAR=CreateTmpRegVar(r, fbt->func.ret);
                vec_push(&Compiler.valueStack, VALUE_VAR(rvar));
                return;
            }
            vec_push(&Compiler.valueStack, VALUE_VAR(var));
        } else {
            RaiseError(exp,"Unknown name %s.",exp->name);
            vec_push(&Compiler.valueStack, VALUE_INT(0));
        }
        break;
    }
    case AST_ASM_REG: {
      vec_push(&Compiler.valueStack, VALUE_INT(0));
      break;
    }
    case AST_PRINT_STRING: {
        jit_prepare(Compiler.JIT);
        jit_putargi(Compiler.JIT, (jit_value)RegisterString(exp->string));
        jit_putargi(Compiler.JIT, 0);
        jit_putargi(Compiler.JIT, 0);
        jit_call(Compiler.JIT,TOSPrint);
        vec_push(&Compiler.valueStack, VALUE_INT(0));
        break;
    }
    case __AST_DECL_TAILS: {
        int iter;
        CDeclTail decl;
        vec_foreach(&exp->declTail,decl,iter) {
            CVariable *v=GetVariable(decl.name->name);
            if(decl.dft) {
                if(decl.dft->type==AST_ARRAY_LITERAL) {
                    CompileCompoundLiteral(v,decl);
                } else {
                    __CompileAST(decl.dft);
                    CType *f =AssignTypeToNode(decl.dft);
                    WarnOnWeirdPass(decl.dft,decl.finalType, f);
                    CompileAssign(VALUE_VAR(v), vec_pop(&Compiler.valueStack));
                }
            }
        }
        vec_push(&Compiler.valueStack,VALUE_INT(0));
        break;
    }
    case AST_NOP:
        vec_push(&Compiler.valueStack,VALUE_INT(0));
        break;
    case AST_STMT_GROUP: {
        int iter;
        AST *stmt;
        vec_foreach(&exp->stmts, stmt, iter) {
            //Control nodes provide own breakpoints
            if(!IsControlNode(stmt))
                CompileBreakpoint(stmt);
            __CompileAST(stmt);
            ReleaseValue(&vec_pop(&Compiler.valueStack));
        }
        //All operations push a value to the stack
        vec_push(&Compiler.valueStack,VALUE_INT(0));
        break;
    }
    case AST_MEMBER_ACCESS: {
      if(Compiler.addrofFrameoffsetMode) {
	//Get class containing anaoymus member
	vec_str_t members;
	vec_init(&members);
	AST *top=exp;
	while(top->type==AST_MEMBER_ACCESS) {
	  vec_push(&members, top->memberAccess.member);
	  top=top->memberAccess.a;
	}
	if(top->type==AST_NAME) {
	  CType **t;
	  if(t=map_get(&Compiler.types, top->name)) {
	    int64_t offset=0;
	    CType *T=ResolveType(*t);
	    int iter;
	    char *name;
	    vec_foreach_rev(&members, name, iter) {
	      CMember *mem=NULL;
	      if(T->type==TYPE_CLASS){
		int iter;
		CMember *mem2;
		vec_foreach_ptr(&T->cls.members, mem2, iter) {
		  if(!strcmp(mem2->name,name)) {
		    mem=mem2;
		    break;
		  }
		}
		if(!mem) goto mnf;
		offset+=mem->offset;
		T=mem->type;
	      } else if(T->type==TYPE_UNION) {
		int iter;
		CMember *mem2;
		vec_foreach_ptr(&T->un.members, mem2, iter) {
		  if(!strcmp(mem2->name,name)) {
		    mem=mem2;
		    break;
		  }
		}
		if(!mem) goto mnf;
		offset+=mem->offset;
		T=mem->type;
	      } else {
		mnf:
		vec_deinit(&members);
		RaiseError(exp , "Couldn't find member %s.",name);
		vec_push(&Compiler.valueStack, VALUE_INT(0));
		return;
	      }
	    }
	    vec_deinit(&members);
	    vec_push(&Compiler.valueStack, VALUE_INT(offset));
	    return;
	  }
	}
	vec_deinit(&members);
	
      }
        AST *conv =ConvertMembersToPtrs(exp);
        __CompileAST(conv);
        return;
    }
    case AST_PAREN:
        return __CompileAST(exp->unopArg);
    case AST_INT: {
        CValue r=VALUE_INT(exp->integer);
        vec_push(&Compiler.valueStack,r);
        return;
    }
    case AST_FLOAT: {
        CValue r=VALUE_FLOAT(exp->floating);
        vec_push(&Compiler.valueStack,r);
        return;
    }
    case AST_CHAR: {
        CValue r=VALUE_INT(exp->chr);
        vec_push(&Compiler.valueStack,r);
        return;
    }
    case AST_STRING: {
        CValue r=VALUE_STRING(exp->string);
        vec_push(&Compiler.valueStack,r);
        return;
    }
    case AST_COMMA: {
        vec_AST_t vec=CommaToVec(exp);
        AST *expr;
        int iter;
        if(vec.data[0]->type==AST_STRING) {
            long off=jit_allocai(Compiler.JIT,(vec.length-1)*8);
            int origsz=Compiler.valueStack.length;
            for(iter=1; iter!=vec.length; iter++) {
                CType *target =AssignTypeToNode(vec.data[iter]),*totype =NULL;
                CType *bt =BaseType(target);
                if(bt->type==TYPE_ARRAY||bt->type==TYPE_ARRAY_REF) {
                    AST *ptr =PtrifyIfNeeded(vec.data[iter]);
                    __CompileAST(ptr);
                    target=AssignTypeToNode(ptr);
                } else
                    __CompileAST(vec.data[iter]);
                CValue v AF_VALUE=vec_pop(&Compiler.valueStack);
                if(IsF64(target))
                    totype=CreatePrimType(TYPE_F64);
                else if(IsIntegerType(target))
                    totype=CreatePrimType(TYPE_I64);
                else if(IsPtrType(target)) {
                    totype=target;
                } else if(IsCompositeType(target)) {
                    totype=CreatePrimType(TYPE_U64);
                    //In templeOS all arguments are 64bit,so put in a register
                    CVariable *var AF_VAR=CreateTmpRegVar(GetIntTmpReg(), target);
                    CValue varv =VALUE_VAR(var);
                    vec_push(&Compiler.valueStack, varv);
                    CompileAssign(varv, v);
                    vec_pop(&Compiler.valueStack);
                    ReleaseValue(&v);
                    v=VALUE_VAR(var);
                }
                {
                    CType *target2 ;
                    if(IsF64(target)) target2=CreatePrimType(TYPE_F64);
                    else target2=CreatePrimType(TYPE_I64);
                    CVariable *foff AF_VAR=CreateTmpVarAtFrameOff(off+(iter-1)*8, target2);
                    CValue foffv AF_VALUE=VALUE_VAR(foff);
                    CompileAssign(foffv,v);
                }
prneskip:
                ;
            }
            __CompileAST(vec.data[0]);
            CValue str AF_VALUE=vec_pop(&Compiler.valueStack);
            jit_value strval=MoveValueToIntRegIfNeeded(str,1);
            jit_addi(Compiler.JIT,R(0),R_FP,off);
            jit_prepare(Compiler.JIT);
            jit_putargr(Compiler.JIT,strval);
            jit_putargi(Compiler.JIT,vec.length-1);
            jit_putargr(Compiler.JIT,R(0));
            jit_call(Compiler.JIT,TOSPrint);
            //All operations are required to put an item on the stack
            vec_push(&Compiler.valueStack,VALUE_INT(0));
            return;
        } else {
            vec_foreach(&vec, expr, iter) {
                __CompileAST(expr);
                if(iter+1!=vec.length)
                    ReleaseValue(&vec_pop(&Compiler.valueStack));
            }
        }
        vec_deinit(&vec);
        return;
    }
    case AST_SHL: {
#define INT_OP(iop,rop,totype,operator) \
  __CompileAST(exp->binop.a); \
  CType *at =AssignTypeToNode(exp->binop.a); \
  TypecastValFromStackIfNeeded(at,totype); \
  __CompileAST(exp->binop.b); \
  CType *bt =AssignTypeToNode(exp->binop.b); \
  WarnOnWeirdPass(exp,at, bt); \
  TypecastValFromStackIfNeeded(bt,totype); \
  CValue b AF_VALUE=vec_pop(&Compiler.valueStack); \
  CValue a AF_VALUE=vec_pop(&Compiler.valueStack); \
  int rr=GetIntTmpReg(); \
  if(b.type==VALUE_INT) { \
if(a.type==VALUE_INT) { \
  if(IsU64(totype)) { \
vec_push(&Compiler.valueStack, VALUE_INT((uint64_t)a.integer operator (uint64_t)b.integer)); \
return ; \
  } else if(IsI64(totype)) { \
vec_push(&Compiler.valueStack, VALUE_INT((int64_t)a.integer operator (int64_t)b.integer)); \
return ; \
  } \
} \
jit_value av=MoveValueToIntRegIfNeeded(a,0); \
iop(Compiler.JIT, R(rr), av, b.integer); \
  } else { \
jit_value av=MoveValueToIntRegIfNeeded(a,0); \
jit_value bv=MoveValueToIntRegIfNeeded(b,1); \
rop(Compiler.JIT, R(rr), av, bv); \
  } \
  CType *rtype =AssignTypeToNode(exp); \
  vec_push(&Compiler.valueStack,VALUE_VAR(CreateTmpRegVar(rr, rtype)));
        CType *t =AssignTypeToNode(exp->binop.a);
        if(IsF64(t)) {
            CType *r=t;
#define FSH(func) \
__CompileAST(exp->binop.a); \
CType *at =AssignTypeToNode(exp->binop.a); \
TypecastValFromStackIfNeeded(at,r); \
__CompileAST(exp->binop.b); \
CType *bt =CreatePrimType(TYPE_I64); \
TypecastValFromStackIfNeeded(bt,r); \
WarnOnWeirdPass(exp,at,bt); \
CValue b AF_VALUE=vec_pop(&Compiler.valueStack); \
CValue a AF_VALUE=vec_pop(&Compiler.valueStack); \
jit_value f1=MoveValueToFltRegIfNeeded(a, 0); \
jit_value i1=MoveValueToIntRegIfNeeded(b, 1); \
jit_prepare(Compiler.JIT); \
jit_fputargr(Compiler.JIT, f1, 8); \
jit_putargr(Compiler.JIT, i1); \
jit_call(Compiler.JIT, func); \
CVariable *vr AF_VAR=CreateTmpRegVar(GetFltTmpReg(), r); \
jit_fretval(Compiler.JIT,FR(vr->reg),8); \
vec_push(&Compiler.valueStack,VALUE_VAR(vr));
            FSH(F64Shl);
        } else if(IsIntegerType(t)) {
            INT_OP(jit_lshi,jit_lshr,t,<<);
        }
        return;
    }
    case AST_SHR: {
        //TODO forbid F64
        CType *r =AssignTypeToNode(exp->binop.a);
        if(IsF64(r)) {
            FSH(F64Shr);
        } else if(TypeIsSigned(r)) {
            INT_OP(jit_rshi,jit_rshr,r,>>);
        } else {
            INT_OP(jit_rshi_u,jit_rshr_u,r,>>);
        }
        return;
    }
    case AST_POW: {
        CType *r =AssignTypeToNode(exp);
        __CompileAST(exp->binop.a);
        CType *at =AssignTypeToNode(exp->binop.a);
        TypecastValFromStackIfNeeded(at,r);
        __CompileAST(exp->binop.b);
        CType *bt =AssignTypeToNode(exp->binop.b);
        TypecastValFromStackIfNeeded(bt,r);
        CValue b AF_VALUE=vec_pop(&Compiler.valueStack);
        CValue a AF_VALUE=vec_pop(&Compiler.valueStack);
        if(IsF64(r)) {
            jit_value a2=MoveValueToFltRegIfNeeded(a, 0);
            jit_value b2=MoveValueToFltRegIfNeeded(b, 1);
            jit_prepare(Compiler.JIT);
            jit_fputargr(Compiler.JIT,a2,8);
            jit_fputargr(Compiler.JIT,b2,8);
            jit_call(Compiler.JIT, &pow);
            int t;
            jit_value r=FR(t=GetFltTmpReg());
            jit_fretval(Compiler.JIT, r, 8);
            vec_push(&Compiler.valueStack,VALUE_VAR(CreateTmpRegVar(t, CreatePrimType(TYPE_F64))));
            return;
        } else {
            jit_value a2=MoveValueToIntRegIfNeeded(a, 0);
            jit_value b2=MoveValueToIntRegIfNeeded(b, 1);
            jit_prepare(Compiler.JIT);
            jit_putargr(Compiler.JIT,a2);
            jit_putargr(Compiler.JIT,b2);
            if(TypeIsSigned(r))
                jit_call(Compiler.JIT, &PowI64);
            else
                jit_call(Compiler.JIT, &PowU64);
            int t;
            jit_value r=R(t=GetIntTmpReg());
            jit_retval(Compiler.JIT, r);
            vec_push(&Compiler.valueStack,VALUE_VAR(CreateTmpRegVar(t, CreatePrimType(TYPE_I64))));
            return ;
        }
        return;
    }
    case AST_MUL: {
        CType *r =AssignTypeToNode(exp);
        if(IsF64(r)) {
#define FLOAT_OP(iop,rop,totype, operator ) \
__CompileAST(exp->binop.a); \
CType *at =AssignTypeToNode(exp->binop.a); \
TypecastValFromStackIfNeeded(at,totype); \
__CompileAST(exp->binop.b); \
CType *bt =AssignTypeToNode(exp->binop.b); \
WarnOnWeirdPass(exp, at, bt); \
TypecastValFromStackIfNeeded(bt,totype); \
CValue b AF_VALUE=vec_pop(&Compiler.valueStack); \
CValue a AF_VALUE=vec_pop(&Compiler.valueStack); \
int rr=GetFltTmpReg(); \
if(b.type==VALUE_INT) { \
  jit_value av=MoveValueToFltRegIfNeeded(a,0); \
  iop(Compiler.JIT, FR(rr), av, b.integer); \
} else if(b.type==VALUE_FLOAT) { \
  if(a.type==VALUE_FLOAT) { \
vec_push(&Compiler.valueStack,VALUE_FLOAT(a.flt operator b.flt)); \
return; \
  } \
  jit_value av=MoveValueToFltRegIfNeeded(a,0); \
  iop(Compiler.JIT, FR(rr), av, b.flt); \
} else { \
  jit_value av=MoveValueToFltRegIfNeeded(a,0); \
  jit_value bv=MoveValueToFltRegIfNeeded(b,1); \
  rop(Compiler.JIT, FR(rr), av, bv); \
} \
CType *rtype =AssignTypeToNode(exp); \
vec_push(&Compiler.valueStack,VALUE_VAR(CreateTmpRegVar(rr, rtype)));
            FLOAT_OP(jit_fmuli,jit_fmulr,r, * );
        } else if(TypeIsSigned(r)) {
            INT_OP(jit_muli,jit_mulr,r,*);
        } else {
            INT_OP(jit_muli_u,jit_mulr_u,r,*);
        }
        return;
    }
    case AST_DIV: {
        CType *r =AssignTypeToNode(exp);
        if(IsF64(r)) {
            FLOAT_OP(jit_fdivi,jit_fdivr,r, / );
        } else if(TypeIsSigned(r)) {
            INT_OP(jit_divi,jit_divr,r, / );
        } else {
            INT_OP(jit_divi_u,jit_divr_u,r, / );
        }
        return;
    }
    case AST_MOD: {
        CType *r =AssignTypeToNode(exp);
        if(IsF64(r)) {
            CType *r =AssignTypeToNode(exp);
            __CompileAST(exp->binop.a);
            CType *at =AssignTypeToNode(exp->binop.a);
            TypecastValFromStackIfNeeded(at,r);
            __CompileAST(exp->binop.b);
            CType *bt =AssignTypeToNode(exp->binop.b);
            TypecastValFromStackIfNeeded(bt,r);
            CValue b AF_VALUE=vec_pop(&Compiler.valueStack);
            CValue a AF_VALUE=vec_pop(&Compiler.valueStack);
            jit_value a2 =MoveValueToFltRegIfNeeded(a, 0);
            jit_value b2 =MoveValueToFltRegIfNeeded(b, 1);

            jit_prepare(Compiler.JIT);
            jit_fputargr(Compiler.JIT,a2,8);
            jit_fputargr(Compiler.JIT,b2,8);
            jit_call(Compiler.JIT, &fmod);
            int t;
            jit_value fr=FR(t=GetFltTmpReg());
            jit_fretval(Compiler.JIT, fr, 8);
            vec_push(&Compiler.valueStack,VALUE_VAR(CreateTmpRegVar(t, CreatePrimType(TYPE_F64))));
        } else if(TypeIsSigned(r)) {
            INT_OP(jit_modi,jit_modr,r, % );
        } else {
            INT_OP(jit_modi_u,jit_modr_u,r, % );
        }
        return;
    }
    case AST_BAND: {
        CType *r =AssignTypeToNode(exp);
        if(IsF64(r)) {
#define FBITWISE(func) \
__CompileAST(exp->binop.a); \
CType *at =AssignTypeToNode(exp->binop.a); \
TypecastValFromStackIfNeeded(at,r); \
__CompileAST(exp->binop.b); \
CType *bt =AssignTypeToNode(exp->binop.b); \
TypecastValFromStackIfNeeded(bt,r); \
if(!TypeEqual(at, bt)) RaiseError(exp,"Type-mismatch for F64 bitwise operator."); \
CValue b AF_VALUE=vec_pop(&Compiler.valueStack); \
CValue a AF_VALUE=vec_pop(&Compiler.valueStack); \
jit_value f1=MoveValueToFltRegIfNeeded(a, 0); \
jit_value f2=MoveValueToFltRegIfNeeded(b, 1); \
CVariable *rv =CreateTmpRegVar(GetFltTmpReg(), r); \
jit_prepare(Compiler.JIT); \
jit_fputargr(Compiler.JIT, f1, 8); \
jit_fputargr(Compiler.JIT, f2, 8); \
jit_call(Compiler.JIT, func); \
jit_fretval(Compiler.JIT,FR(rv->reg),8); \
vec_push(&Compiler.valueStack,VALUE_VAR(rv));
            FBITWISE(F64And);
        } else {
            INT_OP(jit_andi,jit_andr,r, & );
        }
        return;
    }
    case AST_BXOR: {
        CType *r =AssignTypeToNode(exp);
        if(IsF64(r)) {
            FBITWISE(F64Xor);
        } else {
            INT_OP(jit_xori,jit_xorr,r, ^ );
        }
        return;
    }
    case AST_BOR: {
        CType *r =AssignTypeToNode(exp);
        if(IsF64(r)) {
            FBITWISE(F64Or);
        } else {
            INT_OP(jit_ori,jit_orr,r, | );
        }
        return;
    }
    case AST_ADD: {
        CType *r =AssignTypeToNode(exp);
        if(IsPtrType(r)) {
            CType *at =AssignTypeToNode(exp->binop.a);
            CType *bt =AssignTypeToNode(exp->binop.b);
            CType *ptr=(IsPtrType(at)>IsPtrType(bt))?at:bt;
            CType *deref =DerrefedType(ptr);
            long mul=TypeSize(deref);
            AST *ptrv ,*offv ;
            if(IsPtrType(at)) {
                ptrv=PtrifyIfNeeded(exp->binop.a);
                offv=exp->binop.b;
            } else {
                offv=exp->binop.a;
                ptrv=PtrifyIfNeeded(exp->binop.b);
            }
            __CompileAST(ptrv);
            __CompileAST(offv);
            CValue o AF_VALUE=vec_pop(&Compiler.valueStack);
            jit_value ov=MoveValueToIntRegIfNeeded(o,1);
            int rr=GetIntTmpReg();
            CVariable *rv AF_VAR=CreateTmpRegVar(rr, r);
            jit_muli(Compiler.JIT, R(rr),ov, mul);
            CValue p AF_VALUE=vec_pop(&Compiler.valueStack);
            jit_value pv=MoveValueToIntRegIfNeeded(p,0);
            jit_addr(Compiler.JIT, R(rr), R(rr), pv);
            vec_push(&Compiler.valueStack,VALUE_VAR(rv));
        } else if(IsF64(r)) {
            FLOAT_OP(jit_faddi, jit_faddr,r, +);
        } else {
            INT_OP(jit_addi,jit_addr,r, + );
        }
        return;
    }
    case AST_SUB: {
        CType *r =AssignTypeToNode(exp);
        CType *at =AssignTypeToNode(exp->binop.a);
        CType *bt =AssignTypeToNode(exp->binop.b);
        if(IsPtrType(at)&&IsPtrType(bt)) {
            CType *deref =DerrefedType(at);
            long div=TypeSize(deref);
            __CompileAST(exp->binop.b);
            __CompileAST(exp->binop.a);
            int rr=GetIntTmpReg();
            CValue av AF_VALUE=vec_pop(&Compiler.valueStack);
            CValue bv AF_VALUE=vec_pop(&Compiler.valueStack);
            jit_value ar=MoveValueToIntRegIfNeeded(av, 0);
            jit_value br=MoveValueToIntRegIfNeeded(bv, 1);
            CVariable *rv AF_VAR=CreateTmpRegVar(rr, r);
            jit_subr(Compiler.JIT,R(rr),ar,br);
            jit_divi(Compiler.JIT, R(rr), R(rr), div);
            vec_push(&Compiler.valueStack,VALUE_VAR(rv));
        } else if(IsPtrType(at)) {
            CType *deref =DerrefedType(at);
            long mul=TypeSize(deref);
            __CompileAST(exp->binop.b);
            __CompileAST(exp->binop.a);
            int rr=GetIntTmpReg();
            CValue av AF_VALUE=vec_pop(&Compiler.valueStack);
            CValue bv AF_VALUE=vec_pop(&Compiler.valueStack);
            jit_value ar=MoveValueToIntRegIfNeeded(av, 0);
            jit_value br=MoveValueToIntRegIfNeeded(bv, 1);
            CVariable *rv AF_VAR=CreateTmpRegVar(rr, r);
            jit_muli(Compiler.JIT, R(rr), br, mul);
            jit_subr(Compiler.JIT, R(rr), ar,R(rr));
            vec_push(&Compiler.valueStack,VALUE_VAR(rv));
        } else if(IsF64(r)) {
            FLOAT_OP(jit_fsubi, jit_fsubr,r, -);
        } else {
            INT_OP(jit_subi,jit_subr,r, - );
        }
        return;
    }
    case AST_LT: {
        CType *r =AssignTypeToNode(exp->binop.a);
        if(IsF64(r)) {
#define FCLR_IF_TR(bi,br) \
CType *f64 =CreatePrimType(TYPE_F64); \
__CompileAST(exp->binop.a); \
CType *at =AssignTypeToNode(exp->binop.a); \
TypecastValFromStackIfNeeded(at,f64); \
__CompileAST(exp->binop.b); \
CType *bt =AssignTypeToNode(exp->binop.b); \
WarnOnWeirdPass(exp, at, bt); \
TypecastValFromStackIfNeeded(bt,f64); \
CValue b AF_VALUE =vec_pop(&Compiler.valueStack); \
CValue a AF_VALUE=vec_pop(&Compiler.valueStack); \
int t;\
jit_value rr=R(t=GetIntTmpReg()); \
jit_movi(Compiler.JIT, rr,0); \
jit_op *tp=NULL; \
jit_value a2=MoveValueToFltRegIfNeeded(a, 0); \
if(b.type==VALUE_FLOAT) {  \
  tp=bi(Compiler.JIT,(uint64_t)NULL,a2,b.flt); \
} else  { \
  jit_value b2=MoveValueToFltRegIfNeeded(b, 1); \
  tp=br(Compiler.JIT,(uint64_t)NULL,a2,b2); \
} \
jit_movi(Compiler.JIT, rr,1); \
CType *bool  =CreatePrimType(TYPE_BOOL); \
vec_push(&Compiler.valueStack,VALUE_VAR(CreateTmpRegVar(t, bool))); \
jit_patch(Compiler.JIT, tp);
            FCLR_IF_TR(jit_fbgei,jit_fbger);
        } else if(TypeIsSigned(r)) {
            INT_OP(jit_lti,jit_ltr,r, < );
        } else {
            INT_OP(jit_lti_u,jit_ltr_u,r, <);
        }
        return;
    }
    case AST_GT: {
        CType *r =AssignTypeToNode(exp->binop.a);
        if(IsF64(r)) {
            FCLR_IF_TR(jit_fblei,jit_fbler);
        } else if(TypeIsSigned(r)) {
            INT_OP(jit_gti,jit_gtr,r, >);
        } else {
            INT_OP(jit_gti_u,jit_gtr_u,r, >);
        }
        return;
    }
    case AST_LE: {
        CType *r =AssignTypeToNode(exp->binop.a);
        if(IsF64(r)) {
            FCLR_IF_TR(jit_fbgti,jit_fbgtr);
        } else if(TypeIsSigned(r)) {
            INT_OP(jit_lei,jit_ler,r, <=);
        } else {
            INT_OP(jit_lei_u,jit_ler_u,r, <=);
        }
        return;
    }
    case AST_GE: {
        CType *r =AssignTypeToNode(exp->binop.a);
        if(IsF64(r)) {
            FCLR_IF_TR(jit_fblti,jit_fbltr);
        } else if(TypeIsSigned(r)) {
            INT_OP(jit_gei,jit_ger,r, >=);
        } else {
            INT_OP(jit_gei_u,jit_ger_u,r, >=);
        }
        return;
    }
    case AST_RANGE: {
        int iter=1;
        int op,final_reg;
        CType *bool =CreatePrimType(TYPE_BOOL);
        CVariable *a AF_VAR=NULL,*b AF_VAR=NULL,*final AF_VAR=CreateTmpRegVar(final_reg=GetIntTmpReg(), bool);
#define TRANSFER(to,from) \
  { \
CType *t =AssignTypeToNode(from); \
if(!IsArith(t)) { char *ts=TypeToString(t);RaiseError(from,"Cant compare non-arithmetic type. %s",ts);TD_FREE(ts);}  \
if(IsF64(t)) { \
  to=CreateTmpRegVar(GetFltTmpReg(), t); \
} else { \
  to=CreateTmpRegVar(GetIntTmpReg(), t); \
} \
__CompileAST(from); \
CValue v AF_VALUE=vec_pop(&Compiler.valueStack); \
CompileAssign(VALUE_VAR(to), v); \
  }
        jit_movi(Compiler.JIT,R(final_reg),1);
        vec_push(&Compiler.valueStack,VALUE_VAR(final));
        __CompileAST(exp->range.operands.data[0]);
        for(; iter<=exp->range.ops.length; iter++) {
            //Stack: FINAL,A
            AST *anode =ValueToAST(vec_last(&Compiler.valueStack));
            //Stack: FINAL,A
            TRANSFER(b,exp->range.operands.data[iter]);
            vec_push(&Compiler.valueStack,VALUE_VAR(b));
            //Stack: FINAL,A,B
            AST *dummy =CreateBinop(anode, CreateVarNode(b), exp->range.ops.data[iter-1]);
            SetPosFromOther(dummy, exp->range.operands.data[iter-1]);
            __CompileAST(dummy);
            //Stack FINAL,A,B,RES
            //Pop RES and bAND with FINAL(which is a reigster)
            CValue res AF_VALUE=vec_pop(&Compiler.valueStack);
            jit_value resr=MoveValueToIntRegIfNeeded(res, 0);
            jit_andr(Compiler.JIT, R(final_reg), R(final_reg),resr);
            //Stack: FINAL,B,A (B will become the next a)
            vec_swap(&Compiler.valueStack, Compiler.valueStack.length-1, Compiler.valueStack.length-2);
            ReleaseValue(&vec_pop(&Compiler.valueStack));
        }
        //FINAL,A
        ReleaseValue(&vec_pop(&Compiler.valueStack));
        //FINAL
        return;
    }
    case AST_EQ: {
        CType *r =AssignTypeToNode(exp->binop.a);
        if(IsF64(r)) {
            FCLR_IF_TR(jit_fbnei, jit_fbner);
        } else {
            INT_OP(jit_eqi, jit_eqr,r, ==);
        }
        return;
    }
    case AST_NE: {
        CType *r =AssignTypeToNode(exp->binop.a);
        if(IsF64(r)) {
            FCLR_IF_TR(jit_fbeqi, jit_fbeqr);
        } else {
            INT_OP(jit_nei, jit_ner,r, !=);
        }
        return;
    }
    case AST_LAND: {
        //TODO TD_FREE
#define FBOOLIFY(to,a) { \
AST *dummy =CreateBinop(ValueToAST(a), CreateF64(0.0), AST_NE); \
SetPosFromOther(dummy, exp); \
__CompileAST(dummy); \
to=vec_pop(&Compiler.valueStack); \
  }
        //TODO FRE
#define BOOLIFY(to,a) { \
  AST *ast =ValueToAST(a); \
  CType *t =AssignTypeToNode(ast); \
  AST *tc =CreateImplicitTypecast(CreateI64(0),CreateTypeNode(t)); \
AST *dummy =CreateBinop(ast, tc, AST_NE); \
SetPosFromOther(dummy, exp); \
__CompileAST(dummy); \
to=vec_pop(&Compiler.valueStack); \
  }

#define F_LOGICAL(op) \
  CValue a2 AF_VALUE,b2 AF_VALUE; \
  b=Compiler.valueStack.data[Compiler.valueStack.length-1]; \
  a=Compiler.valueStack.data[Compiler.valueStack.length-2]; \
  FBOOLIFY(b2,b); \
  Compiler.valueStack.data[Compiler.valueStack.length-1]=b2; \
  FBOOLIFY(a2,a); \
  vec_pop(&Compiler.valueStack); \
  vec_pop(&Compiler.valueStack); \
  int ir=GetIntTmpReg(); \
  vec_push(&Compiler.valueStack,VALUE_VAR(CreateTmpRegVar(ir, CreatePrimType(TYPE_I64)))); \
  op(Compiler.JIT,R(ir),R(a2.var->reg),R(b2.var->reg));

#define INT_LOGICAL(op) \
  CValue a2 AF_VALUE,b2 AF_VALUE; \
  b=Compiler.valueStack.data[Compiler.valueStack.length-1]; \
  a=Compiler.valueStack.data[Compiler.valueStack.length-2]; \
  BOOLIFY(b2,b); \
  Compiler.valueStack.data[Compiler.valueStack.length-1]=b2; \
  BOOLIFY(a2,a); \
  vec_pop(&Compiler.valueStack); \
  vec_pop(&Compiler.valueStack); \
  int ir=GetIntTmpReg(); \
  vec_push(&Compiler.valueStack,VALUE_VAR(CreateTmpRegVar(ir, CreatePrimType(TYPE_I64)))); \
  jit_value ar=MoveValueToIntRegIfNeeded(a2,0); \
  jit_value br=MoveValueToIntRegIfNeeded(b2,1); \
  op(Compiler.JIT,R(ir),ar,br);

        CType *r =AssignTypeToNode(exp->binop.a);
        __CompileAST(exp->binop.a);
        __CompileAST(exp->binop.b);
        CValue a AF_VALUE,b AF_VALUE;
        if(IsF64(r)) {
            F_LOGICAL(jit_andr);
        } else {
            INT_LOGICAL(jit_andr);
        }
        return;
    }
    case AST_LXOR: {
        CType *r =AssignTypeToNode(exp->binop.a);
        __CompileAST(exp->binop.a);
        __CompileAST(exp->binop.b);
        CValue a AF_VALUE,b AF_VALUE;
        if(IsF64(r)) {
            F_LOGICAL(jit_xorr);
        } else {
            INT_LOGICAL(jit_xorr);
        }
        return;
    }
    case AST_LOR: {
        CType *r =AssignTypeToNode(exp->binop.a);
        __CompileAST(exp->binop.a);
        __CompileAST(exp->binop.b);
        CValue a AF_VALUE,b AF_VALUE;
        if(IsF64(r)) {
            F_LOGICAL(jit_orr);
        } else {
            INT_LOGICAL(jit_orr);
        }
        return;
    }
    case AST_ASSIGN: {
        CType *at =AssignTypeToNode(exp->binop.a);
        CType *bt =AssignTypeToNode(exp->binop.b);
        WarnOnWeirdPass(exp, at, bt);
        __CompileAST(exp->binop.a);
        __CompileAST(exp->binop.b);
        CValue src AF_VALUE=vec_pop(&Compiler.valueStack);
        CValue dst AF_VALUE=vec_pop(&Compiler.valueStack);
        if(dst.type==VALUE_VAR||dst.type==VALUE_INDIR_VAR) {
            //All Good,is a lvalue
        } else
            RaiseError(exp->binop.a,"Expected lvalue.");
        CompileAssign(dst, src);
        vec_push(&Compiler.valueStack,CloneValue(dst));
        return ;
    }
    case AST_ASSIGN_SHL:
    {
#define ASSIGN_OP(oper) \
  __CompileAST(exp->binop.a); \
  "Stack: dst "; \
  AST *dst =ValueToAST(vec_last(&Compiler.valueStack)); \
  AST *b =CreateBinop(dst, exp->binop.b, oper); \
  SetPosFromOther(b, exp); \
  __CompileAST(b); \
  "Stack: dst,res "; \
  CValue res AF_VALUE=vec_pop(&Compiler.valueStack); \
  CValue dst2 AF_VALUE=vec_pop(&Compiler.valueStack); \
  CompileAssign(dst2, res); \
  vec_push(&Compiler.valueStack,CloneValue(dst2));
        ASSIGN_OP(AST_SHL);
        return ;
    }
    case AST_ASSIGN_SHR: {
        ASSIGN_OP(AST_SHR);
        return;
    }
    case AST_ASSIGN_MUL: {
        ASSIGN_OP(AST_MUL);
        return;
    }
    case AST_ASSIGN_DIV: {
        ASSIGN_OP(AST_DIV);
        return;
    }
    case AST_ASSIGN_MOD: {
        ASSIGN_OP(AST_MOD);
        return;
    }
    case AST_ASSIGN_BAND: {
        ASSIGN_OP(AST_BAND);
        return;
    }
    case AST_ASSIGN_BOR: {
        ASSIGN_OP(AST_BOR);
        return;
    }
    case AST_ASSIGN_BXOR: {
        ASSIGN_OP(AST_BXOR);
        return;
    }
    case AST_ASSIGN_ADD: {
        ASSIGN_OP(AST_ADD);
        return;
    }
    case AST_ASSIGN_SUB: {
        ASSIGN_OP(AST_SUB);
        return;
    }
    case AST_POST_INC: {
#define POST_OP(mod_assign) \
  long offset=1; \
  __CompileAST(exp->unopArg); \
  CType *expt =AssignTypeToNode(exp->unopArg); \
  CVariable *tmp AF_VAR=NULL ; \
  if(IsF64(expt)) { \
tmp=CreateTmpRegVar(GetFltTmpReg(), expt); \
  } else if(IsPtr(expt)) { \
AST *ptr =PtrifyIfNeeded(exp->unopArg); \
CType *ptrt =AssignTypeToNode(ptr); \
CType *de =DerrefedType(ptrt); \
expt=de; \
tmp=CreateTmpRegVar(GetIntTmpReg(), expt); \
  } else { \
tmp=CreateTmpRegVar(GetIntTmpReg(), expt); \
  } \
  CValue a=VALUE_VAR(tmp),b=vec_pop(&Compiler.valueStack); \
  CompileAssign(a,b); \
  vec_push(&Compiler.valueStack,a); \
  "Modify b(exp->unopArg)"; \
  AST *bast =ValueToAST(b),*aoff ,*modify ; \
  modify=CreateBinop(bast, aoff=CreateI64(offset), mod_assign); \
  SetPosFromOther(modify, exp); \
  __CompileAST(modify); \
  ReleaseValue(&vec_pop(&Compiler.valueStack));

        POST_OP(AST_ASSIGN_ADD);
        return;
    }
    case AST_POST_DEC: {
        POST_OP(AST_ASSIGN_SUB);
        return ;
    }
    case AST_ARRAY_ACCESS: {
        CType *t =AssignTypeToNode(exp);
        AST *arr =PtrifyIfNeeded(exp->arrayAccess.array);
        AST *subs =exp->arrayAccess.subscript;
        AST *off =CreateBinop(arr, subs, AST_ADD);
        SetPosFromOther(off, exp);
        AST *derref=CreateUnop(off, AST_DERREF);
        __CompileAST(derref);
        return;
    }
    case AST_PRE_INC: {
        __CompileAST(exp->unopArg);
        CValue top AF_VALUE=vec_pop(&Compiler.valueStack);
        AST *tast =ValueToAST(top);
        AST *one =CreateI64(1);
        AST *asn =CreateBinop(tast, one, AST_ASSIGN_ADD);
        SetPosFromOther(asn, exp);
        __CompileAST(asn);
        return;
    }
    case AST_PRE_DEC: {
        __CompileAST(exp->unopArg);
        CValue top AF_VALUE=vec_pop(&Compiler.valueStack);
        AST *tast =ValueToAST(top);
        AST *one =CreateI64(1);
        AST *asn =CreateBinop(tast, one, AST_ASSIGN_SUB);
        SetPosFromOther(asn, exp);
        __CompileAST(asn);
        return;
    }
    case AST_POS: {
        __CompileAST(exp->unopArg);
        return;
    }
    case AST_NEG: {
        __CompileAST(exp->unopArg);
        CValue v AF_VALUE=vec_pop(&Compiler.valueStack);
        CType *t =AssignTypeToNode(exp->unopArg);
        CVariable *ret AF_VAR;
        if(IsF64(t)) {
            ret=CreateTmpRegVar(GetFltTmpReg(), CreatePrimType(TYPE_F64));
            jit_fnegr(Compiler.JIT, FR(ret->reg), MoveValueToFltRegIfNeeded(v, 0));
        } else {
            ret=CreateTmpRegVar(GetIntTmpReg(), t);
            jit_negr(Compiler.JIT, R(ret->reg), MoveValueToIntRegIfNeeded(v, 0));
        }
        vec_push(&Compiler.valueStack,VALUE_VAR(ret));
        return;
    }
    case AST_LNOT: {
        CType *t =AssignTypeToNode(exp->unopArg);
        if(!IsArith(t)) {
            char *ts=TypeToString(t);
            RaiseError(exp,"Atempt to use op \"!\" on non-arithmetic type %s.",ts);
            TD_FREE(ts);
        }
        if(IsF64(t)) {
            AST *z =CreateF64(0.0);
            AST *exp2 =CreateBinop(exp->unopArg, z,AST_EQ);
            SetPosFromOther(exp2, exp);
            __CompileAST(exp2);
        } else {
            AST *z =CreateI64(0);
            CType *t =AssignTypeToNode(exp->unopArg);
            AST *tc =CreateImplicitTypecast(z, CreateTypeNode(t));
            AST *exp2 =CreateBinop(exp->unopArg, tc,AST_EQ);
            SetPosFromOther(exp2, exp);
            __CompileAST(exp2);
        }
        return;
    }
    case AST_BNOT: {
        CType *t =AssignTypeToNode(exp->unopArg);
        if(!IsArith(t)) {
            char *ts=TypeToString(t);
            RaiseError(exp,"Atempt to use op \"~\" on non-arithmetic type %s.",ts);
            TD_FREE(ts);
        }
        CVariable *ret AF_VAR;
        if(IsF64(t)) {
            //TODO
        } else {
            __CompileAST(exp->unopArg);
            CValue v AF_VALUE=vec_pop(&Compiler.valueStack);
            ret=CreateTmpRegVar(GetIntTmpReg(), t);
            jit_value vr=MoveValueToIntRegIfNeeded(v, 0);
            jit_notr(Compiler.JIT, R(ret->reg), vr);
        }
        vec_push(&Compiler.valueStack,VALUE_VAR(ret));
        return;
    }
    case AST_DERREF: {
        CType *t =AssignTypeToNode(exp->unopArg);
        if(!IsPtr(t)) {
            char *ts=TypeToString(t);
            RaiseError(exp,"Can't derrefernce type %s.",ts);
            TD_FREE(ts);
        }
        AST *ptr =PtrifyIfNeeded(exp->unopArg); //Account for arrays
        __CompileAST(ptr);
        if(DerrefedType(t)->type==TYPE_FUNC) return;
        CVariable *ret AF_VAR=CreateTmpRegVar(GetIntTmpReg(), t);
        CValue dst AF_VALUE=VALUE_VAR(ret),src AF_VALUE;
        CompileAssign(dst, src=vec_pop(&Compiler.valueStack));
        if(Compiler.boundsCheckMode) {
            jit_prepare(Compiler.JIT);
            jit_putargr(Compiler.JIT, R(ret->reg));
            jit_putargi(Compiler.JIT, TypeSize(DerrefedType(t)));
            jit_call(Compiler.JIT, WhineOnOutOfBounds);
        }
        CValue ret2 = {.type=VALUE_INDIR_VAR,.var=CloneVariable(ret)}; //Returned so dont TD_FREE
        vec_push(&Compiler.valueStack,ret2);
        return;
    }
    case AST_ADDROF: {
        CType *t =AssignTypeToNode(exp);
        //Avoid implicit function calls
        if(exp->unopArg->type==AST_NAME) {
          CVariable *var AF_VAR= GetVariable(exp->unopArg->name);
          if(Compiler.addrofFrameoffsetMode) {
            if(var->isGlobal) {
              //Continue as normal
            } else {
              vec_push(&Compiler.valueStack, VALUE_INT(Compiler.frameOffset+var->frameOffset));
	      return;
            }
          }
          if(!var) {
            __CompileAST(exp->unopArg);
          } else {
            CValue v=VALUE_VAR(var);
            vec_push(&Compiler.valueStack, v);
          }
        } else
          __CompileAST(exp->unopArg);
        CValue v AF_VALUE=vec_pop(&Compiler.valueStack),ret;
        /**
         * Arrays references do nothing when getting the addr of it
         */
        if(v.type==VALUE_INDIR_VAR) {
            CType *deref  =DerrefedType(v.var->type);
            if(deref->type==TYPE_ARRAY_REF) {
                //do nothing
            } else
                vec_push(&Compiler.valueStack,VALUE_VAR(v.var));
        } else if(v.type==VALUE_VAR) {
            if(v.var->type->type==TYPE_ARRAY_REF) {
                //do nothing
            } else {
                if(v.var->isGlobal) {
                    CVariable *pvar AF_VAR=CreateTmpRegVar(GetIntTmpReg(),t);
                    MoveGlobalPtrToReg(v.var,pvar->reg);
                    vec_push(&Compiler.valueStack,VALUE_VAR(pvar));
                } else if(v.var->isNoreg) {
                    int r;
                    CVariable *var AF_VAR=CreateTmpRegVar(r=GetIntTmpReg(), t);
                    jit_addi(Compiler.JIT, R(r), R_FP, v.var->frameOffset+Compiler.frameOffset);
                    vec_push(&Compiler.valueStack,VALUE_VAR(var));
                } else {
                    RaiseError(exp,"Cant get address of a variable in a register.");
                    vec_push(&Compiler.valueStack, VALUE_INT(0));
                }
            }
        } else {
            RaiseError(exp,"(INTERNAL!!!)Cant get address of non-variable.");
            vec_push(&Compiler.valueStack, VALUE_INT(0));
        }
        return;
    }
    case AST_IMPLICIT_TYPECAST: {
        CType *t =AssignTypeToNode(exp->typecast.base);
        __CompileAST(exp->typecast.base);
        if(IsIntegerType(exp->typecast.toType->type2)&&IsIntegerType(t)) {
            break;
        } else if(IsF64(exp->typecast.toType->type2)&&IsF64(t)) {
            break;
        } else if(IsF64(exp->typecast.toType->type2)) {
            CValue src AF_VALUE=vec_pop(&Compiler.valueStack);
            CVariable *ftmp AF_VAR=CreateTmpRegVar(GetFltTmpReg(), exp->typecast.toType->type2);
            CValue ret=VALUE_VAR(ftmp);
            CompileAssign(ret, src);
            vec_push(&Compiler.valueStack, ret);
        } else if(IsIntegerType(t)) {
            CValue src AF_VALUE=vec_pop(&Compiler.valueStack);
            CVariable *ftmp AF_VAR=CreateTmpRegVar(GetIntTmpReg(), exp->typecast.toType->type2);
            CValue ret=VALUE_VAR(ftmp);
            CompileAssign(ret, src);
            vec_push(&Compiler.valueStack, ret);
        } else {
            char *ts=TypeToString(t);
            char *ts2=TypeToString(exp->typecast.toType->type2);
            RaiseError(exp,"(INTERNAL!!!)Implicit typecast to of strange types(%s and %s) .Aborting.",ts,ts2);
            abort();
        }
    }
    /**
     * Explicit typecasts will bit for bit transfer ints to F64
     */
    case AST_EXPLICIT_TYPECAST: {
        CType *to =BaseType(exp->typecast.toType->type2);
        CType *from ,*bt ;
        bt=BaseType(from=AssignTypeToNode(exp->typecast.base));
        if(IsF64(to)) {
            if(exp->typecast.base->type==AST_INT) {
                union {
                    uint64_t i;
                    double d;
                } pn;
                pn.i=exp->typecast.base->integer;
                vec_push(&Compiler.valueStack, VALUE_FLOAT(pn.d));
            } else if(IsIntegerType(from)) {
                jit_prepare(Compiler.JIT);
                __CompileAST(exp->typecast.base);
                CValue v AF_VALUE=vec_pop(&Compiler.valueStack);
                MoveValueToIntRegIfNeeded(v, 0);
                jit_putargr(Compiler.JIT, R(0));
                jit_call(Compiler.JIT, Bit4BitU64ToF64);
                CVariable *var AF_VAR=CreateTmpRegVar(GetFltTmpReg(), CreatePrimType(TYPE_F64));
                jit_fretval(Compiler.JIT, FR(var->reg),8);
                vec_push(&Compiler.valueStack,VALUE_VAR(var));
            } else if(IsF64(from)) {
                __CompileAST(exp->typecast.base);
            } else {
                goto strangecast;
            }
        } else if(IsIntegerType(to)) {
            if(IsIntegerType(from)) {
                __CompileAST(exp->typecast.base);
                CValue v AF_VALUE=vec_pop(&Compiler.valueStack);
                CVariable *var AF_VAR=CreateTmpRegVar(GetIntTmpReg(), from);
                CValue ret AF_VALUE=VALUE_VAR(var);
                CompileAssign(ret, v);
                vec_push(&Compiler.valueStack, VALUE_VAR(var));
            } else if(exp->typecast.base->type==AST_FLOAT) {
                vec_push(&Compiler.valueStack,VALUE_INT(*(uint64_t*)&exp->typecast.base->floating));
            } else if(IsF64(from)) {
                jit_prepare(Compiler.JIT);
                __CompileAST(exp->typecast.base);
                CValue v AF_VALUE=vec_pop(&Compiler.valueStack);
                jit_value vr=MoveValueToFltRegIfNeeded(v, 0);
                jit_fputargr(Compiler.JIT, vr,8);
                jit_call(Compiler.JIT, Bit4BitF64ToU64);
                CVariable *var AF_VAR= CreateTmpRegVar(GetIntTmpReg(), CreatePrimType(TYPE_F64));
                jit_retval(Compiler.JIT, R(var->reg));
                vec_push(&Compiler.valueStack,VALUE_VAR(var));
            } else {
strangecast:
                ;
                char *ts=TypeToString(to);
                char *fs=TypeToString(from);
                RaiseError(exp,"Atempt to typecast %s to %s.",fs,ts);
                TD_FREE(ts),TD_FREE(fs);
                vec_push(&Compiler.valueStack,VALUE_INT(0));
            }
        } else {
            goto strangecast;
        }
exptcend:
        return;
    }
    case AST_FUNC_CALL: {
        int origstacksz=Compiler.valueStack.length;
        /**
         * Function aquire the name of thier function,otherwise a function name on its own is treated as a  implicit function call.
         */
        CType *ftype =NULL;
        if(exp->funcCall.func->type!=AST_NAME&&exp->funcCall.func->type!=AST_VAR) {
            ftype=AssignTypeToNode(exp->funcCall.func);
            __CompileAST(exp->funcCall.func);
        } else if(exp->funcCall.func->type==AST_NAME) {
            CVariable *var;
            if(var=GetVariable(exp->funcCall.func->name)) {
                ftype=var->type;
                vec_push(&Compiler.valueStack, VALUE_VAR(var));
            } else {
                RaiseError(exp->funcCall.func,"Unknown function %s.",exp->funcCall.func->name);
                vec_push(&Compiler.valueStack, VALUE_INT(0));
                return;
            }
        } else if(exp->funcCall.func->type==AST_VAR) {
            ftype=exp->funcCall.func->var->type;
            vec_push(&Compiler.valueStack, VALUE_VAR(exp->funcCall.func->var));
        }
        if(ftype==NULL) {
            vec_push(&Compiler.valueStack, VALUE_INT(0));
            return ;
        }
        CType *fbt =BaseType(ftype);
        //
        //STACK:func
        //
        int fargc;
        if(IsPtr(fbt)) {
            CType *d =DerrefedType(fbt);
            if(d->type!=TYPE_FUNC) {
notfunction:
                ;
                char *ts=TypeToString(d);
                RaiseError(exp,"Can't call type %s.",ts);
                TD_FREE(ts);
                vec_push(&Compiler.valueStack, VALUE_INT(0));
                return;
            }
            CType *f =BaseType(d);
            fargc=f->func.arguments.length;
            fbt=f;
        } else {
            fargc=fbt->func.arguments.length;
        }
        if(fbt->type!=TYPE_FUNC) goto notfunction;

        //STACK:func,args[argc,...]
        vec_AST_t args=CommaToVec(exp->funcCall.args);
        FillInLastclasses(exp,fbt,&args);
        /**!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
           !  Values Must be put in imediates or reigsters      !
           !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!**/
        AST *arg;
        int iter;
        vec_foreach(&args, arg, iter) {
            if(!arg) {
                if(fbt->func.dftArgs.length>iter) {
                    if(fbt->func.dftArgs.data[iter]) {
                        __CompileAST(fbt->func.dftArgs.data[iter]);
                        //Stuff in reg as values must be put in regs
                        CValue v AF_VALUE=vec_pop(&Compiler.valueStack);
                        CVariable *var AF_VAR=CreateTmpRegVar(GetIntTmpReg(), fbt->func.arguments.data[iter]);
                        CValue ret AF_VALUE=VALUE_VAR(var);
                        CompileAssign(ret, v);
                        vec_push(&Compiler.valueStack,VALUE_VAR(var));
                    } else {
                        RaiseError(exp->funcCall.func,"No default argument provided for argument %ld.",iter);
                        vec_push(&Compiler.valueStack,VALUE_INT(0));
                    }
                } else {
                    RaiseError(exp->funcCall.func,"Passing empty argument to vargs %ld.",iter);
                    //Is a blank argume to vargs ,so 0?
                    vec_push(&Compiler.valueStack,VALUE_INT(0));
                }
                continue;
            }
            if(fbt->func.arguments.length>iter) {
                CType *t =AssignTypeToNode(arg);
                WarnOnWeirdPass(arg, fbt->func.arguments.data[iter], t);
            }
            __CompileAST(arg);
            CValue v=vec_pop(&Compiler.valueStack);
            switch(v.type) {
            case VALUE_STRING:
            case VALUE_INT:
                if(fbt->func.arguments.length>iter) {
                    if(IsIntegerType(fbt->func.arguments.data[iter])) {
                        vec_push(&Compiler.valueStack,CloneValue(v));
                        break;
                    } else if(IsF64(fbt->func.arguments.data[iter])) {
                        vec_push(&Compiler.valueStack,VALUE_FLOAT(v.integer));
                        break;
                    } else {
                        char *txt=TypeToString(fbt->func.arguments.data[iter]);
                        RaiseError(fbt->func.dftArgs.data[iter],"Passing integer to %s.");
                        TD_FREE(txt);
                    }
                }
                vec_push(&Compiler.valueStack,CloneValue(v));
                break;
            case VALUE_FLOAT:
                if(fbt->func.arguments.length>iter) {
                    if(IsIntegerType(fbt->func.arguments.data[iter])) {
                        vec_push(&Compiler.valueStack,VALUE_INT(v.flt));
                        break;
                    } else if(IsF64(fbt->func.arguments.data[iter])) {
                        vec_push(&Compiler.valueStack,CloneValue(v));
                        break;
                    } else {
                        char *txt=TypeToString(fbt->func.arguments.data[iter]);
                        RaiseError(fbt->func.dftArgs.data[iter],"Passing integer to %s.");
                        TD_FREE(txt);
                    }
                }
                vec_push(&Compiler.valueStack,CloneValue(v));
                break;
            default:
                ;
                CType *vt =NULL;
                if(v.type==VALUE_VAR) {
                    vt=v.var->type;
                } else if(v.type==VALUE_INDIR_VAR) {
                    vt=DerrefedType(v.var->type);
                } else {
                    vec_push(&Compiler.valueStack,VALUE_INT(0));
                    break;
                }
                if(fbt->func.arguments.length>iter) {
                    if(IsIntegerType(fbt->func.arguments.data[iter])) {
stuffint:
                        ;
                        int r;
                        jit_value jv=MoveValueToIntRegIfNeeded(v,r=GetIntTmpReg());
                        jit_movr(Compiler.JIT, R(r),jv);
                        CVariable *rv AF_VAR=CreateTmpRegVar(r, fbt->func.arguments.data[iter]);
                        vec_push(&Compiler.valueStack,VALUE_VAR(rv));
                        break;
                    } else if(IsF64(fbt->func.arguments.data[iter])) {
                        int r;
                        jit_value jv=MoveValueToFltRegIfNeeded(v,r=GetFltTmpReg());
                        jit_fmovr(Compiler.JIT, FR(r), jv);
                        CVariable *rv AF_VAR=CreateTmpRegVar(r, fbt->func.arguments.data[iter]);
                        vec_push(&Compiler.valueStack,VALUE_VAR(rv));
                        break;
                    } else goto stuffint;
                } else {
                    int r=((IsF64(vt))?GetFltTmpReg():GetIntTmpReg());
                    CType *dvt ;
                    if(IsF64(vt)) dvt=CreatePrimType(TYPE_F64);
                    else dvt=CreatePrimType(TYPE_I64);
                    if(IsVarRegCanidate(v.var)&&v.type==VALUE_VAR)
                        r=v.var->reg;
                    else if(IsF64(vt))
                        MoveValueToFltRegIfNeeded(v,r);
                    else
                        MoveValueToIntRegIfNeeded(v,r);
                    CVariable* rvar AF_VAR=CreateTmpRegVar(r,vt);
                    vec_push(&Compiler.valueStack,VALUE_VAR(rvar));
                    break;
                }
            }
        }
        //Fill in dft args
        for(iter=args.length; iter<fbt->func.arguments.length; iter++) {
            if(fbt->func.dftArgs.data[iter]) {
                __CompileAST(fbt->func.dftArgs.data[iter]);
                //Stuff in reg as values must be put in regs
                CValue v AF_VALUE=vec_pop(&Compiler.valueStack);
                CVariable *var AF_VAR=CreateTmpRegVar(GetIntTmpReg(), fbt->func.arguments.data[iter]);
                CValue ret AF_VALUE=VALUE_VAR(var);
                CompileAssign(ret, v);
                vec_push(&Compiler.valueStack,VALUE_VAR(var));
            } else {
                RaiseError(exp->funcCall.func,"No default argument provided for argument %ld.",iter);
                vec_push(&Compiler.valueStack,VALUE_INT(0));
            }
        }
        CVariable *argv AF_VAR=NULL;
        long argc=0;
        //Spill excess arguments to an area on the stack.
        int hasexcess=Compiler.valueStack.length-fbt->func.arguments.length>origstacksz+1;
        if(hasexcess&&fbt->func.hasvargs) {
            long excess=Compiler.valueStack.length-fbt->func.arguments.length-(origstacksz+1);
            argc=excess;
            long offset=jit_allocai(Compiler.JIT, excess*8); //In TempleOS,all arguments are 64bit
            while(--excess>=0) {
                CType *type =AssignTypeToNode(args.data[excess+fbt->func.arguments.length]);
                CValue top AF_VALUE=vec_pop(&Compiler.valueStack);
                if(IsF64(type)) {
                } else {
                    //In TempleOS all argument passes are 64bit
                    type=CreatePrimType(TYPE_I64);
                }
                CVariable *frame AF_VAR=CreateTmpVarAtFrameOff(offset+excess*8, type);
                CValue framev AF_VALUE=VALUE_VAR(frame);
                CompileAssign(framev,top);
            }
            int tmp_reg;
            argv=CreateTmpRegVar(tmp_reg=GetIntTmpReg(), CreatePrimType(TYPE_I64));
            jit_addi(Compiler.JIT, R(tmp_reg), R_FP, offset);
        } else if(hasexcess) {
            char *fs=TypeToString(fbt);
            RaiseError(args.data[fbt->func.arguments.length],"Excess arguments for type %s.",fs);
            TD_FREE(fs);
        }
        //Function
        CValue func =Compiler.valueStack.data[origstacksz]; //Will be released later
        //In B!,all functions are stored as CVariables
        jit_value fv=MoveValueToIntRegIfNeeded(func, 0);
        jit_prepare(Compiler.JIT);
        //+1 ignores function on stack
        for(iter=origstacksz+1; iter!=Compiler.valueStack.length; iter++) {
            CValue v=Compiler.valueStack.data[iter];
            switch(v.type) {
            case VALUE_FLOAT:
                jit_fputargi(Compiler.JIT, v.flt, 8);
                break;
            case VALUE_INT:
                jit_putargi(Compiler.JIT, v.integer);
                break;
            case VALUE_STRING:
                jit_putargi(Compiler.JIT, v.string);
                break;
            case VALUE_VAR:
                if(IsF64(v.var->type))
                    jit_fputargr(Compiler.JIT, FR(v.var->reg),8);
                else if (IsIntegerType(v.var->type)||IsPtrType(v.var->type)||IsCompositeType(v.var->type))
                    jit_putargr(Compiler.JIT, R(v.var->reg));
                break;
            default:
                abort();
            }
        }
        if(fbt->func.hasvargs) {
            jit_putargi(Compiler.JIT, argc);
            if(argv)
                jit_putargr(Compiler.JIT, R(argv->reg));
            else
                jit_putargi(Compiler.JIT, NULL);
        }
        jit_callr(Compiler.JIT, fv);
        //Pop values on stack until origstacksz
        while(Compiler.valueStack.length>origstacksz) {
            ReleaseValue(&vec_pop(&Compiler.valueStack));
        }
        //
        if(fbt->func.ret->type==TYPE_U0) {
            //Dummy value
            vec_push(&Compiler.valueStack,VALUE_INT((uint64_t)NULL));
        } else if(IsF64(fbt->func.ret)) {
            CVariable *tmp AF_VAR=CreateTmpRegVar(GetFltTmpReg(), CreatePrimType(TYPE_F64));
            jit_fretval(Compiler.JIT,FR(tmp->reg),8);
            vec_push(&Compiler.valueStack,VALUE_VAR(tmp));
        } else if(IsIntegerType(fbt->func.ret)) {
            CVariable *tmp AF_VAR=CreateTmpRegVar(GetIntTmpReg(), CreatePrimType(TYPE_I64));
            jit_retval(Compiler.JIT,R(tmp->reg));
            vec_push(&Compiler.valueStack,VALUE_VAR(tmp));
        } else { //Is object type
            //TODO
            CVariable *tmp AF_VAR=CreateTmpRegVar(GetIntTmpReg(), CreatePrimType(TYPE_F64));
            jit_retval(Compiler.JIT,R(tmp->reg));
            vec_push(&Compiler.valueStack,VALUE_VAR(tmp));
        }
        return ;
    }
    case AST_RETURN: {
        if(!exp->retval) {
            //Put dummy value on stack
            vec_push(&Compiler.valueStack,VALUE_INT(0));
            if(Compiler.debugMode) {
                jit_prepare(Compiler.JIT);
                jit_call(Compiler.JIT, DbgLeaveFunction);
            }
            jit_reti(Compiler.JIT, 0);
            return;
        }
        CType *rtype =AssignTypeToNode(exp->retval);
        WarnOnWeirdPass(exp->retval, Compiler.returnType, rtype);
        __CompileAST(exp->retval);
        if(Compiler.debugMode) {
            jit_prepare(Compiler.JIT);
            jit_call(Compiler.JIT, DbgLeaveFunction);
        }
        if(Compiler.returnType)
            TypecastValFromStackIfNeeded(rtype, Compiler.returnType);
        CValue v AF_VALUE = vec_pop(&Compiler.valueStack);
        CType *rbt =BaseType(Compiler.returnType);
        if(IsF64(Compiler.returnType)) {
            if(v.type==VALUE_FLOAT)
                jit_freti(Compiler.JIT, v.flt, 8);
            else {
                jit_value f=MoveValueToFltRegIfNeeded(v, 0);
                jit_fretr(Compiler.JIT, f, 8);
            }
        } else if(IsIntegerType(Compiler.returnType)) {
            if(v.type==VALUE_INT)
                jit_reti(Compiler.JIT, v.integer);
            else {
                jit_value i=MoveValueToIntRegIfNeeded(v, 0);
                jit_retr(Compiler.JIT, i);
            }
        } else if(rbt->type==TYPE_U0) {
            jit_reti(Compiler.JIT, 0);
        } else {
            jit_value r=MoveValueToIntRegIfNeeded(v,0);
            jit_retr(Compiler.JIT, r);
        }
        //Put dummy value on stack
        vec_push(&Compiler.valueStack,VALUE_INT(0));
        return ;
    }
    case AST_VAR: {
        vec_push(&Compiler.valueStack,VALUE_VAR(exp->var));
        return;
    }
    case AST_IF: {
        CompileBreakpoint(exp->ifStmt.cond);
        __CompileAST(exp->ifStmt.cond);
        /**
         * jmp !cond->elbranch
         * true
         * jmp endjmp
         * elbranch:
         * false
         * endjmp:
         */
        struct jit_op *elbranch,*endjmp=NULL;
        CValue cond AF_VALUE=vec_pop(&Compiler.valueStack);
        if(cond.type==AST_FLOAT) {
            if(cond.flt) goto alwaystrue;
            else goto alwaysfalse;
        } else if(cond.type==AST_STRING||cond.type==AST_CHAR||cond.type==AST_INT) {
            if(cond.integer) goto alwaystrue;
            else goto alwaysfalse;
        } else {
            CType *ct =AssignTypeToNode(exp->ifStmt.cond);
            if(IsIntegerType(ct)||IsPtrType(ct)) {
                jit_value cr=MoveValueToIntRegIfNeeded(cond, 1);
                elbranch=jit_beqi(Compiler.JIT, (jit_value)NULL, cr, 0);
            } else if(IsF64(ct)) {
                jit_value cr=MoveValueToFltRegIfNeeded(cond, 1);
                elbranch=jit_fbeqi(Compiler.JIT, (jit_value)NULL, cr, 0);
            } else {
                char *cts=TypeToString(ct);
                RaiseError(exp->ifStmt.cond,"Expected arithmetic value for condition(got %s).",cts);
                TD_FREE(cts);
            }
        }
        if(exp->ifStmt.body) {
            //true
            CompileBodyWithBreakpoints(exp->ifStmt.body);
            ReleaseValue(&vec_pop(&Compiler.valueStack));

            if(exp->ifStmt.elseBody)
                endjmp=jit_jmpi(Compiler.JIT, (jit_value)JIT_FORWARD);
        }
        jit_patch(Compiler.JIT, elbranch);
        if(exp->ifStmt.elseBody) {
            CompileBodyWithBreakpoints(exp->ifStmt.elseBody);
            ReleaseValue(&vec_pop(&Compiler.valueStack));
        }
        if(endjmp)
            jit_patch(Compiler.JIT, endjmp);
        //All nodes return a value on the stack
        vec_push(&Compiler.valueStack, VALUE_INT(0));
        return;
alwaystrue:
        CompileBodyWithBreakpoints(exp->ifStmt.body);
        return;
alwaysfalse:
        CompileBodyWithBreakpoints(exp->ifStmt.elseBody);
        return;
    }
    case AST_FOR: {
        if(exp->forStmt.init) {
            CompileBreakpoint(exp->forStmt.init);
            __CompileAST(exp->forStmt.init);
            ReleaseValue(&vec_pop(&Compiler.valueStack));
        }
        struct jit_label *start=jit_get_label(Compiler.JIT);
        struct jit_op *condjmp=NULL;
        if(exp->forStmt.cond) {
            CType *ct =AssignTypeToNode(exp->forStmt.cond);
            if(!IsArith(ct)) {
                char *cts=TypeToString(ct);
                RaiseError(exp,"Expected arithmetic type for for loop");
                TD_FREE(cts);
                vec_push(&Compiler.valueStack,VALUE_INT(0));
            } else {
                CompileBreakpoint(exp->forStmt.cond);
                __CompileAST(exp->forStmt.cond);
                CValue cv AF_VALUE=vec_pop(&Compiler.valueStack);
                jit_value cr=MoveValueToIntRegIfNeeded(cv, 1);
                condjmp=jit_beqi(Compiler.JIT,(jit_value)JIT_FORWARD,cr,0);
            }
        }

        vec_jit_op_t oldbreaks=Compiler.breakops;
        vec_init(&Compiler.breakops);

        CompileBodyWithBreakpoints(exp->forStmt.body);
        ReleaseValue(&vec_pop(&Compiler.valueStack));

        vec_jit_op_t breaks=Compiler.breakops;
        Compiler.breakops=oldbreaks;

        if(exp->forStmt.next) {
            CompileBreakpoint(exp->forStmt.next);
            __CompileAST(exp->forStmt.next);
            ReleaseValue(&vec_pop(&Compiler.valueStack));
        }
        jit_jmpi(Compiler.JIT,start);
        if(condjmp)
            jit_patch(Compiler.JIT,condjmp);
        struct jit_op *b;
        int iter;
        vec_foreach(&breaks, b, iter) {
            jit_patch(Compiler.JIT,b);
        }
        vec_deinit(&breaks);
        //All nodes return a value on the stack
        vec_push(&Compiler.valueStack, VALUE_INT(0));
        return;
    }
    case AST_DO: {
        struct jit_label *start=jit_get_label(Compiler.JIT);
        struct jit_op *condjmp=NULL;
        vec_jit_op_t oldbreaks=Compiler.breakops;
        vec_init(&Compiler.breakops);

        CompileBodyWithBreakpoints(exp->doStmt.body);
        ReleaseValue(&vec_pop(&Compiler.valueStack));

        vec_jit_op_t breaks=Compiler.breakops;
        Compiler.breakops=oldbreaks;
        struct jit_op *b;
        int iter;
        vec_foreach(&breaks, b, iter) {
            jit_patch(Compiler.JIT,b);
        }
        CompileBreakpoint(exp->doStmt.cond);
        __CompileAST(exp->doStmt.cond);
        CValue cond AF_VALUE=vec_pop(&Compiler.valueStack);
        CType *ct =AssignTypeToNode(exp->doStmt.cond);
        if(IsIntegerType(ct)) {
            jit_value cr=MoveValueToIntRegIfNeeded(cond, 1);
            jit_bnei(Compiler.JIT, (jit_value)start, cr, 0);
        } else if(IsF64(ct)) {
            jit_value cr=MoveValueToFltRegIfNeeded(cond, 1);
            jit_fbnei(Compiler.JIT, (jit_value)start, cr, 0);
        } else {
            char *cts=TypeToString(ct);
            RaiseError(exp->doStmt.cond,"Do statement requires and arithmetic type(got type %s).",cts);
            TD_FREE(cts);
        }
        //All nodes return a value on the stack
        vec_push(&Compiler.valueStack, VALUE_INT(0));
        return ;
    }
    case AST_WHILE: {
        struct jit_label *start=jit_get_label(Compiler.JIT);
        CompileBreakpoint(exp->whileStmt.cond);
        __CompileAST(exp->whileStmt.cond);
        CValue cond AF_VALUE=vec_pop(&Compiler.valueStack);
        struct jit_op *branch;
        CType *ct =AssignTypeToNode(exp->whileStmt.cond);
        if(IsIntegerType(ct)) {
            jit_value cr=MoveValueToIntRegIfNeeded(cond, 1);
            branch=jit_beqi(Compiler.JIT, NULL, cr, 0);
        } else if(IsF64(ct)) {
            jit_value cr=MoveValueToFltRegIfNeeded(cond, 1);
            branch=jit_fbeqi(Compiler.JIT, NULL,cr, 0.0);
        } else {
            char *cts=TypeToString(ct);
            RaiseError(exp->whileStmt.cond,"While statement requires and arithmetic type(got type %s).",cts);
            TD_FREE(cts);
        }

        vec_jit_op_t oldbreaks=Compiler.breakops;
        vec_init(&Compiler.breakops);

        CompileBodyWithBreakpoints(exp->whileStmt.body);
        ReleaseValue(&vec_pop(&Compiler.valueStack));
        jit_jmpi(Compiler.JIT,start);

        vec_jit_op_t breaks=Compiler.breakops;
        Compiler.breakops=oldbreaks;
        struct jit_op *b;
        int iter;
        vec_foreach(&breaks, b, iter) {
            jit_patch(Compiler.JIT,b);
        }
        vec_deinit(&breaks);
        jit_patch(Compiler.JIT,branch);
        //All nodes return a value on the stack
        vec_push(&Compiler.valueStack, VALUE_INT(0));
        return ;
    }
    case AST_DECL: {
        CVariable **var;
        if(Compiler.inFunction) {
            var=map_get(&Compiler.locals,exp->decl.name);
        } else {
            var=map_get(&Compiler.globals,exp->decl.name);
        }
        if(!var) {
            RaiseError(exp,"Unknown name %s.",exp->decl.name);
            //All nodes return a value on the stack
            vec_push(&Compiler.valueStack, VALUE_VAR(*var));
            return;
        }
        if(exp->decl.dft) {
            if(exp->decl.dft) {
                if(exp->decl.dft->type==AST_ARRAY_LITERAL) {
                    CDeclTail tmp;
                    memset(&tmp,0,sizeof(tmp));
                    tmp.finalType=exp->decl.type;
                    tmp.dft=exp->decl.dft;
                    CompileCompoundLiteral(GetVariable(exp->decl.name),tmp);
                }
            } else  {
                CType *dt =AssignTypeToNode(exp->decl.dft);
                WarnOnWeirdPass(exp->decl.dft, exp->decl.type, dt);
            }
            CValue dst AF_VALUE=VALUE_VAR(*var);
            __CompileAST(exp->decl.dft);
            CValue src AF_VALUE=vec_pop(&Compiler.valueStack);
            CompileAssign(dst,src);
        }
        //All nodes return a value on the stack
        vec_push(&Compiler.valueStack, VALUE_VAR(*var));
        return;
    }
    case AST_SWITCH: {
        CType *bool =CreatePrimType(TYPE_BOOL);
        CType *i64 =CreatePrimType(TYPE_I64);
        //Push a temp value on the stack,that will detirmine if in sub-switch.
        int sswitr=GetIntTmpReg();
        CVariable *sswitvar AF_VAR=CreateTmpRegVar(sswitr,bool);
        CValue sswitv AF_VALUE=VALUE_VAR(sswitvar);
        vec_push(&Compiler.valueStack,sswitv);
        //Push a temp value on the stack for the switch code
        int coder=GetIntTmpReg();
        CVariable *codevar AF_VAR=CreateTmpRegVar(coder,i64);
        CValue codev AF_VALUE=VALUE_VAR(codevar);
        vec_push(&Compiler.valueStack,codev);
        //
        vec_AST_t cases=FindCasesInSwitch(exp);
        int64_t iter,prevcs=-1;
        AST *cs;
        vec_foreach(&cases, cs, iter) {
            assert(cs->type==AST_CASE);
            if(cs->cs.isUndef) {
                cs->cs.low=++prevcs;
                cs->cs.high=cs->cs.low;
            } else if(!cs->cs.high) {
                cs->cs.high=cs->cs.low;
            } else {
                //Both high and low should be defined
            }
        }
        vec_sort(&cases, CaseCmp);
        int error=0;
        int64_t min=0,max=0;
        //Ensure no overlaps
        vec_foreach(&cases,cs,iter) {
            if(cs->cs.low<min) min=cs->cs.low;
            if(cs->cs.high>max) max=cs->cs.high;

            AST *cs2;
            int iter2;
            vec_foreach(&cases, cs2, iter2) {
                if(iter==iter2) continue;
                //https://stackoverflow.com/questions/3269434/whats-the-most-efficient-way-to-test-two-integer-ranges-for-overlap
                if(!(cs->cs.low<=cs2->cs.high&&cs->cs.high>=cs2->cs.low)) {
                    //ok
                } else {
                    error=1;
                    RaiseError(cs,"Range [%ld,%ld] overlaps with range [%ld,%ld].",cs->cs.low,cs->cs.high,cs2->cs.low,cs2->cs.high);
                    RaiseWarning(cs2,"Overlap from here.");
                }
            }
        }
        uint8_t isDft[max-min+1];
        memset(isDft,1,max-min+1);
        vec_foreach(&cases,cs,iter) {
            int c;
            for(c=cs->cs.low-min; c<=cs->cs.high-min; c++)
                isDft[c]=0;
        }

        struct jit_op *_table[max-min+1];
        memset(_table,0,sizeof(struct jit_op*)*(max-min+1));
        vec_jit_op_t table;
        vec_init(&table);
        vec_pusharr(&table, _table, max-min+1);
        //
        /**
         * subswitv=0
         * start:
         * ...
         * case: subswitv=2
         * ...
         *[subswit-start:]
         * subswitv=2
         * scodelab:
         * if(subswitv==0) {subswitv=1,goto start}
         * case : if(subswitv==0) goto scodelab;
         *
         */
        CompileAssign(sswitv, VALUE_INT(0));
        //
        CompileBreakpoint(exp->switStmt.cond);
        __CompileAST(exp->switStmt.cond);
        CValue cond AF_VALUE=vec_pop(&Compiler.valueStack);
        CompileAssign(codev, cond);
        struct jit_label *start=jit_get_label(Compiler.JIT);
        CType *ct =AssignTypeToNode(exp->switStmt.cond);
        if(!IsArith(ct)) {
            char *cts =TypeToString(ct);
            RaiseError(exp->switStmt.cond,"Switch needs aritmetic condition(have %s).",cts);
            TD_FREE(cts);
        }
        if(R(0)!=coder) jit_movr(Compiler.JIT,R(0),R(coder));
        struct jit_op *overbr=jit_bgti(Compiler.JIT,(jit_value)NULL, R(0), max);
        jit_subi(Compiler.JIT,R(0),R(0),min);
        //Make a latter
        vec_foreach(&cases,cs,iter) {
            int c;
            for(c=cs->cs.low-min; c<=cs->cs.high-min; c++)
                table.data[c]=(void*)cs;
        }
        vec_jit_op_t dfts;
        vec_init(&dfts);
        iter=max-min;
        while(iter>=0) {
            int64_t consec=1;
consec_loop:
            if(iter-consec>=0) {
                if(table.data[iter-consec]==table.data[iter]) {
                    consec++;
                    goto consec_loop;
                }
            }
            long cnt=consec;
            struct jit_op *op=jit_bgti(Compiler.JIT, (jit_value)NULL, R(0), iter-consec);;
            if(!isDft[iter]) {
                while(--cnt>=0) {
                    table.data[iter-cnt]=op;
                }
            } else
                vec_push(&dfts,op);
            iter-=consec;
        }
        vec_push(&dfts,overbr);
        vec_push(&dfts,jit_jmpi(Compiler.JIT, (jit_value)NULL));
        //
        vec_jit_op_t oldbreaks=Compiler.breakops;
        vec_init(&Compiler.breakops);
        __CompileSwitBody(start, &sswitv, exp->switStmt.body, &cases,min, table.data, dfts);
        struct jit_op *op;
        vec_foreach(&Compiler.breakops, op, iter) jit_patch(Compiler.JIT,op);
        vec_deinit(&Compiler.breakops);
        Compiler.breakops=oldbreaks;
        vec_deinit(&cases),vec_deinit(&table),vec_deinit(&dfts);
        //
        vec_pop(&Compiler.valueStack);
        vec_pop(&Compiler.valueStack);
        //All nodes return a value on the stack
        vec_push(&Compiler.valueStack, VALUE_INT(0));
        return;
    }
    case AST_BREAK: {
        struct jit_op *op=jit_jmpi(Compiler.JIT,(jit_value)NULL);
        vec_push(&Compiler.breakops,op);
        //All nodes return a value on the stack
        vec_push(&Compiler.valueStack, VALUE_INT(0));
        break;
    }
    case AST_GOTO: {
        char *lab=exp->labelNode->name;
        if(!Compiler.inFunction) {
            RaiseError(exp,"No goto's in global scope.");
        } else if(!map_get(&Compiler.labels,lab)) {
            struct jit_op *jmp=jit_jmpi(Compiler.JIT, (jit_value)NULL);
            vec_CLabelRef_t *refs;
reglabrefs:
            ;
            if((refs=map_get(&Compiler.labelRefs,lab))) {
              CLabelRef r=(CLabelRef){exp,jmp};
                vec_push(refs,r);
            } else {
                vec_CLabelRef_t new;
                vec_init(&new);
                map_set(&Compiler.labelRefs,lab,new);
                goto reglabrefs;
            }
        } else if(map_get(&Compiler.labels,lab)) {
            jit_jmpi(Compiler.JIT, (jit_value)*map_get(&Compiler.labels,lab));
        }
        //All nodes return a value on the stack
        vec_push(&Compiler.valueStack, VALUE_INT(0));
        break;
    }
    case AST_LOCAL_LABEL:
    case AST_LABEL: {
        char *lab=exp->labelNode->name;
        if(!Compiler.inFunction) {
            RaiseError(exp,"No labels in global scope.");
        } else if(map_get(&Compiler.labels,lab)) {
	  char *readable=GetLabelReadableName(exp->labelNode);
	  RaiseError(exp,"Repeat label \"%s\".",readable);
	  TD_FREE(readable);
        } else if(!map_get(&Compiler.labels,lab)) {
            vec_CLabelRef_t *ops=map_get(&Compiler.labelRefs,lab);
            if(ops) {
                int iter;
                CLabelRef ref;
                vec_foreach(ops, ref, iter)
                jit_patch(Compiler.JIT,ref.op);
                vec_deinit(ops);
                map_remove(&Compiler.labelRefs, lab);
            }
            map_set(&Compiler.labels,lab,jit_get_label(Compiler.JIT));
	    map_set(&Compiler.labelPtrs,lab,NULL);
	    jit_dump_ptr(Compiler.JIT,map_get(&Compiler.labelPtrs,lab));
        }

        //All nodes return a value on the stack
        vec_push(&Compiler.valueStack, VALUE_INT(0));
        break;
    } 
    case AST_EXPORT_LABEL: {
         char *lab=exp->labelNode->name;
	 if(Compiler.inFunction&&!map_get(&Compiler.labels,lab)) {
	   vec_CLabelRef_t *ops=map_get(&Compiler.labelRefs,lab);
	   if(ops) {
	     int iter;
	     CLabelRef ref;
	     vec_foreach(ops, ref, iter)
                jit_patch(Compiler.JIT,ref.op);
	     vec_deinit(ops);
	     map_remove(&Compiler.labelRefs, lab);
	   }
	   map_set(&Compiler.labels,lab,jit_get_label(Compiler.JIT));
	   map_set(&Compiler.labelPtrs,lab,NULL);
	   jit_dump_ptr(Compiler.JIT,map_get(&Compiler.labelPtrs,lab));
	 }
	 //Create a U0* that will point to the label
	 CVariable *exp2=TD_MALLOC(sizeof(CVariable));
	 exp2->isGlobal=1;
	 exp2->name=strdup(lab);
	 exp2->type=CreatePtrType(CreatePrimType(TYPE_U0));
	 exp2->linkage.type=LINK_NORMAL;
	 //POINTER to POINTER
	 exp2->linkage.globalPtr=TD_MALLOC(sizeof(void*));
	 jit_dump_ptr(Compiler.JIT,&exp2->linkage.globalPtr);
	 exp2->fn=strdup(exp->fn);
	 exp2->line=exp->ln;
	 CVariable **exist;
	 if(exist=map_get(&Compiler.globals,lab))
	   ReleaseVariable(*exist);
	 map_set(&Compiler.globals,lab,exp2);
	 //Resolve
      vec_push(&Compiler.valueStack,VALUE_INT(0));
      return;
    }
    default:
        abort();
    }
}
int IsVarRegCanidate(CVariable *var) {
    if(var->isNoreg) return 0;
    if(var->isGlobal) return 0;
    CType *bt =BaseType(var->type);
    int ret;
    if(IsIntegerType(bt)||bt->type==TYPE_PTR) ret=1;
    else if(bt->type==TYPE_F64) ret=1;
    else ret=0;
    return ret;
}
void CompileBreakpoint(AST *at) {
    if(!at->fn) return;
    if(!Compiler.debugMode) return;
    char buffer[1024];
    sprintf(buffer,"%s:%li",at->fn,at->ln);
add:
    ;
    CBreakpoint *bp;
    if(bp=map_get(&Compiler.breakpoints,buffer)) {
    } else {
        CBreakpoint en;
        en.fn=at->fn; //From Lexer.filenames
        en.ln=at->ln;
        en.enabled=0;
        map_set(&Compiler.breakpoints,buffer,en);
        goto add;
    }
    jit_prepare(Compiler.JIT);
    jit_putargi(Compiler.JIT,bp);
    jit_call(Compiler.JIT, VisitBreakpoint);
}
void ReleaseFunction(CFunction*func) {
    CFunction *f=func;
    jit_free(f->JIT);
}
AST* CreateReturn(AST *exp) {
    AST *r=TD_CALLOC(1,sizeof(AST));
    r->refCnt=1;
    r->type=AST_RETURN;
    r->retval=exp;
    return r;
}
static void AssumeArrLitDims(CType *type,vec_AST_t toscan) {
    AST *item;
    int iter;
    if(type->type==TYPE_ARRAY) {
        if(-1==type->array.dim) {
            long max=0;
            vec_foreach(&toscan,item, iter)
            if(item->type==AST_ARRAY_LITERAL)
                max=(max>item->arrayLiteral.length)?max:item->arrayLiteral.length;
            type->array.dim=max;
        }
        if(type->array.base->type==TYPE_ARRAY) {
            vec_AST_t toscan2;
            vec_init(&toscan2);
            vec_foreach(&toscan, item, iter) {
                AST *item2;
                int iter2;
                if(item->type==AST_ARRAY_LITERAL) {
                    vec_foreach(&item->arrayLiteral, item2, iter2)
                    vec_push(&toscan2,item2);
                }
            }
            if(toscan2.length)
                AssumeArrLitDims(type->array.base,toscan2);
            vec_deinit(&toscan2);
        }
    }
}
static void __ScanAssumeArrayDims(AST *node,void *data) {
    if(node->type==AST_DECL) {
        if(node->decl.type->type==TYPE_ARRAY)
            if(node->decl.dft)
                if(node->decl.dft->type==AST_ARRAY_LITERAL) {
                    vec_AST_t one;
                    vec_init(&one);
                    vec_push(&one,node->decl.dft);
                    AssumeArrLitDims(node->decl.type,one);
                    vec_deinit(&one);
                }
    } else if(node->type==__AST_DECL_TAILS) {
        CDeclTail tail;
        int iter;
        vec_foreach(&node->declTail, tail, iter)
        if(tail.finalType->type==TYPE_ARRAY)
            if(tail.dft)
                if(tail.dft->type==AST_ARRAY_LITERAL) {
                    vec_AST_t one;
                    vec_init(&one);
                    vec_push(&one,tail.dft);
                    AssumeArrLitDims(tail.finalType,one);
                    vec_deinit(&one);
                }
    }
}
void __CompileAST(AST *exp) {
    long orig=Compiler.valueStack.length;
    ___CompileAST(exp);
    assert(orig+1==Compiler.valueStack.length);
}
static void __ScanForVars(AST *node,void *data) {
    if(node->type==AST_DECL) {
        AST *dummy =TD_MALLOC(sizeof(AST));
        dummy->refCnt=1;
        dummy->type=AST_NAME;
        dummy->name=strdup(node->decl.name);
        AddVariable(dummy, node->decl.type);
    } else if(node->type==__AST_DECL_TAILS) {
        AddDeclsToScope(node);
    }
}
/**
 * Find items that are addressed by pointer,which includes
 * - addr-of operators(&)
 * - Member accesses
 * - Compund literals
 */
static void __ScanForAddrofs(AST *node,void *data) {
    __ScanAssumeArrayDims(node,data);
    __ScanForVars(node,data);
    CVariable **var;
#define CHECK_OPERAND(a) \
  if((a)->type==AST_NAME) { \
    var=map_get((map_CVariable_t*)data,(a)->name); \
    if(var) var[0]->isNoreg=1; \
  } else if((a)->type==AST_VAR) { \
    (a)->var->isNoreg=1; \
  }
    if(node->type==AST_MEMBER_ACCESS) {
        CHECK_OPERAND(node->memberAccess.a);
    } else if(node->type==AST_ADDROF) {
        CHECK_OPERAND(node->unopArg);
    } else if(node->type==AST_DECL) {
        if(node->decl.dft->type==AST_ARRAY_LITERAL) {
            var=map_get((map_CVariable_t*)data,node->decl.name);
            if(var)
                var[0]->isNoreg=1;
        }
    } else if(node->type==__AST_DECL_TAILS) {
        CDeclTail tail;
        int iter;
        vec_foreach(&node->declTail,tail, iter)
        if(tail.dft)
            if(tail.dft->type==AST_ARRAY_LITERAL)
                if(var=map_get((map_CVariable_t*)data,tail.name->name))
                    var[0]->isNoreg=1;
    }
}
static void Blank() {
    fprintf(stderr,"<Called nil function>\n");
}
/**
 * Expression,function args(CVariable *) are put in ...(terminated with NULL)
 */
CFunction *CompileAST(map_CVariable_t *locals,AST *exp,vec_CVariable_t args) {
    /**
     * R(0)==accmulator
     * R(1)==Dest ptr
     */
    int frameSize=0,ireg=2,freg=2;
    //Add args to scope
    int iter;
    CVariable *arg;
    vec_init(&Compiler.valueStack); //Freed
    vec_init(&Compiler.breakops); //Freed
    map_init(&Compiler.labels); //Freed
    map_init(&Compiler.labelRefs); //Freed
    if(locals!=&Compiler.locals)
    map_init(&Compiler.locals); //Freed
    int assign_offs=0;
    //Find noregs
    if(!locals) { 
      locals=&Compiler.locals;
      vec_foreach(&args, arg, iter) {
        map_set(locals,arg->name,CloneVariable(arg));
      }
      ScanAST(exp,__ScanForAddrofs,&Compiler.locals);
      assign_offs=1;
    } else {
      Compiler.locals=*locals;
    }
    map_CVariable_t dbgNoregs;
    if(Compiler.debugMode)
        map_init(&dbgNoregs);
    if(assign_offs) {
        //Compute frame layout
        map_iter_t liter=map_iter(locals);
        const char *_var;
        while((_var=map_next(locals,&liter))) {
            CVariable *var=*map_get(locals,_var);
            if(var->isNoreg||Compiler.debugMode) {
noreg:
                if(Compiler.debugMode)
                    map_set(&dbgNoregs,var->name,CloneVariable(var));
                var->isReg=0;
                var->isNoreg=1;
                int align=TypeAlign(var->type);
                frameSize+=(align-(frameSize%align))%align;
                var->frameOffset=frameSize;
                frameSize+=TypeSize(var->type);
            } else if(IsVarRegCanidate(var)) {
                if(!IsF64(var->type))
                    var->reg=ireg++;
                else
                    var->reg=freg++;
            } else goto noreg;
        }
    }
    struct jit *oldjit=Compiler.JIT;
    int oldflag=Compiler.errorFlag;
    struct jit *curjit=Compiler.JIT=jit_init();
    void(*funcptr)(int64_t,...);
    CFunction *func=TD_CALLOC(1,sizeof(CFunction));
    func->JIT=curjit;
    func->locals=Compiler.locals;
    if(Compiler.debugMode)
        func->funcInfo.noregs=dbgNoregs;
    else
        map_init(&func->funcInfo.noregs);
    jit_prolog(Compiler.JIT, &funcptr);
    Compiler.tmpFltRegStart=freg;
    Compiler.tmpIntRegStart=ireg;
    if(assign_offs)
      Compiler.frameOffset=jit_allocai(Compiler.JIT, frameSize);
    //Load function arguments
    CVariable *var;
    int argc;
    jit_value forarg[args.length];
    vec_foreach(&args,var,argc) {
        if(IsF64(var->type)) {
            jit_declare_arg(Compiler.JIT, JIT_FLOAT_NUM, 8);
        } else if(IsI64(var->type)) {
            jit_declare_arg(Compiler.JIT, JIT_SIGNED_NUM, TypeSize(var->type));
        } else if(IsU64(var->type)) {
            jit_declare_arg(Compiler.JIT, JIT_UNSIGNED_NUM, TypeSize(var->type));
        } else if(IsPtr(var->type)) {
            jit_declare_arg(Compiler.JIT, JIT_PTR, TypeSize(var->type));
        } else {
            //TODO
            abort();
        }
        if(IsF64(var->type)) {
            if(IsVarRegCanidate(var))
                jit_getarg(Compiler.JIT, FR(var->reg), argc);
            else {
                forarg[argc]=freg++;
                jit_getarg(Compiler.JIT, FR(forarg[argc]), argc);
                //Will load into frame later
            }
        } else if(IsIntegerType(var->type)) {
            if(IsVarRegCanidate(var))
                jit_getarg(Compiler.JIT, R(var->reg), argc);
            else {
                forarg[argc]=ireg++;
                jit_getarg(Compiler.JIT, R(forarg[argc]), argc);
                //Will load into frame later
            }
        } else {
            //TODO
            abort();
        }
    }
    vec_foreach(&args,var,argc) {
      if(IsF64(var->type)) {
          if(IsVarRegCanidate(var))
              ;
          else {
              jit_value freg=forarg[argc];
              CVariable *var2 AF_VAR=CreateTmpRegVar(freg, var->type);
              CValue v1 AF_VALUE=VALUE_VAR(var), v2 AF_VALUE=VALUE_VAR(var2);
              CompileAssign(v1, v2);
          }
      } else if(IsIntegerType(var->type)) {
          if(IsVarRegCanidate(var))
              ;
          else {
              jit_value ireg=forarg[argc];
              CVariable *var2 AF_VAR=CreateTmpRegVar(ireg, var->type);
              CValue v1 AF_VALUE=VALUE_VAR(var), v2 AF_VALUE=VALUE_VAR(var2);
              CompileAssign(v1, v2);
          }
      } else {
          //TODO
          abort();
      }
    }

    //Call enter function
    if(Compiler.debugMode) {
        jit_addi(Compiler.JIT,R(0),R_FP,Compiler.frameOffset);
        jit_prepare(Compiler.JIT);
        jit_putargr(Compiler.JIT, R(0));
        jit_putargi(Compiler.JIT, func);
        jit_call(Compiler.JIT, DbgEnterFunction);
    }
    CompileBodyWithBreakpoints(exp);
    ReleaseValue(&vec_pop(&Compiler.valueStack));
    //jit_dump_ops(Compiler.JIT,0); //DEBUG
    if(Compiler.debugMode) {
        jit_prepare(Compiler.JIT);
        jit_call(Compiler.JIT, DbgLeaveFunction);
    }
    jit_reti(Compiler.JIT,0);
    //if(Compiler.debugMode)
    //    jit_check_code(curjit,JIT_WARN_ALL);
    //if(Compiler.debugMode)
    //    jit_dump_ops(curjit,1);

    //Ensure there is no unresolved labels
    map_iter_t lriter=map_iter(&Compiler.labelRefs);
    const char *key2;
    while(key2=map_next(&Compiler.labelRefs, &lriter)) {
      vec_CLabelRef_t *refs=map_get(&Compiler.labelRefs,key2);
      if(refs->length) {
        CLabelRef ref;
        int iter;
        vec_foreach(refs, ref, iter) {
	  char *readable=GetLabelReadableName(ref.at->labelNode);
          RaiseError(ref.at,"Unresolved label %s.",readable);
	  TD_FREE(readable);
        }
      }
    }
    if(!Compiler.errorFlag) {
        jit_disable_optimization(curjit, JIT_OPT_JOIN_ADDMUL); //Seems to mess things up
        jit_generate_code(curjit,func);
    }
    //if(Compiler.debugMode)
    //    jit_dump_ops(curjit,1);
    if(Compiler.errorFlag)  {
        funcptr=(void(*)(int64_t,...))Blank;
    }
    func->funcptr=funcptr;

    vec_deinit(&Compiler.valueStack);
    map_iter_t miter=map_iter(&Compiler.locals);
    const char *key;
    while(key=map_next(&Compiler.locals, &miter))
        ReleaseVariable(*map_get(&Compiler.locals,key));
    vec_deinit(&Compiler.breakops);
    miter=map_iter(&Compiler.labelRefs);
    while(key=map_next(&Compiler.labelRefs, &miter))
        vec_deinit(map_get(&Compiler.labelRefs,key));
    map_deinit(&Compiler.labelRefs);
    map_deinit(&Compiler.labels);

    Compiler.JIT=oldjit;
    Compiler.errorFlag=oldflag;
    return func;
}
void EvalDebugExpr(CFuncInfo *info,AST *exp,void *framePtr) {
    COldFuncState old=CreateCompilerState();
    //Add args to scope
    int iter;
    CVariable *arg;
    vec_init(&Compiler.valueStack); //Freed
    map_init(&Compiler.locals); //Freed
    map_iter_t liter=map_iter(&info->noregs);
    const char *key;
    while(key=map_next(&info->noregs,&liter)) {
        CVariable *v=*map_get(&info->noregs,key);
        CVariable *g=TD_MALLOC(sizeof(CVariable));
        g->refCount=1;
        g->type=v->type;
        g->isGlobal=1;
        g->isNoreg=1;
        g->linkage.globalPtr=framePtr+v->frameOffset;
        map_set(&Compiler.locals,key,g);
    }

    struct jit *oldjit=Compiler.JIT;
    int oldflag=Compiler.errorFlag;
    struct jit *curjit=Compiler.JIT=jit_init();
    void(*funcptr)(int64_t,...);
    CFunction *func=TD_CALLOC(1,sizeof(CFunction));
    func->JIT=curjit;
    func->locals=Compiler.locals;
    map_init(&func->funcInfo.noregs);
    jit_prolog(Compiler.JIT, &funcptr);
    CType * t =AssignTypeToNode(exp);
    if(t->type==TYPE_ARRAY||t->type==TYPE_ARRAY_REF) {
        CType *d =DerrefedType(t);
        t=CreatePtrType(d);
    }
    uint8_t buffer[TypeSize(t)];
    CVariable *ret AF_VAR=TD_MALLOC(sizeof(CVariable));
    ret->refCount=1;
    ret->type=t;
    ret->isGlobal=1;
    ret->isNoreg=1;
    ret->linkage.globalPtr=buffer;
    AST *vnode =CreateVarNode(ret);
    AST *assign =CreateBinop(vnode,exp,AST_ASSIGN);
    SetPosFromOther(assign, exp);
    __CompileAST(assign);
    ReleaseValue(&vec_pop(&Compiler.valueStack));
    jit_reti(Compiler.JIT,0);
    if(!Compiler.errorFlag) {
        jit_disable_optimization(curjit, JIT_OPT_ALL);
        jit_generate_code(curjit,func);
        func->funcptr=funcptr;
	GC_Disable();
        #ifndef TARGET_WIN32
        signal(SIGINT,SignalHandler);
        func->funcptr(0);
        signal(SIGINT,SIG_IGN);
        #else
        func->funcptr(0);
        #endif
	GC_Enable();
	t=AssignTypeToNode(exp);
        if(t->type==TYPE_ARRAY||t->type==TYPE_ARRAY_REF)
          DbgPrintVar(t, *(void**)buffer); //Arrays return pointer to self
        else
          DbgPrintVar(t, buffer);
    }
    if(Compiler.errorFlag)
        funcptr=(void(*)(int64_t,...))Blank;

    vec_deinit(&Compiler.valueStack);
    Compiler.JIT=oldjit;
    Compiler.errorFlag=oldflag;
    //
    liter=map_iter(&Compiler.locals);
    while(key=map_next(&Compiler.locals,&liter))
        ReleaseVariable(*map_get(&Compiler.locals,key));
    map_deinit(&Compiler.locals);

    RestoreCompilerState(old);
}
void AssignUnionBasetype(AST *t,AST *bt) {
    if(TypeSize(t->type2)>TypeSize(bt->type2)) {
        char *ts=TypeToString(t->type2);
        char *bts=TypeToString(bt->type2);
        RaiseError(bt, "Base type %s is smaller than union %s.",bts,ts);
        TD_FREE(ts),TD_FREE(bts);
    }
    t->type2->un.baseType=bt->type2;
}
void __CompileAssignV2V(CVariable *var,CVariable *var2) {
    if(var->isGlobal)
        __CompileAssignV2GV(var, var2);
    else
        __CompileAssignV2LV(var, var2);
}
static void LinkToUnlinkedUnions(CType *ret,char *name) {
remimp:
    ;
    CType *type;
    int iter;
    vec_foreach(&Compiler.unlinkedImportTypes,type,iter) {
        if(type->type!=TYPE_UNION) continue;
        CLinkage *link=&type->un.linkage;
        if(link->type==LINK__IMPORT) {
            if(0==strcmp(link->importLink,name)) {
                link->type=LINK_EXTERN;
                link->externType=ret;
                vec_remove(&Compiler.unlinkedImportTypes,type);
                goto remimp;
            }
        }
    }
    //Remove resolved externs
remext:
    ;
    vec_foreach(&Compiler.unlinkedExternTypes,type,iter) {
        if(type->type!=TYPE_UNION) continue;
        CLinkage *link=&type->un.linkage;
        if(link->type==LINK_EXTERN) {
            if(link->externType==ret) {
                vec_remove(&Compiler.unlinkedExternTypes,type);
                goto remimp;
            }
        }
    }
}
CType *CreateUnionForwardDecl(AST *linkage,AST *name) {
    if(!name) goto createfwd;
    CType **t=map_get(&Compiler.types,name->name);
    if(t) {
        if(t[0]->type==TYPE_UNION)
            if(t[0]->un.isFwd)
                return t[0];
        map_remove(&Compiler.types, name->name);
createfwd:
        ;
        CType *ret=TD_MALLOC(sizeof(CType));
        if(name) {
          ret->un.fn=name->fn;
          ret->un.ln=name->ln;
        }
        if(linkage)
            ret->un.linkage=CloneTypeLinkage(linkage->linkage);
        else
            ret->un.linkage.type=LINK_NORMAL;
        ret->type=TYPE_UNION;
        ret->refCnt=1;
        ret->un.isFwd=1;
        if(name)
            ret->un.name=strdup(name->name);

        switch(ret->un.linkage.type) {
        case LINK_NORMAL:
            break;
        case LINK__EXTERN:
        case LINK__IMPORT: {
            CType **find=map_get(&Compiler.types,ret->un.linkage.importLink);
            if(find) {
                ret->un.linkage.externType=*find;
                ret->un.linkage.type=LINK_EXTERN;
            }
            break;
        }
        case LINK_EXTERN:
        case LINK_IMPORT:
            //ret->un.linkage.externType=ret->un.linkage.externType;
            ret->un.linkage.type=LINK_EXTERN;
            break;
        }
        if(name)
            map_set(&Compiler.types,name->name,ret);
        if(name) {
            LinkToUnlinkedUnions(ret,name->name);
        }
        return ret;
    }
    goto createfwd;
}
int TypeIsForward(CType *t) {
    CLinkage *linkage=NULL;
    if(t->type==TYPE_CLASS) {
        linkage=&t->cls.linkage;
    } else if(t->type==TYPE_UNION) {
        linkage=&t->un.linkage;
    } else return 0;
    switch(linkage->type) {
    case LINK_NORMAL:
        return (t->type==TYPE_CLASS)?t->cls.isFwd:t->un.isFwd;;
    case LINK_EXTERN:
    case LINK__EXTERN:
        return TypeIsForward(linkage->externType);
    //Imports will implicetly be converted to externs when resolved
    case LINK_IMPORT:
    case LINK__IMPORT:
        return 0;
    }
    return 0;
}
static void LinkToUnlinkedClasses(CType *ret,char *name) {
remimp:
    ;
    CType *type;
    int iter;
    vec_foreach(&Compiler.unlinkedImportTypes,type,iter) {
        if(type->type!=TYPE_CLASS) continue;
        CLinkage *link=&type->cls.linkage;
        if(link->type==LINK__IMPORT) {
            if(0==strcmp(link->importLink,name)) {
                link->type=LINK_EXTERN;
                link->externType=ret;
                vec_remove(&Compiler.unlinkedImportTypes,type);
                goto remimp;
            }
        }
    }
    //Remove resolved externs
remext:
    ;
    vec_foreach(&Compiler.unlinkedExternTypes,type,iter) {
        if(type->type!=TYPE_CLASS) continue;
        CLinkage *link=&type->cls.linkage;
        if(link->type==LINK_EXTERN) {
            if(link->externType==ret) {
                vec_remove(&Compiler.unlinkedExternTypes,type);
                goto remimp;
            }
        }
    }
}
CLinkage CloneVarLinkage(CLinkage link) {
    CLinkage ident=link;
    switch(ident.type) {
    case LINK_NORMAL:
        break;
    case LINK__EXTERN:
    case LINK__IMPORT:
        ident.importLink=strdup(ident.importLink);
        break;
    case LINK_EXTERN:
    case LINK_IMPORT:
        ident.externVar=CloneVariable(ident.externVar);
    }
    return ident;
}
CLinkage CloneTypeLinkage(CLinkage link) {
    CLinkage ident=link;
    switch(ident.type) {
    case LINK_NORMAL:
        break;
    case LINK__EXTERN:
    case LINK__IMPORT:
        ident.importLink=strdup(ident.importLink);
        break;
    case LINK_EXTERN:
    case LINK_IMPORT:
        ident.externType=ident.externType;
    }
    return ident;
}

CType *CreateClassForwardDecl(AST *linkage,AST *name) {
    if(!name) goto createfwd;
    CType **t=map_get(&Compiler.types,name->name);
    if(t) {
        if(t[0]->type==TYPE_CLASS)
            if(t[0]->cls.isFwd)
                return t[0];
        map_remove(&Compiler.types, name->name);
createfwd:
        ;
        CType *ret=TD_MALLOC(sizeof(CType));
        if(name) {
          ret->cls.fn=name->fn;
          ret->cls.ln=name->ln;
        }
        ret->type=TYPE_CLASS;
        ret->refCnt=1;
        ret->cls.isFwd=1;
        if(name)
            ret->cls.name=strdup(name->name);
        if(linkage)
            ret->cls.linkage=CloneTypeLinkage(linkage->linkage);
        else
            ret->cls.linkage.type=LINK_NORMAL;
        switch(ret->cls.linkage.type) {
        case LINK_NORMAL:
            break;
        case LINK__EXTERN:
        case LINK__IMPORT: {
            CType **find=map_get(&Compiler.types,ret->cls.linkage.importLink);
            if(find) {
                ret->cls.linkage.externType=*find;
                ret->cls.linkage.type=LINK_EXTERN;
            }
            break;
        }
        case LINK_EXTERN:
        case LINK_IMPORT:
            //ret->cls.linkage.externType=ret->cls.linkage.externType;
            ret->cls.linkage.type=LINK_EXTERN;
            break;
        }
        if(name)
            map_set(&Compiler.types,name->name,ret);
        if(name)
            LinkToUnlinkedClasses(ret,name->name);
        return ret;
    }
    goto createfwd;
}
void InheritFromType(CType *btt,CType *wt) {
    if(btt->type==TYPE_CLASS) {
        btt->cls.align=TypeAlign(wt);
        btt->cls.size=TypeSize(wt);
#define EXTEND_MEMBERS(__members) \
    CMember mem; \
    int iter; \
    if(wt->type==TYPE_CLASS) { \
      vec_foreach(&wt->cls.members,mem,iter) { \
        mem.type=mem.type; \
        CMetaData meta; \
        int iter2; \
        vec_CMetaData_t old=mem.meta; \
        vec_init(&mem.meta); \
        vec_pusharr(&mem.meta, old.data, old.length); \
        vec_foreach(&old, meta, iter2) { \
          meta.name=meta.name; \
          meta.value=meta.value; \
          mem.meta.data[iter2]=meta; \
        } \
        vec_push(__members, mem); \
      } \
    } else if(wt->type==TYPE_UNION) { \
      vec_foreach(&wt->un.members,mem,iter) { \
        mem.type=mem.type; \
        CMetaData meta; \
        int iter2; \
        vec_CMetaData_t old=mem.meta; \
        vec_init(&mem.meta); \
        vec_pusharr(&mem.meta, old.data, old.length); \
        vec_foreach(&old, meta, iter2) { \
          meta.name=meta.name; \
          meta.value=meta.value; \
          mem.meta.data[iter2]=meta; \
        } \
        vec_push(__members, mem); \
      } \
    }
        EXTEND_MEMBERS(&btt->cls.members);
        btt->cls.inheritsFrom=wt;
    } else if(btt->type==TYPE_UNION) {
        btt->un.align=TypeAlign(wt);
        btt->un.size=TypeSize(wt);
        EXTEND_MEMBERS(&btt->un.members);
        btt->un.inheritsFrom=wt;
    } else
        abort();
};
AST *CreateStaticLinkage() {
    AST *r=TD_MALLOC(sizeof(AST));
    r->refCnt=1;
    r->type=AST_LINKAGE;
    r->linkage.type=LINK_STATIC;
    return r;
}
AST *CreateExternLinkage(AST *_name) {
    AST *r=TD_MALLOC(sizeof(AST));
    r->refCnt=1;
    r->type=AST_LINKAGE;
    r->linkage.type=(_name)?LINK__EXTERN:LINK_EXTERN;
    if(_name)
      r->linkage.importLink=strdup(_name->name);
    return r;
}
AST * ApplyLinkageToDecls(AST *linkage,AST *tails) {
    assert(tails->type==__AST_DECL_TAILS);
    CDeclTail *item;
    int iter;
    vec_foreach_ptr(&tails->declTail,item,iter)
    item->linkage=linkage;
    return tails;
}
AST *CreateImportLinkage(AST *_name) {
    AST *r=TD_MALLOC(sizeof(AST));
    r->refCnt=1;
    r->type=AST_LINKAGE;
    r->linkage.type=(_name)?LINK__IMPORT:LINK_IMPORT;
    if(_name)
        r->linkage.importLink=strdup(_name->name);
    return r;
}
void CreateFuncForwardDecl(AST *linkage,CType *rtype,AST *name,AST *args,int hasvargs) {
    CType *ftype =CreateFuncType(rtype,args,hasvargs);
    if(!linkage)
        goto EXT;
    switch(linkage->linkage.type) {
    case LINK_NORMAL:
    case LINK_EXTERN:
EXT:
        Extern(ftype, name, NULL);
        break;
    case LINK__EXTERN:
        Extern(ftype, name, linkage->linkage.importLink);
        break;
    case LINK_IMPORT:
        Import(ftype, name, NULL);
        break;
    case LINK__IMPORT:
        Import(ftype, name, linkage->linkage.importLink);
        break;
    }
}
CVariable *Extern(CType *type,AST *name,char *importname) {
    if(!importname)
        importname=name->name;
    CVariable **found;
    if(found=map_get(&Compiler.globals,name->name))
      map_remove(&Compiler.globals, name->name);
    CType *bt2 =BaseType(type);
    CVariable *ret=TD_MALLOC(sizeof(CVariable));
    ret->type=type;
    ret->refCount=1;
    ret->isFunc=bt2->type==TYPE_FUNC;
    ret->name=strdup(name->name);
    ret->linkage.type=LINK__IMPORT;
    ret->linkage.dummyPtr=TD_MALLOC(sizeof(void*));
    ret->linkage.importLink=strdup(importname);
    ret->isGlobal=1;
    vec_push(&Compiler.unlinkedImportVars, CloneVariable(ret));
    map_set(&Compiler.globals,ret->name,CloneVariable(ret));
    return ret;
}
CVariable *Import(CType *type,AST *name,char *importname) {
    if(!importname)
        importname=name->name;
    if(map_get(&Compiler.globals,name->name))
      map_remove(&Compiler.globals, name->name);
    CType *bt2 =BaseType(type);
    CVariable *ret=TD_MALLOC(sizeof(CVariable));
    ret->type=type;
    ret->isFunc=bt2->type==TYPE_FUNC;
    ret->refCount=1;
    ret->name=strdup(name->name);
    ret->linkage.type=LINK__IMPORT;
    ret->linkage.dummyPtr=TD_MALLOC(sizeof(void*));
    ret->linkage.importLink=strdup(importname);
    ret->isGlobal=1;
    vec_push(&Compiler.unlinkedImportVars, CloneVariable(ret));
    map_set(&Compiler.globals,ret->name,CloneVariable(ret));
    return ret;
}
COldFuncState EnterFunction(CType *returnType,AST *_args) {
    Compiler.globalLabelCount=0;
    Compiler.returnType=returnType;
    COldFuncState old=CreateCompilerState();
    Compiler.inFunction=1;
    vec_init(&Compiler.asmPatches);
    return old;
}
void LeaveFunction(COldFuncState old) {
    Compiler.inFunction=0;
    Compiler.returnType=NULL;
    int iter;
    CAsmPatch *pat;
    vec_foreach(&Compiler.asmPatches, pat, iter)
      TD_FREE(pat);
    vec_deinit(&Compiler.asmPatches);
    
    RestoreCompilerState(old);
}
void CompileFunction(AST *linkage,CType *rtype,AST *name,AST *args,AST *body,int hasvargs) {
    if(linkage) {
        //Ensire linkage is either static or public
        switch(linkage->linkage.type) {
        case LINK__EXTERN:
        case LINK__IMPORT:
        case LINK_EXTERN:
        case LINK_IMPORT:
        case LINK_NORMAL:
            RaiseError(linkage,"Linkage for function definition must be static if provided.");
            break;
        case LINK_STATIC:
            ;
        }
    }
    if(map_get(&Compiler.globals,name->name)) {
        ReleaseVariable(*map_get(&Compiler.globals,name->name));
        map_remove(&Compiler.globals, name->name);
    }
    CreateFuncForwardDecl(NULL,rtype,name,args,hasvargs);
    COldFuncState oldstate=EnterFunction(rtype,args);
    CType *ftype=CreateFuncType(rtype, args,hasvargs);
    ftype->func.hasvargs=hasvargs;
    char *name2;
    int iter;
    vec_CVariable_t args2;
    vec_init(&args2);
    vec_foreach(&ftype->func.names,name2,iter) {
        CVariable *arg=TD_MALLOC(sizeof(CVariable));
        arg->refCount=1;
        arg->name=strdup(name2);
        arg->type=ftype->func.arguments.data[iter];
        vec_push(&args2,arg);
    }
    CVariable *argc AF_VAR=NULL;
    CVariable *argv AF_VAR=NULL;
    if(hasvargs) {
        argc=TD_MALLOC(sizeof(CVariable));
        argc->refCount=1;
        argc->name=strdup("argc");
        argc->type=CreatePrimType(TYPE_I64);
        vec_push(&args2, argc);
        argv=TD_MALLOC(sizeof(CVariable));
        argv->refCount=1;
        argv->name=strdup("argv");
        argv->type=CreatePtrType(argc->type);
        vec_push(&args2, argv);
    }
    CFunction *f=CompileAST(NULL, body, args2);
    Assembler.active=1;
    ApplyPatches();
    Assembler.active=0;
    if(name)
        f->name=strdup(name->name);
    LeaveFunction(oldstate);
    GC_SetDestroy(f,ReleaseFunction);;
    __AddFunctionVar(name,ftype,f);
    vec_deinit(&args2);
    CVariable *fv=*map_get(&Compiler.globals,name->name);
    fv->isFunc=1;
    fv->func=f;
}
void RunStatement(AST *s) {
    if(s->type==AST_NOP) return;
    COldFuncState old=CreateCompilerState();
    vec_CVariable_t empty;
    vec_init(&empty);
    map_CVariable_t locals;
    map_init(&locals);
    CType *t =CreatePrimType(TYPE_I64);
    AST *tnode =CreateTypeNode(t);
    Compiler.returnType=t;
    int oldflag=Compiler.errorFlag;
    CFunction *stmt=CompileAST(NULL,s,empty);
    if(stmt&&!Compiler.errorFlag&&!Compiler.tagsFile) {
      GC_Disable();
      #ifndef TARGET_WIN32
      signal(SIGINT,SignalHandler);
      ((void(*)())stmt->funcptr)();
      signal(SIGINT,SIG_IGN);
      #else
      ((void(*)())stmt->funcptr)();
      #endif
      GC_Enable();
    }
    Compiler.errorFlag=oldflag;
    vec_deinit(&empty);
    map_deinit(&locals);
    RestoreCompilerState(old);
}
AST *AppendToArrLiteral(AST *start,AST *end) {
    if(!start) {
        AST *r=TD_CALLOC(1,sizeof(AST));
        r->type=AST_ARRAY_LITERAL;
        r->refCnt=1;
        vec_push(&r->arrayLiteral,end);
        return r;
    }
    if(start->type==AST_ARRAY_LITERAL) {
        vec_push(&start->arrayLiteral,end);
        return start;
    }
    AST *r=TD_CALLOC(1,sizeof(AST));
    r->type=AST_ARRAY_LITERAL;
    r->refCnt=1;
    vec_push(&r->arrayLiteral,start);
    vec_push(&r->arrayLiteral,end);
    return r;
}
void ScanAST(AST *t,void(*func)(AST *,void *),void *data) {
    if(!t) return;
    func(t,data);
    int _iter;
    AST *_iterv;
#define SCANVEC(vec) vec_foreach(&(vec),_iterv,_iter) ScanAST(_iterv,func,data);
    switch(t->type) {
    case AST_ARRAY_LITERAL: {
        SCANVEC(t->arrayLiteral);
        break;
    }
    case AST_ASM_OPCODE: {
      ScanAST(t->asmOpcode.name,func,data);
      SCANVEC(t->asmOpcode.operands);
      break;
    }
    case AST_ASM_ADDR: {
      ScanAST(t->asmAddr.base,func,data);
      ScanAST(t->asmAddr.disp,func,data);
      ScanAST(t->asmAddr.index,func,data);
      ScanAST(t->asmAddr.scale,func,data);
      ScanAST(t->asmAddr.segment,func,data);
      break;
    }
    case __AST_DECL_TAILS: {
        int iter;
        CDeclTail tail;
        vec_foreach(&t->declTail, tail, iter) {
            ScanAST(tail.dft,func,data);
            ScanAST(tail.metaData,func,data);
            ScanAST(tail.name,func,data);
        }
        break;
    }
    case AST_NOP:
        break;
    case AST_STMT_GROUP: {
        SCANVEC(t->stmts);
        break;
    }
    case AST_NAME:
        break;
    case AST_MEMBER_ACCESS:
        ScanAST(t->memberAccess.a,func,data);
        break;
    case AST_PAREN:
unop:
        ScanAST(t->unopArg,func,data);
        break;
    case AST_INT:
        break;
    case AST_FLOAT:
        break;
    case AST_CHAR:
        break;
    case AST_STRING:
        break;
    case AST_COMMA:
binop:
        ScanAST(t->binop.a,func,data);
        ScanAST(t->binop.b,func,data);
        break;
    case AST_SHL:
        goto binop;
    case AST_SHR:
        goto binop;
    case AST_POW:
        goto binop;
    //
    case AST_MUL:
        goto binop;
    case AST_DIV:
        goto binop;
    case AST_MOD:
        goto binop;
    //
    case AST_BAND:
        goto binop;
    case AST_BXOR:
        goto binop;
    case AST_BOR:
        goto binop;
    //
    case AST_ADD:
        goto binop;
    case AST_SUB:
        goto binop;
    //
    case AST_LT:
        goto binop;
    case AST_GT:
        goto binop;
    case AST_LE:
        goto binop;
    case AST_GE:
        goto binop;
    case AST_RANGE: {
        SCANVEC(t->range.operands);
        break;
    }
    //
    case AST_EQ:
        goto binop;
    case AST_NE:
        goto binop;
    //
    case AST_LAND:
        goto binop;
    case AST_LXOR:
        goto binop;
    case AST_LOR:
        goto binop;
    //
    case AST_ASSIGN:
        goto binop;
    case AST_ASSIGN_SHL:
        goto binop;
    case AST_ASSIGN_SHR:
        goto binop;
    case AST_ASSIGN_MUL:
        goto binop;
    case AST_ASSIGN_MOD:
        goto binop;
    case AST_ASSIGN_DIV:
        goto binop;
    case AST_ASSIGN_BAND:
        goto binop;
    case AST_ASSIGN_BOR:
        goto binop;
    case AST_ASSIGN_BXOR:
        goto binop;
    case AST_ASSIGN_ADD:
        goto binop;
    case AST_ASSIGN_SUB:
        goto binop;
    //
    case AST_POST_INC:
        goto unop;
    case AST_POST_DEC:
        goto unop;
    //
    case AST_ARRAY_ACCESS:
        goto binop;
    //
    case AST_PRE_INC:
        goto unop;
    case AST_PRE_DEC:
        goto unop;
    //
    case AST_POS:
        goto unop;
    case AST_NEG:
        goto unop;
    //
    case AST_LNOT:
        goto unop;
    case AST_BNOT:
        goto unop;
    //
    case AST_DERREF:
        goto unop;
    case AST_ADDROF:
        goto unop;
    //
    case AST_SIZEOF:
        ScanAST(t->sizeofExp,func,data);
        break;
    //
    case AST_TOKEN:
        break;
    //
    case AST_IMPLICIT_TYPECAST:
    case AST_EXPLICIT_TYPECAST:
        ScanAST(t->typecast.base,func,data);
        ScanAST(t->typecast.toType,func,data);
        break;
    //
    case AST_FUNC_CALL:
        ScanAST(t->funcCall.func,func,data);
        ScanAST(t->funcCall.args,func,data);
        break;
    //
    case AST_IF:
        ScanAST(t->ifStmt.body,func,data);
        ScanAST(t->ifStmt.cond,func,data);
        ScanAST(t->ifStmt.elseBody,func,data);
        break;
    case AST_FOR:
        ScanAST(t->forStmt.body,func,data);
        ScanAST(t->forStmt.cond,func,data);
        ScanAST(t->forStmt.init,func,data);
        ScanAST(t->forStmt.next,func,data);
        break;
    case AST_DO:
        ScanAST(t->doStmt.body,func,data);
        ScanAST(t->doStmt.cond,func,data);
        break;
    case AST_WHILE:
        ScanAST(t->whileStmt.body,func,data);
        ScanAST(t->whileStmt.cond,func,data);
        break;
    //
    case AST_METADATA: {
        int iter;
        CMetaData meta;
        vec_foreach(&t->metaData, meta, iter) {
            ScanAST(meta.name,func,data);
            ScanAST(meta.value,func,data);
        }
        break;
    }
    //
    case AST_DECL: {
        ScanAST(t->decl.dft,func,data);
        break;
    }
    case AST_VAR:
        break;
    //
    case AST_KEYWORD:
        break;
    //
    case AST_CASE:
        break;
    case AST_SWITCH:
        ScanAST(t->switStmt.body,func,data);
        ScanAST(t->switStmt.cond,func,data);
        break;
    case AST_SUBSWITCH:
        ScanAST(t->subswitch.body,func,data);
        ScanAST(t->subswitch.body,func,data);
        break;
    case AST_BREAK:
        break;
    case AST_DEFAULT:
        break;
    //
    case AST_RETURN:
        ScanAST(t->retval,func,data);
        break;
    //
    case AST_TYPE:
        break;
    //
    case AST_GOTO:
        ScanAST(t->labelNode,func,data);
        break;
    case AST_LABEL:
        ScanAST(t->labelNode,func,data);
        break;
    //
    case AST_TRY:
        ScanAST(t->try.catch,func,data);
        ScanAST(t->try.try,func,data);
        break;
    case AST_PRINT_STRING:
        break;
    }
}
