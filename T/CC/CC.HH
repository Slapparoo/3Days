/* CC — an ANSI C compiler for 3Days.
 * Compiles a C subset to x86-64 machine code into executable memory
 * (MAlloc is RWX here) and calls it through a zero-arg function pointer.
 * See T/CC/PLAN.md for the roadmap. */
#ifndef CC_HH
#define CC_HH 1

#define TK_EOF		0
#define TK_IDENT	1
#define TK_NUM		2
#define TK_PUNCT	3
#define TK_STR		4

#define KW_NONE		0
#define KW_INT		1
#define KW_RETURN	2
#define KW_IF		3
#define KW_ELSE		4
#define KW_WHILE	5
#define KW_BREAK	6
#define KW_CHAR		7
#define KW_FOR		8
#define KW_CONTINUE	9
#define KW_STRUCT	10
#define KW_CONST	11	//M13: qualifier, no-op in the byte/slot model
#define KW_VOID		12	//M13: maps to the byte (char) model
#define KW_UNSIGNED	13	//M13: fold to the single int type
#define KW_LONG		14	//M13: fold to the single int type
#define KW_SIGNED	15	//M13: fold to the single int type
#define KW_TYPEDEF	16	//M13: name bindings, see CCTypedefFind

/* Minimal expression types (everything is a 64-bit slot; pointers
 * carry only their element size for scaling). */
#define TY_INT		0	//8-byte integer slot
#define TY_PTR		1	//pointer to 8-byte slots (int*, int[] )
#define TY_CHAR		2	//byte-valued scalar (stored in a slot)
#define TY_CHARP	3	//pointer to bytes (char*, char[], "str")
#define TY_STRUCT	4	//address of an 8-byte-slot struct (tag in cc->sty)
#define TY_VOID		5	//M13: `void` as written; callers fold it to
				//the byte (TY_CHAR) model before storing

/* Two-char punct tokens (single chars come through as their ASCII). */
#define OP_EQ		200
#define OP_NE		201
#define OP_LE		202
#define OP_GE		203
#define OP_AND		204
#define OP_OR		205
#define OP_INC		206
#define OP_DEC		207
#define OP_SHL		208	//<<
#define OP_SHR		209	//>>
//Compound assignment: keep OP_PLUSEQ..OP_SHREQ CONTIGUOUS — the parser
//dispatches with a range test — and their low nibble is not reused as
//a single-char punct.
#define OP_PLUSEQ	210	//+=
#define OP_MINUSEQ	211	//-=
#define OP_STAREQ	212	//*=
#define OP_SLASHEQ	213	///=
#define OP_PCTEQ	214	//%=
#define OP_AMPEQ	215	//&=
#define OP_PIPEEQ	216	//|=
#define OP_CARETEQ	217	//^=
#define OP_SHLEQ	218	//<<=
#define OP_SHREQ	219	//>>=
#define OP_ARROW	220	//->  (outside the PLUSEQ..SHREQ compound range!)

/* x86 condition codes we need (SETcc opcode low nibble base 0x90). */
#define CC_OPC_NE	0x95
#define CC_OPC_EQ	0x94
#define CC_OPC_L	0x9C
#define CC_OPC_LE	0x9E
#define CC_OPC_G	0x9F
#define CC_OPC_GE	0x9D

#define CC_CODE_SIZE	0x10000
#define CC_STACK_SIZE	0x8000
#define CC_TOK_MAX	128
#define CC_LOCAL_SIZE	0x10000
#define CC_OUT_SIZE	0x10000	//harness-printed output buffer (power of 2)
#define CC_SYMS		256

#define SC_GLOBAL	0
#define SC_LOCAL	1
#define SC_FUN		2
#define SC_PARAM	3

#define CC_MAX_PARAMS	8
#define CC_PATCHES	16	//max deferred call sites per function
#define CC_STRUCTS	16
#define CC_MAX_MEMBERS	8
#define CC_FIXUPS	4096	//M10: max recorded external absolute addresses

