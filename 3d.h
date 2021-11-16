#pragma once
#include "alloc.h"
#include <stdint.h>
#include <assert.h>
#include <stdio.h>
#include <signal.h>
struct AST;
#ifndef HC_STYPE
#define HC_STYPE struct AST *
#endif
#ifndef D_STYPE
#define D_STYPE struct AST *
#endif
#include "HolyC.tab.h"
#include "ext/map/src/map.h"
#include "ext/vec/src/vec.h"
#include "ext/C_Unescaper/escaper.h"
#include "ext/myjit/myjit/jitlib.h"
#include "ext/mropes/include/mropes/rope.h"
#include "rl.h"
extern char CompilerPath[1024];
#ifdef TARGET_WIN32
#include "winjmp.h"
#define BLOCK_SIGS \
void *oldhand=signal(SIGINT,SIG_IGN);
#define UNBLOCK_SIGS \
signal(SIGINT,oldhand);
#else
#include "linjmp.h"
#define BLOCK_SIGS \
sigset_t oldset; \
sigset_t newset; \
sigfillset(&newset); \
sigdelset(&newset,SIGSEGV); \
sigprocmask(SIG_BLOCK,&newset,&oldset);
#define UNBLOCK_SIGS \
sigprocmask(SIG_SETMASK,&oldset,NULL);
#endif
#ifndef TARGET_WIN32
#define ARM_SIGNALS \
signal(SIGSEGV,SignalHandler); \
signal(SIGABRT,SignalHandler); \
signal(SIGBUS,SignalHandler); \
signal(SIGFPE,SignalHandler); \
signal(SIGILL,SignalHandler); \
signal(SIGINT,SignalHandler);
#else
#define ARM_SIGNALS \
signal(SIGSEGV,SignalHandler); \
signal(SIGABRT,SignalHandler);

