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
#define KW_CASE		17	//M14: switch labels (C lexer)
#define KW_DEFAULT	18	//M14
#define KW_SWITCH	19	//M14
#define KW_DO		20	//M14
#define KW_SIZEOF	21	//M15
#define KW_SHORT	22	//M15: folds to the single int width
				//like unsigned/long (C 2-byte short is a
				//documented deviation)
#define KW_GOTO		23	//M16
#define KW_UNION		24	//M19: shares the struct tag namespace and
					//the whole slot machinery (see CCStructDef)
#define KW_EXTERN		25	//M19 qualifiers: accepted and folded to
#define KW_STATIC		26	//no-ops — one width, one arena, one link
#define KW_REGISTER		27	//namespace in this model (see PLAN M19)
#define KW_AUTO		28
#define KW_VOLATILE		29
#define KW_INLINE		30
#define KW_ENUM		31	//M20: constants + int-model tag

/* Minimal expression types (everything is a 64-bit slot; pointers
 * carry only their element size for scaling). */
#define TY_INT		0	//8-byte integer slot
#define TY_PTR		1	//pointer to 8-byte slots (int*, int[] )
#define TY_CHAR		2	//byte-valued scalar (stored in a slot)
#define TY_CHARP	3	//pointer to bytes (char*, char[], "str")
#define TY_STRUCT	4	//address of an 8-byte-slot struct (tag in cc->sty)
//M25 ELEMENT-TYPE ENCODING: an element type is stored as `ty + 1`, with
//0 meaning "unknown/not an array". A raw type cannot serve as its own
//absent sentinel because TY_INT is 0 — `int a[1]` read as absent and kept
//its pre-M25 nil-deref bug. Encode with +1 at the three write sites
//(CCSetArrTy, the array-parameter branch, CCArrNext) and decode with -1
//where a subscript consumes it. No CC_AEL() macro: this build's host
//preprocessor has no FUNCTION-LIKE macros (the same limitation PLAN
//documents for CC's own prep-lite), so `#define CC_AEL(ty) ((ty)+1)`
//fails to parse the umbrella at compile time.
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
#define CC_B_CODE	7	//M18: the code buffer itself — the base
				//for FUNCTION ADDRESSES (fp values). A
				//saved object re-derives code+delta on
				//reload exactly like the other kinds.

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
#define CC_PREP_SCRA	0x10000	//M31: the macro-argument scratch, FIXED
				//capacity on purpose: a scanner caches its
				//region pointer, and a scratch that grows
				//would leave every open scan pointing into a
				//freed buffer. Overflow is a diagnostic.
#define CC_MAX_ARGS	16	//M31: arguments collected for ONE invocation of
				//a function-like macro (the variadic extras are
				//part of this count)
#define CC_MAX_TYPEDEF	32
#define CC_MAX_CONSTS	64	//M20: enumerator names per module
#define CC_MAX_ENUMTAGS	16	//M20: enum tags (redefinition check only)
#define CC_INIT_MAX	512	//M22: elements (or literal bytes) in one
				//array initializer
/* M30: the initializer TARGET SHAPE bundle. The writer that owns the object
 * builds it and CCInitCollect reads it to resolve DESIGNATORS, because the
 * legality of a designation depends on the type of the object the value sits
 * in: `[i]` applies only where that object is an array, `.name` only where it
 * is a struct or union, and a chain like `[1].a` crosses from one kind to the
 * other exactly as the declarator does. Two levels travel because a brace
 * group has already descended one level (C 6.7.9/20-21), so a designation
 * inside a group addresses the element the group opened.
 *   [0] outer level kind   [1] its tag id, else -1
 *   [2] element level kind [3] its tag id, else -1
 *   [4] charr (byte elements: a string literal IS its bytes)
 *   [5] A, the mapper's target arity (members per element, or row length)
 * The mapper is also told NARR, the number of ELEMENTS the object has: A
 * brace enclosure takes a whole element only when the level above is an ARRAY,
 * and a flat value may only roll into a new element if one exists. NARR is -1
 * for a scalar tag object (one aggregate, whose subobjects are its member
 * slots), 0 for an array whose length the initializer will DEDUCE, and the
 * length otherwise. */
#define CC_KI_SCALAR	0	//nothing below this can be designated
#define CC_KI_ARR	1	//`[i]` designates an element of it
#define CC_KI_TAG	2	//`.name` designates one of its members
#define CC_KI_UNION	3	//`.name` validates; every member IS the slot

