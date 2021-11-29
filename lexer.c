#include "3d.h"
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <libgen.h>
#include <unistd.h>
#ifndef TARGET_WIN32
#include <dirent.h>
#else
#include <fileapi.h>
#endif
#include <sys/stat.h>
#include "HolyC.tab.h"
#include <limits.h>
#include <stdlib.h>
#include "rl.h"
#define REPL_SOURCE_NAME "(nofile)"
void DisableREPL() {
    Lexer.replMode=0;
}
void EnableREPL() {
    Lexer.replMode=1;
}
CLexer Lexer;
static void SpliceToCurrentFromBehind(long behind);
static void PopNullBytes(vec_char_t *v) {
    while(v->length!=0) if(vec_last(v)==0) vec_pop(v);
        else break;
}
static void __InsertSrcText(long at,char *exp) {
    long inslen=strlen(exp);
    if(at>=Lexer.cursor_pos) {
        Lexer.physical_fpos_start+=inslen;
    }
    //Move file ends forward
    CLexerFileState *tmp;
    int iter;
    vec_foreach_ptr(&Lexer.fileStates,tmp,iter) {
        tmp->physical_fpos_start+=inslen;
    }
    mrope_t *tmpr=TD_MALLOC(sizeof(mrope_t));
    mrope_init(tmpr);
    //"CHUNK" the input text to make it smaller for each peice to be modified
    #define CHUNK_SIZE 256
    int64_t len=strlen(exp);
    for(;*exp;) {
        char buffer[CHUNK_SIZE+1];
        if(len<=CHUNK_SIZE) {
            mrope_append_text(tmpr, strdup(exp));
            exp+=len;
        } else {
            strncpy(buffer,exp,CHUNK_SIZE);
            buffer[CHUNK_SIZE]=0;
            len-=CHUNK_SIZE;
            exp+=CHUNK_SIZE;
            mrope_append_text(tmpr, strdup(buffer));
        }
    }

    mrope_insert(Lexer.source, at, tmpr);
    mrope_free(tmpr);
}
void FPosToLineCol(char *fn,long fpos,int  *line,int *col) {
    vec_int_t*find=map_get(&Lexer.fileLineStarts,fn);
    if(!find) {
        FILE *f=fopen(fn,"rb");
        if(!f) {
            if(line) *line=1;
            if(col) *col=fpos;
            return;
        }
        fseek(f,0,SEEK_END);
        long cnt=ftell(f);
        fseek(f,0,SEEK_SET);

        int _line=1;
        int _col=1;
        vec_int_t starts;
        vec_init(&starts);
        vec_push(&starts,0);
        while(--cnt>=0) {
            char chr;
            if(-1==(chr=fgetc(f))) abort();
            if(chr=='\n') {
                vec_push(&starts,ftell(f));
                _col=1;
            } else
                _col++;
        }
        map_set(&Lexer.fileLineStarts,fn,starts);
        FPosToLineCol(fn,fpos,line,col);
    } else {
        int line2=0,lnstart,iter;
        vec_foreach(find, lnstart, iter) {
            if(lnstart==fpos) {
                if(line) *line=line2;
                if(col) *col=1;
                return;
            } else if(lnstart>fpos) {
prevln:
                if(line) *line=line2;
                if(col) *col=1+fpos-find->data[iter-1];
                return;
            }
            line2++;
        }
        goto prevln;
    }
}
void LexerRaiseError(char *msg) {
    int ln,col;
    FPosToLineCol(Lexer.filename,Lexer.fpos,&ln,&col);
    if(!Compiler.silentMode||Compiler.errorsFile) fprintf((Compiler.errorsFile)?Compiler.errorsFile:stderr,"%s:%i:%i: WARNING %s\n",Lexer.filename,ln,col,msg);
}

