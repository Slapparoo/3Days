#include "3d.h"
#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <libgen.h>
#include <signal.h>
#ifndef TARGET_WIN32
#include <sys/syscall.h>
#include "winjmp.h"
#include <execinfo.h>
#endif
int jit_FunctionParents(void **pars,int max);
int jit_ParentFramePtrs(void **fptrs,int max);
static const char* ListLines(const char *fn,long start,long end);
void SignalHandler(int s) {
    #ifdef BOOTSTRAPED
    Backtrace(-1,1);
    #endif
    printf("\n");
    switch(s) {
      case SIGINT:
        if(Compiler.blockSignals) return ;
    case SIGABRT:
        printf("User abort.type \"Exit(0);\" to exit.\n");
        break;
    case SIGSEGV:
        printf("Segmentation fault.\n");
        break;
    case SIGFPE:
        printf("Arithmetic exception.\n");
        break;
    default:
        printf("Strange signal %d.\n",s);
        break;
    }
    throw(0);
}
static CFunction *GetFunctionByPtr(void *ptr) {
    map_iter_t iter=map_iter(Compiler.globals);
    const char *key;
    while(key=map_next(&Compiler.globals,&iter)) {
        CVariable *var=*map_get(&Compiler.globals,key);
        if(var->isFunc) {
            if(var->func)
            if(var->func->JIT) {
                if((char*)ptr>=(char*)var->func->funcptr)
                    if((char*)ptr<=((char*)var->func->funcptr)+jit_bin_size(var->func->JIT))
                        return var->func;
            }
        }
    }
    return NULL;
}
CBreakpoint *GetBreakpointByPtr(void *ptr) {
    CFunction *f=GetFunctionByPtr(ptr);
    if(!f) return NULL;
    jit_breakpoint_t *bp=(void*)jit_get_breakpoint_btr_ptr(f->JIT,ptr);
    if(!bp) return NULL;
    return *(CBreakpoint**)(bp->user_data);
}
jit_breakpoint_t *GetJITBreakpointByPtr(void *ptr) {
    CFunction *f=GetFunctionByPtr(ptr);
    if(!f) return NULL;
    jit_breakpoint_t *bp=(void*)jit_get_breakpoint_btr_ptr(f->JIT,ptr);
    return bp;
}
#ifndef BOOTSTRAPED
void DbgPrintVar(CType *type,void *ptr) {
}
void VisitBreakpoint(struct jit_debugger_regs *reg,CBreakpoint *bp,void *caller_ptr) {}
void Backtrace(int stkpos,int trim) {}
#else
void EnterDebugger() {
    if(!Compiler.debugMode) {
      fprintf(stderr, "Run with -d or -b to use the debugger.\n");
      return;
    }
    //jit_VisitBreakpoint
    //parent
    void *bt[1+1];
    jit_FunctionParents(bt,2);
    VisitBreakpoint(NULL,NULL,bt[1]);
}
void DbgPrintVar(CType *type,void *ptr) {
    int code;
    if(HCSetJmp(EnterCTry())) {
        ARM_SIGNALS;
        if(code==SIGSEGV) {
            char *ts=TypeToString(type);
            if(type->type==TYPE_PTR)
                printf("<Failed to print %s at %p>",ts,*(void**)ptr);
            TD_FREE(ts);
        }
    } else {
        CType *bt =BaseType(type);
        switch(bt->type) {
        case TYPE_ARRAY_REF:
        case TYPE_ARRAY: {
            CType *deref =DerrefedType(bt);
            printf("[");
            long iter;
            for(iter=0; iter!=bt->array.dim; iter++) {
                DbgPrintVar(deref,ptr+iter*TypeSize(bt->array.base));
                printf(",");
            }
            printf("]");
            break;
        }
        case TYPE_BOOL:
            printf("%lu",(unsigned long)*(unsigned char*)ptr);
            break;
        case TYPE_CLASS: {
            printf("{");
            long iter;
            CMember mem;
            vec_foreach(&bt->cls.members, mem, iter) {
                printf("%s:",mem.name);
                DbgPrintVar(mem.type,ptr+mem.offset);
                printf(",");
            }
            printf("}");
            break;
        }
        case TYPE_F64:
            printf("%lf",*(double*)ptr);
            break;
        case TYPE_FUNC:
            printf("<function:%p>",ptr);
            break;
        case TYPE_I16:
            printf("%i",*(int16_t*)ptr);
            break;
        case TYPE_I32:
            printf("%i",*(int32_t*)ptr);
            break;
        case TYPE_I64:
            printf("%li",*(int64_t*)ptr);
            break;
        case TYPE_I8:
            printf("%i",*(int8_t*)ptr);
            break;
        case TYPE_PTR: {
            CType *derefed =DerrefedType(type);
            CType *bt =BaseType(derefed);
            if(bt->type==TYPE_U8) {
                long len=strlen(*(void**)ptr);
                int dotdotdot=0;
                if(len>100) len=100,dotdotdot=1;
                char *slice=TD_MALLOC(len+1);
                strncpy(slice,*(void**)ptr,len);
                char *buffer=TD_MALLOC(len*3+3+1);
                unescapeString(slice, buffer);
                if(dotdotdot) strcat(buffer, "...");
                printf("\"%s\"",buffer);
                TD_FREE(buffer),TD_FREE(slice);
                break;
            }
            printf("%p",*(void**)ptr);
            break;
        }
        case TYPE_U0:
            printf("U0");
            break;
        case TYPE_U16:
            printf("%u",*(uint16_t*)ptr);
            break;
        case TYPE_U32:
            printf("%u",*(uint32_t*)ptr);
            break;
        case TYPE_U64:
            printf("%lu",*(uint64_t*)ptr);
            break;
        case TYPE_U8:
            printf("%c",*(uint8_t*)ptr);
            break;
        case TYPE_UNION: {
            printf("{");
            long iter;
            CMember mem;
            vec_foreach(&bt->un.members, mem, iter) {
                printf("%s:",mem.name);
                DbgPrintVar(mem.type,ptr+mem.offset);
                printf(",");
            }
            printf("}");
            break;
        }
        }
        PopTryFrame();
    }
}
CDebugger Debugger;
static char *__SkipWhitespace(char *text) {
    while(isblank(*text)) text++;
    return text;
}
static char *__WordAtPoint(char *text) {
    char *orig=text;
    while(isalnum(*text)||*text=='_') text++;
    if(text==orig) return NULL;
    return strncpy(TD_MALLOC(1+text-orig),orig,text-orig);
}

