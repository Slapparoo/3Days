#include "3d.h"
#ifdef TARGET_WIN32
#include "memoryapi.h"
#else
#include <sys/mman.h>
#endif
static char *MStrCat(char *a,char *b);
static char *Parenify(char *t) {
    return MStrCat(MStrCat(strdup("("),t),strdup(")"));
}
static char *DumpExpr(AST *expr) {
    char buffer[128];
    switch(expr->type) {
    case AST_TYPE: return TypeToString(expr->type2);
    case AST_NAME: return strdup(expr->name);
    case AST_MEMBER_ACCESS:  {
        CType *discard AF_TYPE=NULL;
        char *first=DumpExpr(expr->memberAccess.a);
        if(IsPtr(discard=AssignTypeToNode(expr->memberAccess.a)))
            return MStrCat(MStrCat(first,strdup("->")),strdup(expr->memberAccess.member));
        return MStrCat(MStrCat(first,strdup(".")),strdup(expr->memberAccess.member));
    }
    case AST_LASTCLASS: return strdup("lastclass");
    case AST_PAREN:
    return Parenify(DumpExpr(expr->unopArg));
    case AST_CHAR:
    case AST_INT: {
        sprintf(buffer,"(%lld)",(long long)expr->integer);
        return strdup(buffer);
    }
    case AST_FLOAT: {
        union {
            double f64;
            int64_t i;
        } un;
        un.f64=expr->floating;
        sprintf(buffer,"((%lld)(F64))",(long long)un.i);
        return strdup(buffer);
    }
    case AST_STRING: {
        char *s=TD_MALLOC(strlen(expr->string)*4+1);
        unescapeString(expr->string,s);
        return MStrCat(MStrCat(strdup("\""),s),strdup("\""));
    }
    case AST_COMMA:
    #define BINOP(op) \
    {char *middle=MStrCat(MStrCat(DumpExpr(expr->binop.a),strdup(op)),DumpExpr(expr->binop.b)); \
    return Parenify(middle);}
    BINOP(",");
    case AST_SHL: BINOP("<<");
    case AST_SHR: BINOP(">>");
    case AST_POW: BINOP("`");
    case AST_MUL: BINOP("*");
    case AST_MOD: BINOP("%");
    case AST_BAND: BINOP("&");
    case AST_BXOR: BINOP("^");
    case AST_BOR: BINOP("|");
    case AST_ADD: BINOP("+");
    case AST_SUB: BINOP("-");
    case AST_LT: BINOP("<");
    case AST_GT: BINOP(">");
    case AST_LE: BINOP("<=");
    case AST_GE: BINOP(">=");
    case AST_LAND: BINOP("&&");
    case AST_LXOR: BINOP("^^");
    case AST_LOR: BINOP("||");
    case AST_ASSIGN: BINOP("=");
    case AST_ASSIGN_SHL: BINOP("<<=");
    case AST_ASSIGN_SHR: BINOP(">>=");
    case AST_ASSIGN_MUL: BINOP("*=");
    case AST_ASSIGN_MOD: BINOP("%=");
    case AST_ASSIGN_DIV: BINOP("/=");
    case AST_ASSIGN_BAND: BINOP("&=");
    case AST_ASSIGN_BOR: BINOP("|=");
    case AST_ASSIGN_BXOR: BINOP("^=");
    case AST_ASSIGN_ADD: BINOP("+=");
    case AST_ASSIGN_SUB: BINOP("-=");
    case AST_POST_INC:
    return Parenify(MStrCat(DumpExpr(expr->unopArg),strdup("++")));
    case AST_POST_DEC:
    return Parenify(MStrCat(DumpExpr(expr->unopArg),strdup("--")));
    case AST_ARRAY_ACCESS:;
    char *ret=MStrCat(MStrCat(strdup("("),DumpExpr(expr->arrayAccess.array)),strdup(")"));
    ret=MStrCat(ret,strdup("["));
    ret=MStrCat(ret,DumpExpr(expr->arrayAccess.subscript));
    ret=MStrCat(ret,strdup("]"));
    return ret;
    case AST_PRE_DEC:
        return Parenify(MStrCat(strdup("++"),DumpExpr(expr->unopArg)));
    case AST_PRE_INC:
        return Parenify(MStrCat(strdup("--"),DumpExpr(expr->unopArg)));
    case AST_POS:
        return Parenify(MStrCat(strdup("+"),DumpExpr(expr->unopArg)));
    case AST_NEG:
        return Parenify(MStrCat(strdup("-"),DumpExpr(expr->unopArg)));
    case AST_LNOT:
        return Parenify(MStrCat(strdup("!"),DumpExpr(expr->unopArg)));
    case AST_BNOT:
        return Parenify(MStrCat(strdup("~"),DumpExpr(expr->unopArg)));
    case AST_SIZEOF:
        return MStrCat(MStrCat(strdup("sizeof("),DumpExpr(expr->sizeofExp)),strdup(")"));
    case AST_DERREF:
        return Parenify(MStrCat(strdup("*"),DumpExpr(expr->unopArg)));
    case AST_ADDROF:
        return Parenify(MStrCat(strdup("&"),DumpExpr(expr->unopArg)));
    case AST_EXPLICIT_TYPECAST: {
        char *ret=MStrCat(DumpExpr(expr->typecast.base),strdup("("));
        ret=MStrCat(ret,DumpExpr(expr->typecast.toType));
        ret=MStrCat(ret,strdup(")"));
        return Parenify(ret);
    }
    case AST_FUNC_CALL:;
        ret=MStrCat(DumpExpr(expr->funcCall.func),strdup("("));
        AST *a;
        long iter;
        vec_AST_t args=CommaToVec(expr->funcCall.args);
        vec_foreach(&args,a,iter) {
            if(a) ret=MStrCat(ret,DumpExpr(a));
            if(iter+1<args.length)
                ret=MStrCat(ret,strdup(","));
            else
                ret=MStrCat(ret,strdup(")"));
        }
        vec_deinit(&args);
        return Parenify(ret);

    default: abort();
    }
}
static char *SerializeMembers(vec_CMember_t *members,long start);
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
 #define LABEL_LOCAL 0
 #define LABEL_NORMAL 1
 #define LABEL_EXPORTED 2
 typedef struct  {
    int64_t is_rel;
    int64_t rel_offset;
    int64_t width;
    int64_t offset;
    int64_t labelContext;
 } CBinAsmPatch;
 typedef struct {
    int64_t offset;
    //0 for local
    //1 for normal
    //2 For exported
    int64_t type;;
    char name;
 } CBinLabel;
 typedef struct {
    int64_t size;
    char name;
 } CBinStatic;