#define CC_INC_MAX	8	//nested-#include depth guard
#define CC_SWITCH_CASES	32	//M14: case-label cap per switch statement
#define CC_LABELS	64	//M16: labels + gotos per function

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
  I64	size;			//8*nmem bytes (8 for a union)
  I64	isu;			//M23: TRUE for a union — initializer
				//arity is ONE member, not nmem
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
  I64	dim2;		//M17: second dimension (2-D row-major); 0 = 1-D
  I64	ael;		//M25: element TYPE of an array object (what one
			//subscript yields: TY_CHAR/TY_PTR/TY_CHARP/
			//TY_INT/TY_STRUCT), captured from the declarator
			//BEFORE array-ification and stored +1 per the
			//encoding note above. NON-ZERO IS THE TEST FOR
			//"this symbol is an array" — dim cannot serve:
			//it is 1 for one-element arrays and for every
			//array parameter. 0 = not an array, so the
			//subscript arm keeps guessing from the scale.
  I64	aelsty;		//that element's struct tag (when it is, or
			//points to, one), else -1
  I64	persist;	//M21: `static` block object: SC_LOCAL name,
				//global-arena storage (value survives calls)
  I64	init;		//global constant initializer value
  I64	has_init;
  I64	refs;		//M34: a name lookup ever asked this GLOBAL for its
			//storage (CCVarAddr sets it; a silent sizeof scan
			//does not). C only needs a definition for an object
			//something actually uses, which is why an unreferenced
			//`extern int g;` is legal and a referenced one is not.
  I64	def;		//M34: this file-scope object HAS a definition. A
			//declaration without `extern` is a TENTATIVE
			//definition and counts as one (C completes it with
			//a zero initializer, which is what the zeroed
			//arena already is); `extern int g;` alone is not,
			//so a name whose only declarations are extern ones
			//has no object and CCParse says so.
  I64	patches[CC_PATCHES];	//call sites (rel32 operand pos) awaiting def
  I64	npatch;
  I64	apatch[CC_PATCHES];	//M18: MOVABS-operand positions wanting this
  I64	napatch;		//function's code address (fp = f before
				//definition); resolved by CCParseFun
  I64	fargc;		//SC_FUN: parameter count (CCFind arg thunks)
  I64	extstub;	//cached address of the zero-arg ext[] stub
			//(CCExtStub); 0 = not built yet; resets with CCInit
};