/* Absolute addresses that point OUTSIDE the code buffer. Every such
 * emit (CCQuadAbs) records (kind, delta); a dumped object (CCSave)
 * re-applies them against a fresh CCInit'd control block (CCLoad).
 * Everything else in the code is self-relative (rel32 calls/jumps,
 * RBP-relative locals) and relocates by itself. */
#define CC_B_LOCALS	0	//globals + string pool arena
#define CC_B_STACK	1	//private C stack
#define CC_B_SAVE	2	//caller-RSP save slot
#define CC_B_OUT	3	//harness-printed output buffer
#define CC_B_CTRL	4	//the cc block itself (argc/argv fields)
#define CC_B_HFLUSH	5	//the HolyC CCFlush() helper address
#define CC_B_LIB	6	//cc->lib[] stdlib delegation helper table

/* M12: cc->lib[] slots — zero-arg HolyC helpers that read staged args
 * from cc->ext[] and call kernel routines (CC_LIB.HC). Slot order is
 * the ABI between CCSetLib and CCEmitLibCall deltas (8*slot). */
#define CC_LIB_STRLEN	0
#define CC_LIB_MEMCPY	1
#define CC_LIB_MEMSET	2
#define CC_LIB_MEMCMP	3
#define CC_LIB_MALLOC	4
#define CC_LIB_GETCHAR	5
#define CC_LIB_SLOTS	8
/* M13 preprocessor-lite + typedef tables (compile-time only: a saved
 * object carries code and relocations, never source-level names, so
 * CCSave/CCLoad ignore all of these). */
#define CC_MAX_MACROS	32
#define CC_MACRO_BODY	256
#define CC_MAX_TYPEDEF	32
#define CC_INC_MAX	8	//nested-#include depth guard

/* Struct tag: one table entry per `struct Name { ... }` definition.
 * Every member is one 8-byte slot (members at byte offset 8*i); a char
 * member keeps byte load/store semantics via its TY_CHAR in mty. */
class CStag
{
  U8	name[CC_TOK_MAX];
  U8	mname[CC_MAX_MEMBERS*CC_TOK_MAX];	//flat, i*CC_TOK_MAX
  I64	mty[CC_MAX_MEMBERS];	//member TY_* (INT/CHAR/PTR/CHARP)
  I64	msty[CC_MAX_MEMBERS];	//member's struct tag (TY_PTR to struct), else -1
  I64	moff[CC_MAX_MEMBERS];	//byte offset = 8*i
  I64	nmem;
  I64	size;			//8*nmem bytes
};

class CSym
{
  U8	name[CC_TOK_MAX];
  I64	kind;		//SC_* (every symbol holds one I64 slot)
  I64	offset;		//locals: [RBP-8-off]; globals: byte offset
			//into cc->locals; funs: code offset (-1 = undefined yet)
  I64	ty;		//TY_INT/TY_PTR/TY_CHAR/TY_CHARP/TY_STRUCT
  I64	sty;		//struct tag id when ty==TY_STRUCT or a pointer
 			//to one; -1 otherwise
  I64	dim;		//>1: array of that many slots/bytes at offset
  I64	init;		//global constant initializer value
  I64	has_init;
  I64	patches[CC_PATCHES];	//call sites (rel32 operand pos) awaiting def
  I64	npatch;
  I64	fargc;		//SC_FUN: parameter count (CCFind arg thunks)
  I64	extstub;	//cached address of the zero-arg ext[] stub
			//(CCExtStub); 0 = not built yet; resets with CCInit
};