#define UNTIL_PHYSICAL ((Lexer.cursor_pos>Lexer.physical_fpos_start)?0:Lexer.physical_fpos_start-Lexer.cursor_pos)
static long LEXER_NEXT() {
    if(UNTIL_PHYSICAL==0) {
        if(Lexer.macroExpandDepth==0) {
            Lexer.fpos++;
            Lexer.physical_fpos_start=1+Lexer.cursor_pos;
        }
        if(Lexer.fpos>=Lexer.curFileEnd) {
            if(Lexer.fileStates.length) {
                //Flush old file's source
                if(Lexer.isFreeToFlush) {
                  SpliceToCurrentFromBehind(0);
                }
                //Truncate source to size of last file cursor
                RestoreLexerFileState(vec_pop(&Lexer.fileStates));
            }
        }
    }
    return Lexer.cursor_pos++;
}
#define LEXER_REWIND(n) {Lexer.cursor_pos-=n;}
char LEXER_PEEK() {
    if(mrope_length(Lexer.source)>Lexer.cursor_pos)
        return mrope_index(Lexer.source,Lexer.cursor_pos);
    return 0;
}
static int IsEscaped(int at) {
    if(at-1>=0) {
        if(mrope_index(Lexer.source, at-1)=='\\')
            return !IsEscaped(at-1);
    }
    return 0;
}
typedef vec_t(CCompletion) vec_CCompletion_t;
CCompletion *ReplCompleteions(const char *start,long srcoff,const char *text,long *length) {
    map_iter_t gi=map_iter(&Compiler.globals);
    vec_CCompletion_t ret;
    vec_init(&ret);
    const char* key;
    char buffer[1024];
    while(key=map_next(&Compiler.types, &gi)) {
        CType **find=map_get(&Compiler.types,key);
        if(text) if(0!=strncmp(text, key,strlen(text))) goto memberchk;
        sprintf(buffer,"%s:%s",((find[0]->type==TYPE_CLASS)?"Class":"Union"),key);
        CCompletion comp= {strdup(buffer),strdup(key)};
        vec_push(&ret, comp);
        //Complete members
memberchk:
        ;
        vec_CMember_t *members=NULL;
        if(find[0]->type==TYPE_CLASS) members=&find[0]->cls.members;
        else if(find[0]->type==TYPE_UNION) members=&find[0]->un.members;
        CMember mem;
        int iter;
        if(members)
            vec_foreach(members, mem, iter) {
            if(text) if(0!=strncmp(text, mem.name,strlen(text))) continue;
            char *ts=TypeToString(mem.type);
            sprintf(buffer,"Member:%s.%s(%s)",key,mem.name,ts);
            CCompletion comp= {strdup(buffer),strdup(mem.name)};
            TD_FREE(ts);
            vec_push(&ret, comp);
        }
    }
    gi=map_iter(&Compiler.globals);
    while(key=map_next(&Compiler.globals, &gi)) {
        if(text) if(0!=strncmp(text, key,strlen(text))) continue;
        CVariable *find=map_get(&Compiler.globals,key)[0];
        if(find->isInternal) continue;
        char *type=TypeToString(find->type);
        if(!find->isFunc)
            sprintf(buffer,"Global:%s(%s)",key,type);
        else
            sprintf(buffer,"Function:%s(%s)",key,type);
        CCompletion comp= {strdup(buffer),strdup(key)};
        vec_push(&ret, comp);
        TD_FREE(type);
    }
    map_iter_t li=map_iter(&Compiler.locals);
    while(key=map_next(&Compiler.locals, &gi)) {
        if(text) if(0!=strncmp(text, key,strlen(text))) continue;
        char *type=TypeToString(map_get(&Compiler.locals,key)[0]->type);
        sprintf(buffer,"Local:%s(%t)",key,type);
        CCompletion comp= {strdup(buffer),strdup(key)};
        vec_push(&ret, comp);
        TD_FREE(type);
    }
    *length=ret.length;
    return ret.data;
}
void InitREPL() {
    InitRL();
    rlACGen=&ReplCompleteions;
}
void WaitForInput() {
    if(Lexer.isFreeToFlush) FlushLexer();
#ifndef TARGET_WIN32
    sigset_t oldset;
    sigset_t newset;
    sigfillset(&newset);
    sigprocmask(SIG_UNBLOCK,&newset,&oldset);
    void *oldhand=signal(SIGINT,SignalHandler);
    char *input=rl("HolyCC:>> ");
    sigprocmask(SIG_SETMASK,&oldset,NULL);
    signal(SIGINT,oldhand);
    #else
    char *input=rl("HolyCC:>> ");
    #endif
set:
    ;
    //Add to REPL_SOURCE_NAME's file lines
    vec_int_t *lns;
    if(lns=map_get(&Lexer.fileLineStarts,REPL_SOURCE_NAME)) {
        vec_push(lns,ftell(Lexer.replSource));
    } else {
        vec_int_t dummy;
        vec_init(&dummy);
        map_set(&Lexer.fileLineStarts,REPL_SOURCE_NAME,dummy);
        goto set;
    }
    mrope_append_text(Lexer.source, input);
    mrope_append_text(Lexer.source, strdup("\n"));
    fwrite(input,strlen(input),1,Lexer.replSource);
    fputc('\n', Lexer.replSource);
}
static char *rope2str(mrope_t *r) {
    long len=mrope_length(r);
    char *ret=TD_CALLOC(1,len+1);
    while(--len>=0)
        ret[len]=mrope_index(r, len);
    return ret;
}
void FlushLexer() {
    CLexerFileState state;
    int iter;
    vec_foreach_rev(&Lexer.fileStates, state, iter)
    RestoreLexerFileState(state);
    mrope_delete(Lexer.source, 0, mrope_length(Lexer.source));
    Lexer.fpos=Lexer.cursor_pos=0;
    Lexer.physical_fpos_start=0;
    fseek(Lexer.replSource, 0, SEEK_SET);
    vec_int_t *lns;
    if(lns=map_get(&Lexer.fileLineStarts,REPL_SOURCE_NAME))
        vec_deinit(lns);
    Lexer.source=TD_MALLOC(sizeof(mrope_t));
    mrope_init(Lexer.source);
}
char *LexExpandText(char*text) {
    CLexerFileState s=CreateLexerFileState();
    long oldpos=Lexer.cursor_pos;
    mrope_t *oldSrc=Lexer.source;
    Lexer.source=TD_MALLOC(sizeof(mrope_t));
    mrope_init(Lexer.source);
    mrope_append_text(Lexer.source, strdup(text));
    Lexer.cursor_pos=0;
    int oldreplmode=Lexer.replMode;
    Lexer.replMode=0;
    //
loop:
    SkipWhitespace(SW_SKIP_STRING);
    if(LEXER_PEEK()=='#') {
        LexMacro();
    } else if(LEXER_PEEK()==0) {
        goto end;
    } else if(isalnum(LEXER_PEEK())||LEXER_PEEK()=='_') {
        LexExpandWord(); //Moves past expasnsion
    } else {
        LEXER_NEXT();
    }
    goto loop;
    //
end:
    ;
    char *ret;
    ret=rope2str(Lexer.source);
    mrope_free(Lexer.source);
    Lexer.replMode=oldreplmode;
    Lexer.source=oldSrc;
    Lexer.cursor_pos=oldpos;
    RestoreLexerFileState(s);
    return ret;
}
static char *__EscapeString(char stopat) {
    int old=Lexer.isFreeToFlush;
    Lexer.isFreeToFlush=0;
    vec_char_t vec;
    vec_init(&vec);
    int i;
    for(; LEXER_PEEK()!=stopat;) {
        if(!LEXER_PEEK()&&Lexer.replMode) {
            FlushLexer();
            WaitForInput();
            continue;
        } else if(!LEXER_PEEK()) {
            LexerRaiseError("Expected string terminator.");
            return NULL;
        }
        if(LEXER_PEEK()!='\\') {
            vec_push(&vec,LEXER_PEEK());
            LEXER_NEXT();
        } else {
            LEXER_NEXT();
            switch(LEXER_PEEK()) {
                char buffer[1024];
            default:
                sprintf(buffer,"Invalid escape charactor '%c'.",LEXER_PEEK());
                LexerRaiseError(buffer);
                break;
            case 'b':
                vec_push(&vec, '\b');
                LEXER_NEXT();
                break;
            case 't':
                vec_push(&vec, '\t');
                LEXER_NEXT();
                break;
            case 'e':
                vec_push(&vec, '\e');
                LEXER_NEXT();
                break;
            case 'r':
                vec_push(&vec, '\r');
                LEXER_NEXT();
                break;
            case 'n':
                vec_push(&vec, '\n');
                LEXER_NEXT();
                break;
            case '\\':
                vec_push(&vec, '\\');
                LEXER_NEXT();
                break;
            case '\'':
                vec_push(&vec, '\'');
                LEXER_NEXT();
                break;
            case '\"':
                vec_push(&vec, '\"');
                LEXER_NEXT();
                break;
            case 'x': {
                LEXER_NEXT();
                int xdigit=0;
#define XDIGIT(v) \
  switch(toupper(v)) { \
case '0'...'9': xdigit=(v)-'0'; break;  \
case 'A'...'F': xdigit=toupper(v)-'A'+10; break;  \
default:  \
goto xmalformed; \
  }
                XDIGIT(LEXER_PEEK());
                LEXER_NEXT();
                uint8_t hi=xdigit<<4;
                XDIGIT(LEXER_PEEK());
                LEXER_NEXT();
                uint8_t lo=xdigit;
                vec_push(&vec, hi|lo);
                break;
xmalformed:
                LexerRaiseError("Invalid hex escape.");
                vec_push(&vec, hi|lo);
                break;
            }
        case '0'...'7': {
            uint64_t es=0;
            int t=0;
            while(t<3) {
                if(LEXER_PEEK()>='0'&&'7'>=LEXER_PEEK()) {
                    es*=8;
                    es+=LEXER_PEEK()-'0';
                    LEXER_NEXT();
                }
                t++;
                }
                if(es>255) LexerRaiseError("Octal escapes excedes 255.");
                vec_push(&vec,es);
                break;
            }
            }
        }
    }
end:
    ;
    Lexer.isFreeToFlush=old;
    vec_push(&vec, 0);
    char *ret=strdup(vec.data);
    vec_deinit(&vec);
    return ret;
}
int LexExpandWord() {
    if(isalpha(LEXER_PEEK())||LEXER_PEEK()=='_') {
        long word_start=Lexer.cursor_pos;
        vec_char_t word;
        vec_init(&word);
        while(isalnum(LEXER_PEEK())||LEXER_PEEK()=='_') {
            vec_push(&word,LEXER_PEEK());
            LEXER_NEXT();
        }
        vec_push(&word, 0);
        CMacro *macro=map_get(&Lexer.macros, word.data);
        vec_deinit(&word);
        if(!macro) {
            goto end;
        }
        //Ensure macro of same name isnt being expanded
        char *name;
        int i;
        vec_foreach(&Lexer.expandChain, name, i) {
            if(0==strcmp(name,macro->name)) {
                char buffer[1024];
                sprintf(buffer,"Macro expansion loop with macro %s.",macro->name);
                LexerRaiseError(buffer);
                goto end;
            }
        }
        if(macro) {
            vec_push(&Lexer.expandChain,macro->name);
            SpliceToCurrentFromBehind(word_start);
            char *exp=LexExpandText(macro->expand);
            __InsertSrcText(word_start,exp);
            TD_FREE((exp));
            vec_pop(&Lexer.expandChain);
            return 1;
        }
    }
    while(isalnum(LEXER_PEEK()))
        LEXER_NEXT();
end:
    return 0;
}
void LexerSearch(char *text) {
    int old=Lexer.isFreeToFlush;
    Lexer.isFreeToFlush=0;
    char *orig=text;
#define CHECK \
  if(!*text) { \
    goto end;\
  }if(LEXER_PEEK()) { \
    if(LEXER_PEEK()==*text) { \
      text++; \
    } else { \
      text=orig; \
    } \
  }
loop:
    if(!LEXER_PEEK()&&Lexer.replMode) {
        FlushLexer();
        WaitForInput();
        goto loop;
    } else if(!LEXER_PEEK()&&!Lexer.replMode)
        goto fail;
    if(isalnum(LEXER_PEEK())||LEXER_PEEK()=='_') {
        long oldcpos=Lexer.cursor_pos;
        LexExpandWord(); //Moves past expasnsion
        Lexer.cursor_pos=oldcpos;
    }
    CHECK;
    LEXER_NEXT();
    goto loop;
end:
    ;
    LEXER_REWIND(strlen(orig));
fail:
    Lexer.isFreeToFlush=old;
}

