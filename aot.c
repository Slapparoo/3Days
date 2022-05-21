#include "3d.h"
#ifdef TARGET_WIN32
#include "memoryapi.h"
#else
#include <sys/mman.h>
#endif
static char *MStrCat(char *a,char *b);
static void FreeAsmPatcherSym(CSymbol *sym) {
    long iter;
    map_iter_t miter;
    CAsmRelocation *areloc;
    const char *key;
    vec_foreach_ptr(&sym->asm_relocs,areloc,iter) {
	FreeAsmPatcherSym(areloc->func);
    }
    miter=map_iter(&sym->relocs);
    while(key=map_next(&sym->relocs,&miter))
    	vec_deinit(map_get(&sym->relocs,key));
    map_deinit(&sym->relocs);
    vec_deinit(&sym->asm_relocs);
    map_deinit(&sym->label_ptrs);
    map_deinit(&sym->statics);
    TD_FREE(sym->value_ptr);
    TD_FREE(sym);
}
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
/**
 * Values are defined by function @@Main
 * BinVar: U8 ident=0;
 *         U8 is_internal(to module);
 *         U8 name[];
 *         I64 size;
 */
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
CSymbol *LoadAOTFunction(FILE *f,int verbose,int flags) {
    char buffer[256],sym_name[256];
    vec_CBinPatchp_t patches;
    vec_init(&patches);
    char chr;int idx=0;
    while(chr=fgetc(f)) buffer[idx++]=chr;
    buffer[idx]=0;
    strcpy(sym_name,buffer);
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
    long total_str_bytes=0;
    while(--string_count>=0) {
        int64_t offset;
        fread(&offset,1,sizeof(offset),f);
        int64_t len;
        fread(&len,1,sizeof(len),f);
        char buffer[len];
        fread(buffer,1,len,f);
        CBinString *str=TD_MALLOC(2*sizeof(int64_t)+len);
        str->len=len;
        str->offset=offset;
        strncpy(str->str,buffer,len);
        vec_push(&strings,str);
        if(verbose) {
            printf("STRING:%s(@%p)\n",buffer,offset);
        }
        total_str_bytes+=len;
    }
    //STATICS
    map_void_t statics;
    {
        char buffer[256];
        int64_t static_count;
        fread(&static_count,1,sizeof(static_count),f);
        map_init(&statics);
        while(--static_count>=0) {
            int64_t size;
            fread(&size,1,sizeof(size),f);
            char chr;int idx=0;
            while(chr=fgetc(f)) buffer[idx++]=chr;
            buffer[idx]=0;
            map_set(&statics,buffer,TD_MALLOC(size));
        }
    }
    map_void_t labels;
    map_init(&labels);
    map_str_t exported;
    map_init(&exported);
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
            if(type==LABEL_EXPORTED) {
                //EXPORTED LABELS ARE ACTUALLY POINTERS
                CSymbol new;
                new.type=SYM_VAR;
                new.is_importable=!(flags&AOT_NO_IMPORT_SYMS);
                new.value_ptr=(void*)offset;
                map_set(&Loader.symbols,buffer,new);
                map_set(&exported,buffer,NULL);
            }
            //Will be added to function pointer later
            map_set(&labels,buffer,(void*)offset);
        }
    }
    //FUNC CODE
    int64_t code_size;
    fread(&code_size,1,sizeof(code_size),f);
    CSymbol v;
    void * mem=PoopMAlloc32(code_size);
    fread(mem,1,code_size,f);
    v.value_ptr=mem;
    v.type=SYM_FUNC;
    v.statics=statics;
    v.is_importable=!(flags&AOT_NO_IMPORT_SYMS);
    v.size=code_size;
    map_init(&v.relocs);
    CBinPatch *pat;
    long iter;
    vec_foreach(&patches,pat,iter) {
        loop:;
        vec_CRelocationp_t *vec;
        if(vec=map_get(&v.relocs,pat->name)) {
            CRelocation *where=TD_MALLOC(sizeof(CRelocation));
            where->ptr=mem+pat->offset;
            vec_push(vec,where);
        } else {
            vec_CRelocationp_t new;
            vec_init(&new);
            map_set(&v.relocs,pat->name,new);
            goto loop;
        }
        TD_FREE(pat);
    }
    CBinString *str;
    char *string_bytes=TD_CALLOC(1,total_str_bytes+1);
    long offset=0;
    vec_foreach(&strings,str,iter) {
        strncpy(string_bytes+offset,&str->str,str->len);
        offset+=str->len;
    }
    offset=0;
    vec_foreach(&strings,str,iter) {
        *(char**)(mem+str->offset)=string_bytes+offset;
        offset+=str->len;
        TD_FREE(str);
    }
    v.strings=string_bytes;
    //Add label offsets to function pointer
    map_iter_t miter=map_iter(&labels);
    const char *key;
    while(key=map_next(&labels,&miter)) {
            char**lab=(char**)map_get(&labels,key);
            *lab+=(int64_t)mem;
    }
    miter=map_iter(&exported);
    while(key=map_next(&exported,&miter)) {
        map_get(&Loader.symbols,key)->value_ptr+=(int64_t)mem;
    }
    map_deinit(&exported);
    v.label_ptrs=labels;
    //FILL IN FUNCTION STATICS
    sloop:;
    miter=map_iter(&statics);
    while(key=map_next(&statics,&miter)) {
        vec_CRelocationp_t *relocs;
        if(relocs=map_get(&v.relocs,key)) {
            long iter;
            CRelocation *reloc;
            vec_foreach(relocs,reloc,iter) {
                *reloc->ptr=map_get(&statics,key);
                TD_FREE(reloc);
            }
            vec_deinit(relocs);
            map_remove(&v.relocs,key);
            goto sloop;
        }
    }
    //ASM PTCHES
    vec_init(&v.asm_relocs);
    int64_t asmpatch_count;
    fread(&asmpatch_count,1,sizeof(asmpatch_count),f);
    while(--asmpatch_count>=0) {
        CAsmRelocation apatch;
        fread(&apatch.patch,1,sizeof(apatch.patch),f);
        apatch.func=LoadAOTFunction(f,verbose,flags|AOT_MALLOCED_SYM);
        vec_push(&v.asm_relocs,apatch);
    }
    if(!(flags&AOT_MALLOCED_SYM)) {
        map_set(&Loader.symbols,buffer,v);
        return map_get(&Loader.symbols,buffer);
    } else {
        CSymbol *ret=TD_MALLOC(sizeof(v));
        *ret=v;
        return ret;
    }
}
//See PARSER.HC
#define LOCAL_LAB_FMT "%s.%d"
void FillInRelocations(CSymbol *sym,int label_scope) {
    map_iter_t iter=map_iter(&sym->relocs);
    char buffer[128];
    const char *key;
    while(key=map_next(&sym->relocs,&iter)) {
            long i;
            int scope;
            CRelocation *reloc;
            sprintf(buffer,LOCAL_LAB_FMT,key,label_scope);
            vec_foreach(map_get(&sym->relocs,key),reloc,i) {
                if(map_get(&sym->label_ptrs,key)) {
                    *reloc->ptr=*map_get(&sym->label_ptrs,key);
                } else if(map_get(&sym->label_ptrs,buffer)) {
                    *reloc->ptr=*map_get(&sym->label_ptrs,buffer);
                } else if(map_get(&sym->statics,key)) {
                    *reloc->ptr=*map_get(&sym->statics,key);
                } else if(map_get(&Loader.symbols,key)) {
                    *reloc->ptr=map_get(&Loader.symbols,key)->value_ptr;
                } else
                    fprintf(stderr,"Missing symbol %s at %p.\n",key,reloc->ptr);
            }
    }
    long iter2;
    CAsmRelocation areloc;
    vec_foreach(&sym->asm_relocs,areloc,iter2) {
        //Use label pointers from parent function
        iter=map_iter(&sym->label_ptrs);
        while(key=map_next(&sym->label_ptrs,&iter)) {
            map_set(&areloc.func->label_ptrs,key,*map_get(&sym->label_ptrs,key));
        }
        iter=map_iter(&sym->statics);
        while(key=map_next(&sym->statics,&iter)) {
            map_set(&areloc.func->statics,key,*map_get(&sym->statics,key));
        }
        FillInRelocations(areloc.func,areloc.patch.labelContext);
        void *ptr=((void*(*)())areloc.func->value_ptr)();
        void *dptr=sym->value_ptr+areloc.patch.offset;
        if(areloc.patch.is_rel) {
            ptr-=dptr+areloc.patch.width;
        }
        switch(areloc.patch.width) {
            case 1:
            *(uint8_t*)dptr=ptr;
            break;
            case 2:
            *(uint16_t*)dptr=ptr;
            break;
            case 4:
            *(uint32_t*)dptr=ptr;
            break;
            case 8:
            *(uint64_t*)dptr=ptr;
            break;
        }
        FreeAsmPatcherSym(areloc.func);
    }
    vec_deinit(&sym->asm_relocs);
}
void LoadAOTBin(FILE *f,int flags,char **header) {
    char buffer[256];
    char *header_text=NULL;
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
                    CSymbol v;
                    v.type=SYM_VAR;
                    v.size=size;
                    v.is_importable=!(flags&AOT_NO_IMPORT_SYMS);
                    v.value_ptr=PoopMAlloc32(size);
                    map_set(&Loader.symbols,buffer,v);
                    break;
                }
                case 1:; //Function
                    LoadAOTFunction(f,0,flags);
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
    CSymbol *aaMain=NULL;
    map_iter_t iter=map_iter(&Loader.symbols);
    const char *key;
    while(key=map_next(&Loader.symbols,&iter)) {
        if(!strcmp(key,"@@Main"))
            aaMain=map_get(&Loader.symbols,key);
        if(map_get(&Loader.symbols,key)->type==SYM_FUNC)
            FillInRelocations(map_get(&Loader.symbols,key),-100);
    }
    if(aaMain)
        FFI_CALL_TOS_0(aaMain->value_ptr);
    Load("HCRT.BIN",0);
    if(header)
        *header=header_text;
    else TD_FREE(header);
}
char *GetAOTFunctionNameByPtr(void *ptr) {
	map_iter_t iter=map_iter(&Loader.symbols);
	const char *key;
	CSymbol *sym;
	while(key=map_next(&Loader.symbols,&iter)) {
		sym=map_get(&Loader.symbols,key);
		if(sym->type==SYM_FUNC) {
			if(sym->value_ptr<=ptr&&sym->value_ptr+sym->size>=ptr)
				return key;
		}
	}
	return NULL;
}