#endif
typedef vec_t(void***) vec_voidppp_t;
typedef map_t(vec_voidppp_t) map_vec_voidppp_t;
struct CType;
extern ExceptBuf SigPad;
typedef vec_t(struct AST *) vec_AST_t;
typedef vec_t(struct CType*) vec_CType_t;
typedef struct {
    struct AST *name;
    struct AST *value;
} CMetaData;
typedef vec_t(CMetaData) vec_CMetaData_t;
typedef struct {
    int offset;
    char *name;
    struct CType *type;
    vec_CMetaData_t meta;
    char *fn;
    int line;
} CMember;
typedef vec_t(CMember) vec_CMember_t;
struct AST;
typedef struct {
    union {
        void *globalPtr;
        union {
            struct CVariable *externVar;
            struct CType *externType;
        };
        char *importLink;
    };
    enum {
        LINK_NORMAL=0,
        LINK_EXTERN=1,
        LINK__EXTERN=2,
        LINK_IMPORT=3,
        LINK__IMPORT=4,
        LINK_STATIC=5,
    } type;
} CLinkage;
typedef struct CType {
    int refCnt;
    enum {
        TYPE_U0,
        TYPE_I8,
        TYPE_U8,
        TYPE_I16,
        TYPE_U16,
        TYPE_I32,
        TYPE_U32,
        TYPE_I64,
        TYPE_U64,
        TYPE_F64,
        TYPE_BOOL,
        TYPE_PTR,
        TYPE_FUNC,
        TYPE_ARRAY,
        /**
         * This is the location of an array
         */
        TYPE_ARRAY_REF,
        TYPE_CLASS,
        TYPE_UNION,
    } type;
    union {
        struct CType *ptr;
        struct {
            struct CType *ret;
            vec_CType_t arguments;
            vec_AST_t dftArgs;
            vec_str_t names;
            unsigned int hasvargs:1;
        } func;
        struct {
            struct CType* base;
            //-1 for undefined
            int64_t dim;
        } array;
        struct {
            CLinkage linkage;
            char *name;
            vec_CMember_t members;
            int size;
            int align;
            struct CType *baseType;
            unsigned int isFwd:1;
            struct CType *inheritsFrom;
            char *fn;
            long ln;
        } cls;
        struct {
            CLinkage linkage;
            char *name;
            vec_CMember_t members;
            int size;
            int align;
            struct CType *baseType;
            unsigned int isFwd:1;
            struct CType *inheritsFrom;
            char *fn;
            long ln;
        } un;
    };
} CType;
typedef struct {
    struct AST* linkage;
    struct AST* name;
    struct CType *finalType;
    //Write into me
    struct CType *basetype;
    struct AST *dft;
    struct AST *metaData;
} CDeclTail;
typedef vec_t(CDeclTail) vec_CDeclTail_t;
struct CVariable;
struct CType;
typedef struct AST {
    int refCnt;
    enum {
        AST_ASM_OPCODE,
        AST_ASM_REG,
        AST_ASM_ADDR,
        AST_EXPORT_LABEL,
        AST_LOCAL_LABEL,
        AST_ASM_DU8,
        AST_ASM_DU16,
        AST_ASM_DU32,
        AST_ASM_DU64,
        AST_ASM_IMPORT,
        AST_ASM_ALIGN,
        AST_ASM_BINFILE,
	AST_ASM_BLK, //like AST_STMT_GROUP
        //
        AST_LINKAGE,
        //
        /**
         * This is a werid one. There is a type that is initialily a U0,but the
         * pointer for this dummy type will be stored so you can change it.
         */
        __AST_DECL_TAILS,
        //
        AST_NOP,
        AST_STMT_GROUP,
        AST_ARRAY_LITERAL,
        //
        AST_NAME,
        AST_MEMBER_ACCESS,
        //
        AST_PAREN,
        AST_INT,
        AST_FLOAT,
        AST_CHAR,
        AST_STRING,
        //
        AST_COMMA,
        //
        AST_SHL,
        AST_SHR,
        AST_POW,
        //
        AST_MUL,
        AST_DIV,
        AST_MOD,
        //
        AST_BAND,
        AST_BXOR,
        AST_BOR,
        //
        AST_ADD,
        AST_SUB,
        //
        AST_LT,
        AST_GT,
        AST_LE,
        AST_GE,
        AST_RANGE,
        //
        AST_EQ,
        AST_NE,
        //
        AST_LAND,
        AST_LXOR,
        AST_LOR,
        //
        AST_ASSIGN,
        AST_ASSIGN_SHL,
        AST_ASSIGN_SHR,
        AST_ASSIGN_MUL,
        AST_ASSIGN_MOD,
        AST_ASSIGN_DIV,
        AST_ASSIGN_BAND,
        AST_ASSIGN_BOR,
        AST_ASSIGN_BXOR,
        AST_ASSIGN_ADD,
        AST_ASSIGN_SUB,
        //
        AST_POST_INC,
        AST_POST_DEC,
        //
        AST_ARRAY_ACCESS,
        //
        AST_PRE_INC,
        AST_PRE_DEC,
        //
        AST_POS,
        AST_NEG,
        //
        AST_LNOT,
        AST_BNOT,
        //
        AST_DERREF,
        AST_ADDROF,
        //
        AST_SIZEOF,
        //
        AST_TOKEN,
        //
        AST_IMPLICIT_TYPECAST,
        AST_EXPLICIT_TYPECAST,
        //
        AST_FUNC_CALL,
        //
        AST_IF,
        AST_FOR,
        AST_DO,
        AST_WHILE,
        //
        AST_METADATA,
        //
        AST_DECL,
        AST_VAR,
        //
        AST_KEYWORD,
        //
        AST_CASE,
        AST_SWITCH,
        AST_SUBSWITCH,
        AST_BREAK,
        AST_DEFAULT,
        //
        AST_RETURN,
        //
        AST_TYPE,
        //
        AST_GOTO,
        AST_LABEL,
        //
        AST_TRY,
        //
        /**
         * Use Comma "string",operands for sassy print statents.
         * AST_PRINT_STRING only prints a single string (.single).
         */
        AST_PRINT_STRING,
        //
        AST_LASTCLASS,
    } type;
    union {
        //For use with DUxx is asm blocks
        vec_AST_t duData;
        vec_AST_t asmImports;
        struct AST *asmBinfile;
        struct {
          struct AST *align;
          struct AST *fill;
        } asmAlign;
        /**
         * This points to a CReg* from ASM.HC
         */
        void *asmReg;
        struct {
            struct AST *segment;
            struct AST *scale;
            struct AST *index;
            struct AST *base;
            struct AST *disp;
            int64_t width;
        } asmAddr;
        struct {
          struct AST *name;
          vec_AST_t operands;
        } asmOpcode;
        vec_AST_t arrayLiteral;
        struct AST *sizeofExp;
        struct {
            struct AST *try;
                struct AST *catch;
        } try;
        struct AST *retval;
        struct AST *labelNode;
        struct {
            int64_t low;
            int64_t high;
            struct AST* owner;
            unsigned char isUndef:1;
        } cs;
        struct {
            char *name;
            struct AST *dft;
            CType *type;
        } decl;
        vec_CMetaData_t metaData;
        struct {
            struct AST *a,*b;
        } binop;
        struct AST *unopArg;
        char *string;
        uint64_t integer;
        double floating;
        uint64_t chr;
        int tokenAtom;
        int keyword;
        struct {
            vec_int_t ops;
            vec_AST_t operands;
        } range;
        struct {
            struct AST *a;
            char *member;
        } memberAccess;
        struct {
            struct AST *array;
            struct AST *subscript;
        } arrayAccess;
        struct {
            struct AST *base;
            struct AST *toType;
        } typecast;
        struct {
            struct AST *func;
            struct AST *args; //See AST_COMMA
        } funcCall;
        struct {
            struct AST *cond;
            struct AST *body;
            struct AST *elseBody;
        } ifStmt ;
        struct {
            struct AST *init;
            struct AST *cond;
            struct AST *next;
            struct AST *body;
        } forStmt;
        struct {
            struct AST *cond;
            struct AST *body;
        } doStmt;
        struct {
            struct AST *cond;
            struct AST *body;
        } whileStmt;
        struct {
            struct AST *cond;
            struct AST *body;
        } switStmt;
        struct CVariable *var;
        struct {
            struct AST *startCode;
            struct AST *body;
        } subswitch;
        CLinkage linkage;
        vec_AST_t stmts;
        vec_CDeclTail_t declTail;
        //Local labels use LOCAL_LAB_FMT as the name,so be careful,see GetLabelReadableName(AST *t)
        char *name;
    };
  long issynChecked:1;
  long inAsmBlk:1;
  long labelContext:30;
    CType *type2;
    /**
     * From Lexer.filenames
     */
    char *fn;
    int ln,col;
} AST;
char *GetLabelReadableName(struct AST *t);
int IsBinop(AST *node);
AST *CreatePrint(char *prn);
int IsUnop(AST *node);
AST *CreateBinop(AST *a,AST *b,int type);
AST *CreateSizeof(AST *a);
AST *CreateTypecast(AST *a,AST *to_type);
AST *AppendToRange(AST *a,AST *b,int type);
AST *CreateUnop(AST *a,int type);
AST *CreateArrayAccess(AST *a,AST *sub);
AST *CreateMemberAccess(AST *a,AST *member) ;
AST *CreateTypecast(AST *a,AST *to_type);
AST *CreateFuncCall(AST *func,AST *args /*AST_COMMA*/);
AST *CreateIf(AST *cond,AST *body,AST *el) ;
CType *CloneType(CType*);
AST *AppendToMetaData(AST *first,AST *name,AST * value);
CType *CreatePrimType(int type);
CType *CreatePtrType(CType * type);
CType *CreateArrayType(CType * base,AST *dim);
CType *CreateFuncType(CType *ret,AST *args,int hasvargs) ;
int IsF64(CType *type);
int IsU64(CType *type);
int IsI64(CType *type);
int IsPtr(CType *type);
CType *AssignTypeToNode(AST *node);
void ReleaseType(CType *t);
CType *DerrefedType(CType *type);
int IsArith(CType *t);
AST *CreateDo(AST *cond,AST *body) ;
AST *CreateWhile(AST *cond,AST *body) ;
AST *AppendToStmts(AST *a,AST *b);
AST *CreateNop();
AST *CreateSubswitch(AST *body);
AST *CreateSwitch(AST* cond,AST *body);
vec_AST_t FindCasesInSwitch(AST *swit);
AST *CreateFor(AST *init,AST *cond,AST *next,AST*body);
AST *CreateCase(AST *low,AST *high);
AST *CreateDefault() ;
void EnableREPL();
void DisableREPL();
void RaiseError(AST *at,...);
void RaiseWarning(AST *at,...);
CType *CreateClassForwardDecl(AST *linkage,AST *name);
CType *CreateUnionForwardDecl(AST *linkage,AST *name);
typedef struct CMacro {
    char *name;
    char *expand;
    char *fn;
    long line;
} CMacro;
typedef struct {
    char *oldfile;
    char *olddir;
    long fpos;
    long physical_fpos_start;
    long fileEnd;
    long cursor;
    int isFreeToFlush:1;
} CLexerFileState;
typedef vec_t(CLexerFileState) vec_CLexerFileState_t;
typedef map_t(CMacro ) map_CMacro_t;
typedef void (*lexer_cb_t)(int tok,AST *d);
typedef map_t(vec_int_t) map_vec_int_t_t;
typedef struct {
    char success;
    char hasElse;
} CPreprocIfState;
typedef vec_t(CPreprocIfState) vec_CPreprocIfState_t;
typedef struct CLexer {
    map_str_t filenames;
    map_vec_int_t_t fileLineStarts;
    map_CMacro_t macros;
    map_int_t keywords;
    vec_str_t expandChain;
    char filename[1024];
    long fpos;
    int multilineComment:1;
    int replMode:1;
    int errorFlag:1;
    int stopAtNewline:1;
    int isExeMode:1;
    int isDebugExpr:1;
    //True if not in string or comment
    int isFreeToFlush:1;
    //Used for #if's
    int isEvalMode:1;
    int isEvalNoCommaMode:1;
    mrope_t *source;
    lexer_cb_t cb;
    /**
     * This is placed after a macro expansion to specify where the physical fpos is
     */
    long physical_fpos_start;
    long cursor_pos;
    long curFileEnd;
    //Includes all macros expansions
    int macroExpandDepth;
    vec_CLexerFileState_t fileStates;
    //Used with error reporting for fatal parser errors.
    AST *lastToken;
    enum {
        PARSER_HOLYC,
        PARSER_DEBUG_EXPR,
    } whichParser;
    vec_CPreprocIfState_t ifStates;
    FILE *replSource;
    mrope_t *currentExeStream;
    /**
     * Dont use this,The assembler is written in HolyC and the lexer will call IsOpcode(which calls HCRT/ASM.HC)
     * This is used in tags generation if Compiler.tagsFile is set.
     */
    map_void_t __opcodes;
    //Same
    map_void_t __registers;
} CLexer;
extern CLexer Lexer;
#define LTF_EXPANSION 1
char *LexExpandText(char*text) ;
void ReleaseAST(AST *t);
AST *CreateString(char *txt) ;
void FreMacro(CMacro *macro);
int LexExpandWord() ;
void LexerSearch(char *text);
AST * LexString() ;
#define EXPAND_MACROS 1
char *WordAtPoint(int expandMacro);
#define SW_SKIP_STRING 1
void SkipWhitespace(int flags);
void RestoreLexerFileState(CLexerFileState backup) ;
CLexerFileState CreateLexerFileState();
void FreeMacro(CMacro *macro);
void CreateLexer(int whichParser) ;
int LexMacro();
AST *CreateI64(uint64_t i);
AST *CreateF64(double);
AST *LexItem();
void KillParser();
void AttachParserToLexer();
struct CFunction;
void ReleaseFunction(struct CFunction*func);
void RunStatement(AST *s);
typedef struct CVariable {
    int isNoreg:1;
    int isReg:1;
    int isGlobal:1;
    int isFunc:1;
    int isPublic:1;
    char *name;
    CType *type;
    int refCount;
    CLinkage linkage;
    union {
        long frameOffset;
        jit_value reg;
        struct CFunction *func;
    };
    char *fn;
    long line;
} CVariable;
typedef struct CValue {
    enum {
        VALUE_FLOAT,
        VALUE_INT,
        VALUE_VAR,
        VALUE_STRING,
        VALUE_INDIR_VAR,
    } type;
    union {
        struct CVariable *var;
        uint64_t integer;
        double flt;
        char *string;
    };
} CValue;
AST *CreateTry(AST *try,AST *catch);
CVariable *CloneVariable(CVariable *var);
#define VALUE_VAR(vari) ((CValue){VALUE_VAR,.var=CloneVariable(vari)})
#define VALUE_INDIR_VAR(vari) ((CValue){VALUE_INDIR_VAR,.var=CloneVariable(vari)})
#define VALUE_INT(i) ((CValue){VALUE_INT,.integer=i})
#define VALUE_FLOAT(f) ((CValue){VALUE_FLOAT,.flt=f})
char *RegisterString(char *string);
#define VALUE_STRING(s) ((CValue){VALUE_INT,.string=RegisterString(s)})
typedef map_t(CType*) map_CType_t;
typedef map_t(CVariable*) map_CVariable_t;
typedef vec_t(CVariable*) vec_CVariable_t;
typedef vec_t(CValue) vec_CValue_t;
typedef vec_t(struct jit_op*) vec_jit_op_t;
typedef map_t(struct jit_label*) map_jit_label_t;
CVariable *Import(CType *type,AST *name,char *importname);
CVariable *Extern(CType *type,AST *name,char *importname);
typedef struct CBreakpoint {
    char *fn;
    long ln;
    unsigned char enabled:1;
} CBreakpoint;
typedef map_t(CBreakpoint) map_CBreakpoint_t;
typedef struct CLabelRef {
  AST *at;
  struct jit_op *op;
} CLabelRef;
typedef vec_t(CLabelRef) vec_CLabelRef_t;
typedef map_t(vec_CLabelRef_t) map_vec_CLabelRef_t;
typedef long LabelContext;
typedef struct {
  unsigned int isRel:1;
  void *ptr;
  AST *exp;
  long width,rel_offset;
  LabelContext context;
} CAsmPatch;
typedef vec_t(CAsmPatch *) vec_CAsmPatchP_t;
typedef struct {
  struct jit_label *label;
  LabelContext context;
  unsigned long inAsmBlk:1;
} CLabel;
typedef map_t(CLabel) map_CLabel_t;
typedef struct {
  vec_CAsmPatchP_t asmPatches;
  vec_CVariable_t unlinkedImportVars;
    vec_CVariable_t unlinkedExternVars;
    vec_CType_t unlinkedImportTypes;
    vec_CType_t unlinkedExternTypes;
    map_CVariable_t globals;
    map_CType_t types;
    map_CVariable_t locals;
    CType *returnType;
    int inFunction:1;
    int errorFlag:1;
    int debugMode:1;
    int boundsCheckMode:1;
    int silentMode:1;
    //addrofFrameoffsetMode will make addr-ofs return the frame offset
    int addrofFrameoffsetMode:1;
    char *tagsFile;
    FILE *errorsFile;
    struct jit *JIT;
    int frameOffset;
    int tmpIntRegStart;
    int tmpFltRegStart;
    vec_CValue_t valueStack;
    vec_jit_op_t breakops;
    map_CLabel_t labels;
  map_void_t labelPtrs;
  map_str_t strings;
  map_vec_CLabelRef_t labelRefs;
    map_CBreakpoint_t breakpoints;
    /**
     * Used for debug print-expresions.
     * This points to the frame pointer of the debugged function
     */
    void *debugFramePtr;
    CType *lastclass;
    struct AST *lastLabel;
  vec_AST_t __addedGlobalLabels;
  //Will be filled with NULL if not defined yet
  map_jit_label_t asmTaintedLabels;
  LabelContext labelContext;
  map_CVariable_t exportedLabels;
  map_vec_voidppp_t currentRelocations;
} CCompiler;
#define LOCAL_LAB_FMT "@@(%li):%s"
extern CCompiler Compiler;