class CCCtrl
{
  U8	*src;		//source text (MAlloced, NUL terminated)
  U8	fn[CC_TOK_MAX];	//M33: the file that text came from, set by the caller
			//that read it ("" when nobody knows).  Only the
			//preprocessor looks at it: it is what lets
			//`#include "x.h"` find a header beside the includer.
			//Prep-time only, like the macro table, and never in a
			//saved object.
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
  I64	in_loop;	//M14: inside a while/for/do body — `continue` in a
  			//switch must belong to an enclosing loop (CCContStmt
  			//diagnoses it otherwise); switch itself is NOT a loop.
  /* M16: statement labels and the forward-goto queue, PER FUNCTION
   * (reset where CCParseFun consumes '{'): C functions never nest, so
   * one table per function is correct — the M14 nested-switch lesson
   * was about state that must nest WITHIN a function; this does not.
   * goto_pos entries become -1 once the label patches them; whatever
   * is still >=0 at the closing '}' is an undefined-label error. */
  I64	nlab;
  U8	lab_name[CC_LABELS][CC_TOK_MAX];
  I64	lab_pos[CC_LABELS];
  I64	ngoto;
  U8	goto_name[CC_LABELS][CC_TOK_MAX];
  I64	goto_pos[CC_LABELS];
  I64	fpdim;		//M26: CCParseFunPtrDecl's array count, for
  				//`int (*tab[2])(int)`: 0 = no brackets,
  				//-1 = `[]` (size from the initializer or
  				//unknown, i.e. a parameter), n = count.
  				//Reset at every declarator entry, since a
  				//failed peek gets rewound by the caller.
  I64	spec_static;	//M21: CCAcceptBase saw `static` in this spec
  I64	spec_extern;	//M34: CCAcceptBase saw `extern` in this spec. C makes
			//that the difference between a DEFINITION and a
			//declaration that only names an object living
			//elsewhere, so the declarator has to know it.
  I64	sym_base;	//M21: waterline of the innermost open scope:
  				//CCSymNew's dup check stops here (inner scopes
  				//may redeclare — that is shadowing)
  I64	celem;		//M25: element type of the CURRENT array/pointer
			//expression, set only by Primary (chain roots) and
			//by the subscript arm; 0 = unknown, and the
			//subscript arm then falls back to the pre-M25
			//scale-based guess. Cleared at every expression
			//entry so no unrelated residue can reach a base.
  I64	celebsty;	//that element's struct tag, else -1
  I64	lofs_peak;	//M21: deepest frame slot any block reached this
  				//function: SUB RSP is patched to the peak, since
  				//closing a block rewinds the waterline
  I64	noemit;		//M15: sizeof(expr) parses the operand to learn its
  			//TYPE only — every code-writing site checks this and
  			//stays silent (CCByte choke-holds CCWord/CCHalf/CCQuad;
  			//CCQuadAbs skips its fixup RECORD too, else the saved
  			//position would patch live code at link time; jumps
  			//return -1 and CCPatchJmp ignores -1; CCCall skips,
  			//which also keeps forward-call bookkeeping clean).
  			//String pooling and symbol lookups still happen: the
  			//region allocates a little unused pool, by design.
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
  I64	pdim2[CC_MAX_PARAMS];	//M17: 2nd dim of array params (row scale
  				//source); 0 = none. 1st dim of a param is
  				//ALWAYS unknown (callers own the count).
  I64	pael[CC_MAX_PARAMS];	//M25: element type of array params (the
  I64	paelsty[CC_MAX_PARAMS];	//declarator type BEFORE array-ification,
   				//plus its struct tag): what one subscript
   				//of the param yields
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
  /* M17: multi-dim index scales, expression state. esz2!=0 means
   * "the value in hand decays to a ROW, not an element": the Postfix
   * index arm then scales by esz (bytes per row) and consumes the
   * pair (esz=esz2, esz2=0), making the row itself the new decay.
   * Set ONLY by the ident arms (from the symbol's dim/dim2); CCVal
   * clears esz2 so stale scales can never hijack a later plain
   * pointer/int index — the price is that a PARENTHESIZED 2-D decay
   * `(m)[0]` loses the row scale (documented; index without parens).
   * 1-D arrays never set esz2 and keep today's ety-driven scales. */
  I64	esz;
  I64	esz2;
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
  /* M24: the same relocation record for the 8-byte pointer slots that
   * initializers write INTO the arena (string tables, function tables,
   * `&global`). The arena is a fresh allocation on every load, so an
   * absolute address baked into it at compile time is as stale as one
   * baked into code — CCLoad re-derives these from (base, delta) too. */
  I64	nafx;
  I64	afx_pos[CC_FIXUPS];	//arena byte offset of the slot
  I64	afx_kind[CC_FIXUPS];	//CC_B_* base
  I64	afx_delta[CC_FIXUPS];	//byte offset from that base
  /* M13: object-like macros (name -> text) filled by the prep pass,
   * and typedef bindings (name -> TY_* base) filled by the parser.
   * Both reset with CCInit and live only for the source translation. */
  I64	mn;
  U8	mname[CC_MAX_MACROS][CC_TOK_MAX];
  U8	mbody[CC_MAX_MACROS][CC_MACRO_BODY];
  /* M31: function-like macros. mnarg[mi] is -1 for an object-like macro and
   * the number of NAMED parameters for a function-like one (0 = takes `()` and
   * nothing else); mvar[mi] says the parameter list ended with `...`, which is
   * what makes `__VA_ARGS__` an operand. mprm[mi] holds the parameter names
   * separated by newlines (a flat buffer like the member-name table rather than
   * a third array dimension: 32*8*128 bytes would triple the macro table for a
   * list that never holds more than eight short names). The `...` itself is not
   * stored as a name, so nothing in here can match `__VA_ARGS__`. None of this
   * travels into a saved object: macros are source-level and CCPrep resets the
   * table at the start of every compile. */
  U8	mprm[CC_MAX_MACROS][CC_MACRO_BODY];
  I64	mnarg[CC_MAX_MACROS];
  I64	mvar[CC_MAX_MACROS];
  I64	tn;
  U8	tname[CC_MAX_TYPEDEF][CC_TOK_MAX];
  I64	tty[CC_MAX_TYPEDEF];
  /* M20: enumerator constants (flat name->value table: enumerator names
   * must be TU-unique, mirroring C's shared ordinary-identifier
   * namespace; a variable of the same name WINS via CCSymFind checks)
   * and enum tags (fold to the int model, so a tag only exists to
   * catch redefinition). Both reset with CCInit. */
  I64	ncnst;
  U8	cname[CC_MAX_CONSTS][CC_TOK_MAX];
  I64	cvalue[CC_MAX_CONSTS];
  I64	netag;
  U8	etag[CC_MAX_ENUMTAGS][CC_TOK_MAX];
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
extern U0 CCArenaPtr(CCCtrl *cc,I64 off,I64 kind,I64 addr);
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