void VisitBreakpoint(struct jit_debugger_regs *regs,jit_breakpoint_t *bp,void *caller_ptr) {
    Debugger.curBp=bp;
    if(!bp) goto dbg;
    dbg:
    ;
    CBreakpoint *cbp=NULL;
    void *_backtrace[256];
    void *_frameptrs[256];
    int stacksz=jit_FunctionParents(_backtrace,256);
    jit_ParentFramePtrs(_frameptrs,256);
    long idx=0;
    for(;idx!=256;idx++)
        if(_backtrace[idx]==caller_ptr)
            break;
    if(idx!=256) {
        memmove(_backtrace,&_backtrace[idx],(256-idx)*sizeof(void*));
        memmove(_frameptrs,&_frameptrs[idx],(256-idx)*sizeof(void*));
    }
    stacksz-=idx;
    //Look for caller pointer
    printf("Welcome to debugger land.\n");
    long stkpos=0;
    cbp=GetBreakpointByPtr(_backtrace[stkpos]);
    long curln=-1;//=Debugger.callStack.data[stkpos].lastbp->ln-1; //Lines are 1 indexed
    const char*curfn="???";//Debugger.callStack.data[stkpos].lastbp->fn;
    if(cbp) {
      curln=cbp->ln;
      curfn=cbp->fn;
      ListLines(curfn, curln-1, curln);
    }
    static char prevCommand[512];
loop:
    ;
    char ibuffer[1024];
    char *s=rl("BSAT> ");
    char *_gcs=strdup(s); //Garbage collected
    if(strlen(s)) strcpy(prevCommand,s);
    if(!strlen(s)) _gcs=strdup(prevCommand);
    TD_FREE(s);
    s=__SkipWhitespace(_gcs);
    char *word=__WordAtPoint(s);
    if(!word) goto loop;
    if(0==strcmp(word,"down")) {
        if(0<stkpos) stkpos--;
        else {
            printf("At bottommost stack item!\n");
            goto lfail;
        }
        cbp=GetBreakpointByPtr(_backtrace[stkpos]);
        if(cbp) {
            curln=cbp->ln;
            curfn=cbp->fn;
            ListLines(curfn, curln-1, curln);
        } else {
            curln=-1;
            curfn="???";
        }
    } else if(0==strcmp(word,"up")) {
        if(stacksz>1+stkpos) stkpos++;
        else {
            printf("At topmost stack item!\n");
            goto lfail;
        }
        cbp=GetBreakpointByPtr(_backtrace[stkpos]);
        if(cbp) {
            curln=cbp->ln;
            curfn=cbp->fn;
            ListLines(curfn, curln-1, curln);
        } else {
            curln=-1;
            curfn="???";
        }
    } else if(0==strcmp(word,"help")) {
help:
        printf("help\t-- Display this message\n");
        printf("up\t-- Go up the call stack.\n");
        printf("down\t-- Go down the call stack.\n");
        printf("list\t-- List lines at current position\n");
        printf("list fn:num\t-- List lines at position.\n");
        printf("list num\t-- List lines at position.\n");
        printf("b\t-- Set breakpoint at current line.\n");
        printf("b fn:num\t-- Set breakpoint at position.\n");
        printf("b num\t-- Set breakpoint at line in current function.\n");
        printf("d\t-- Delete all breakpoints.\n");
        printf("d num\t-- Delete a breakpoint.\n");
        printf("s\t-- Step into function.\n");
        printf("n\t-- Next line.\n");
        printf("f\t-- Finish function.\n");
        printf("listb\t-- List all active breakpoints.\n");
        printf("bt\t-- Backtrace.\n");
        printf("exit\t-- Exit the debugger.\n");
        printf("Press enter for last command.");
    } else if(0==strcmp(word,"b")) {
        s+=strlen("b");
        s=__SkipWhitespace(s);
        char *colon=strrchr(s, ':');
        long line=-1;
        if(colon) {
            sscanf(colon+1,"%ld",&line);
            *colon=0;
        } else if(sscanf(s,"%ld",&line)) {
            if(cbp)
                s=cbp->fn;
        } else if(cbp) {
                s=cbp->fn;
                line=cbp->ln;
        } else {
            line=-1;
        }
        int matched=0;
        //Match filename
        if(GetVariable("LexMatchFile")) {
            const char *fn=((const char*(*)(void*,char *))GetVariable("LexMatchFile")->func->funcptr)(Lexer.HCLexer,s);
            if(fn) {
                char buffer[256];
                sprintf(buffer,"%s:%ld",fn,line);
                CBreakpoint *bps=map_get(&Compiler.breakpoints,buffer);
                if(bps) {
                    matched++;
                    printf("Match at %s:%ld\n",bps->fn,bps->ln);
                    jit_breakpoint_t *bp;
                    long iter;
                    int prev_enabled=0;
                    vec_foreach(&bps->jit_bps,bp,iter) {
                        if(bp->enabled)
                            prev_enabled=1;
                        bp->enabled=1;
                    }
                    if(prev_enabled) {
                        printf("Breakpoint is already present at %s:%li\n",bps->fn,bps->ln);
                    } else {
                        sprintf(buffer,"%ld",++Debugger.breakpointNumber);
                        map_set(&Debugger.activeBreakpoints,buffer,bps);
                    }
                }
            }
        }
        if(!matched)
            printf("Failed to match any breakpoints\n");
    } else if(0==strcmp(word,"d")) {
        s++;
        s=__SkipWhitespace(s);
        long num;
        if(!*s) {
            printf("Delete all breakpoints?\n");
            char *res=rl("[Yn]: ");
            if(toupper(*res)=='Y') {
                const char *key;
                remallloop:;
                map_iter_t biter=map_iter(&Debugger.activeBreakpoints);
                while(key=map_next(&Debugger.activeBreakpoints,&biter)) {
                    CBreakpoint *bps=map_get(&Debugger.activeBreakpoints,key);
                    jit_breakpoint_t *bp;
                    long iter;
                    vec_foreach(&bps->jit_bps,bp,iter) {
                        bp->enabled=1;
                    }
                    map_remove(&Debugger.activeBreakpoints,key);
                    goto remallloop;
                }
            }
#ifndef TARGET_WIN32
            TD_FREE(res);
#endif
        } else if(!sscanf(s, "%ld", &num)) {
            printf("Expected a breakpoint to delete.\n");
        } else {
            char buffer[strlen(s)+1];
            sprintf(buffer,"%ld",num);
            CBreakpoint **bps=map_get(&Debugger.activeBreakpoints,buffer);
            if(bp) {
                jit_breakpoint_t *bp;
                long iter;
                vec_foreach(&bps[0]->jit_bps,bp,iter) {
                    bp->enabled=1;
                }
                map_remove(&Debugger.activeBreakpoints,buffer);
            } else {
                printf("Active breakpoint %ld doesn't exist.\n",num);
            }
        }
    } else if(0==strcmp(word,"listb")) {
        long i;
        for(i=1; i<=Debugger.breakpointNumber; i++) {
            char buffer[256];
            sprintf(buffer,"%ld",i);
            CBreakpoint **bp;
            if(bp=map_get(&Debugger.activeBreakpoints,buffer)) {
                printf("[%ld] %s:%ld\n",i,bp[0]->fn,bp[0]->ln);
            }
        }
    } else if(0==strcmp(word,"s")) {
        Debugger.ctrl.code=JIT_DBG_STEP;
        goto end;
    } else if(0==strcmp(word,"f")) {
        Debugger.ctrl.code=JIT_DBG_FIN;
        //Typically VisitBreakpoint is called indirecty so finish when above caller of caller
        Debugger.ctrl.prev_stack_ptr=__builtin_frame_address(2);
        goto end;
    } else if(0==strcmp(word,"p")) {
        Debugger.curBp=GetJITBreakpointByPtr(_backtrace[stkpos]);
        if(stkpos!=0&&regs) {
            s+=strlen(word);
            s=__SkipWhitespace(s);
            CFunction *f=GetFunctionByPtr(_backtrace[stkpos]);
            if(!f) {
                printf("Cant get info for current function.\n");
                DebugEvalExpr(NULL, NULL, s,NULL);
            } else
                DebugEvalExprAtFrame(cbp, &f->funcInfo, s, _frameptrs[stkpos]);
        } else {
            s+=strlen(word);
            s=__SkipWhitespace(s);
            CFunction *f=GetFunctionByPtr(_backtrace[stkpos]);
            if(!f) {
                printf("Cant get info for current function.\n");
                DebugEvalExpr(NULL, NULL, s,NULL);
            } else
                DebugEvalExpr(cbp, &f->funcInfo, s,regs);
        }
    } else if(0==strcmp(word,"n")) {
        Debugger.ctrl.code=JIT_DBG_NEXT;
        //Typically VisitBreakpoint is called indirecty so finish when above caller of caller
        Debugger.ctrl.prev_stack_ptr=__builtin_frame_address(2);
        goto end;
    } else if(0==strcmp(word,"bt")) {
        Backtrace(stkpos,idx);
    } else if(0==strcmp(word,"exit")) {
        Debugger.ctrl.code=0;
        goto end;
    } else if(0==strcmp(word,"list")) {
        s+=strlen(word);
        s=__SkipWhitespace(s);
        char *colon=strrchr(s,':');
        long tmpln;
        if(colon) {
            *colon=0;
            if(!sscanf(colon+1, "%ld", &tmpln)) {
                printf("Expected a line number.\n");
                goto lfail;
            }
        } else if(1==sscanf(s, "%ld", &tmpln)) {
            curln=tmpln;
        } else
            //Are we at the end of the string or do we have a filenmae
            if(*s) {

                const char *tmpfn;
                if(tmpfn=ListLines(s, 0, 0)) //Dummy run to get fn match
                    curfn=tmpfn;
                else goto lfail;
                curln=tmpln;
            }
        long start=curln-5;
        if(start<0) start=0;
        long endln=start+11;
        ListLines(curfn, start,endln);
        curln+=11;
lfail:
        ;
    } else {
        if(word) printf("Unkown command %s\n\n",word);
        goto help;
    }
    TD_FREE(word);
    TD_FREE(_gcs);
    goto loop;
end:
    TD_FREE(word);
    TD_FREE(_gcs);
}