typedef struct CLiveInfo {
    jit_value value;
    CVariable *var;
    long stackIndex;
} CLiveInfo;
typedef map_t(CLiveInfo) map_CLiveInfo_t;
typedef vec_t(map_CLiveInfo_t) vec_map_CLiveInfo_t;
typedef struct CFuncInfo {
    map_CVariable_t noregs;
    vec_map_CLiveInfo_t table;
} CFuncInfo;
typedef struct CFunction {
    struct jit *JIT;
    void(*funcptr)(int64_t,...);
    CType *type;
    map_CVariable_t locals;
    CFuncInfo funcInfo;
    char *name;
    map_vec_voidppp_t relocations;
} CFunction;
void AddVariable(AST *name,CType *type);
/**
 * tmp_basetype is the temporary base type which will be replaced
 */
AST *AppendToDecls(AST *appendto,CType *tmp_basetype,CType *type,AST *name,AST *dft,AST *metadata);
void AddDeclsToScope(AST *decls);
void DeclsFillInBasetype(CType *bt,AST *tails);
CType *CreateMultiLvlPtr(CType *type,int lvl);
AST *CreateVarDecl(CType *type,AST *name,AST *dft);
AST *CreateTypeNode(CType *t) ;
void ReleaseVariable(CVariable *var);
void AttachParserToLexer();
void KillParser();
AST *LexToken();
void PrintAST(AST *n,int tabs);
int TypeAlign(CType *t) ;
int TypeSize(CType *t);
CType *BaseType(CType *bt);
int IsIntegerType(CType *t);
CVariable *GetVariable(char *name);
void __CompileAssignV2V(CVariable *var,CVariable *var2);
int IsVarRegCanidate(CVariable *var);
CVariable *CreateTmpRegVar(int v,CType *type);
void __CompileAssignInt2V(CVariable *var,uint64_t i);
void CompileAssign(CValue dst,CValue src);
int IsPtrType(CType *t);
AST *PtrifyIfNeeded(AST *ast);
AST *ConvertMembersToPtrs(AST *node);
int TypeIsSigned(CType *t);
AST *CreateVarNode(CVariable *var);
void ReleaseValue(CValue *v);
AST *CreateImplicitTypecast(AST *a,AST *to_type);
AST *CreateExplicitTypecast(AST *a,AST *to_type);
AST *CreateReturn(AST *exp);
#define C_AST_FRAME_OFF_DFT -1
/**
 * Use C_AST_FRAME_OFF_DFT to compute the frame offset for you
 */
