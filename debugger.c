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
#endif
CDebugger Debugger;
void Backtrace() {
    CBacktrace *bt;
    int iter;
    vec_foreach_ptr_rev(&Debugger.callStack, bt, iter) {
        if(bt->lastbp)
            fprintf(stderr,"  [%i] %s:%li<%s>\n",iter,bt->lastbp->fn,bt->lastbp->ln,bt->func->name);
        else
            fprintf(stderr,"  [%i] ?:?<%s>\n",iter,bt->func->name);
    }
}
void WhineOnOutOfBounds(void *ptr) {
    if(!InBounds(ptr)) {
        Backtrace();
    }
}
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
void EnterDebugger() {
    if(!Compiler.debugMode) return;
    VisitBreakpoint(NULL);
}
void DbgPrintVar(CType *type,void *ptr) {
    ExceptBuf old;
    memcpy(&old,&SigPad,sizeof(SigPad));
    int code;
    if(code=HCSetJmp(SigPad)) {
        vec_truncate(&Debugger.callStack,0);
#ifndef TARGET_WIN32
        sigset_t empty;
        sigfillset(&empty);
        sigprocmask(SIG_UNBLOCK,&empty,NULL);
#endif
        signal(SIGSEGV,SignalHandler);
        signal(SIGABRT,SignalHandler);
        signal(SIGINT,SignalHandler);
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
    }
    memcpy(&SigPad,&old,sizeof(SigPad));
}
static CBreakpoint *GetBreakInFile(char *filename,long line) {
    char buffer[256];
    sprintf(buffer,"%s:%ld",filename,line);
    CBreakpoint *bp=map_get(&Compiler.breakpoints,buffer);
    if(bp) return bp;
    printf("Breakpoint %s,%ld doesn't exist(may not be Compiled yet).\nWould you like to pend on this location?.",filename,line);
    char *res=rl("[Yn]: ");
    if('Y'==toupper(res[0])) {
        CBreakpoint bp;
        bp.fn=*(char**)map_get(&Lexer.filenames,filename);
        bp.ln=line;
        bp.enabled=0;
        map_set(&Compiler.breakpoints, buffer, bp);
        return map_get(&Compiler.breakpoints, buffer);
    }
#ifndef TARGET_WIN32
    TD_FREE(res);
#endif
    return NULL;
}
static int FilenameMatch(const char *_from,const char *_match) {
    if(0==strcmp(_from,_match)) return 1;
    char *from=strdup(_from);
    char* match=strdup(_match);
    char *of=from,*om=match;
loop:
    if(0==strcmp(basename(from),basename(match))) {
        from=dirname(from);
        match=dirname(match);
        if(0==strcmp(".", from)||0==strcmp("/", from)) goto end;
        if(0==strcmp(".", match)||0==strcmp("/", match)) goto end;
        goto loop;
    } else {
        TD_FREE(of);
        TD_FREE(om);
        return 0;
    }
end:
    TD_FREE(of);
    TD_FREE(om);
    return 1;
}
//Returns matched file
static const char* ListLines(const char *fn,long start,long end) {
    map_iter_t fiter=map_iter(&Lexer.filenames);
    const char *key;
    while(key=map_next(&Lexer.filenames, &fiter)) {
        if(FilenameMatch(key,fn)) {
            fn=key;
            goto found;
        }
    }
    printf("Coudln't find a match for file \"%s\"(may not be compiled yet).",fn);
found:
    ;
    FILE *f=fopen(fn,"r");
    if(!f) {
        printf("Can't open \"%s\" for listing",fn);
        return  NULL;
    }
    vec_int_t *lines=map_get(&Lexer.fileLineStarts,fn);
    long ln=start;
    for(; ln!=end; ln++) {
        if(lines->length<=ln) break;
        long pstart=lines->data[ln];
        long pend;
        if(lines->length>ln+1)
            pend=lines->data[ln+1];
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
void VisitBreakpoint(CBreakpoint *bp) {
    if(!bp) goto dbg;
    int stksz=Debugger.callStack.length;
    if(bp)
        vec_last(&Debugger.callStack).lastbp=bp;
    else bp=vec_last(&Debugger.callStack).lastbp;
    if(Debugger.fin) {
        if(stksz<Debugger.prevStackDepth) {
            Debugger.fin=0;
            goto dbg;
        }
    } else if(Debugger.next) {
        if(stksz<=Debugger.prevStackDepth) {
            Debugger.next=0;
            goto dbg;
        }
    } else if(Debugger.step) {
        Debugger.step=0;
        goto dbg;
    }
    if(bp)
        if(!bp->enabled) return;
dbg:
    ;
    Debugger.prevStackDepth=Debugger.callStack.length;
    printf("Welcome to debugger land.\n");
    long stkpos=Debugger.callStack.length-1;
    long curln=Debugger.callStack.data[stkpos].lastbp->ln-1; //Lines are 1 indexed
    const char*curfn=Debugger.callStack.data[stkpos].lastbp->fn;
    ListLines(curfn, curln, curln+1);
    static char prevCommand[512];
loop:
    ;
    char ibuffer[1024];
    char *s=rl("BSAT> ");
    char *_gcs=strdup(s); //Garbage collected
    if(strlen(s)) strcpy(prevCommand,s);
    if(!strlen(s)) _gcs=strdup(prevCommand);
    if(strlen(s)) AddHistory(s);
    TD_FREE(s);
    s=__SkipWhitespace(_gcs);
    char *word=__WordAtPoint(s);
    if(!word) goto loop;
    if(0==strcmp(word,"down")) {
        if(stkpos+1<=Debugger.callStack.length) stkpos++;
        else {
            printf("At bottommost stack item!\n");
            goto lfail;
        }
        curln=Debugger.callStack.data[stkpos].lastbp->ln-1; //Lines are 1 indexed
        curfn=Debugger.callStack.data[stkpos].lastbp->fn;
        ListLines(curfn, curln, curln+1);
    } else if(0==strcmp(word,"up")) {
        if(stkpos-1>=0) stkpos--;
        else {
            printf("At topmost stack item!\n");
            goto lfail;
        }
        curln=Debugger.callStack.data[stkpos].lastbp->ln-1; //Lines are 1 indexed
        curfn=Debugger.callStack.data[stkpos].lastbp->fn;
        ListLines(curfn, curln, curln+1);
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
        CBacktrace cur=Debugger.callStack.data[stkpos];
        char *colon=strrchr(s, ':');
        long line=-1;
        if(colon) {
            sscanf(colon+1,"%ld",&line);
            *colon=0;
        } else if(sscanf(s,"%ld",&line)) {
            s=(char *)cur.lastbp->fn;
        } else {
            line=cur.lastbp->ln;
            s=(char *)cur.lastbp->fn;
        }
        int matched=0;
        //Match filename
        map_iter_t fiter=map_iter(&Lexer.filenames);
        const char *key;
        while(key=map_next(&Lexer.filenames, &fiter)) {
            if(FilenameMatch((char*)key,s)) {
                CBreakpoint *bp=GetBreakInFile((char*)key,line);
                if(bp) {
                    matched++;
                    printf("Match at %s:%ld\n",bp->fn,bp->ln);
                    if(bp->enabled) {
                        printf("Breakpoint is already present at %s:%li\n",bp->fn,bp->ln);
                    } else {
                        bp->enabled=1;
                        char buffer[256];
                        sprintf(buffer,"%ld",++Debugger.breakpointNumber);
                        map_set(&Debugger.activeBreakpoints,buffer,bp);
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
                map_iter_t biter=map_iter(&Debugger.activeBreakpoints);
                while(key=map_next(&Debugger.activeBreakpoints,&biter)) {
                    map_get(&Debugger.activeBreakpoints,key)[0]->enabled=0;
                    map_remove(&Debugger.activeBreakpoints,key);
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
            CBreakpoint **bp=map_get(&Debugger.activeBreakpoints,buffer);
            if(bp) {
                bp[0]->enabled=0;
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
        Debugger.step=1;
        goto end;
    } else if(0==strcmp(word,"f")) {
        Debugger.fin=1;
        goto end;
    } else if(0==strcmp(word,"p")) {
        ExceptBuf old;
        memcpy(&old,&SigPad,sizeof(SigPad));
        int code;
        if(code=HCSetJmp(SigPad)) {
#ifndef TARGET_WIN32
            sigset_t empty;
            sigfillset(&empty);
            sigprocmask(SIG_UNBLOCK,&empty,NULL);
#endif
            signal(SIGSEGV,SignalHandler);
            signal(SIGABRT,SignalHandler);
            signal(SIGINT,SignalHandler);
        } else {
            s+=strlen(word);
            s=__SkipWhitespace(s);
            CBacktrace cur=Debugger.callStack.data[stkpos];
            DebugEvalExpr(cur.bp, &cur.func->funcInfo, s);
        }
        memcpy(&SigPad,&old,sizeof(old));
    } else if(0==strcmp(word,"n")) {
        Debugger.next=1;
        goto end;
    } else if(0==strcmp(word,"bt")) {
        CBacktrace *bt;
        int iter;
        vec_foreach_ptr_rev(&Debugger.callStack, bt, iter) {
            char *marker=(iter==stkpos)?" *":"  ";
            if(bt->lastbp)
                printf("%s[%i] %s:%li<%s>\n",marker,iter,bt->lastbp->fn,bt->lastbp->ln,bt->func->name);
            else
                printf("%s[%i] ?:?<%s>\n",marker,iter,bt->func->name);
        }
    } else if(0==strcmp(word,"exit")) {
        Debugger.next=Debugger.step=Debugger.fin=0;
        goto end;
    } else if(0==strcmp(word,"list")) {
        s+=strlen(word);
        s=__SkipWhitespace(s);
        char *colon=strrchr(s,':');
        long tmpln;
        if(colon) {
            *colon=0;
            if(!sscanf(s, "%ld", &tmpln)) {
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
    CBacktrace bt;
    bt.bp=baseptr;
    bt.func=func;
    bt.lastbp=NULL;
    vec_push(&Debugger.callStack,bt);
}
void DbgLeaveFunction() {
    vec_pop(&Debugger.callStack);
}
void SignalHandler(int s) {
    Backtrace();
    printf("\n");
    switch(s) {
    case SIGABRT:
        printf("User abort.type \"Exit(0);\" to exit.\n");
        break;
    case SIGSEGV:
        printf("Segmentation fault.\n");
        break;
    default:
        printf("Strange signal %d.\n",s);
        break;
    }
    HCLongJmp(SigPad);
}