AST *LexString() {
    if(LEXER_PEEK()=='\''||LEXER_PEEK()=='"') {
        long orig_pos=Lexer.cursor_pos+1;
        char delim=LEXER_PEEK();
        LEXER_NEXT();
        char *txt=__EscapeString(delim);
        LEXER_NEXT();
        AST *r=NULL;
        if(delim=='"')
            r=CreateString(txt);
        else {
            r=TD_MALLOC(sizeof(AST));
            r->refCnt=1;
            r->type=AST_CHAR;
            r->chr=0;
            long len=strlen(txt),i=0;
            while(i!=len) {
                r->chr|=txt[i]<<(i*8);
                i++;
            }
        }
        TD_FREE(txt);
        return r;
    }
    return NULL;
}
void SkipWhitespace(int flags) {
loop:
    if(!LEXER_PEEK()&&Lexer.replMode) {
        WaitForInput();
        goto loop;
    }
    if(flags&SW_SKIP_STRING)
        if(LEXER_PEEK()=='\''||LEXER_PEEK()=='"') {
            ReleaseAST(LexString());
        }
    long orig_pos=Lexer.cursor_pos;
    if(LEXER_PEEK()=='/') {
        LEXER_NEXT();
        if(LEXER_PEEK()=='/') {
            LexerSearch("\n");
            if(LEXER_PEEK()) {
                LEXER_NEXT();
            } else
                SpliceToCurrentFromBehind(orig_pos);
        } else if(LEXER_PEEK()=='*') {
            LexerSearch("*/");
            if(LEXER_PEEK()) {
                LEXER_NEXT(),LEXER_NEXT();
                SpliceToCurrentFromBehind(orig_pos);
            }
        } else {
            Lexer.cursor_pos=orig_pos;
            return;
        }
    } else if(Lexer.stopAtNewline&&LEXER_PEEK()=='\n') {
        return;
    } else if(isblank(LEXER_PEEK())||LEXER_PEEK()=='\n'||LEXER_PEEK()=='\r') {
        LEXER_NEXT();
    } else {
        return;
    }
    goto loop;
}
char *WordAtPoint(int expandMacro) {
    long origcpos=Lexer.cursor_pos;
    if(expandMacro)
        LexExpandWord(); //Moves past expasnsion
    Lexer.cursor_pos=origcpos;
    SkipWhitespace(0);
    if(isalpha(LEXER_PEEK())||LEXER_PEEK()=='_') ;
    else return NULL;
    vec_char_t ret;
    vec_init(&ret);
loop:
    if(LEXER_PEEK()=='_'||isalnum(LEXER_PEEK())) {
        vec_push(&ret,LEXER_PEEK());
        LEXER_NEXT();
        goto loop;
    }
    vec_push(&ret,0);
    char *ret2=strdup(ret.data);
    vec_deinit(&ret);
    return ret2;
}
static void SpliceToCurrentFromBehind(long behind) {
    //Move file ends backward
    CLexerFileState *tmp;
    int iter;
    vec_foreach_ptr(&Lexer.fileStates,tmp,iter) {
        tmp->physical_fpos_start-=Lexer.cursor_pos-behind;
    }
    mrope_delete(Lexer.source, behind, Lexer.cursor_pos-behind);
    Lexer.physical_fpos_start-=Lexer.cursor_pos-behind;
    Lexer.cursor_pos=behind;
}
CLexerFileState CreateLexerFileState() {
    char olddir[1024];
    CLexerFileState backup;
    backup.oldfile=strdup(Lexer.filename);
    backup.olddir=strdup(getcwd(olddir,1024));
    backup.fpos=Lexer.fpos;
    backup.physical_fpos_start=Lexer.physical_fpos_start;
    backup.fileEnd=Lexer.curFileEnd;
    backup.cursor=Lexer.cursor_pos;
    backup.isFreeToFlush=Lexer.isFreeToFlush;
    return backup;
}
void StreamPrint(const char *fmt,int64_t argc,int64_t *argv) {
    if(Lexer.currentExeStream) {
        char *txt=MStrPrint(fmt,argc,argv);
        mrope_append_text(Lexer.currentExeStream, strdup(txt));
        TD_FREE(txt);
    }
}
void RestoreLexerFileState(CLexerFileState backup) {
    strcpy(Lexer.filename,backup.oldfile);
    TD_FREE(backup.oldfile);
    chdir(backup.olddir);
    TD_FREE(backup.olddir);
    Lexer.fpos=backup.fpos;
    Lexer.physical_fpos_start=backup.physical_fpos_start;
    Lexer.curFileEnd=backup.fileEnd;
}
void HandleLexerIf(int success) {
    CPreprocIfState state= {.success=success,.hasElse=0};
    vec_push(&Lexer.ifStates,state);
}
static int IsBinFile(char *text) {
    char *dot;
    if(dot=strrchr(text,'.'))
        if(!strcmp(dot+1,"BIN")||!strcmp(dot+1,"bin"))
            return 1;
    return 0;
}
static int IsZSourceFile(char *text) {
    char *dot;
    char clone[strlen(text)+1];
    strcpy(clone,text);
    if(dot=strrchr(clone,'.')) {
        if(!strcmp(dot+1,"Z")||!strcmp(dot+1,"z")) {
            *dot=0;
            return !IsBinFile(clone);
        }
    }
    return 0;
}
static int IsZBinFile(char *text) {
    char *dot;
    char clone[strlen(text)+1];
    strcpy(clone,text);
    if(dot=strrchr(clone,'.')) {
        if(!strcmp(dot+1,"Z")||!strcmp(dot+1,"z")) {
            *dot=0;
            return IsBinFile(clone);
        }
    }
    return 0;
}
static FILE *ExpandFile(char *fn) {
    FILE *ret=NULL;
    CVariable **expand;
    CType **carcCompress;
    if(!(expand=map_get(&Compiler.globals,"ExpandBuf"))) {
        LexerRaiseError("ExpandBuf not loaded,can't load \".Z\" files.");
    } else if(!(carcCompress=map_get(&Compiler.types,"CArcCompress"))) {
        LexerRaiseError("CArcCompress not loaded,can't load \".Z\" files.");
    } else {
        assert(expand[0]->func->funcptr);
        FILE *f=fopen(fn,"rb");
        long start=ftell(f);
        fseek(f,0,SEEK_END);
        long end=ftell(f);
        fseek(f,0,SEEK_SET);
        void *buf=TD_MALLOC(end-start);
        fread(buf,1,end-start,f);
        fclose(f);
        void *exp=((void*(*)(void*))expand[0]->func->funcptr)(buf);

        long iter,offset=-1;
        CMember mem;
        vec_foreach(&carcCompress[0]->cls.members,mem,iter)
            if(!strcmp(mem.name,"expanded_size")) {
                offset=mem.offset;
                break;
            }
        assert(mem.offset!=-1);
        long expanded_size=*(int64_t*)((char*)buf+offset);

        TD_FREE(buf);
        if(!exp) {
            LexerRaiseError("Invalid 3Days archive.");
        } else {
            ret=tmpfile();
            fwrite(exp,1,expanded_size,ret);
            fseek(ret,0,SEEK_SET);
        }
    }
    return ret;
}
int LexMacro() {
    int ret=0;
    long orig_pos=Lexer.cursor_pos;
    if(LEXER_PEEK()!='#') return ret;
    LEXER_NEXT();
    char *word=WordAtPoint(EXPAND_MACROS);
    if(!word) {
        LexerRaiseError("Unexpected '#'.");
        goto end;
    } else if(0==strcmp(word,"ifdef")) {
        ret=1;
        SkipWhitespace(0);
        char *word=WordAtPoint(0);
        LexerSearch("\n");
        SpliceToCurrentFromBehind(orig_pos);
        if(!word) {
            LexerRaiseError("Expected macro name.");
            goto end;
        }
        HandleLexerIf(NULL!=map_get(&Lexer.macros, word));
    } else if(0==strcmp(word,"ifndef")) {
        ret=1;
        SkipWhitespace(0);
        char *word=WordAtPoint(0);
        LexerSearch("\n");
        SpliceToCurrentFromBehind(orig_pos);
        if(!word) {
            LexerRaiseError("Expected macro name.");
            goto end;
        }
        HandleLexerIf(NULL==map_get(&Lexer.macros, word));
    } else if(0==strcmp(word,"ifjit")) {
        ret=1;
        SkipWhitespace(0);
        LexerSearch("\n");
        SpliceToCurrentFromBehind(orig_pos);
        HandleLexerIf(1);
    } else if(0==strcmp(word,"ifaot")) {
        ret=1;
        SkipWhitespace(0);
        LexerSearch("\n");
        SpliceToCurrentFromBehind(orig_pos);
        HandleLexerIf(0);
    } else if(0==strcmp(word,"if")) {
        AST *dummy AF_AST=TD_MALLOC(sizeof(AST));
        dummy->refCnt=1;
        SetPosFromLex(dummy);
        ret=1;
        SkipWhitespace(0);
        HandleLexerIf(EvalConditionForPreproc());
    } else if(0==strcmp(word,"else")) {
        ret=1;
        if(!Lexer.ifStates.length) {
            LexerRaiseError("Unexpected #else.");
        } else if(vec_last(&Lexer.ifStates).hasElse) {
            LexerRaiseError("Unexpected #else(#else already provided).");
        } else {
            vec_last(&Lexer.ifStates).success^=1;
        }
        SpliceToCurrentFromBehind(orig_pos);
    } else if(0==strcmp(word,"endif")) {
        ret=1;
        if(!Lexer.ifStates.length) {
            LexerRaiseError("Unexpected #endif.");
        } else {
            vec_pop(&Lexer.ifStates);
        }
        SpliceToCurrentFromBehind(orig_pos);
    } else if(0==strcmp(word,"assert")) {
        AST *dummy AF_AST=TD_MALLOC(sizeof(AST));
        dummy->refCnt=1;
        SetPosFromLex(dummy);
        ret=1;
        SkipWhitespace(0);
        long asststart=Lexer.cursor_pos;
        int cond=EvalConditionForPreproc();
        char buffer[Lexer.cursor_pos-asststart+1];
        long cnt=Lexer.cursor_pos-asststart;
        buffer[cnt]=0;
        while(--cnt>=0)
            buffer[cnt]=mrope_index(Lexer.source, cnt+asststart);
        SpliceToCurrentFromBehind(orig_pos);
        if(!cond) RaiseWarning(dummy,"Assertion \"%s\" failed.",buffer);
    } else if(0==strcmp(word,"define")) {
        ret=1;
        SkipWhitespace(0);
        char *mname=WordAtPoint(0);
        if(!mname) {
            LexerRaiseError("#define requires a macro name.");
            goto define_splice;
        }
        long macro_start=Lexer.cursor_pos;
        LexerSearch("\n");
        long len=Lexer.cursor_pos-macro_start;
        char *mtext=TD_CALLOC(1,len+1);
        long c=0;
        for(; c!=len; c++)
            mtext[c]=mrope_index(Lexer.source,macro_start+c);
        mtext[len]=0;
        CMacro macro;
        macro.expand=mtext;
        macro.name=mname;
        int ln;
        FPosToLineCol(Lexer.filename, Lexer.fpos, &ln, NULL);
        macro.line=ln;
        fnloop:;
        char **txt;
        if(txt=map_get(&Lexer.filenames,Lexer.filename)) {
            macro.fn=*txt;
        } else {
            map_set(&Lexer.filenames,Lexer.filename,strdup(Lexer.filename));
            goto fnloop;
        }

        CMacro *existing;
        if(existing=map_get(&Lexer.macros, mname)) {
            FreeMacro(existing);
            map_remove(&Lexer.macros, mname);
        }
        map_set(&Lexer.macros, mname, macro);
        //
define_splice:
        if(!Lexer.preprocessMode)
          SpliceToCurrentFromBehind(orig_pos);
    } else if(0==strcmp(word,"exe")) {
        ret=1;
        //This will cuase a HC_EXE token to be sent
        Lexer.isExeMode=1;
        mrope_t *old_rope=Lexer.currentExeStream;
        Lexer.currentExeStream=TD_MALLOC(sizeof(mrope_t));
        mrope_init(Lexer.currentExeStream);
        HC_parse();
        SpliceToCurrentFromBehind(orig_pos);
        char *text=rope2str(Lexer.currentExeStream);
        __InsertSrcText(Lexer.cursor_pos, text);
        mrope_free(Lexer.currentExeStream);
        Lexer.currentExeStream=old_rope;
        TD_FREE(text);
    } else if(0==strcmp(word,"include")) {
        ret=1;
        SkipWhitespace(0);
        AST *str=LexString();
        if(!str) {
expfn:
            LexerRaiseError("#include requires a filename.");
            SpliceToCurrentFromBehind(orig_pos);
            goto end;
        } else if(str->type==AST_CHAR) {
            ReleaseAST(str);
            goto expfn;
        }
        SpliceToCurrentFromBehind(orig_pos);
        if(Lexer.ifStates.length) {
            CPreprocIfState st;
            int iter;
            vec_foreach(&Lexer.ifStates, st, iter)
            if(!st.success) {
                return ret;
            }
        }
        //This saves the current directory so run it before we change directorires
        long origcpos=Lexer.cursor_pos;
        CLexerFileState backup=CreateLexerFileState();
        vec_push(&Lexer.fileStates,backup);
        char *clone1=strdup(str->string);
        char *clone2=strdup(str->string);
        char *fn=basename(clone1);
        char *dn=dirname(clone2);
        #ifndef TARGET_WIN32
            char *rp=realpath(str->string,NULL);
            if(rp) strcpy(Lexer.filename, rp);
            free(rp);
        #else
            GetFullPathNameA(str->string,sizeof(Lexer.filename),Lexer.filename,NULL);
        #endif
        chdir(dn);
        struct stat s;
        stat(fn, &s);
        FILE *f=fopen(fn,"rb");
        if(!f||!(s.st_mode&S_IFREG)) { //Regular file
            char buffer[1024];
            sprintf(buffer,"Can't open file %s/%s.",dn,fn);
            RestoreLexerFileState(vec_pop(&Lexer.fileStates));
            LexerRaiseError(buffer);
            Lexer.cursor_pos=origcpos;
        } else if(IsBinFile(fn)) {
            loadBinary:;
            CBinaryModule mod;
            map_set(&Compiler.binModules,fn,mod=LoadAOTBin(f,0));
            //Make sure we load the main function's code in AOT mode by appending named @@Main call to the global statements
            if(Compiler.AOTMode) {
                static long count;
                CVariable *main=*map_get(&mod.vars,"@@Main");
                char buffer[1024];
                sprintf(buffer,"__SECRET_%ldMain",count++);
                TD_FREE(main->name);
                TD_FREE(main->func->name);
                main->func->name=strdup(buffer);
                main->name=strdup(buffer);
                map_set(&Compiler.globals,buffer,main);
                vec_push(&Compiler.AOTGlobalStmts,CreateFuncCall(CreateName(buffer),NULL));
            }
            RestoreLexerFileState(vec_pop(&Lexer.fileStates));
        } else if(IsZBinFile(fn)) {
            fclose(f);
            f=ExpandFile(fn);
            goto loadBinary;
        } else if(IsZSourceFile(fn)) {
            fclose(f);
            f=ExpandFile(fn);
            goto loadSource;
        } else {
            loadSource:;
            //Unload the old line starts
            vec_int_t *lns=map_get(&Lexer.fileLineStarts,Lexer.filename);
            if(lns) {
                vec_deinit(lns);
                map_remove(&Lexer.fileLineStarts,Lexer.filename);
            }
            fseek(f,0,SEEK_END);
            long size=ftell(f);
            fseek(f,0,SEEK_SET);
            size-=ftell(f);
            //
            Lexer.curFileEnd=size+1; //Make room for extra newline
            Lexer.fpos=0;
            //
            char *ftext=TD_CALLOC(1,size+1+1); //Make room for extra n
            fread(ftext, 1, size, f);
	    ftext[size]='\n';
            fclose(f);
            Lexer.cursor_pos=orig_pos;
            //Make room for exp
            __InsertSrcText(orig_pos, ftext);
            Lexer.physical_fpos_start=orig_pos;
            TD_FREE(ftext);
        }
        ReleaseAST(str);
        TD_FREE(clone1),TD_FREE(clone2);
    }
end:
    TD_FREE((word));
    return ret;
}
void FreeMacro(CMacro *macro) {
    TD_FREE((macro->expand));
    TD_FREE((macro->name));
}
void CreateLexer(int whichparser) {
    const char *regs[]={
        "AL",
        "CL",
        "DL",
        "BL",
        "AH",
        "CH",
        "DH",
        "BH",
        "R8u8",
        "R9u8",
        "R10u8",
        "R11u8",
        "R12u8",
        "R13u8",
        "R14u8",
        "R15u8",
        "RSPu8",
        "RBPu8",
        "RSIu8",
        "RDIu8",
        "AX",
        "CX",
        "DX",
        "BX",
        "SP",
        "BP",
        "SI",
        "DI",
        "R8u16",
        "R9u16",
        "R10u16",
        "R11u16",
        "R12u16",
        "R13u16",
        "R14u16",
        "R15u16",
        "EAX",
        "ECX",
        "EDX",
        "EBX",
        "ESP",
        "EBP",
        "ESI",
        "EDI",
        "R8u32",
        "R9u32",
        "R10u32",
        "R11u32",
        "R12u32",
        "R13u32",
        "R14u32",
        "R15u32",
        "EIP",
        "RAX",
        "RCX",
        "RDX",
        "RBX",
        "RSP",
        "RBP",
        "RSI",
        "RDI",
        "R8",
        "R9",
        "R10",
        "R11",
        "R12",
        "R13",
        "R14",
        "R15",
        "R8u64",
        "R9u64",
        "R10u64",
        "R11u64",
        "R12u64",
        "R13u64",
        "R14u64",
        "R15u64",
        "RIP",
        "ES",
        "CS",
        "SS",
        "DS",
        "FS",
        "GS",
        "ST0",
        "ST1",
        "ST2",
        "ST3",
        "ST4",
        "ST5",
        "ST6",
        "ST7",
        "MM0",
        "MM1",
        "MM2",
        "MM3",
        "MM4",
        "MM5",
        "MM6",
        "MM7",
        "XMM0",
        "XMM1",
        "XMM2",
        "XMM3",
        "XMM4",
        "XMM5",
        "XMM6",
        "XMM7",
    };
    const char *opcodes[]={
        "PUSH",
        "PUSHAW",
        "PUSHAD",
        "PUSHFW",
        "PUSHFD",
        "POP",
        "POPAW",
        "POPAD",
        "POPFW",
        "POPFD",
        "MOV",
        "ADC",
        "ADD",
        "AND",
        "CMP",
        "OR",
        "SBB",
        "SUB",
        "TEST",
        "NOP",
        "NOP2",
        "XCHG",
        "XOR",
        "CMOVO",
        "CMOVNO",
        "CMOVB",
        "CMOVAE",
        "CMOVE",
        "CMOVNE",
        "CMOVBE",
        "CMOVA",
        "CMOVS",
        "CMOVNS",
        "CMOVP",
        "CMOVNP",
        "CMOVL",
        "CMOVGE",
        "CMOVLE",
        "CMOVG",
        "CALL",
        "JMP",
        "JO",
        "JNO",
        "JB",
        "JAE",
        "JE",
        "JNE",
        "JBE",
        "JA",
        "JS",
        "JNS",
        "JP",
        "JNP",
        "JL",
        "JGE",
        "JLE",
        "JG",
        "JCXZ",
        "INC",
        "DEC",
        "NOT",
        "NEG",
        "MUL",
        "IMUL",
        "IMUL2",
        "DIV",
        "IDIV",
        "AAA",
        "AAD",
        "AAM",
        "AAS",
        "ARPL",
        "BOUND",
        "BSF",
        "BSR",
        "BSWAP",
        "BT",
        "BTC",
        "BTR",
        "BTS",
        "CBW",
        "CWDE",
        "CDQE",
        "CWD",
        "CDQ",
        "CQO",
        "CLC",
        "CLD",
        "CLI",
        "CLTS",
        "CMC",
        "CMPSB",
        "CMPSW",
        "CMPSD",
        "CMPSQ",
        "CMPXCHG",
        "CHPXCHG8B",
        "DAA",
        "DAS",
        "ENTER",
        "HLT",
        "IN",
        "INS",
        "INSB",
        "INSW",
        "INSD",
        "INTO",
        "INT3",
        "INT",
        "INVD",
        "IRET",
        "LAHF",
        "LAR",
        "LEA",
        "LEAVE",
        "LGDT",
        "SGDT",
        "LIDT",
        "SIDT",
        "LLDT",
        "SLDT",
        "LMSW",
        "SMSW",
        "LOCK",
        "LODSB",
        "LODSW",
        "LODSD",
        "LODSQ",
        "LOOP",
        "LOOPE",
        "LOOPNE",
        "LSL",
        "LTR",
        "MOVSB",
        "MOVSW",
        "MOVSD",
        "MOVSQ",
        "MOVSX",
        "MOVSXD",
        "MOVZX",
        "OUT",
        "OUTSB",
        "OUTSW",
        "OUTSD",
        "REP_INSB",
        "REP_INSW",
        "REP_INSD",
        "REP_MOVSB",
        "REP_MOVSW",
        "REP_MOVSD",
        "REP_MOVSQ",
        "REP_OUTSB",
        "REP_OUTSW",
        "REP_OUTSD",
        "REP_LODSB",
        "REP_LODSW",
        "REP_LODSD",
        "REP_LODSQ",
        "REP_STOSB",
        "REP_STOSW",
        "REP_STOSD",
        "REP_STOSQ",
        "REPE_CMPSB",
        "REPE_CMPSW",
        "REPE_CMPSD",
        "REPE_CMPSQ",
        "REPE_SCASB",
        "REPE_SCASW",
        "REPE_SCASD",
        "REPE_SCASQ",
        "REPNE_CMPSB",
        "REPNE_CMPSW",
        "REPNE_CMPSD",
        "REPNE_CMPSQ",
        "REPNE_SCASB",
        "REPNE_SCASW",
        "REPNE_SCASD",
        "REPNE_SCASQ",
        "RET",
        "RET1",
        "RETF",
        "RETF1",
        "REX",
        "REX2",
        "RSM",
        "SAHF",
        "SCASB",
        "SCASW",
        "SCASD",
        "SCASQ",
        "SEGCS",
        "SEGSS",
        "SEGDS",
        "SEGES",
        "SEGFS",
        "SEGGS",
        "SETO",
        "SETNO",
        "SETB",
        "SETAE",
        "SETE",
        "SETNE",
        "SETBE",
        "SETA",
        "SETS",
        "SETNS",
        "SETP",
        "SETNP",
        "SETL",
        "SETGE",
        "SETLE",
        "SETG",
        "SHLD",
        "SHRD",
        "STC",
        "STD",
        "STI",
        "STOSB",
        "STOSW",
        "STOSD",
        "STOSQ",
        "STR",
        "VERR",
        "VERW",
        "WAIT",
        "FWAIT",
        "XADD",
        "XLATB",
        "ROL",
        "ROL1",
        "ROR",
        "ROR1",
        "RCL",
        "RCL1",
        "RCR",
        "RCR1",
        "SHL",
        "SHL1",
        "SHR",
        "SHR1",
        "SAR",
        "SAR1",
        "FILD",
        "FISTP",
        "FISTTP",
        "FLD",
        "FSTP",
        "FST",
        "FRSTOR",
        "FSAVE",
        "FYL2X",
        "FYL2XP1",
        "F2XM1",
        "FABS",
        "FCHS",
        "FSIN",
        "FCOS",
        "FPTAN",
        "FPATAN",
        "FSQRT",
        "FMULP",
        "FMUL",
        "FIMUL",
        "FDIVP",
        "FDIV",
        "FDIVRP",
        "FDIVR",
        "FPREM",
        "FADDP",
        "FADD",
        "FSUBP",
        "FSUB",
        "FSUBRP",
        "FSUBR",
        "FCOMIP",
        "FCOMI",
        "FCLEX",
        "FNCLEX",
        "FSTSW",
        "FDECSTP",
        "FINCSTP",
        "FFREE",
        "FRNDINT",
        "FSCALE",
        "FXTRACT",
        "FLD1",
        "FLDL2T",
        "FLDL2E",
        "FLDPI",
        "FLDLG2",
        "FLDLN2",
        "FLDZ",
        "FXCH",
        "FTST",
        "FXAM",
        "FINIT",
        "FNINIT",
        "FSTCW",
        "FLDCW",
        "FXSAVE",
        "FXRSTOR",
        "WBINVD",
        "CLFLUSH",
        "INVLPG",
        "CPUID",
        "WRMSR",
        "RDTSC",
        "RDMSR",
        "PAUSE",
        "MOV_CR0_EAX",
        "MOV_EAX_CR0",
        "MOV_CR2_EAX",
        "MOV_EAX_CR2",
        "MOV_CR3_EAX",
        "MOV_EAX_CR3",
        "MOV_CR4_EAX",
        "MOV_EAX_CR4",
        "MOV_CR0_RAX",
        "MOV_RAX_CR0",
        "MOV_CR2_RAX",
        "MOV_RAX_CR2",
        "MOV_CR3_RAX",
        "MOV_RAX_CR3",
        "MOV_CR4_RAX",
        "MOV_RAX_CR4",
    };
    struct {
        char *name;
        int kw;
    } kws[]= {
        {"union",HC_UNION},
        {"catch",HC_CATCH},
        {"class",HC_CLASS},
        {"try",HC_TRY},
        {"if",HC_IF},
        {"else",HC_ELSE},
        {"for",HC_FOR},
        {"while",HC_WHILE},
        {"extern",HC_EXTERN},
        {"_extern",HC_EXTERN2},
        {"return",HC_RET},
        {"sizeof",HC_SIZEOF},
        {"intern",HC_INTERN},
        {"do",HC_DO},
        {"goto",HC_GOTO},
        {"break",HC_BREAK},
        {"switch",HC_SWITCH},
        {"start",HC_START},
        {"end",HC_END},
        {"case",HC_CASE},
        {"default",HC_DEFAULT},
        {"public",HC_PUBLIC},
        {"import",HC_IMPORT},
        {"_import",HC_IMPORT2},
        {"lastclass",HC_LASTCLASS},
        {"static",HC_STATIC},
        {"DU8",HC_DU8},
        {"DU16",HC_DU16},
        {"DU32",HC_DU32},
        {"DU64",HC_DU64},
        {"ALIGN",HC_ALIGN},
        {"BINFILE",HC_BINFILE},
        {"asm",HC_ASM},
	{"IMPORT",HC_ASM_IMPORT},
    };
    memset(&Lexer,0,sizeof(Lexer));
    long opc_cnt=sizeof(opcodes)/sizeof(*opcodes);
    map_init(&Lexer.__opcodes);
    while(--opc_cnt>=0) {
        map_set(&Lexer.__opcodes,opcodes[opc_cnt],NULL);
    }
    map_init(&Lexer.__registers);
    long reg_cnt=sizeof(regs)/sizeof(*regs);
    while(--reg_cnt>=0) {
        map_set(&Lexer.__registers,regs[reg_cnt],NULL);
    }
    Lexer.source=TD_MALLOC(sizeof(mrope_t));
    mrope_init(Lexer.source);
    Lexer.whichParser=whichparser;
    map_init(&Lexer.filenames);
    map_init(&Lexer.fileLineStarts);
    //
    map_init(&Lexer.keywords);
    long cnt=sizeof(kws)/sizeof(*kws);
    while(--cnt>=0) {
        switch(whichparser) {
        case PARSER_HOLYC:
            ;
            map_set(&Lexer.keywords, kws[cnt].name,kws[cnt].kw);
            break;
        }
    }
    //
    vec_init(&Lexer.expandChain);
    map_init(&Lexer.macros);
    Lexer.curFileEnd=-1;
    Lexer.fpos=0;
    Lexer.cursor_pos=0;
    strcpy(Lexer.filename,REPL_SOURCE_NAME);
    vec_init(&Lexer.fileStates);
    Lexer.macroExpandDepth=0;
    Lexer.replSource=tmpfile();
    Lexer.isFreeToFlush=1;
}
void DestroyLexer() {
    mrope_free(Lexer.source);
    map_deinit(&Lexer.__registers);
    map_deinit(&Lexer.__opcodes);
    map_deinit(&Lexer.keywords);
    char *s;
    long iter;
    vec_foreach(&Lexer.expandChain,s,iter) {
        TD_FREE(s);
    }
    vec_deinit(&Lexer.expandChain);
    CLexerFileState f;
    vec_foreach(&Lexer.fileStates,f,iter) {
        TD_FREE(f.olddir);
        TD_FREE(f.oldfile);
    }
    vec_deinit(&Lexer.fileStates);
    map_iter_t miter=map_iter(&Lexer.filenames);
    const char *key;
    while(key=map_next(&Lexer.filenames,&miter)) {
        TD_FREE(*map_get(&Lexer.filenames,key));
    }
    map_deinit(&Lexer.filenames);
    miter=map_iter(&Lexer.macros);
    while(key=map_next(&Lexer.macros,&miter)) {
        FreeMacro(map_get(&Lexer.macros,key));
    }
    map_deinit(&Lexer.macros);
}
void JoinWithOldLexer(CLexer old_lex) {
    CLexer toDestroy=Lexer;
    const char *key;
    map_iter_t miter=map_iter(&toDestroy.macros);
    while(key=map_next(&toDestroy.macros,&miter)) {
        CMacro new,*old=map_get(&toDestroy.macros,key);
        new.expand=strdup(old->expand);
        if(old->fn) new.fn=strdup(old->fn);
        new.name=strdup(old->name);
        new.line=old->line;
        map_set(&old_lex.macros,key,new);
    }
    Lexer=toDestroy;
    DestroyLexer();
    Lexer=old_lex;
}
AST *LexMatchF64() {
    int isf64=0;
    int digitsfound;
#define CHECK_DIGITS \
  digitsfound=0;\
  while(isdigit(LEXER_PEEK())) \
    digitsfound++,LEXER_NEXT(); \
  //1.2E-10
    long origcpos=Lexer.cursor_pos;
    if(isdigit(LEXER_PEEK())) {
        CHECK_DIGITS;
        if(LEXER_PEEK()=='.') {
            isf64=1;
            LEXER_NEXT();
            CHECK_DIGITS;
        }
checkE:
        if(toupper(LEXER_PEEK())=='E') {
            isf64=1;
            LEXER_NEXT();
            switch(LEXER_PEEK()) {
            case '+':
            case '-':
                LEXER_NEXT();
                break;
            default:
                if(!isdigit(LEXER_PEEK())) goto malformed;
                break;
            }
            CHECK_DIGITS;
            if(!digitsfound) {
malformed:
                LexerRaiseError("Malformed floating-point.");
            }

        }
        long len=Lexer.cursor_pos-origcpos,c=0;
        LEXER_REWIND(len);
        char buffer[len+1];
        for(c=0; c!=len; c++) {
            buffer[c]=LEXER_PEEK();
            LEXER_NEXT();
        }
        double d;
        sscanf(buffer,"%lf", &d);
        if(isf64)
            return CreateF64(d);
    } else if(LEXER_PEEK()=='.') {
        LEXER_NEXT();
        CHECK_DIGITS;
        if(digitsfound) {
            isf64=1;
            goto checkE;
        }
    }
    Lexer.cursor_pos=origcpos;
    return NULL;;
}
AST *LexMatchBase10() {
    if(isdigit(LEXER_PEEK())&&LEXER_PEEK()!='0') {
        int malformed=0;
        vec_char_t num;
        vec_init(&num);
        while(isalnum(LEXER_PEEK())) {
            if(!isdigit(LEXER_PEEK())) malformed=1;
            vec_push(&num,LEXER_PEEK());
            LEXER_NEXT();
        }
        uint64_t i=0;
        int iter;
        char d;
        vec_foreach(&num, d, iter) {
            i*=10;
            i+=d-'0';
        }
        if(malformed) LexerRaiseError("Malformed integer.");
        return CreateI64(i);
    }
    return NULL;
}
static int isoctal(int c) {
    switch(c) {
    case '0'...'7':
        return 1;
    }
    return 0;
}
AST *LexMatchBase8() {
    if(LEXER_PEEK()=='0') {
        LEXER_NEXT();
        int malformed=0;
        vec_char_t num;
        vec_init(&num);
        while(isdigit(LEXER_PEEK())) {
            if(!isoctal(LEXER_PEEK()))
                malformed=1;
            vec_push(&num,LEXER_PEEK());
            LEXER_NEXT();
        }
        uint64_t i=0;
        int iter;
        char d;
        vec_foreach(&num, d, iter) {
            i*=8;
            i+=d-'0';
        }
        if(malformed) LexerRaiseError("Malformed octal.");
        return CreateI64(i);
    }
    return NULL;
}
AST *LexMatchBase2() {
    if(LEXER_PEEK()=='0') {
        LEXER_NEXT();
        if(LEXER_PEEK()!='b') return NULL;
        LEXER_NEXT();
        int malformed=0;
        vec_char_t num;
        vec_init(&num);
        while(isdigit(LEXER_PEEK())) {
            if(!strchr("01",LEXER_PEEK()))
                malformed=1;
            vec_push(&num,LEXER_PEEK());
            LEXER_NEXT();
        }
        uint64_t i=0;
        int iter;
        char d;
        vec_foreach(&num, d, iter) {
            i*=2;
            i+=d-'0';
        }
        if(malformed) LexerRaiseError("Malformed octal.");
        return CreateI64(i);
    }
    return NULL;
}
AST *LexMatchBase16() {
    long origcpos=Lexer.cursor_pos;
    if(LEXER_PEEK()=='0') {
        LEXER_NEXT();
        if(toupper(LEXER_PEEK())=='X') {
            LEXER_NEXT();
            int malformed=0;
            vec_char_t num;
            vec_init(&num);
            while(isalnum(LEXER_PEEK())) {
                if(!isxdigit(LEXER_PEEK()))
                    malformed=1;
                vec_push(&num,LEXER_PEEK());
                LEXER_NEXT();
            }
            uint64_t i=0;
            long iter;
            char d;
            vec_foreach(&num, d, iter) {
                i*=16;
                switch(toupper(d)) {
                case 'A'...'F':
                    i+=10+toupper(d)-'A';
                    break;
                default:
                    i+=d-'0';
                }
            }
            if(malformed) LexerRaiseError("Malformed hexidecimal.");
            return CreateI64(i);
        }
    }
    Lexer.cursor_pos=origcpos;
    return NULL;
}
AST *LexName() {
    char *word=WordAtPoint(EXPAND_MACROS);
    if(!word) return NULL;
    if(GetRegister(word)||map_get(&Lexer.__registers,word)) {
      AST *r=TD_CALLOC(1,sizeof(AST));
      r->refCnt=1;
      r->type=AST_ASM_REG;
      r->asmReg=GetRegister(word);
      return r;
    }
    if(map_get(&Lexer.keywords, word)) {
        AST *r=TD_CALLOC(1,sizeof(AST));
        r->refCnt=1;
        if(0==strcmp("lastclass",word)) {
            r->type=AST_LASTCLASS;
            TD_FREE(word);
            return r;
        }
        r->type=AST_KEYWORD;
        r->keyword=*map_get(&Lexer.keywords, word);
        TD_FREE(word);
        return r;
    }

    AST *r=TD_CALLOC(1,sizeof(AST));
    r->refCnt=1;
    r->type=AST_NAME;
    r->name=word;
    return r;
}
AST *LexToken() {
    struct tpair {
        char *text;
        int token;
        int dbgToken;
    } tpairs[]= {
        {".",HC_DOT},
        {"->",HC_ARROW,},
        {"!",HC_LNOT,},
        {"~",HC_BNOT,},
        {"`",HC_POW,},
        {"<<",HC_SHL,},
        {">>",HC_SHR,},
        {"*",HC_MUL},
        {"/",HC_DIV},
        {"%",HC_MOD},
        {"&",HC_BAND},
        {"^",HC_BXOR},
        {"|",HC_BOR},
        {"+",HC_ADD},
        {"-",HC_SUB},
        {"<",HC_LT},
        {">",HC_GT},
        {">=",HC_GE},
        {"<=",HC_LE},
        {"==",HC_EQ},
        {"!=",HC_NE},
        {"&&",HC_LAND},
        {"||",HC_LOR},
        {"^^",HC_LXOR},
        {"=",HC_ASSIGN},
        {"<<=",HC_EQ_SHL},
        {">>=",HC_EQ_SHR},
        {"*=",HC_EQ_MUL},
        {"/=",HC_EQ_DIV},
        {"&=",HC_EQ_BAND},
        {"^=",HC_EQ_BXOR},
        {"|=",HC_EQ_BOR},
        {"+=",HC_EQ_ADD},
        {"-=",HC_EQ_SUB},
        {",",HC_COMMA},
        {"(",HC_LEFT_PAREN},
        {")",HC_RIGHT_PAREN},
        {"++",HC_INC},
        {"--",HC_DEC},
        {"[",HC_LEFT_SQAURE},
        {"]",HC_RIGHT_SQAURE},
        {";",HC_SEMI},
        {"{",HC_LEFT_CURLY},
        {"}",HC_RIGHT_CURLY},
        {":",HC_COLON},
        {"...",HC_DOT_DOT_DOT},
        {"::",HC_DOUBLE_COLON},
        {"@@",HC_DOUBLE_AT},
    };
    int cnt=sizeof(tpairs)/sizeof(*tpairs),ret=-1;
    long origpos=Lexer.cursor_pos;
    while(--cnt>=0) {
        int l=strlen(tpairs[cnt].text);
        int i=-1;
        Lexer.cursor_pos=origpos;
        while(++i!=l) {
            if(LEXER_PEEK()!=tpairs[cnt].text[i]) goto fail;
            LEXER_NEXT();
        }
        if(ret==-1) {
            ret=cnt;
        } else {
            if(l>strlen(tpairs[ret].text)) ret=cnt;
        }
fail:
        ;
    }
end:
    if(ret!=-1) {
        Lexer.cursor_pos=origpos+strlen(tpairs[ret].text);
        AST *r=TD_CALLOC(1,sizeof(AST));
        r->type=AST_TOKEN;
        if(Lexer.whichParser==PARSER_HOLYC)
            r->tokenAtom=tpairs[ret].token;
        else if(Lexer.whichParser==PARSER_DEBUG_EXPR)
            r->tokenAtom=tpairs[ret].dbgToken;
        r->refCnt=1;
        return r;
    }
    return NULL;
}
//See HolyC.h
int ASTToToken(AST *t) {
    if(Lexer.whichParser==PARSER_HOLYC) {
        if(t->type==AST_LASTCLASS) return HC_LASTCLASS;
        if(t->type==AST_CHAR) return HC_CHAR;
        if(t->type==AST_STRING) return HC_STRING;
        if(t->type==AST_INT) return HC_INT;
        if(t->type==AST_FLOAT) return HC_FLOAT;
        if(t->type==AST_NAME) {
            if(map_get(&Compiler.types, t->name)) return HC_TYPENAME;
            if(Compiler.tagsFile||Compiler.AOTMode) {
                /**
                 * The assembler is written in HolyC and IsOpcode calls HolyC code,(code is not generated in tags mode)
                 */
                if(map_get(&Lexer.__opcodes,t->name))
                    return HC_OPCODE;
            }
            if(IsOpcode(t->name)) return HC_OPCODE;
            return HC_NAME;
        }
        if(t->type==AST_ASM_REG) return HC_REGISTER;
    }
    if(t->type==AST_KEYWORD) return t->keyword;
    if(t->type==AST_TOKEN) return t->tokenAtom;
    return -1;
}
AST *LexItem() {
  if(Lexer.isEvalNoCommaMode) {
      Lexer.isEvalNoCommaMode=0;
      AST *r=TD_CALLOC(1,sizeof(AST));
      r->type=AST_TOKEN;
      r->refCnt=1;
      r->tokenAtom=HC_EVAL_NOCOMMA;
      return r;
  } else if(Lexer.isDebugExpr) {
      Lexer.isDebugExpr=0;
      AST *r=TD_CALLOC(1,sizeof(AST));
      r->type=AST_TOKEN;
      r->refCnt=1;
      r->tokenAtom=HC_DBG;
      return r;
  } else if(Lexer.isExeMode) {
        Lexer.isExeMode=0;
        AST *r=TD_CALLOC(1,sizeof(AST));
        r->type=AST_TOKEN;
        r->refCnt=1;
        r->tokenAtom=HC_EXE;
        return r;
    } else if(Lexer.isEvalMode) {
        Lexer.isEvalMode=0;
        AST *r=TD_CALLOC(1,sizeof(AST));
        r->type=AST_TOKEN;
        r->refCnt=1;
        r->tokenAtom=HC_EVAL;
        return r;
    }
loop:
    ;
    if(Lexer.stopAtNewline&&LEXER_PEEK()=='\n') {
        AST *r=TD_CALLOC(1,sizeof(AST));
        r->type=AST_TOKEN;
        r->refCnt=2;
        r->tokenAtom=HC_NL;
        return Lexer.lastToken=r;
    }
    long origcpos=Lexer.cursor_pos;
    AST *ret;
    SkipWhitespace(0);
    if(LexMacro()) goto loop;
    SkipWhitespace(0);
    if(Lexer.cursor_pos!=origcpos) goto loop;
    if(LexExpandWord()) goto loop; //Moves past expasnsion
    Lexer.cursor_pos=origcpos;

    AST dummypos;
    SetPosFromLex(&dummypos);
#define try_item(func) {if(ret=func()) {SetPosFromLex(ret);goto found;} else {Lexer.cursor_pos=origcpos;}}
    try_item(LexMatchF64);
    try_item(LexMatchBase16);
    try_item(LexMatchBase2);
    try_item(LexMatchBase8);
    try_item(LexMatchBase10);
    try_item(LexString);
    try_item(LexName);
    try_item(LexToken);
    SkipWhitespace(0);
    if(LEXER_PEEK()) {
        LexerRaiseError("Unable to lex item!");
        FlushLexer();
    }
    return NULL;
found:
    //If we are in an inactive if statement,move onto the next lexer item
    if(Lexer.ifStates.length) {
        CPreprocIfState st;
        int iter;
        vec_foreach(&Lexer.ifStates, st, iter)
        if(!st.success) {
            ReleaseAST(ret);
            goto loop;
        }
    }
    Lexer.lastToken=ret; //TODO replace with dummy
    if(Lexer.cb)
        Lexer.cb(ASTToToken(ret),SetPosFromOther(ret,&dummypos));

    return ret;
}
AST *SetPosFromLex(AST *t) {
    FPosToLineCol(Lexer.filename, Lexer.fpos, &t->ln,&t->col);
    char** txt;
loop:
    if(txt=map_get(&Lexer.filenames,Lexer.filename)) {
        t->fn=*txt;
    } else {
        map_set(&Lexer.filenames,Lexer.filename,strdup(Lexer.filename));
        goto loop;
    }
    return t;
}
AST *SetPosFromOther(AST *dst,AST *src) {
    dst->ln=src->ln;
    dst->col=src->col;
    dst->fn=src->fn;
    return dst;
}
CLexer ForkLexer(int whichparser) {
    CLexer old=Lexer;
    CreateLexer(whichparser);
    //Import old macros
    map_iter_t liter=map_iter(&Lexer.macros);
    const char *key;
    while(key=map_next(&old.macros,&liter)) {
        CMacro macro=*map_get(&old.macros,key);
        macro.expand=strdup(macro.expand);
        macro.name=strdup(macro.name);
        map_set(&Lexer.macros,macro.name,macro);
    }
    return old;
}
char *PreprocessFile(char *to_include) {
  CLexer old=Lexer;
  //Include a fresh lexer
  CreateLexer(PARSER_HOLYC);
  Lexer.preprocessMode=1;
  char buffer[1024];
  FILE *f2=fopen(to_include, "rb");
  fseek(f2, 0, SEEK_END);
  long end=ftell(f2);
  fseek(f2, 0, SEEK_SET);
  long start=ftell(f2);
  char *buf=TD_MALLOC(end-start+1);
  fread(buf, 1, end-start, f2);
  fclose(f2);
  __InsertSrcText(0, buf);
  TD_FREE(buf);
  Lexer.replMode=Lexer.isFreeToFlush=0;
  while(LEXER_PEEK()) {
      ReleaseAST(LexItem());
  }
  char *r=rope2str(Lexer.source);
  DestroyLexer();
  Lexer=old;
  return r;
}