CFunction *CompileAST(map_CVariable_t *locals,AST *exp,vec_CVariable_t args,long frameOffset);
CValue CloneValue(CValue v) ;
AST *SetPosFromLex(AST *t);
AST *SetPosFromOther(AST *dst,AST *src);
void CreateFuncForwardDecl(AST *linkage,CType *rtype,AST *name,AST *args,int hasvargs);
void CompileFunction(AST *linkage,CType *rtype,AST *name,AST *args,AST *body,int hasvargs);
int ASTToToken(AST *t);
AST *CreateIfStmt(AST *cond,AST *body,AST *el);
AST *CreateForStmt(AST *i,AST *c,AST *n,AST *body);
AST *CreateWhileStmt(AST *cond,AST *body);
AST *CreateLabel(AST *name);
AST *CreateGoto(AST *name);
#define EVAL_INT_F_PRESERVE_LOCALS 1
int64_t EvaluateInt(AST *exp,int flags);
AST *CreateBreak();
char *MStrPrint(const char *fmt,int64_t argc,int64_t *argv);
void TOSPrint(const char *fmt,int64_t argc,int64_t *argv);
void ScanAST(AST *t,void(*func)(AST *,void *),void *data);
void InheritFromType(CType *btt,CType *wt);
/**
 * appendto is a AST_TYPE with TYPE_CLASS/TYPE_UNION
 * member can be a DECL_TAIL or an anoynumus class/union(AST_TYPE)
 * member can also be a AST_METADATA
 */