typedef struct {
    int64_t offset;
    //This member is terminated with '\0'
    char name;
} CBinPatch;
typedef struct {
    int64_t offset;
    int64_t len;
    char str;
} CBinString;
typedef vec_t(CBinString*) vec_CBinStringp_t;
typedef vec_t(CBinPatch*) vec_CBinPatchp_t;
typedef vec_t(CBinStatic*) vec_CBinStaticp_t;
void SerializeFunction(FILE *f,CFunction *func) {
    if(func->type) {
        //First serialize dft argument variables,TempleOS computes dft argumnets when the function is defined
        AST *dft;long dfti;
        vec_foreach(&func->type->func.dftArgs,dft,dfti) {
            if(!dft) continue;
            if(dft->type==AST_VAR)
                SerializeVar(f,dft->var);
        }
    }
    //
    fputc(1,f);
    if(func->name) fputs(func->name,f); //NAME
    fputc(0,f);
    vec_CBinPatchp_t patches;
    vec_init(&patches);
    map_iter_t iter=map_iter(&func->relocations);
    const char *key;
    vec_CBinStaticp_t statics;
    vec_init(&statics);
    while(key=map_next(&func->relocations,&iter)) {
        CVariable **stat;
        if(stat=map_get(&func->statics,key)) {
            CBinStatic *stat2=TD_MALLOC(sizeof(int64_t)+strlen(key)+1);
            stat2->size=TypeSize(stat[0]->type);
            strcpy(&stat2->name,key);
            vec_push(&statics,stat2);
        }
        vec_voidppp_t *relocs=map_get(&func->relocations,key);
        long iter;
        void ***ptr;
        vec_foreach(relocs,ptr,iter) {
            CBinPatch *pat=TD_MALLOC(sizeof(int64_t)+strlen(key)+1);
            pat->offset=((char*)*ptr)-(char*)func->funcptr;
            strcpy(&pat->name,key);
            vec_push(&patches,pat);
        }
    }
    int64_t pcount=patches.length; //PATCH COUNT
    fwrite(&pcount,1,sizeof(pcount),f);
    //PATCHES
    {
        long iter;
        CBinPatch *p;
        vec_foreach(&patches,p,iter) {
            fwrite(p,MSize(p),1,f);
        }
    }
    //STRINGS
    vec_CBinStringp_t strings;
    vec_init(&strings);
    iter=map_iter(&func->stringRelocations);
    while(key=map_next(&func->stringRelocations,&iter)) {
        vec_voidppp_t *relocs=map_get(&func->stringRelocations,key);
        long iter;
        void ***ptr;
        vec_foreach(relocs,ptr,iter) {
            CBinString *pat=TD_MALLOC(2*sizeof(int64_t)+strlen(key)+1);
            pat->offset=((char*)*ptr)-(char*)func->funcptr;
            pat->len=strlen(key)+1;
            strcpy(&pat->str,key);
            vec_push(&strings,pat);
        }
    }
    {
        int64_t scount=strings.length;
        fwrite(&scount,1,sizeof(scount),f);
        long iter;
        CBinString *p;
        vec_foreach(&strings,p,iter) {
            fwrite(p,MSize(p),1,f);
            TD_FREE(p);
        }
    }
    //STATICS
    {
        int64_t scount=statics.length;
        fwrite(&scount,1,sizeof(scount),f);
        long iter;
        CBinStatic *stat;
        vec_foreach(&statics,stat,iter) {
            fwrite(stat,MSize(stat),1,f);
            TD_FREE(stat);
        }
        vec_deinit(&statics);
    }
    //LABEL_PTRS
    {
        map_void_t *labels=&func->labelPtrs;
        int64_t count=func->labelPtrs.base.nnodes;
        fwrite(&count,1,sizeof(count),f);
        map_iter_t iter=map_iter(labels);
        while(key=map_next(labels,&iter)) {
            long ul;char buffer[256];
            void *ptr=*map_get(labels,key);
            CBinLabel *lab=TD_MALLOC(2*sizeof(int64_t)+strlen(key)+1);
            void *glob=NULL;
            if(map_get(&Compiler.exportedLabels,key)) glob=GetGlobalPtr(*map_get(&Compiler.exportedLabels,key));
            if(ptr==glob)
                lab->type=LABEL_EXPORTED;
            else if(!sscanf(key, LOCAL_LAB_FMT,&ul,buffer))
                lab->type=LABEL_LOCAL;
            else
                lab->type=LABEL_NORMAL;
            lab->offset=(char*)ptr-(char*)func->funcptr;
            strcpy(&lab->name,key);
            fwrite(lab,1,MSize(lab),f);
            TD_FREE(lab);
        }
    }
    //BINSIZE
    int64_t binsize=func->funcInfo.funcSize;
    fwrite(&binsize,1,sizeof(pcount),f);
    //BINARY
    fwrite(func->funcptr,1,binsize,f);
    //ASM PATCHES
    int64_t patch_count=func->asmPatches.length;
    fwrite(&patch_count,1,sizeof(patch_count),f);
    while(--patch_count>=0) {
        CAsmPatch *orig=func->asmPatches.data[patch_count];
        CBinAsmPatch *ap=TD_MALLOC(sizeof(CBinAsmPatch));
        ap->is_rel=orig->isRel;
        ap->labelContext=orig->context;
        ap->offset=(char*)(orig->ptr)-(char*)(func->funcptr);
        ap->rel_offset=orig->rel_offset;
        ap->width=orig->width;
        fwrite(ap,1,MSize(ap),f);
        TD_FREE(ap);
        SerializeFunction(f,orig->apply);
    }
}
/**
 * Values are defined by function @@Main
 * BinVar: U8 ident=0;
 *         U8 is_internal(to module);
 *         U8 name[];
 *         I64 size;
 */
