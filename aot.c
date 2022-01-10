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
    char name[64];
 } CBinLabel;
 typedef struct {
    int64_t size;
    char name[64];
 } CBinStatic;
typedef struct {
    int64_t offset;
    //This member is terminated with '\0'
    char name[64];
} CBinPatch;
typedef struct {
    int64_t offset;
    int64_t len;
    char str[64];
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
            strcpy(stat2->name,key);
            vec_push(&statics,stat2);
        }
        vec_voidppp_t *relocs=map_get(&func->relocations,key);
        long iter;
        void ***ptr;
        vec_foreach(relocs,ptr,iter) {
            CBinPatch *pat=TD_MALLOC(sizeof(int64_t)+strlen(key)+1);
            pat->offset=((char*)*ptr)-(char*)func->funcptr;
            strcpy(pat->name,key);
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
            strcpy(pat->str,key);
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
            strcpy(lab->name,key);
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
void SerializeModule(FILE *f,char *header) {
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
    if(header) {
      fputc(2,f);
      int64_t len;
      char *buf=FileRead(header,&len);
      fwrite(buf, 1,len, f);
      TD_FREE(buf);
    }
}
static char *MStrCat(char *a,char *b) {
    char buffer[strlen(a)+strlen(b)+1];
    strcat(strcpy(buffer,a),b);
    TD_FREE(a),TD_FREE(b);
    return strdup(buffer);
}
typedef vec_t(vec_int_t) vec_vec_int_t;
typedef map_t(vec_vec_int_t) map_vec_vec_int_t;
static int longCmp(const void *a,const void *b) {
    return *(long*)a-*(long*)b;
}
static void FuncCodeDestroy(void *mem,void *sz) {
  CFunction *f=mem;
#ifndef TARGET_WIN32
	munmap(f->funcptr,(int64_t)sz);
#else
	VirtualFree(f->funcptr,0,MEM_RELEASE);
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
            strcpy(pat->name,buffer);
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
        strncpy(str->str,buffer,len);
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
            var->isInternal=1;
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
    mem=mmap(NULL,code_size,PROT_EXEC|PROT_WRITE|PROT_READ,MAP_PRIVATE|MAP_ANONYMOUS,-1,0);
    #else
    mem=VirtualAlloc(NULL,code_size,MEM_COMMIT | MEM_RESERVE,PAGE_EXECUTE_READWRITE);
    #endif
    fread(mem,1,code_size,f);
    v->func->funcptr=mem;
    v->func->statics=statics;
    v->name=strdup(buffer);
    v->func->name=strdup(buffer);
    v->func->funcInfo.funcSize=code_size;
    v->isInternal=1;
    GC_SetDestroy(v->func, FuncCodeDestroy,(void*)code_size);
    CBinPatch *pat;
    long iter;
    vec_foreach(&patches,pat,iter) {
        loop:;
        vec_voidppp_t *vec;
        if(vec=map_get(&v->func->relocations,pat->name)) {
            void ***where=TD_MALLOC(sizeof(void**));
            *where=mem+pat->offset;
            vec_push(vec,where);
        } else {
            vec_voidppp_t new;
            vec_init(&new);
            map_set(&v->func->relocations,pat->name,new);
            goto loop;
        }
    }
    CBinString *str;
    vec_foreach(&strings,str,iter) {
        *(char**)(mem+str->offset)=*map_get(&Compiler.strings,str->str);
        sloop:
        if(!map_get(&v->func->stringRelocations,str->str)) {
            vec_voidppp_t vp;
            vec_init(&vp);
            map_set(&v->func->stringRelocations,str->str,vp);
            goto sloop;
        } else {
            void ***new=TD_MALLOC(sizeof(void**));
            *new=(char*)mem+str->offset;
            vec_push(map_get(&v->func->stringRelocations,str->str),new);
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
CBinaryModule LoadAOTBin(FILE *f,int flags) {
    char buffer[256];
    //We will remove any item not in the header. So if we have an intenrnal symbol that shares a name with a global,restore the previous global
    map_CVariable_t oldGlobals;
    map_init(&oldGlobals);
    const char *key;
    map_iter_t miter=map_iter(&Compiler.globals);
    while(key=map_next(&Compiler.globals,&miter)) {
      map_set(&oldGlobals,key,*map_get(&Compiler.globals, key));
    }
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
                    CType *arr=TD_MALLOC(sizeof(CType));
                    arr->type=TYPE_ARRAY;
                    arr->array.dim=size;
                    arr->array.base=CreatePrimType(TYPE_U8);
                    v->type=arr;
                    v->isGlobal=1;
                    v->linkage.globalPtr=TD_MALLOC(size);
                    v->linkage.type=LINK_NORMAL;
                    v->name=strdup(buffer);
                    v->isInternal=1;
                    map_set(&Compiler.globals,buffer,v);

                    vec_push(&loaded, v);
                    break;
                }
                case 1:; //Function
                    CVariable *v=LoadAOTFunction(f,0,0);
                    if(!strcmp("@@Main",v->name)) {
                        v->type=CreateFuncType(CreatePrimType(TYPE_U0),NULL,0);
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
    #ifndef BOOTSTRAPED
        DisableREPL();
        mrope_append_text(Lexer.source,strdup(header_text));
    #else
        if(!Lexer.HCLexer) CreateLexer(0);
        DisableREPL();
        void(*inc)(void*cc,char *fn,char *src,int64_t act_f)=(void*)GetVariable("LexIncludeStr")->func->funcptr;
        inc(Lexer.HCLexer,"(nofile)",header_text,0);
        //Reload the macros now that we have the HolyC Lexer laoded
        RegisterBuiltins();
    #endif
    Compiler.allowForwardAfterDefine=1;
    if(!(flags&AOT_F_NO_ADD_SYMBOLS)) HC_parse();
    Compiler.allowForwardAfterDefine=0;
    #ifdef BOOTSTRAPED
    //Remove all internal variables(they can be made external in the header.) Restore any old globals that share a name with an intenal variable
    RegisterBuiltins();
    #endif
    miter=map_iter(&module.vars);
    while(key=map_next(&module.vars,&miter)) {
      CVariable **find=map_get(&Compiler.globals, key);
      if(find) {
        if(find[0]->isInternal) {
          CVariable **old;
          if(old=map_get(&oldGlobals,key)) {
            map_set(&Compiler.globals,key,old[0]);
          } else {
            map_remove(&Compiler.globals, key);
          }
        }
      }
    }
    if(!(flags&AOT_F_NO_ADD_SYMBOLS))
        map_deinit(&oldGlobals);
    else {
        map_deinit(&Compiler.globals);
        Compiler.globals=oldGlobals;
    }
    return module;
}