AST *AppendToTypeMembers (AST *appendto,AST* member);
void __CompileAST(AST *exp);
void CompileTry(AST *t);
AST *AppendToArrLiteral(AST *start,AST *end);
void AssignUnionBasetype(AST *t,AST *bt);
jit_value MoveGlobalPtrToReg(CVariable *var,int r);
AST *CreateExternLinkage(AST *_name);
AST *CreateImportLinkage(AST *_name);
int TypeIsForward(CType *t);
/**
 * Will resolve a type with linkage,or if none is provided it will just return the type
 */
CType *ResolveType(CType *type);
CLinkage CloneTypeLinkage(CLinkage link);
CLinkage CloneVarLinkage(CLinkage link);
int TypeEqual(CType *_a,CType *_b);
void WarnOnWeirdPass(AST *at,CType *_to,CType *_from);
void RegisterBuiltins();
static void _AF_AST(AST **node) {
    ReleaseAST(*node);
}
#define AF_AST __attribute__((cleanup(_AF_AST)))
static void _AF_TYPE(CType **node) {
    ReleaseType(*node);
}
#define AF_TYPE __attribute__((cleanup(_AF_TYPE)))
static void _AF_VALUE(CValue *v) {
    ReleaseValue(v);
}
#define AF_VALUE __attribute__((cleanup(_AF_VALUE)))