class CCCtrl
{
  U8	*src;		//source text (MAlloced, NUL terminated)
  U8	*pc;		//lexer cursor into src
  I64	tk;		//current token kind
  I64	tk_kw;		//keyword id when tk==TK_IDENT
  U8	tk_str[CC_TOK_MAX];	//identifier text
  I64	tk_num;		//number value / punct char
  U8	*code;		//executable code buffer (MAlloced)
  U8	*ep;		//emit pointer
  U8	*stack;		//private C stack (HolyC fp calls enter with rsp at
			//the mapped stack bottom: the entry stub switches RSP
			//to this, saving the old value in *save)
  U8	*save;		//one I64 slot holding the caller RSP
  U8	*locals;	//global variable arena, zeroed before each run
  U8	*out;		//program output buffer: [0..7]=I64 LE count,
 			//bytes from [8..]; the HolyC harness prints it
 			//through the kernel — JIT'd code never does I/O
  U8	*hflush;	//address of the HolyC CCFlush() helper: generated
			//putchar code zero-arg-CALLs it on '\n' and when the
			//buffer is nearly full (M6c streaming). Set by
			//CCCompile/CCLoad; a relocation base (CC_B_HFLUSH)
			//so saved objects rebind to the loading session.
  U8	*lib[CC_LIB_SLOTS];	//M12 stdlib delegation helpers (CC_LIB_*);
			//CCSetLib fills them, CC_B_LIB relocations rebase
			//them, CCLoad rebinds them per session
  U8	libscr[512];	//M12: dedicated scratch quads, pointed at via
			//RDI before any CALL into JIT-compiled HolyC code.
			//Verified from JIT bytes: a HolyC-compiled callee's
			//frame-zeroing prologue is MOV RAX,RDI; MOV RCX,n;
			//MOV AL,0; REP STOSQ — it WRITES 8*n bytes at the
			//address arriving in RDI. The HolyC-to-HolyC ABI
			//supplies a scratch pointer there; generated code
			//must too, or the callee zeroes whatever pointer
			//happened to be in RDI — observed un-fixed: the
			//string pool wiped mid-run, memcmp("abc","abd")
			//then reading both-wipe-equal as 0, correct other
			//runs: silently run-to-run nondeterministic.
  I64	goff;		//next free byte offset in locals
  I64	lofs;		//current function's local area usage (slots)
  I64	cur_entry;	//current function is the HolyC-called entry (main)
  I64	cur_argc;	//current function's parameter count
  I64	entry_off;	//code offset of 'main' after parse (-1 = none)
  I64	entry_inject;	//entry prologue should write argc/argv into the
  			//leading parameter slots (set for main only)
  I64	argc;		//argc/argv the harness sets before calling main;
  U8	**argv;		//main(int,char*[]) reads them from these fields
  I64	ext[4];		//argument staging slots: CCExtStub copies these
			//into a function's parameter slots, so HolyC can call
			//arg-taking C with a zero-arg indirect call (no pushed
			//args => no dependence on the JIT's deferred arg-pop;
			//see CCFindCallN and PLAN.md M11)
  U8	pname[CC_MAX_PARAMS*CC_TOK_MAX];	//params being parsed
  I64	pty[CC_MAX_PARAMS];	//their TY_* codes
  I64	psty[CC_MAX_PARAMS];	//their struct tag ids (-1 = not a struct ptr)
  I64	nparam;
  I64	brk[64];	//pending break-jump patch positions
  I64	brk_cnt;
  I64	cont[64];	//pending continue-jump patch positions (for-loops)
  I64	cont_cnt;
  /* Pending-lvalue model: expression parsers leave RAX holding a
   * VALUE unless is_addr says it holds an ADDRESS to be loaded later
   * by the consumer (CCVal/&/=/++/--/return). 2=array name decayed to
   * its address (never loads, never assigns). aty: 1=byte at [RAX],
   * 0=8-byte slot. ety: TY_* of the last-parsed expression. */
  I64	is_addr;
  I64	aty;
  I64	ety;
  I64	sty;		//struct tag id of the last-parsed expression
  			//(meaningful when ety==TY_STRUCT, or TY_PTR to a
  			//struct for '->'); -1 = not struct-related
  CStag	st[CC_STRUCTS];		//struct tag table
  I64	nst;			//defined tag count
  I64	if_depth;	//nesting depth of control-flow blocks
  CSym	syms[CC_SYMS];
  I64	sym_cnt;
  I64	errs;		//error count
  I64	nfix;			//M10: recorded absolute-address sites
  I64	fix_pos[CC_FIXUPS];	//code offset of the 8-byte absolute
  I64	fix_kind[CC_FIXUPS];	//CC_B_* base
  I64	fix_delta[CC_FIXUPS];	//byte offset from that base
  /* M13: object-like macros (name -> text) filled by the prep pass,
   * and typedef bindings (name -> TY_* base) filled by the parser.
   * Both reset with CCInit and live only for the source translation. */
  I64	mn;
  U8	mname[CC_MAX_MACROS][CC_TOK_MAX];
  U8	mbody[CC_MAX_MACROS][CC_MACRO_BODY];
  I64	tn;
  U8	tname[CC_MAX_TYPEDEF][CC_TOK_MAX];
  I64	tty[CC_MAX_TYPEDEF];
};