void DbgEnterFunction(void *baseptr,CFunction *func) {
}
void DbgLeaveFunction() {
}
void Backtrace(int stkpos,int trim) {
    void *bt[256];
    int top=jit_FunctionParents(bt,256),iter;
    struct CBreakpoint *bp;
    jit_breakpoint_t *jbp;
    for(iter=0;iter<top-(trim+1);iter++) {
        CFunction *func=GetFunctionByPtr(bt[iter+trim+1]);
        if(func) {
            if(func->JIT)
                if(jbp=jit_get_breakpoint_btr_ptr(func->JIT,bt[iter+trim+1])) {
                    bp=*(CBreakpoint**)jbp->user_data;
                    printf(" %c [%d] %s:%s,%d\n",(stkpos==iter?' ':'*'),iter,func->name,bp->fn,bp->ln);
                    goto skip;
                }
            printf(" %c [%d] %s:???\n",(stkpos==iter?' ':'*'),iter,func->name);
            goto skip;
        }
        printf(" %c [%d] ???\n",(stkpos==iter?' ':'*'),iter);
        skip:;
    }
}
void WhineOnOutOfBounds(void *ptr,int64_t sz) {
    if(!InBounds(ptr,sz)) {
        Backtrace(-1,0);
    }
}
//Returns matched file
static const char* ListLines(const char *fn,long start,long end) {
    if(!GetVariable("LexMatchFile")) return NULL;

    fn=((const char*(*)(void*,const char *))GetVariable("LexMatchFile")->func->funcptr)(Lexer.HCLexer,fn);
    if(!fn) {
        printf("Coudln't find a match for file \"%s\"(may not be compiled yet).",fn);
        return NULL;
    }

    assert(GetVariable("LexFileLines"));
    int64_t length;
    int64_t *lines=((int64_t*(*)(void*,const char *,int64_t *))GetVariable("LexFileLines")->func->funcptr)(Lexer.HCLexer,fn,&length);
found:
    ;
    FILE *f=fopen(fn,"r");
    if(!f) {
        printf("Can't open \"%s\" for listing",fn);
        return  NULL;
    }
    long ln=start;
    for(; ln!=end; ln++) {
        if(length<=ln) break;
        long pstart=lines[ln];
        long pend;
        if(length>ln+1)
            pend=lines[ln+1];
        else {
            fseek(f,0, SEEK_END);
            pend=ftell(f);
        }
        fseek(f, pstart, SEEK_SET);
        char buffer[pend-pstart+1];
        fread(buffer, 1, pend-pstart, f);
        buffer[pend-pstart]=0;
        printf("[%li]:  %s",ln+1,buffer);
    }
    fclose(f);
    return fn;
}

#endif