static void _AF_VAR(CVariable **node) {
    ReleaseVariable(*node);
}
#define AF_VAR __attribute__((cleanup(_AF_VAR)))

typedef struct CBacktrace {
    void *bp;
    CFunction *func;
    CBreakpoint *lastbp;
} CBacktrace;
typedef vec_t(CBacktrace) vec_CBacktrace_t;
typedef map_t(CBreakpoint*) map_CBreakpoint_ptr_t;
typedef struct CDebugger {
    vec_CBacktrace_t callStack;
    map_CBreakpoint_ptr_t activeBreakpoints;
    //This is the number of breakpoints enabled since the debuggger started
    int breakpointNumber;
    unsigned int step:1;
    unsigned int fin:1;
    unsigned int next:1;
    /**
     * This is the stack depth on the last breakpoint to be breaked on
     * It is used with next/fin to see if we have exited the function or not
     */
    long prevStackDepth;
} CDebugger;
extern CDebugger Debugger;
void CallDebugger(void *rstack,long index);
void VisitBreakpoint(CBreakpoint *bp);
void CompileBreakpoint(AST *at);
void DbgLeaveFunction();
void DbgEnterFunction(void *baseptr,CFunction *func);
void EnterDebugger();
CLexer ForkLexer(int whichparser);
void DbgPrintVar(CType *type,void *ptr);
void EvalDebugExpr(CFuncInfo *info,AST *exp,void *framePtr);
AST * ApplyLinkageToDecls(AST *linkage,AST *tails) ;
AST *CreateStaticLinkage();
char *TypeToString(CType *t);
void FlushLexer();
typedef struct COldFuncState {
  vec_CAsmPatchP_t asmPatches;
    map_CVariable_t locals;
    CType *returnType;
    int inFunction:1;
    int errorFlag:1;
    int debugMode:1;
    struct jit *JIT;
    int frameOffset;
    int tmpIntRegStart;
    int tmpFltRegStart;
    vec_CValue_t valueStack;
    vec_jit_op_t breakops;
    map_CLabel_t labels;
  map_void_t labelPtrs;
    map_vec_CLabelRef_t labelRefs;
  vec_AST_t __addedGlobalLabels;
  LabelContext labelContext;
  map_vec_voidppp_t currentRelocations;
} COldFuncState;
COldFuncState EnterFunction(CType *returnType,AST *_args);
COldFuncState CreateCompilerState();
typedef struct {
  unsigned int active:1;
  map_CVariable_t imports;
} CAssembler;
extern CAssembler Assembler;
void RestoreCompilerState(COldFuncState old);
int DebugIsTrue(void *frameptr,CFuncInfo *info,char *text);
void DebugEvalExpr(void *frameptr,CFuncInfo *info,char *text);
void WhineOnOutOfBounds(void *ptr,int64_t size);
void SignalHandler(int s);
void StreamPrint(const char *fmt,int64_t argc,int64_t *argv);
void Backtrace();
void DumpTagsToFile(char *fn);
CType *FinalizeClass(CType *class);
double EvaluateF64(AST *exp);
int IsOpcode(char *name);
vec_AST_t CommaToVec(AST* comma);
void * GetRegister(char *name);
AST *AppendToSIB(AST *sib,AST *scale,AST *reg,AST *offset);
void AssembleOpcode(AST *at,char *name,vec_AST_t operands);
AST *CreateExportedLabel(AST *name);
AST *CreateLocalLabel(AST *name);
AST *CreateDU64(AST *comma);
AST *CreateDU32(AST *comma);
AST *CreateDU16(AST *comma);
AST *CreateDU8(AST *comma);
AST *CreateAsmImport(AST *comma);
AST *CreateAsmAlign(AST *a,AST *fill);
void LeaveAssembler();
void EnterAssembler();
void *GetGlobalPtr(CVariable *var);
void *EvalLabelExpr(AST *a,LabelContext context);
struct ExceptFrame;
struct ExceptFrame *EnterTry();
void PopTryFrame();
void ApplyPatches();
void LeaveFunction(COldFuncState old);
char *HashLabel(char *name,LabelContext context,int is_local);
/**
 * This accounts for local labels
 */
char *ResolveLabelByName(char *label,LabelContext context);
LabelContext NextLabelContext();
int64_t EvalExprNoComma(char *text,char **en);
void AssignClassBasetype(AST *t,AST *bt);
void FillFunctionRelocations(CFunction *func);