/* Forward declarations MUST use `extern` here: with a plain prototype,
 * this HolyC build mislinks mutual recursion (calls return garbage).
 * Definitions later in the unit bind correctly only via extern. */
extern U0 CCError(CCCtrl *cc,U8 *msg);

/* Lexer: advance to the next token. */
extern U0 CCLex(CCCtrl *cc);

/* Emit helpers. */
extern U0 CCByte(CCCtrl *cc,I64 b);
extern U0 CCHalf(CCCtrl *cc,I64 h);
extern U0 CCWord(CCCtrl *cc,I64 w);
extern U0 CCQuad(CCCtrl *cc,I64 q);

/* External-address emit: writes base(kind)+delta as 8 bytes and
 * records the site for CCSave/CCLoad (M10 object dumping). */
extern I64 CCBase(CCCtrl *cc,I64 kind);
extern U0 CCQuadAbs(CCCtrl *cc,I64 kind,I64 delta);
/* mov rax,imm64 */
extern U0 CCMovImmRax(CCCtrl *cc,I64 val);
/* Function prologue/return. entry!=0 emits the private-stack stub
 * (only for the function HolyC calls directly); internal functions get
 * a plain frame so C->C calls chain on the shared private stack. */
extern U0 CCPrologue(CCCtrl *cc,I64 entry);
extern U0 CCFunRet(CCCtrl *cc,I64 entry);
/* CALL rel32 to f (patched now or recorded on f for later), then
 * optional ADD RSP to pop n args. */
extern U0 CCCall(CCCtrl *cc,CSym *f);
extern U0 CCPopArgs(CCCtrl *cc,I64 n);
extern U0 CCPatchCallPos(CCCtrl *cc,I64 pos,I64 target_off);

/* Builtins: append AL to cc->out; exit(n)=entry-style return. */
extern U0 CCEmitPutchar(CCCtrl *cc);

/* C helpers, compiled by CC itself, prepended to every module.
 * printf's format lowering calls them at its call sites. */
extern U8 *CC_PRELUDE;

/* Parser (M1: one function whose body is `return <int-expr>;`).
 * Emits code; returns entry offset or -1 on error. */
extern I64 CCParse(CCCtrl *cc);

/* M13 preprocessor-lite: runs on the USER text before the prelude is
 * prepended. Handles #include (T:/CC/INC/<name> or an explicit path),
 * object-like #define/#undef, #ifdef/#ifndef/#else/#endif, #if 0/1,
 * #pragma. Macro names expand inside token text but never inside
 * literals. Returns the expanded NUL-terminated text (MAlloced) or
 * NULL after printing the diagnostic. */
extern U8 *CCPrep(CCCtrl *cc,U8 *text);

/* Symbol table. */
extern CSym *CCSymFind(CCCtrl *cc,U8 *name);
extern CSym *CCSymNew(CCCtrl *cc,U8 *name,I64 kind);
extern CSym *CCSymVar(CCCtrl *cc,U8 *name);	//find or declare

/* Emit RAX = address of a variable's storage (LEA/MOVABS). */
extern U0 CCVarAddr(CCCtrl *cc,CSym *s);

/* Jumps: emit a rel32 jmp/jz with placeholder; return the operand
 * position. CCPatchJmp fills it in (target = absolute code address). */
extern I64 CCJmp(CCCtrl *cc);
extern I64 CCJz(CCCtrl *cc);
extern U0 CCPatchJmp(CCCtrl *cc,I64 pos);
extern U0 CCPatchImm32(CCCtrl *cc,I64 pos,I64 val);

/* test RAX,RAX */
extern U0 CCTestRax(CCCtrl *cc);
/* setcc AL; movzx EAX,AL */
extern U0 CCSetccRax(CCCtrl *cc,I64 op);

#endif