void SerializeVar(FILE *f,CVariable *var) {
    if(map_get(&Compiler.exportedLabels,var->name))
        if(*map_get(&Compiler.exportedLabels,var->name)==var)
            return;
    fputc(0,f); //IDENT
    fputc(var->isInternal,f);
    fputs(var->name,f); //NAME
    fputc(0,f);
    int64_t binsize=TypeSize(var->type);
    fwrite(&binsize,1,sizeof(binsize),f);
}
void SerializeModule(FILE *f) {
    AST *all=TD_MALLOC(sizeof(AST));
    all->type=AST_STMT_GROUP;
    vec_pusharr(&all->stmts,Compiler.AOTGlobalStmts.data,Compiler.AOTGlobalStmts.length);
    vec_CVariable_t empty;
    vec_init(&empty);
    map_remove(&Compiler.globals,"@@Main");
    COldFuncState old=CreateCompilerState();
    //GLOBALS ARE PREVIOISLY ADDED,SO ALLOW REDCLARATIONS
    Compiler.allowRedeclarations=1;
    CFunction *aamain=CompileAST(NULL,all,empty,C_AST_FRAME_OFF_DFT,0);
    RestoreCompilerState(old);
    aamain->name=strdup("@@Main");
    TD_FREE(all);
    SerializeFunction(f,aamain);
    ReleaseFunction(aamain);
    map_remove(&Compiler.globals,"@@Main");
    //
    map_iter_t iter=map_iter(&Compiler.globals);
    const char *key;
    while(key=map_next(&Compiler.globals,&iter)) {
        CVariable *var=GetVariable((char*)key);
        if(var->isBuiltin) continue;
        if(var->linkage.type!=LINK_NORMAL&&var->linkage.type!=LINK_STATIC) continue;
        if(var->isFunc) {
            SerializeFunction(f,var->func);
        } else {
            SerializeVar(f,var);
        }
    }
    //HEADER IDENT
    fputc(2,f);
    MakeHeaderForModule(f);
}
static char *MStrCat(char *a,char *b) {
    char buffer[strlen(a)+strlen(b)+1];
    strcat(strcpy(buffer,a),b);
    TD_FREE(a),TD_FREE(b);
    return strdup(buffer);
}
char *DumpTypeWithName(CType *t,char *name) {
    char buffer[1024];
    char *base;
    switch(t->type) {
    case TYPE_ARRAY:
    case TYPE_ARRAY_REF: {
        base=DumpTypeWithName(t->array.base,name);
        if(t->array.dim!=-1) {
            sprintf(buffer,"%s[%ld]",base,t->array.dim);
        } else {
            sprintf(buffer,"%s[]",base);
        }
        TD_FREE(base);
        return strdup(buffer);
        break;
    }
    case TYPE_PTR: {
        if(t->ptr->type==TYPE_FUNC) {
            base=TypeToString(t->ptr->func.ret);
            base=MStrCat(base,strdup("(*"));
            base=MStrCat(base,strdup(name));
            base=MStrCat(base,strdup(")("));
            t=t->ptr;
            goto args;
        } else {
            base=TypeToString(t);
            base=MStrCat(base,strdup(" "));
            base=MStrCat(base,strdup(name));
            return base;
        }
        break;
    }
    case TYPE_FUNC: {
        base=TypeToString(t->array.base);
        sprintf(buffer,"%s %s(",base,name);
        base=strdup(buffer);
        args:;
        CType *argt=NULL;long iter;
        vec_foreach(&t->func.arguments,argt,iter) {
            if(t->func.names.data[iter]) {
                char *base2=DumpTypeWithName(argt,t->func.names.data[iter]);
                base=MStrCat(base,base2);
                if(!t->func.origDftArgs.data[iter]) {
                } else {
                    base=MStrCat(base,strdup("="));
                    char *dft=DumpExpr(t->func.origDftArgs.data[iter]);
                    base=MStrCat(base,dft);
                }
            } else {
                base=MStrCat(base,TypeToString(argt));
            }
            if(iter+1<t->func.arguments.length)
                base=MStrCat(base,strdup(","));
            else if(t->func.hasvargs) {
                if(t)
                    base=MStrCat(base,strdup(",..."));
                else
                    base=MStrCat(base,strdup("..."));
            }
        }
        base=MStrCat(base,strdup(")"));
        return base;
    }
    case TYPE_UNION:{
        if(!t->un.name) {
            return MStrCat(MStrCat(strdup("union "),SerializeMembers(&t->un.members,0)),strdup(name));
        } else goto normal;
    }
    case TYPE_CLASS: {
        if(!t->cls.name) {
            return MStrCat(MStrCat(strdup("class "),SerializeMembers(&t->cls.members,0)),strdup(name));
        } else goto normal;
    }
    case TYPE_BOOL:
    case TYPE_F64:
    case TYPE_I16:
    case TYPE_I32:
    case TYPE_I64:
    case TYPE_I8:
    case TYPE_U16:
    case TYPE_U32:
    case TYPE_U64:
    case TYPE_U8:
    case TYPE_U0: {
        normal:;
        char *base=TypeToString(t);
        sprintf(buffer,"%s %s",base,name);
        TD_FREE(base);
        return strdup(buffer);
        break;
    }
    }
}
typedef vec_t(vec_int_t) vec_vec_int_t;
typedef map_t(vec_vec_int_t) map_vec_vec_int_t;
static int longCmp(const void *a,const void *b) {
    return *(long*)a-*(long*)b;
}
static long GetFirstMemberAtOffset(vec_CMember_t *members,long start,long offset) {
    CMember mem;
    long iter;
    vec_foreach(members,mem,iter) {
        if(iter<start) continue;
        if(mem.offset==offset) return iter;
    }
    abort();
}
static char *DumpStreaks(char *ret,long *start_at,vec_CMember_t *members,map_vec_vec_int_t *starts,int where) {
    char buffer[18];
    sprintf(buffer,"%ld",where);
    vec_vec_int_t *streaks=map_get(starts,buffer);
    long iter2,iter;
    int idx,added=0;
    vec_int_t streak;
    vec_foreach(streaks,streak,iter) {
        ret=MStrCat(ret,strdup("class {\n"));
        vec_foreach(&streak,idx,iter2) {
            if(idx!=*start_at) continue;
            else (*start_at)++;
            CMember mem=members->data[idx];
            sprintf(buffer,"%ld",mem.offset);
            vec_vec_int_t *find;
            ret=MStrCat(ret,DumpTypeWithName(mem.type,mem.name));
            ret=MStrCat(ret,strdup(";\n"));
            added++;
        }
        if(*start_at>members->length) {
            ret=MStrCat(ret,strdup("};\n"));
            break;
        }
        CMember mem=members->data[*start_at];
        /**
         * If union procedes member,group in class,else(LOOK BELOW)
         */
        if(!(mem.offset>where))  {
            ret=MStrCat(ret,strdup("};\n"));
            continue;
        }
        sprintf(buffer,"%ld",mem.offset);
        vec_vec_int_t *find;
        if((find=map_get(starts,buffer))&&!(mem.offset==where)) {
            ret=MStrCat(ret,strdup("union {\n"));
            ret=DumpStreaks(ret,start_at,members,starts,mem.offset);
            ret=MStrCat(ret,strdup("};\n"));
        }
        /**
         * Go after class.
         */
        if(mem.offset>where) ret=MStrCat(ret,strdup("};\n"));
    }
    if(!added) {
        TD_FREE(ret);
        return strdup("");
    }
    return ret;
}
static char *__SerializeMembers(vec_CMember_t *members,long start,long *written) {
    long current_start=members->data[start].offset;
    long orig_start=current_start;
    long break_start=current_start;
    //Check for sequntal members,if a member precedes or starts at another members offset,it is put in that members start location(which in a union)
    map_vec_vec_int_t starts;
    map_init(&starts);
    long iter=start;
    char *ret=strdup("");
    char buffer[18];
    long switched=0;
    loop:
    //Find union starts
    for(;iter!=members->length;iter++) {
        if(iter+1<members->length) {
            long coff=members->data[iter].offset;
            long noff=members->data[iter+1].offset;
            //Out of order
            if(coff>=noff) {
                sprintf(buffer,"%ld",current_start);
                //Make a union start
                #define REGISTER_START \
                current_start=(coff<noff)?coff:noff; \
                vec_vec_int_t *prev; \
                if(prev=map_get(&starts,buffer)) { \
                } else { \
                    vec_vec_int_t v; \
                    vec_init(&v); \
                    map_set(&starts,buffer,v); \
                }
                REGISTER_START;
                continue;
            }
        }
    }
    sprintf(buffer,"%ld",current_start);
    vec_vec_int_t *prev;
    if(prev=map_get(&starts,buffer)) {
    } else {
        vec_vec_int_t v;
        vec_init(&v);
        map_set(&starts,buffer,v);
    }
    sprintf(buffer,"%ld",members->data[start].offset);
    vec_vec_int_t*streaks=map_get(&starts,buffer);
    assert(streaks);
    //Find streaks
    vec_int_t cur_streak;
    vec_init(&cur_streak);
    current_start=orig_start;
    for(iter=start;iter!=members->length;iter++) {
        sprintf(buffer,"%ld",members->data[iter].offset);
        if(map_get(&starts,buffer)&&cur_streak.length) {
            sprintf(buffer,"%ld",current_start);
            vec_vec_int_t *streaks=map_get(&starts,buffer);
            current_start=members->data[iter].offset;
            vec_push(streaks,cur_streak);
            vec_init(&cur_streak);
        }
        vec_push(&cur_streak,iter);
    }
    if(cur_streak.length) {
        sprintf(buffer,"%ld",current_start);
        vec_vec_int_t *streaks=map_get(&starts,buffer);
        vec_push(streaks,cur_streak);
    }
    return DumpStreaks(ret,&start,members,&starts,members->data[start].offset);
}
static char *SerializeMembers(vec_CMember_t *members,long start) {
    char *ret=MStrCat(strdup("{union{"),__SerializeMembers(members,start,NULL));
    ret=MStrCat(ret,strdup("};}"));
    return ret;
}
typedef struct {
    vec_str_t in;
    vec_str_t out;
    long visited;
    long indegree;
} CTypeGraph;
typedef map_t(CTypeGraph) map_CTypeGraph_t;
static char *__MakeOrGetTypeNode(map_CTypeGraph_t *world,CType *t) {
    if(TypeIsForward(t)) return NULL;
    if(t->isBuiltin) return NULL;
    if(t->type==TYPE_CLASS||t->type==TYPE_UNION) {
        char *ret=NULL;
        if(t->type==TYPE_CLASS) {
            if(!t->cls.name) return NULL;
            ret=strdup(t->cls.name);
        } else {
            if(!t->un.name) return NULL;
            ret=strdup(t->un.name);
        }
        if(map_get(world,ret)) return ret;
        CTypeGraph g;
        vec_init(&g.in);vec_init(&g.out);
        g.visited=0;
        map_set(world,t->un.name,g);
        return ret;
    }
    return NULL;
}
//Kahn's algorithm
static vec_CType_t TopoSort(map_CTypeGraph_t graph) {
    vec_CType_t ret;
    vec_str_t q;
    vec_init(&q);
    vec_init(&ret);
    map_iter_t miter=map_iter(&graph);
    long visited=0,total=0;
    const char *key;
    while(key=map_next(&graph,&miter)) {
        CTypeGraph *node=map_get(&graph,key);
        long ind=node->indegree=node->in.length;
        if(!ind) {
                vec_push(&q,strdup(key));
        }
        total++;
    }
    loop:
    while(q.length) {
        char *str=vec_pop(&q),*str2;
        vec_push(&ret,*map_get(&Compiler.types,str));
        CTypeGraph *node=map_get(&graph,str);
        node->indegree=-1;
        node->visited=visited++;
        long iter;
        vec_foreach(&node->out,str2,iter) {
            CTypeGraph *node=map_get(&graph,str2);
            if(0==--node->indegree) vec_push(&q,strdup(str2));
        }
        TD_FREE(str);
    }
    assert(total==visited);
    return ret;
}
//Avoid members from inhirted class/union
static long UniqueMemberStart(CType *t) {
    CType *inher=NULL;
    if(t->type==TYPE_CLASS) inher=t->cls.inheritsFrom;
    else if(t->type==TYPE_UNION) inher=t->un.inheritsFrom;
    if(!inher) return 0;
    if(inher->type==TYPE_CLASS) return inher->cls.members.length;
    if(inher->type==TYPE_UNION) return inher->un.members.length;
    return 0;
}
//ONLY INCLUDES TYPE BY VALUE ONLY
static vec_CType_t TypesWithin(CType *t,int recur,int force) {
    vec_CType_t ret;
    vec_init(&ret);
    long iter;
    CMember mem;
    switch(t->type) {
    case TYPE_UNION:
        if(t->un.inheritsFrom)
            vec_push(&ret,t->un.inheritsFrom);
        if(t->un.baseType)
            vec_push(&ret,t->un.baseType);
        //Force include anonoymus
        if((!recur)&&t->un.name&&!force) goto normal;
        vec_foreach(&t->un.members,mem,iter) {
            //Recurse if anomus class
            vec_CType_t arr=TypesWithin(mem.type,1,0);
            vec_pusharr(&ret,arr.data,arr.length);
            vec_deinit(&arr);
            vec_push(&ret,mem.type);
        }
    break;
    case TYPE_CLASS:
        //CHeck if anoymus
        if(t->cls.inheritsFrom) vec_push(&ret,t->cls.inheritsFrom);
        if(t->cls.baseType)
            vec_push(&ret,t->cls.baseType);
        //Force include anonoymus
        if((!recur)&&t->cls.name&&!force) goto normal;
        vec_foreach(&t->cls.members,mem,iter) {
            //Recurse if anomus class
            vec_CType_t arr=TypesWithin(mem.type,1,0);
            vec_pusharr(&ret,arr.data,arr.length);
            vec_deinit(&arr);
            vec_push(&ret,mem.type);
        }
    break;
    case TYPE_PTR:
        vec_deinit(&ret);
        return TypesWithin(t->ptr,0,0);
    case TYPE_FUNC:;
        vec_CType_t arr=TypesWithin(t->func.ret,1,0);
        vec_pusharr(&ret,arr.data,arr.length);
        vec_deinit(&arr);
        CType *arg;
        vec_foreach(&t->func.arguments,arg,iter) {
            vec_CType_t arr=TypesWithin(arg,1,0);
            vec_pusharr(&ret,arr.data,arr.length);
            vec_deinit(&arr);
        }
        break;
    case TYPE_ARRAY_REF:
    case TYPE_ARRAY:
        if(recur) {
            vec_CType_t arr=TypesWithin(t->array.base,1,0);
            vec_push(&ret,t->array.base);
            vec_pusharr(&ret,arr.data,arr.length);
            vec_deinit(&arr);
        }
        break;
    default:
        normal:
        if(recur) vec_push(&ret,t);
        break;
    }
    return ret;
}
static map_CTypeGraph_t CreateTypeGraph() {
    map_iter_t iter=map_iter(&Compiler.types);
    const char *key;
    map_CTypeGraph_t ret;
    map_init(&ret);
    while(key=map_next(&Compiler.types,&iter)) {
        CType *t=*map_get(&Compiler.types,key);
        if(TypeIsForward(t)) continue;
        if(t->isBuiltin) continue;
        vec_CMember_t *mems;
        if(t->type==TYPE_CLASS) {
            mems=&t->cls.members;
        } else if(t->type==TYPE_UNION) {
            mems=&t->un.members;
        } else continue;
        char *cur_node=__MakeOrGetTypeNode(&ret,t);
        long iter,iter2;
        vec_CType_t within=TypesWithin(t,1,1);
        CType *type;
        vec_foreach(&within,type,iter2) {
            char *ref=__MakeOrGetTypeNode(&ret,type);
            if(ref) {
                vec_push(&map_get(&ret,cur_node)->in,strdup(ref));
                vec_push(&map_get(&ret,ref)->out,strdup(cur_node));
            }
        }
        vec_deinit(&within);
        TD_FREE(cur_node);
    }
    return ret;
}
void MakeHeaderForModule(FILE *header) {
    //MACROS;
    map_iter_t iter=map_iter(&Lexer.macros);
    const char *key;
    while(key=map_next(&Lexer.macros,&iter)) {
        CMacro *m=map_get(&Lexer.macros,key);
        fprintf(header,"#define %s %s\n",key,m->expand);
    }
    //TYPE FORWARDS
    iter=map_iter(&Compiler.types);
    while(key=map_next(&Compiler.types,&iter)) {
        CType *type=*map_get(&Compiler.types,key);
        if(type->isBuiltin) continue;
        if(type->type==TYPE_CLASS) {
            fprintf(header,"class %s;\n",key);
        } else if(type->type==TYPE_UNION) {
            fprintf(header,"union %s;\n",key);
        }
    }
    //TYPES
    //TOPOLOGICALLY SORT TYPES BY USAGE TO ENSURE A CORRECT LINE OF DESCENT
    map_CTypeGraph_t graph=CreateTypeGraph();
    vec_CType_t sorted=TopoSort(graph);
    CType *type;long iter2;
    vec_foreach(&sorted,type,iter2) {
        if(type->isBuiltin) continue;
        if(TypeIsForward(type)) continue;
        if(type->type==TYPE_CLASS) {
            key=type->cls.name;
            if(type->cls.baseType) {
                char *ser=DumpTypeWithName(type->cls.baseType,"");
                fprintf(header,"%s ",ser);
                TD_FREE(ser);
            }
            fprintf(header,"class %s ",key);
            if(type->cls.inheritsFrom) {
                char *ser=DumpTypeWithName(type->cls.inheritsFrom,"");
                fprintf(header,":%s",ser);
                TD_FREE(ser);
            }
            char * ser=SerializeMembers(&type->cls.members,UniqueMemberStart(type));
            fprintf(header,"%s;\n",ser);
        } else if(type->type==TYPE_UNION) {
            key=type->un.name;
            if(type->un.baseType) {
                char *ser=DumpTypeWithName(type->un.baseType,"");
                fprintf(header,"%s ",ser);
                TD_FREE(ser);
            }
            fprintf(header,"union %s ",key);
            if(type->un.inheritsFrom) {
                char *ser=DumpTypeWithName(type->un.inheritsFrom,"");
                fprintf(header,":%s",ser);
                TD_FREE(ser);
            }
            char * ser=SerializeMembers(&type->un.members,UniqueMemberStart(type));
            fprintf(header,"%s;\n",ser);
        }
    }
    vec_deinit(&sorted);
    //FUNCTIONS
    iter=map_iter(&Compiler.globals);
    while(key=map_next(&Compiler.globals,&iter)) {
        CVariable *v=GetVariable((char*)key);
        if(v->isBuiltin) continue;
        if(!v->name) continue;
        if(v->isInternal) continue;
        char *hd=DumpTypeWithName(v->type,v->name);
        switch(v->linkage.type) {
            case LINK_EXTERN:
            case LINK_NORMAL:
            case LINK_IMPORT:
            fprintf(header,"extern %s;\n",hd);
            break;
            case LINK__EXTERN:
            case LINK__IMPORT:
            fprintf(header,"_extern %s %s;\n",v->linkage.importLink,hd);
            break;
        }
        TD_FREE(hd);
    }
}
static void FuncCodeDestroy(void *mem,void *sz) {
#ifndef TARGET_WIN32
	munmap(mem,(int64_t)sz);
#else
	VirtualFree(mem,0,MEM_RELEASE);
#endif
}
#define LF_F_IS_ASMPATCH 1
CVariable *LoadAOTFunction(FILE *f,int verbose,int flags) {
    char buffer[256];
    vec_CBinPatchp_t patches;
    vec_init(&patches);
    char chr;int idx=0;
    while(chr=fgetc(f)) buffer[idx++]=chr;
    buffer[idx]=0;
    if(verbose) {
        printf("==== FUNC:%s ====\n",buffer);
    }
    int64_t patchc;
    fread(&patchc,1,sizeof(patchc),f);
    while(--patchc>=0) {
            char buffer[256];
            int64_t offset;
            fread(&offset,1,sizeof(offset),f);
            char chr;int idx=0;
            while(chr=fgetc(f)) buffer[idx++]=chr;
            buffer[idx]=0;
            CBinPatch *pat=TD_MALLOC(sizeof(offset)+strlen(buffer)+1);
            pat->offset=offset;
            strcpy((void*)&pat->name,buffer);
            vec_push(&patches,pat);
            if(verbose) {
                    printf("RELOCATION:%s(@%p)\n",buffer,offset);
            }
    }
    int64_t string_count;
    fread(&string_count,1,sizeof(string_count),f);
    vec_CBinStringp_t strings;
    vec_init(&strings);
    while(--string_count>=0) {
        int64_t offset;
        fread(&offset,1,sizeof(offset),f);
        int64_t len;
        fread(&len,1,sizeof(len),f);
        char buffer[len];
        fread(buffer,1,len,f);
        CValue tmp=RegisterString(buffer);
        ReleaseValue(&tmp);
        CBinString *str=TD_MALLOC(2*sizeof(int64_t)+len);
        str->len=len;
        str->offset=offset;
        strncpy(&str->str,buffer,len);
        vec_push(&strings,str);
        if(verbose) {
            printf("STRING:%s(@%p)\n",buffer,offset);
        }
    }
    //STATICS
    map_CVariable_t statics;
    {
        char buffer[256];
        int64_t static_count;
        fread(&static_count,1,sizeof(static_count),f);
        vec_init(&statics);
        CType *u8=CreatePrimType(TYPE_U8);
        while(--static_count>=0) {
            int64_t size;
            fread(&size,1,sizeof(size),f);
            CVariable *var=TD_MALLOC(sizeof(CVariable));
            var->type=u8;
            var->isGlobal=1;
            var->linkage.type=LINK_NORMAL;
            var->linkage.globalPtr=TD_MALLOC(size);
            char chr;int idx=0;
            while(chr=fgetc(f)) buffer[idx++]=chr;
            buffer[idx]=0;
            map_set(&statics,buffer,var);
        }
    }
    map_void_t labels;
    map_init(&labels);
    map_str_t exported;
    {
        //LABELS
        char buffer[128];
        int64_t lcount;
        fread(&lcount,1,sizeof(lcount),f);
        while(--lcount>=0) {
            int64_t offset,type;
            fread(&offset,1,sizeof(offset),f);
            fread(&type,1,sizeof(type),f);
            char chr;int idx=0;
            while(chr=fgetc(f)) buffer[idx++]=chr;
            buffer[idx]=0;
            //ASSEMBLER PATCHES ALWAYS CONTAIN REFERNCE TO LABELS,BUT THERE REFERENCED ONLY; DONT EXPORT THE LABEL
            if(type==LABEL_EXPORTED&&!(flags&LF_F_IS_ASMPATCH)) {
                //EXPORTED LABELS ARE ACTUALLY POINTERS
                CVariable *new=TD_MALLOC(sizeof(CVariable));
                new->type=CreatePtrType(CreatePrimType(TYPE_U0));
                new->name=strdup(buffer);
                new->linkage.type=LINK_NORMAL;
                new->isGlobal=1;
                new->linkage.globalPtr=(void*)offset;
                map_set(&Compiler.exportedLabels,buffer,new);
                map_set(&Compiler.globals,buffer,new);
            }
            //Will be added to function pointer later
            map_set(&labels,buffer,(void*)offset);
        }
    }
    //FUNC CODE
    int64_t code_size;
    fread(&code_size,1,sizeof(code_size),f);
    CVariable *v=TD_MALLOC(sizeof(CVariable));
    v->type=CreatePrimType(TYPE_U0);
    v->isGlobal=1;
    v->isFunc=1;
    v->func=TD_MALLOC(sizeof(CFunction));
    void * mem;
    #ifndef TARGET_WIN32
    mem=mmap(NULL,code_size,PROT_EXEC|PROT_WRITE|PROT_READ,MAP_32BIT|MAP_PRIVATE|MAP_ANONYMOUS,-1,0);
    #else
    mem=VirtualAlloc(NULL,code_size,MEM_COMMIT | MEM_RESERVE,PAGE_EXECUTE_READWRITE);
    #endif
    fread(mem,1,code_size,f);
    v->func->funcptr=mem;
    v->func->statics=statics;
    v->name=strdup(buffer);
    v->func->name=strdup(buffer);
    v->func->funcInfo.funcSize=code_size;
    GCCreateExtPtr(mem,FuncCodeDestroy,(void*)code_size);
    CBinPatch *pat;
    long iter;
    vec_foreach(&patches,pat,iter) {
        loop:;
        vec_voidppp_t *vec;
        if(vec=map_get(&v->func->relocations,&pat->name)) {
            void ***where=TD_MALLOC(sizeof(void**));
            *where=mem+pat->offset;
            vec_push(vec,where);
        } else {
            vec_voidppp_t new;
            vec_init(&new);
            map_set(&v->func->relocations,&pat->name,new);
            goto loop;
        }
    }
    CBinString *str;
    vec_foreach(&strings,str,iter) {
        *(char**)(mem+str->offset)=*map_get(&Compiler.strings,&str->str);
        sloop:
        if(!map_get(&v->func->stringRelocations,&str->str)) {
            vec_voidppp_t vp;
            vec_init(&vp);
            map_set(&v->func->stringRelocations,&str->str,vp);
            goto sloop;
        } else {
            void ***new=TD_MALLOC(sizeof(void**));
            *new=(char*)mem+str->offset;
            vec_push(map_get(&v->func->stringRelocations,&str->str),new);
        }
        TD_FREE(str);
    }
    //Add label offsets to function pointer
    map_iter_t miter=map_iter(&labels);
    const char *key;
    while(key=map_next(&labels,&miter)) {
            char**lab=(char**)map_get(&labels,key);
            *lab+=(int64_t)mem;
            //ASSEMBLER PATCHES ALWAYS CONTAIN REFERNCE TO LABELS,BUT THERE REFERENCED ONLY; DONT EXPORT THE LABEL
            if(map_get(&Compiler.exportedLabels,key)&&!(flags&LF_F_IS_ASMPATCH)) {
                map_get(&Compiler.exportedLabels,key)[0]->linkage.globalPtr=(void*)*lab;
            }
    }
    v->func->labelPtrs=labels;
    //FILL IN FUNCTION STATICS
    miter=map_iter(&statics);
    while(key=map_next(&statics,&miter)) {
        vec_voidppp_t *relocs;
        if(relocs=map_get(&v->func->relocations,key)) {
            long iter;
            void ***reloc;
            vec_foreach(relocs,reloc,iter) {
                **reloc=GetGlobalPtr(*map_get(&statics,key));
                TD_FREE(reloc);
            }
            vec_deinit(relocs);
            map_remove(&v->func->relocations,key);
        }
    }
    //ASM PTCHES
    int64_t asmpatch_count;
    fread(&asmpatch_count,1,sizeof(asmpatch_count),f);
    while(--asmpatch_count>=0) {
        CBinAsmPatch pat;
        fread(&pat,1,sizeof(pat),f);
        CAsmPatch *new=TD_MALLOC(sizeof(CAsmPatch));
        new->isRel=pat.is_rel;
        new->context=pat.labelContext;
        new->ptr=v->func->funcptr+pat.offset;
        new->rel_offset=pat.rel_offset;
        new->width=pat.width;
        CVariable *patv=LoadAOTFunction(f,1,LF_F_IS_ASMPATCH);
        new->apply=patv->func;
        patv->func=NULL;ReleaseVariable(patv);
        vec_push(&v->func->asmPatches,new);
    }
    return v;
}
CBinaryModule LoadAOTBin(FILE *f,int verbose) {
    CType *u0=CreatePrimType(TYPE_U0);
    char buffer[256];
    vec_CVariable_t loaded;
    vec_init(&loaded);
    CVariable *aaMain=NULL;
    //The lexer requires functionality from HolyC to detirimine if a thing is an Opcode/register,so parse after we load the module.
    char *header_text=NULL;
    //All imported types have type U8[size],include a HH to define types.
    for(;;) {
            switch(fgetc(f)) {
                case -1: goto en;
                case 0: //Variable
                {
                    long is_internal=fgetc(f);
                    char chr;int idx=0;
                    while(chr=fgetc(f)) buffer[idx++]=chr;
                    buffer[idx]=0;
                    int64_t size;
                    fread(&size,1,sizeof(size),f);
                    CVariable *v=TD_MALLOC(sizeof(CVariable));
                    v->type=u0;
                    v->isGlobal=1;
                    v->linkage.globalPtr=TD_MALLOC(size);
                    v->linkage.type=LINK_NORMAL;
                    v->name=strdup(buffer);
                    v->isInternal=is_internal;
                    map_set(&Compiler.globals,buffer,v);

                    if(verbose) {
                        printf("VAR:%s(size %ld)\n",buffer,size);
                    }
                    break;
                }
                case 1:; //Function
                    CVariable *v=LoadAOTFunction(f,verbose,0);
                    if(!strcmp("@@Main",v->name)) {
                        v->type=CreateFuncType(u0,NULL,0);
                        aaMain=v;
                    } else {
                        map_set(&Compiler.globals,v->name,v);
                    }
                    vec_push(&loaded,v);
                    break;
                case 2: {
                    long current=ftell(f);
                    fseek(f,0,SEEK_END);
                    long end=ftell(f);
                    fseek(f,current,SEEK_SET);
                    header_text=TD_MALLOC(end-current+1);
                    fread(header_text,1,end-current,f);
                    if(verbose) puts(header_text);
                    break;
                }
                default: abort();
            }
    }
    en:;
    //Put variables in module so the garbage collector sees them even if they are over shadowed by newer version of symbol
    CBinaryModule module;
    map_init(&module.vars);
    long iter;CVariable *v;
    vec_foreach(&loaded,v,iter) {
        map_set(&module.vars,v->name,v);
        if(v->func) {
            FillFunctionRelocations(v->func);
            long iter;
            CAsmPatch *pat;
            vec_foreach(&v->func->asmPatches,pat,iter) {
                PatchLabelExprFunc(pat,v->func);
                FillFunctionRelocations(pat->apply);
            }
            ApplyPatches(v->func);
        }
    }
    vec_deinit(&loaded);
    if(aaMain)
        aaMain->func->funcptr(0);
    CLexer old=ForkLexer(PARSER_HOLYC);
    Lexer.replMode=0;
    mrope_append_text(Lexer.source,strdup(header_text));
    HC_parse();
    JoinWithOldLexer(old);
    return module;
}
