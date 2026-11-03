# CC — an ANSI C compiler for 3Days

A C compiler written in HolyC that runs inside 3Days/TempleOS and
compiles C to x86-64 machine code in executable memory (`MAlloc` is
RWX), then executes it through a zero-arg HolyC function pointer.
Nothing here needs an `HCRT.BIN` rebuild: everything in `T/CC/` is
read at runtime.

## Usage

```
#include "T:/CC/CC.HC"
"rc=%d\n",CCRunFile("T:/CC/TEST/ret42.c");   -> 42
Exit;
```

Run from the repo root:
`timeout 120 stdbuf -oL ./3d_loader -t T -c T/CC/CC_TEST.HC`

Calling a named function, and passing argv:

```
U8 *av[3];
I64 (*fp)();
CCCompile(&cc,CCSrc("prog.c"));
fp=CCFind("helper");           //entry thunk, zero-arg call
"rc=%d\n",fp();
"v=%d\n",CCFindCall3("add3",2,5,7);      //arg-taking C, reliable path
av[0]="prog"; av[1]="data"; av[2]=NULL;
"rc=%d\n",CCRunArgv("prog.c",2,av);   //main(int argc,char *argv[])
```

Dumping a compiled module to a file and running it later (no compiler
pass, no source needed):

```
CCCompile(&cc,CCSrc("prog.c"));
CCSave("prog.cco");                  //BEFORE running: saves the
                                     //compile-time data snapshot
"rc=%d\n",CCRunObjFile("prog.cco");  //relocate + call main
"rc=%d\n",CCRunObj("prog.cco",2,av); //same object, new argv
```

## C subset limitations

Supported: `int`/`char` scalars, pointers, arrays (M17: up to two
dimensions, row-major), structs (tag definitions, `.`/`->`, struct-pointer params,
self-referential links — see M9), unions (M19: one-slot aliased tags,
by-value single-slot members, arrays and array-row params), string and
char literals (M19: `\n \t \r \\ \" \a \b \f \v \e \xHH \ooo`),
enums and compile-time constants (M20: enumerators fold into case labels,
array sizes, global initializers, and every expression position; adjacent
string literals concatenate), block scopes with C shadowing and
`static` block objects on arena storage (M21), aggregate initializers
(`int a[4]={1,2,3,4};`, `int a[]`/`int m[][3]`/`char s[]` size deduction,
`char s[]="text"` byte init) (M22),
`for`/`while`/`if`/`else`/`break`/`continue`/`switch`/`case`/`default`/
`do-while` (M14), `goto`/labels and the conditional `?:` (M16),
functions (forward refs,
recursion, and M13 prototype declarations `int f(int a);` — undefined
at link time is a compile error), function pointers (M18: declared in
any position, assigned from a name or `&name`, called directly or
through `(*fp)`/`(fp)`/member/typedef/param forms), `printf` (literal format:
`%d %s %c %%`), `putchar`, `exit`, and the
M12 stdlib subset — `strlen strcmp strcpy strchr memmove memcpy memset
memcmp malloc atoi abs puts srand rand` (delegated to kernel routines
where they exist; see M12), the M12.1 batch — `strcat strncat strrchr
strstr strncmp strncpy memchr strdup calloc` + `sprintf getchar fgetc
getc fputc putc fputs abort` (see M12.1) — the usual operators —
`++ -- & * []`, comma, short-circuit logic, bitwise `& | ^`, shifts
`<< >>`, `~`, all ten compound assignments (`p+=n` scales), chained
`a=b=5` — with C's full precedence chain (shift vs relational vs the
three bitwise levels are pinned by `bitops.c`). M15 adds `sizeof`
(C-truth sizes — see M15) and C casts (`(char)` truncates, `(char*)`/
`(int*)` retype pointers, `(struct T *)` keeps the tag for `->`).
M13 adds the type
keywords `const` (no-op qualifier), `unsigned`/`long`/`signed`/`short`
(fold to the single 64-bit int — no width distinction; note C-truth
`sizeof(short)` is 2, ours is 8), `void` (folds to the
byte model: `void *` is a byte pointer like memcpy's contract;
`f(void)` is the zero-arg idiom; anonymous parameters and `...` in
prototypes parse), `typedef` for scalar/pointer aliases (`typedef
unsigned long size_t;`), a line-oriented preprocessor-lite, and the
adapted-musl (MIT) headers in `T:/CC/INC/`: stdio.h, stdlib.h,
string.h, stddef.h (guarded, include each other by plain name).

Not supported — each currently fails with a compile diagnostic:

- Multi-declarator globals must not hide a function: `int a,b();` is
  an error — give functions their own declaration. (`int i,s=0;`,
  `int *p,*q;` and `int a=1,b=2;` all work, C's per-declarator star
  typing included.)
- Initializer edges (M22): every element must be a **foldable constant**
  — C permits variable elements in a FRAME array (`int a[2]={x,y};`),
  this model diagnoses "not a compile-time constant" there and requires
  constants everywhere else anyway. Brace groups follow C's element
  semantics (a group targets the NEXT element), so `{{1,2,3},{4,5,6}}`
  and the flat `1,2,3,4,5,6` agree for a `[2][3]`, while `{{1},{2,3},{4}}`
  on a 1-D array is excess elements — C warns and drops, we ERROR. Braces
  nested deeper than the array's rank diagnose for the same reason. A
  512-value cap (CC_INIT_MAX) bounds one initializer. Designators
  (`[0]=x`, `.x=x`) are not supported; struct arrays and function-pointer
  arrays keep their existing rejections; a global/static array whose
  elements need a CODE address (a function table) cannot initialize yet —
  arena values are saved as raw bytes and only code sites carry
  relocations. `extern int a[];` (an incomplete array declared extern) is
  "array size missing". Braced SCALARS are accepted (`int x={5};`).
- `else if` chains parse; nested `switch` works (M14 — but nested
  switches exposed the sharing bug: per-statement parse state must be
  CALL-LOCAL, the ctrl-array version let an inner switch overwrite its
  parent's case table mid-body).
- `switch` case values must be compile-time integer constants — M20's
  folder handles literals, enum constants and folded expressions;
  duplicate case values are not diagnosed;
  `continue` inside a `switch` requires an enclosing loop (clean
  diagnostic — switch is not a loop).
- M16 edges: `goto` into a block (crossing a declaration) is accepted
  where C forbids the jump — the slot exists in the frame either way;
  `goto case 2;` (jumping onto a case label) is "undefined label"
  since case labels are switch-scoped parse state, not statement
  labels; a ternary's middle arm tolerates unparenthesized commas
  (C requires parens — we eat them as the operator, harmless); mixed-
  type arms fold to the int model, identical-kind arms keep element
  types and struct tags so `(c?&p1:&p2)->x` works.
- `float`/`double`.
- M19 edges: qualifier keywords (`extern static register auto volatile
  inline`, added to `const`) are accepted and folded — with C's
  default-int PROMISE when they stand alone (`static x;` is `static
  int x`), which a real base overwrites; consequences: `extern` globals
  get their own arena slot (no cross-module link), and `signed char` is indistinguishable from `char` (the byte
  model ZERO-EXTENDS: `sb=-3; sb==253` is TRUE here, C-truth -3 — the
  deviation the unionc fixture pins). Struct/union TAGS share one
  namespace (`struct S` and `union S` collide; C keeps them separate).
  By-value members are allowed only for ONE-SLOT tags (any union, any
  single-member struct — the member IS a slot); multi-slot struct
  members stay pointer-only (C-by-value copy needs member memcpy).
  Union assignment (`a=b` between union objects) is not supported;
  `sizeof` a string LITERAL answers the pointer size, not the byte
  count (the silent expression route cannot see pool lengths); a tag
  still mid-definition has size 0, so C's illegal by-value self-member
  rejects itself. `void` combinations follow C once the qualifier
  promise is discounted (`const void *v` folds to the byte-pointer
  model like every void*).
- Function-pointer edges (M18): arity is NOT checked — `int (*f)(int)`
  called with the wrong arg count against a mismatched callee is as
  undefined as the C it mirrors (mismatched types at assignment are
  likewise silent — the declarator swallows params wholesale); arrays
  of function pointers are rejected with a clean diagnostic (one
  `(*f)` per variable); calling a HOLYC-JIT function through a C fp is
  unsupported (Callees must be CC-compiled: plain `ret` + caller-side
  pops — HolyC's epilogue differs); `sizeof(*fp)` reports 8 where C
  rejects a function type; a function address used before its
  DEFINITION resolves at definition time (prototype required first,
  like any C use); global fp initializers stay in the constant-only
  club — assign in a function body; a stored fp is a raw code address:
  NULL is the only portable sentinel (`fp==0` / `fp!=0` work).
- Scoping (M21): blocks, `for`-init clauses, switch bodies and function
  bodies are real C scopes — a waterline truncates the symbol, typedef,
  enum-constant and tag tables at the closing brace, so an inner name
  shadows the outer one (its own slot), out-of-scope names are
  undefined again, and a same-scope redeclaration diagnoses
  (`redeclaration in this scope`, duplicate PARAMETER names included,
  which C99 also rejects). `static` block objects get ARENA storage —
  counters survive calls, folded initializers, arrays, structs, fps and
  pointers all work — with block-scoped NAMES: independent twins per
  function, and a static may shadow a global of the same name.
  Remaining scope deviations: a block-scope `extern int x;` becomes an
  ordinary local (it does not link to a file-scope x); `static`
  function-pointer initializers are refused outright (an initializer
  there is an EMITTED store, which would re-assign the target on every
  call where C initializes exactly once); file-scope tentative double
  definitions (`int g; int g;`) still claim two arena slots where C
  merges one.
- Enum edges (M20): `enum x;` consumes `x` as a TAG (C's incomplete-type
  object declaration needs a separate declarator); enumerators cap at 64
  per module (CC_MAX_CONSTS), tags at 16; the constants table is flat
  file-scope like C's shared namespace, but a global later claiming an
  enumerator's name SHADOWS rather than diagnoses; `case (0 && (1/0))`
  errors where C short-circuits (the folder evaluates both sides — it
  refuses constant div/mod by zero loudly instead of faulting the host
  JIT); enum constants do not fold in `#if` (prep arithmetic predates
  the tables); `sizeof(enum E)` and `sizeof(ENUM_CONST)` are the quad
  model. Enum tags live in their own table (`struct S` and `enum S` do
  not collide — C-faithful here, unlike struct-vs-union which share).
- Array edges (M17): THREE dimensions are rejected (symbol carries
  dim/dim2 only); `sizeof(m[i])` on a row goes through the expression
  path and reports the POINTER size (C's row size needs the bare
  `sizeof m` / div by dim, or a dims-carrying symbol the expr path
  does not read); elements of pointer-arrays (`char *ap[2]`) are
  int-shaped — `ap[1][0]` loads the pointer then indexes the QUAD at
  it, so byte access takes the temp-variable form (`p=ap[1]; p[0]`)
  or a cast (`*(char *)(ap[1])`) — the model has one pointer kind;
  postfix on a string LITERAL (`"ab"[0]`) parses as an error (predates
  M17 — Primary's literal arm never enters Postfix; use a variable);
  `++`/`--` on a POINTER-TO-ROW steps by element size, not row size;
  array PARAMETERS discard the first dimension (the caller owns the
  count) and keep the second; struct members may not be arrays.
- `sizeof`/cast edges not supported: `sizeof` needs parentheses (and
  its expression form decays arrays to pointer size there — `sizeof
  f(x)` and a bare `sizeof x` are diagnostics, and the bare-identifier
  form `sizeof b` is the only route reporting array element counts);
  `sizeof(...)` inside a global initializer (or any constant-expression
  site: case label, array size, `static` init) is "not a compile-time
  constant" — M20's folder folds literals, enum constants and arithmetic,
  but not `sizeof` or address values; `(struct T)` non-pointer casts
  yield the address as an int; `sizeof(function)` is a diagnostic.
- Preprocessor pieces that lite skips: function-like `#define F(a)`
  (expands as a bare object macro and the leftover text fails to
  parse — loudly, by design), `#elif`/`#error`/`#line`, expressions
  in `#if` (only literal `0`/`1`), `defined(X)`, includes relative to
  the includer's directory (plain names resolve ONLY against
  `T:/CC/INC/`; drive-qualified or leading-`/` paths are opened as
  written), line-continuation backslashes, and `#if` interacting
  inside block comments (prep is line-shaped; keep conditionals and
  block comments disjoint, as real code does).
- Structs: no by-value passing or return, no struct/array or by-value
  struct members (only `struct T *m` pointers), no initializers,
  `v = w;` copies unsupported, tags must be defined before any use
  (no forward tags), one member may not nest a struct by value.
- Postfix `++`/`--` needs an lvalue primary: `v++` and `buf[i]++` are
  fine, `(*p)++` needs parens support the postfix pass lacks (write
  `*p=*p+1`).
- `free()` does not exist: this port's `Free()` releases whole poopalloc
  chunks and would pull shared pages out from under live arenas, so
  `malloc()` memory is permanent-by-design (see the CCSrc note in CC.HC).
- Builtin names bind BUILTINS: `strlen memcpy memset memcmp malloc
  getchar` (like `printf`/`putchar`/`exit`) are dispatched by name
  before the symbol table, so a user function of the same name is
  shadowed. The pure prelude half (`strcmp strcpy strcat strncat
  strrchr strstr strncmp strncpy strchr memmove memchr strdup calloc
  atoi abs puts fputs fputc putc srand rand sprintf-engine` +
  `CCItoa`) are ordinary symbols BUT pre-reserved: redefining one is
  a compile error (`function redefined` — verified).
- `getchar()`/`fgetc`/`getc` return a HolyC KEYBOARD EVENT (ASCII in
  bits 0..7 plus SCF_* modifier flags up top) and BLOCK until a key
  arrives — headless `-c` has no console input, so the suite gates the
  call shape compile+save only (`getcharc.c`). C byte semantics on a
  console are documented deviation, not emulation. `fputc/putc/fputs`
  accept a stream argument and ignore it: `cc->out` IS the stdio
  buffer (see M12). `abort()` exits the program with 134 from any
  depth (see the M12.1 nested-return landmine).
- `sprintf` interprets its format AT RUNTIME (the format may be a
  variable — the point of the engine): `%d %s %c %x %%`, at most 4
  conversion arguments (extra user args are a diagnostic; missing ones
  read as 0), no width/precision flags, `%x` prints the magnitude of
  a signed value, and the return is the written-character count —
  never pointer subtraction (arena pointers are relocated per session;
  subtraction across a saved-object reload is a different arena).
  `snprintf` deferred: per-write bound guards would bloat every
  conversion site; the engine writes unguarded into the caller's
  buffer (caller's responsibility, same as C UB territory).
- `typedef` binds a name to a base type ONLY in type position, and an
  existing symbol (variable, function) shadows the typedef there, like
  C's ordinary-namespace rule. Struct typedefs are not implemented
  (the tag table is name-keyed and single-sourced at file scope).
  Declaring a header prototype for a builtin (`strlen`) or prelude
  name (`puts`) is inert and legal: builtins dispatch before the
  symbol table, and a prototype after a definition is a no-op merge.
  `EOF`/`NULL`/`EXIT_*` arrive as object macros from the headers.
- Arg-taking C from an fp variable (`fp=CCFind("f"); fp(a,b,c)`) returns
  the right value but the HolyC JIT's stack tracking around multi-arg
  indirect calls desyncs in most statement shapes and breaks *later*
  code; use `CCFindCall1..4(name,...)` (cc->ext staging, see M11) or the
  wrapper-returns-immediately shape. Zero-arg thunks and main-entry
  calls (`CCRunFile`/`CCRunArgv`/`CCRunObj`) are unaffected.

## Layout

| File        | Contents                                        |
|-------------|-------------------------------------------------|
| `CC.HH`     | types, token/opcode constants, `CCCtrl`          |
| `CC_LEX.HC` | lexer (also where `CCError` lives)               |
| `CC_EMIT.HC`| machine-code emitters, prologue/return, patches  |
| `CC_SYM.HC` | symbol table, variable load/store                |
| `CC_PARSE.HC`| recursive-descent parser, direct emission      |
| `CC.HC`     | umbrella include, `CCInit`/`CCSrc`/`CCRunFile`, `CCSave`/`CCLoad` |
| `CC_PREP.HC`| M13 preprocessor-lite (`#include`/`#define`/conditionals), runs on user text before the prelude is prepended |
| `CC_LIB.HC` | M12 zero-arg delegation helpers (spliced into `CC.HC` after `CCCtrl cc;`) |
| `INC/*.h`   | M13 adapted-musl (MIT) headers: `stdio.h` `stdlib.h` `string.h` `stddef.h` — guarded, trimmed to the implemented surface |
| `CC_TEST.HC`| test harness (PASS/FAIL per file in `TEST/`)     |

## Milestones

- [x] **M1** int literals, `return`, `+ - * / %`, unary `-`, parens.
- [x] **M2** entry stub: private C stack (a HolyC fp-call enters with
  RSP at the mapped stack bottom; PUSH before switching RSP faults).
- [x] **M3** `if`/`else`, `while`, `break`, local `int` declarations,
  assignment (incl. inside expressions), `== != < <= > >= !`,
  short-circuit `&& ||`, postfix `++ --`, global variable arena.
- [x] **M4** multiple functions + calls: top-level parse loop (globals
  with constant initializers too); entry stub (RSP switch) only on
  `main`, internal functions get a plain frame; own convention: args
  pushed left-to-right, callee reads `[RBP+16+8*(argc-1-i)]`, caller
  pops `ADD RSP,8n`; forward calls queue on the symbol and patch at
  definition; recursion + mutual recursion verified (`fact`, `fib`,
  `mutual`, `nargs`, `glob`, `globinit`, `param`).
- [x] **M5** `for` (init-declaration, empty cond/step), comma operator,
  unary `&`/`*`, string literals into the arena pool, `int *`/`char *`
  params and locals, local/global arrays with indexing, `++`/`--` and
  `+`/`-` scaled by element size (8 / 1). Pending-lvalue model:
  expressions leave RAX = address with `cc->is_addr` (1=lvalue,
  2=decayed array name) and `CCVal()` materializes the load at value
  positions (`addr`, `deref`, `arr`, `parr`, `str`, `strlen`, `parg`,
  `comma`, `char`, `globstr`, `pinc`, `forloop`, `fordec`, `forcont`).
  The for-step clause is LOCATED by a lexer scan (current position
  before re-lex) and re-parsed after the body so its code — and every
  jump/call patch inside it — lands at its final offsets; single-pass
  emission never rewinds `ep`.
- [x] **M6a** kernel-native I/O: raw `SYSCALL` from JIT'd code was
  rejected by probe — `write(2)` reaches Linux but deterministically
  faults the HolyC task on return (`UserTaskCont`, `T:/KTASK.HC:326`)
  even with RCX/R11 saved. Instead `putchar(c)` emits code appending
  AL to `cc->out` (bytes from 8; LE count at 0..7, counter masked with
  `CC_OUT_SIZE-1`), `exit(n)` does an entry-style return RAX=n, and the
  HolyC harness prints the buffer with `"%s"` — the kernel Print path,
  blue screen and CLI alike. JIT'd code performs no OS interaction.
  Verified by `hello.c` ("hi!") and `exit.c` (nested-recursive
  putchar + `exit(7)`), both in the suite.
- [x] **M6b** `printf` subset: compile-time lowering of a literal
  format string (`%d %s %c %%`, up to 8 args) into calls into a tiny
  C runtime (`CCOutByte/CCStr/CCItoa`) that CC *prepends to every
  module as C source and compiles itself* (CC_PRELUDE in CC_EMIT.HC) —
  symbols exist before user code, so no host code blob and no
  HolyC-calling-convention reversal needed (that reversal is now only
  required for *streaming* writes, kept as M6c). Verified with
  `printf`/`pfmt2` incl. negatives and 4-digit values.
  Prelude param names are prefixed `cc_` and `CCSymFind` scans
  LAST-to-FIRST (innermost declaration wins) so prelude symbols can
  never shadow user variables — the flat table otherwise leaked `s/v/c`
  into user scope and corrupted `str.c`/`nargs`.
- [x] **M7** named entry lookup + argc/argv bridge.
  `CCFind(name)` appends an *entry thunk* to the compiled module (the
  same private-stack switch `main` gets, then `CALL body; RET` with the
  caller's RSP restored) and returns its address — HolyC can call any
  defined zero-arg C function: `fp=CCFind("addup"); fp();`. Works
  because the module's internal `CALL rel32`s were already patched at
  final offsets; the thunk just lands after them (`find.c` exercises a
  thunk whose body calls another function). Arg-taking lookups are
  blocked on reversing HolyC's own fp-call convention (same dependency
  as M6c). `main(int argc,char *argv[])`/`char **argv`: params
  declared with `[]` or a second `*` become slot pointers; the entry
  prologue, when `main` has parameters, loads them from new `cc->argc`
  / `cc->argv` fields straight into the parameter slots, so the body
  sees ordinary params. Driver: `CCRunArgv(fn,argc,argv)` (fields set
  AFTER `CCCompile` since `CCInit` zeroes the block); `CCRunFile`
  defaults to `argc=1, argv={"cc",NULL}`. Verified: `find` (thunk →
  23 via an internal call) and `argv` (prints + returns 2).
- [x] **M7b** multi-declarator statements, locals and globals:
  `int i,s=0;`, `int *p,*q;`, `int a=1,b=2;` all work, with C's
  per-declarator star typing (`int *a,b;` leaves b an int). Found and
  fixed a latent rewind bug while testing it (see the lexer-state
  gotcha): for-loops were swallowing the first token after their body.
- [x] **M8** operator & literal completion: char literals (`'a'`,
  escapes `\n \t \r \0 \\ \'` — lexed straight to TK_NUM); compound
  assignment `+= -= *= /= %= &= |= ^= <<= >>=` (address PUSHed below
  the old value, so it survives CALLs in the RHS; `p+=n` scales by the
  element size; byte lhs loads with MOVZX, stores AL, result in RAX);
  binary `& | ^ << >>` and unary `~`, inserted as real precedence
  levels — shift sits between relational and additive, `&`/`^`/`|`
  between equality and `&&` (bitops.c checks `1|2&4`, `3==3&1==1`,
  `1^2<<1` to pin it). `>>` is SAR (signed model).
  Fixed a latent typing bug found on the way: `CCAssignTail` gave every
  assign-expression pointer type, so `(a=5)+1` scaled the 1 by 8; the
  result now takes the LHS type (assexpr.c would fail without the fix).
  The byte-compound fault then exposed the `else` one-statement dialect
  trap (see gotchas); auditing it found the SAME double-load latent in
  the pre-existing postfix `++` byte path — `buf[i]++` on chars would
  have faulted, never tested before; `chpost.c` pins both.
- [x] **M9** structs. `struct Tag { ... };` at file scope registers a
  tag (members are one 8-byte SLOT each, byte semantics kept for char
  members via their TY). `int|char [*] m;` and `struct T *m;` members —
  the tag is registered BEFORE its `{`, so self-referential
  `struct Node { struct Node *next; }` linked lists chain (`n->next->v`
  verified). Declarations `struct S v;`/`struct S *p;`/`struct S vs[n];`
  work as locals and globals (locals allocate size/8 slots, base at the
  top of the block; globals carve the arena). Struct names DECAY to
  their address like arrays (`v.a`, `vs[i].a`, `&v`, `p->a`, `f(&v)`).
  `->` requires a struct pointer (TY_PTR + tag); indexing a struct or
  struct pointer scales by the tag's byte size (generic `IMUL imm32`).
  Function params by pointer only: `struct S *p` (and `struct S ps[]`).
  The tag id rides beside `ety` as `cc->sty`, carefully cleared/set on
  every expression path (BinEmit/compare/logic/call => -1; `&struct` and
  `*structptr` keep it). Verified: strbasic (locals/globals/arrays/
  `&v==p`), strchar (char + char* members, printf %s/%c of members,
  array-sum over a pointer param), strlink (linked-list walk + chained
  `a.next->v`).
- [x] **M10** object dump/reload (`.o`-style, custom format). Every
  absolute address that leaves the code buffer is emitted via
  `CCQuadAbs(cc,kind,addr)`, which writes the 8 bytes AND records
  (code pos, base kind, delta-from-base); bases are the five external
  arenas/fields (locals+strings, private stack, RSP-save slot, out
  buffer, the cc block's argc/argv). Everything else — rel32 calls and
  jumps, RBP-relative locals — is self-relative and needs nothing.
  `CCSave(fn)` serializes: header (magic/version/entry_off/code_len/
  arena_len/nfix) + relocation triples + code bytes + the *initial*
  data arena (global initializers and the string pool are written to
  the arena at compile time, so the dump is the `.data` snapshot —
  save BEFORE running). `CCLoad(fn)` validates sizes/relocations,
  `CCInit`s fresh arenas, copies, and rewrites each site as
  `newBase+delta`; `CCRunObj`/`CCRunObjFile` then call main. Loaded
  modules run main() only — the symbol table is not part of the
  object, so `CCFind` needs the source recompiled. The suite saves +
  reloads objdemo (all five bases; a *second* reload with argc=2
  proves the `&cc->argc` injection rebases, 307) and strbasic (a
  struct module). Boot image has no FileDelete, so the test object
  lands at the fixed scratch path `T:/CC/TEST.OBJ`, overwritten each
  run.
- [x] **M11** arg-taking C callable from HolyC. `CCFind(name)` now emits
  arg-taking thunks too: the thunk copies the caller's pushed fp-call args
  down into the body's parameter slots (left to right, matching the
  internal convention) and returns the value — verified correct for
  arity 1..4. But *activating* `f3(2,5,7)` through an fp variable
  (`IC_CALL_INDIRECT`) proved unreliable in this boot image's JIT: it
  defers the caller-side arg pops (IC_ADD_RSP1/IC_ADD_RSP) to the
  enclosing block/function end, and in most statement shapes the tracking
  desyncs — later statements in the same function, or statements after a
  top-level fp call, fault or hang (rip = a pushed arg value, or endless
  `UNKOWN[...]` frames from the AOT backtrace printer cycling on a
  corrupted chain). Confounders that made this hard to pin down, all
  verified: (a) `RET imm16 0` consumes a *second* quad (native zero-arg
  callees end in plain `C3`), so zero-arg thunks must stay plain-RET;
  (b) a hand-built probe `CALL rel32=0` landing on the immediately
  following `RET` is a self-trampoline whose second pop eats a real arg;
  (c) probe scripts that end with `Exit;` *inside* the wrapper exit the
  task before the return path runs, so "clean exit" never proved a
  "clean return". A multi-arg fp call to a *native HolyC* callee (fp
  value assigned at runtime so no devirtualization, statements after the
  call, function returns through the POP chain) runs clean in exactly
  the contexts our thunk derails — the desync is a thunk/JIT mismatch
  that no epilogue variant fixes (plain RET, RET imm16 8*argc via
  `cc_find_argpop`, and +8/+16 via `cc_find_popextra` all shift the
  window; the native-style `RET imm16` alone matched native bytes but
  not the JIT's mid-window expectations of our private-stack stubs).
  The reliable API stages arguments through new `cc->ext[4]` slots and
  calls a per-symbol cached **zero-arg** stub (`CCExtStub`: the same
  private-stack entry, `MOVABS RCX,&cc->ext` + PUSH-left-to-right into
  the body's param slots — the M4 argc/argv bridge generalized):
  `I64 CCFindCall1..4(name,a0..a3)`. Every HolyC activation involved is
  direct or zero-arg indirect — the two forms verified clean in every
  context including top-level statements. `CCFind` still returns the
  exact-signature thunk (values correct); its safe calling shape —
  pinned by the `findstub` suite case — is a dedicated wrapper function
  whose last statement returns the call value. New suite cases:
  findargs (Call3+Call1 over one module), findstub (raw thunk, wrapper
  shape), tlcall (Call3 from a top-level statement): 54 cases, green
  twice in fresh processes, clean `CC TESTS: 0 failed` summary.
- [x] **M6c** streaming output — delegated, not reimplemented. The
  `putchar` codegen (the single funnel every `printf`/`putchar` write
  flows through) gained two flush paths: after storing a '\n'
  (C line-buffered semantics) and before storing when the buffer is
  within 64 bytes of `CC_OUT_SIZE` (bounded index; the old AND-wrap
  silently dropped everything past 64K). A flush is
  `PUSH RAX; MOVABS RCX,hflush; CALL RCX; POP RAX` — a zero-argument
  call (the activation verified safe in every context, and the only
  kind a HolyC-compiled callee handles cleanly from generated code;
  `cc_find_popextra`/`RET imm16` variants proved nothing pops extra
  for arity 0). `hflush` is the 6th relocation base (`CC_B_HFLUSH`):
  `CCCompile` binds it to `&CCFlush`, `CCLoad` rebinds it so saved
  objects stream in the loading session. `CCFlush` (HolyC, CC.HC)
  does what the kernel does for HolyC programs — terminate the span,
  `"%s"` print, zero the count — so *all* console I/O, formatting
  delegation aside, is the kernel's own Print path; CC implements no
  OS interface in generated code. RAX is saved around the call
  because C `putchar(c)` returns `c` and the helper clobbers it;
  every other generated-code live value is RBP-anchored and the
  flush clobbers nothing RBP-relative. New cases `stream` (200 lines,
  every '\n' flushes: 200 lines observed arriving live) and
  `streambig` (96KB no-newline output crosses the guard once; every
  byte + markers arrive; buffer empties at exit — old code would have
  lost 30KB); both assert residual count 0 at main's return (the
  mid-program flush paths are what produce it): 56
  cases, green twice, clean summary. Delegation note: `%d` digit
  conversion still runs in compiled-C (CCItoa); routing conversions
  through kernel `Prnt` via `cc->ext` staging is a possible further
  delegation, deliberately not done here (C's format-flag semantics
  would have to survive the mapping).
 - [x] **M12** stdlib, delegated to the kernel wherever it exists.
   Two halves. **Delegated** (parser name-dispatch in `CCExprPrimary`
   -> `CCLibCall`): `strlen`->`StrLen`, `memcpy`->`MemCpy`, `memset`->
   `MemSet`, `memcmp`->`MemCmp`, `malloc`->`MAlloc` (no `free` - see
   limitations). Args are evaluated left-to-right and staged into
   `cc->ext[]` (the M11 pattern), then `MOVABS RAX,&cc->lib[slot];
   CALL [RAX]` invokes a zero-arg HolyC helper (CC_LIB.HC) which
   reads `cc->ext`, calls the kernel routine, and returns the value
   in RAX - zero-arg indirect activation, the form M11/M6c proved.
   `CC_B_LIB` is the 7th relocation base: its quad names a TABLE
   SLOT, so the code LOADS the helper address at call time - a
   reloaded object (or `CCLoad`) picks up the loading session's
   helpers with zero object-format involvement (`CCSetLib` rebinds;
   `objstdlib` proves the round trip). **Pure prelude C** (kernel has
   no equivalent or the semantics are C-specific): `strcmp`, `strcpy`,
   `strchr`, `memmove`, `atoi`, `abs`, `puts`, `srand`/`rand`, spliced
   as C source after CCItoa like CCOutByte/CCStr. `FILE *` = not yet;
   stdout/stderr/stdin are implicit (`cc->out` IS the stdio buffer).
   **The RDI landmine** - found the hard way through a run-to-run
   nondeterministic `memcmp` (result 0 some boots, -1 others): every
   JIT-compiled HolyC callee's frame-zeroing prologue is
   `MOV RAX,RDI; MOV RCX,n; MOV AL,0; REP STOSQ` - it WRITES 8*RCX
   bytes at the address ARRIVING IN RDI (CCFlush n=16 -> 128 bytes;
   verified from live JIT bytes). The HolyC-to-HolyC ABI supplies a
   scratch pointer there; generated C code has no such convention,
   so any CALL into JIT-compiled HolyC must first `MOVABS RDI` a
   dedicated scratch (`cc->libscr`, 512B, `CCSetScratch` - flush and
   libcall sites both do it; without it the callee zeroed whatever
   pointer happened to be in RDI - usually the string pool: literals
   wiped to equal, hence the "equal" memcmp). Two kernel-ABI catches:
   asm `MemCpy`/`MemSet` RETURN dst+cnt (the trail pointer), not the
   C-mandated dst - helpers discard and return the staged dst
   (`v16`/`v17` pinned the difference); and a HolyC function address
   must be captured with `&name` - bare `f=name;` CALLs it and stores
   the RESULT. Host-side: dbg.c now installs its fault handler for
   SIGILL too (an illegal instruction in JIT'd/generated code lands
   in the HolyC debugger with a backtrace instead of killing the
   process - it caught exactly this class of bug this session):
   58 cases (stdlib 29 checks incl. return contracts + objstdlib),
   green twice, 8/8 in-process repeated-run determinism, clean
   `CC TESTS: 0 failed`.
 - [x] **M12.1** completion batch. Builtins: `getchar()` -> slot 5
   `CCXGetChar` -> kernel `GetKey()` (blocks headless — gated
   compile+save, see limitations); `abort()` -> `MOV RAX,134` + the
   exit-shaped return (below); `sprintf` -> parser pads user args to
   the fixed 6-quad frame of the prelude engine `cc_sprintf`, which
   parses the format AT RUNTIME (literal AND variable formats both
   work; `%d %s %c %x %%`; `%%` via numeric literal 37 — the prelude
   lexer gets no percent escape). Prelude C additions: strcat strncat
   strrchr strstr strncmp strncpy memchr strdup calloc (MAlloc does
   not zero-fill so calloc loops explicitly) + fputc/putc/fputs/
   fgetc/getc thin stream-token-ignoring wrappers. `strstr` must NOT
   signal a mismatch with a `needle=0` sentinel: the loop test then
   dereferences ADDRESS ZERO — host page 0 is unmapped (this exact
   bug produced `FAULT sig=11 addr=(nil)`; restructured to a match
   flag + walking to the needle's own NUL). **The nested-return
   landmine** — `exit`/`abort` below `main` has TWO traps, both found
   via the abort-in-helper fixture. (a) The HolyC caller rebuilds its
   frame from RBP, so a return-to-harness jump from a NESTED C
   function must hand back a HOST-VALID RBP: helper's `LEAVE` restores
   MAIN's rbp, a code-stack address; the HolyC epilogue then rets
   through garbage and jumps to address 0 (`rip=(nil)`; the debugger
   frames stop at ???). (b) The obvious fix — `POP RBP` after
   `MOV RSP,[cc->save]` — is WRONG: saved RSP points AT the return
   address, so the popped quad is the return address itself and the
   following `RET` jumps to the leftover entry-address quad — main
   re-executes forever (silent hang, no banner: the loop thrashes
   without faulting, and block-buffered stdout hides the output of
   the killed process). Correct shape: the entry prologue does `PUSH
   RBP` IMMEDIATELY AFTER saving caller RSP (host quad at
   `[cc->save]-8`), and every exit-shaped return (`CCFunRet` entry
   branch — return-from-entry, `exit`, `abort`) restores RSP then
   LOADS `RBP,[RSP-8]` before `RET` — loads, never pops. `abA`/`abB`/
   `abD`/`abE` bisected the two shapes; M11's `CCFindCallN` thunks
   LEAVE through their own frames and never take this branch, which
   is why 60 cases passed before the fixture demanded it. Fixture
   discipline lesson (third time): expected `strstr("foobarz","zz")`,
   the `strncmp` difference direction, and `sprintf("%d!",7)` LENGTH
   were hand-written WRONG (2 chars, not 3) — compute expectations
   independently; the compiler was right all three times.
   61 cases (stdlib 52 + sprintf 12 + abort 134 + getchar-shape),
   green twice, in-process determinism incl. three reloads of the
   saved stdlib object, clean `CC TESTS: 0 failed`.
 - [x] **M13** preprocessor-lite + type keywords + adapted-musl headers.
   `CCPrep` runs on the USER text before the prep-free prelude is
   prepended; it is a line-shaped scanner (never CCLex): literals are
   opaque (macro names inside them don't expand), comments copy raw
   for the real lexer, skipped regions emit one newline per dropped
   line so source shape survives. Directives: `#include` (`"x.h"`/
   `<x.h>` resolve against `T:/CC/INC/`; paths containing a drive or
   leading `/` open as written; depth-capped), object-like `#define`
   (bodies re-expand, depth-bounded rescans) / `#undef`, `#ifdef`/
   `#ifndef`/`#else`/`#endif` (three-bit frames: emit/chain-taken/
   parent-emit — an inner `#else` cannot resurrect an outer-skipped
   region), `#if 0`/`#if 1`, `#pragma`; anything else is a loud error.
   Tables (macros, typedefs) live in CCCtrl, are compile-time only,
   and never touch the object format — a header-using program
   save/reloads identically (`objhdr` probe: 16/16 across reloads).
   Keywords fold in ONE place (`CCAcceptBase`) feeding every type
   position (top level, locals, params, for-init, typedefs, function
   returns): const/signed no-op, unsigned/long/int fold to TY_INT
   (single 64-bit width), `void` → the byte (TY_CHAR) model so
   `void *` IS the memcpy-contract byte pointer, `typedef` binds
   scalar/pointer aliases with existing symbols shadowing them in
   type position. Prototypes (`int f(int a);`) register/merge SC_FUN
   at offset -1 (arity set; a definition wins; a prototype after a
   definition is inert; a never-defined forward is the
   `call to undefined function` compile error — all three probed);
   `f(void)`, anonymous params and `...` parse. Headers (musl, MIT,
   trimmed): stddef/stdio/stdlib/string — size_t, NULL, EOF, EXIT_*,
   `typedef long FILE` (stream tokens; see M12's stdio note).
   Landmines this milestone stepped on, all pinned by probes: (a) the prep
   literal copier ended regions with `n=0` instead of `break` —
   every `char *s="hi\n";` LOST THE TEXT AFTER ITS CLOSING QUOTE
   (the `;` died with it; 8 fixtures failed at once) — the cure for
   opaque-text stages: hexdump-prep-vs-source identity check, which
   showed 77→66 bytes before any of it made sense; (b) `#define`
   forgot to skip the space before the name; (c) scanning text lines
   from the '#' detection cursor stripped indentation (harmless but
   breaks byte-identity — scan from line start); (d) the `f(void)`
   zero-arg branch hijacked `void *dst` as memcpy's FIRST parameter
   (void handling must fork INSIDE the param loop: `)` follows →
   zero-arg, else it is just a base type); (e) `TY_VOID` needed its
   own code — TY_INT is 0, indistinguishable from "folded nothing",
   so CCAcceptBase signals "not a type" as -1; (f) a parenthesized
   boolean with a miscount killed the kernel parse with an error
   lines away from the cause — dispatch gates use staged ifs, not
   one-line boolean monsters; (g) `CIsAlpha`/`CIsDigit` are CC_LEX's
   OWN helpers, not kernel — bisect probes that include only CC.HH
   flake on them; (h) HolyC has no `continue` (see the gotcha above)
   — prep scan, CCAcceptBase and the param binding all had to be
   written as if/else chains. 63 cases (preproc 12 + hdr 16 on top
   of M12.1's 61, all green twice, in-process repeated runs + saved
   object round trip of the header program, clean `CC TESTS: 0
   failed`).
 - [x] **M14** control-flow completion: `switch`/`case`/`default` and
   `do-while`. switch codegen, single pass no rewind: value parked in
   a hidden local slot (`lofs++`, frame patched at function end), an
   entry JMP leaps the body and lands on a dispatch chain emitted
   AFTER it (targets known by then: per-case `MOV RAX,[slot]` +
   `CMP RAX,imm32` — or `MOVABS RCX,imm64; CMP RAX,RCX` past int32 —
   + `JE rel32` to the label), a final `JMP default` if one exists,
   and switch end after the chain (breaks land there via the usual
   frame-sliced brk list; the cont frame is deliberately untouched so
   `continue` inside a switch still targets an enclosing loop —
   `continue` in no loop at all is now a compile error via a new
   `in_loop` counter maintained by while/for/do). Fallthrough is C's.
   Four codegen landmines, each caught by a 6-line probe before the
   suite could name it: (a) a rel32 branch displacement must be
   computed AFTER emitting its opcode bytes — do-while's JNZ computed
   pre-opcode landed 2 bytes into the body (SIGILL on the misaligned
   decode); (b) the entry JMP must be patched at the CHAIN START
   (right after the body), not after the chain — patching at the end
   made every switch entry leap over its own dispatch, matching
   nothing; (c) the dispatch chain sits physically after the body, so
   a fall-off from the last case would re-run the chain and match its
   own case forever (`switch(1){case 1:a=5;}` looped until timeout) —
   a body-end `JMP switch_end` makes the chain entry-only code;
   (d) the case table must be per-invocation CALL-LOCAL: as ctrl
   arrays, an inner switch parsed mid-parent-body overwrote
   `cval[0..]` and the parent silently matched the wrong constants.
   64 cases (switch.c: 20 checks — grouped labels, fallthrough,
   break/continue/fall-out in a loopmix, char/negative/>int32 case
   values, default-before-cases, bare and braced do bodies,
   continue-to-condition, empty switch), green twice, saved-object
   round trip 20/20/20, clean `CC TESTS: 0 failed`.
 - [x] **M15** `sizeof` + casts. Sizes are C truth: char 1, struct the
   tag byte total, everything else the 8-byte quad. Four routes:
   `sizeof(type)` (speculative `CCTrySpec` scan — same fold table as
   CCAcceptBase but silent — +stars +dims); bare `sizeof(ident)`
   (exact symbol answer: THE route that sees array dims — and note
   array symbols are stored POINTER-TYPED from the declaration,
   char[128] is (TY_CHARP, dim 128), so element size must be re-derived
   from C truth here: 1 for CHARP arrays; forgetting that made
   `sizeof(char b[128])` report 1024); `sizeof(expr)` via the NOEMIT
   parse (below); typedef names (`sizeof(size_t)` from the header).
   Casts: speculative spec-scan inside Primary decides cast-vs-group
   (rewind on failure), `(char)` emits MOVZX (truncates: 321→65, -1→
   255), `*` retype keeps char*-ness (byte aliasing works: write
   `(char*)&int` then poke bytes), and `(struct T *)` KEEPS THE TAG so
   `->` continues to work through `(void*)` round-trips — single
   struct-star only, `**` drops it. `short` joins the fold family.
   The **noemit** knob (cc->noemit): every code-writing site is gated
   — CCByte (which choke-holds CCWord/CCHalf/CCQuad), CCQuadAbs ALSO
   skips its fixup RECORD (a recorded position during frozen-pointer
   parsing would relocate bytes of live code at object link), CCCall
   skips (keeping the forward-call patch queue clean too), CCJmp/CCJz
   return -1 and CCPatchJmp ignores -1; jumps inside sizeof(expr) are
   emitted-then-unpatched-free since nothing moves. Landmine: the
   expression chain's LogAnd/LogOr tails normalized `ety=TY_INT`
   UNCONDITIONALLY — harmless for every previous consumer (they read
   RAX and the LHS's type, and CCExprNC re-adds CCVal), but it erased
   the operand type the silent sizeof parse exists to read (`sizeof
   (*cp)` said 8 for a char*). The tails now fire only when `&&`/`||`
   actually ran — the standard fired-flag pattern the other levels
   already used inside their operator loops. `sizeof` in a global
   initializer stays a loud "must be a constant" diagnostic. 65 cases
   (sizeof.c: 25 checks + closer — dims, struct symbols, deref-of-
   cast, chained casts, truncation, `(char*)` byte aliasing of an
   int*, typedef/header size_t, pointer-array types), green twice,
   saved-object round trip 26/26/26, clean `CC TESTS: 0 failed`.
 - [x] **M16** `goto`/labels + conditional `?:`. Labels: peeked at
   statement level with the FULL token snapshot (a `name` token that
   turns out not to head a label must restore the lexer EXACTLY —
   PLAN's rewind rule; CCSnap/CCRestore exist for this), defined at
   the current code position with duplicate rejection, and the
   forward-goto queue patched by the definition (write rel32 straight
   into the placeholder, CCPatchCallPos's pattern: rel =
   label_pos-(operand_pos+4)) or reported at the function's `}`
   ("goto to undefined label" — which catches cross-function gotos for
   free, since label/goto tables reset per function and C functions
   never nest, so one table per function is exact; this is the
   NON-nesting side of the M14 nested-switch lesson). Backward gotos
   jump directly. Queue entries park at -1 when patched, keeping them
   out of both re-patching and the end-of-function check. Ternary: a
   new precedence level between comma and LogOr — reached via
   CCExprNC it upgrades parens, assignment RHSs and call args in one
   move; the Primary paren-group arm calls it directly so `(*p?a:b)`
   works; JZ-to-third / JMP-over-third textbook codegen, right-
   associative by recursing into itself, both arms CCVal-normalized so
   VALUES merge and identical-kind arms keep element types and struct
   tags; under noemit its jumps are the -1 placeholders, so sizeof's
   silent parse handles ternaries for free. Landmine (again): the `:`
   between the arms — in the plan, MISSING from the written function;
   a 3-line probe (`x=0?1:2;`) named it instantly. Error paths pinned:
   undefined label (incl. cross-function), label redefined. Objects
   reload identically (rel32 is self-relative). 67 cases (goto.c 10:
   forward/backward/cross-structure exits/stacked labels/switch-in-
   for bail; tern.c 14: unevaluated arms via a side-effect counter,
   pointer + struct-tag arms through `->`, chained fallbacks, ternary
   in comparison/subscript/printf arg), green twice, saved-object
   round trips 10/10 and 14/14, clean `CC TESTS: 0 failed`.
 - [x] **M17** two-dimensional arrays. Symbols carry `dim2`; array
   parameters carry it via `pdim2` (first dim always ignored — the
   caller owns the count; `t[][64]` and `t[8][64]` compile identically
   and BOTH must match the caller's row layout, which is why the
   second dim is the recorded one). The expression state gains
   `esz`/`esz2`: the ident-decay arms set them from the symbol
   (esz2 = bytes per inner element, esz = per ROW; esz2!=0 IS the
   "this decays to a row" marker), and the Postfix index arm consumes
   the pair: first index scales by esz and the ROW becomes the new
   decay (is_addr stays 2 — so `m[i]=x` stays "assignment to array",
   C-true — and keeps the base type AND tag via call-locals `bty`/
   `sty0`, because the index-EXPRESSION parse clobbers cc->ety/sty);
   the second index runs the ordinary ety path (PTR/CHARP give 8/1 —
   char rows are byte-stride, struct rows stride by tag size).
   CCVal clears esz2 (any plaining of a value spends its row scales —
   the index arm reads them BEFORE its own CCVal), which is what
   keeps stale scales from hijacking a later pointer index. Sizes:
   declaration allocates dims-product (frames/globals all slot-based,
   char byte-stride inside — capacity n*8 bytes per n chars, slack
   documented), sizeof multiplies the dims product by C-truth element
   size. `struct pt g[][2]` params decay to struct-POINTERS and must
   reach the struct scale branch via sty or rows stride by 8 (they
   silently sum 33-as-36-shape garbage otherwise). Latent pre-M17 bug
   fixed on the way: array params of char (`char s[]`) blanket-decayed
   to TY_PTR — C truth is a BYTE-stride char*; every old fixture used
   `char *` so it never bit until M17 probed it. **The LogOr landmine
   — costliest bug of the compiler so far:** restructuring the
   LogAnd/LogOr head loads (so parenthesized decays keep their state)
   was done by hand-rebuilding each loop's text span, and the rebuilt
   LogOr loop SILENTLY LOST `CCTestRax(cc);` before its short-circuit
   CCJz — every `a || b` branched on STALE flags. Micro-probes stayed
   green (no `||` in them); exactly three fixtures failed (the prelude
   atoi's `(c<48)||(c>57)` mis-broke: atoi("1234")=0 — stdlib 46/47 +
   hdr 8), so the fault signature pointed at atoi and the hunt ran
   through its disassembly (correct!), the call site (correct!), pool
   forensics, warmup theories, and a five-way selective-revert bisect
   (all innocent — because the culprit was inside my REVERT anchors'
   blind spot: the loop body, not the gate line). Root-caught only by
   rebuilding the state from HEAD script-by-script and DIFFING — the
   single deleted line jumped out. Lesson pinned: after hand-surgery
   on a parser function, diff against the pre-surgery text and expect
   comments + intended lines ONLY. Expectation trap #4 the same day:
   sumrow(m2,2) was hand-pinned to 33 when rows are 3+33=36 — the
   compiler was right, the comment even listed the elements. 68 cases
   (array2.c: 21 checks + closer — 2-D globals/locals int+char,
   int[][3]/char[][8]/struct[][2] params, byte-stride char[] param,
   rows decaying through strlen/strcpy/atoi, pointer-array elements
   via temp/cast, paren-group decay, row adjacency (strlen walks into
   the next row exactly like C), 2-D sizeof totals), green x3, saved-
   object reloads 22/22/22/22, `||`-and-`&&` side-effect probes 14/14,
   clean `CC TESTS: 0 failed`.
 - [x] **M18** function pointers. Declarator: `CCParseFunPtrDecl`
   matches `(* name ) ( param-list )` after any base type — used from
   local decls, global decls, struct members (int/char/struct-based),
   parameters (named or ANONYMOUS `int (*)(int)`), and typedefs (name
   captured INSIDE the parens); every call site snapshots with
   CCSnap/CCRestore and rewinds on FALSE (`(x)` groupings survive the
   peek). Params are SWALLOWED with a depth counter (nested declarators
   parse their own parens) and deliberately DROPPED: an indirect call
   takes its arg count from the call site, and C only requires a match.
   Values: an SC_FUN name IS a code address — CCVarAddr emits
   MOVABS RAX,code+off with a CC_B_CODE relocation (new base kind;
   CCLoad's kind validation bound moved from CC_B_LIB to CC_B_CODE —
   forgetting it rejects every fp object as "corrupt relocation"),
   is_addr=2 (value, never loads, never assigns: `f=x` stays C's
   error). FORWARD addresses: a prototype-then-definition pair queues
   the MOVABS operand positions on the symbol (apatch/napatch); at
   definition the final absolute is written into the buffer (the code
   buffer is allocated ONCE per compile by CCInit — queued positions
   never slide) AND the fixup record is appended there (delta=entry:
   exactly what CCLoad's generic CCBase+delta patch consumes). A
   prototype whose body never arrives leaves queued addresses →
   "address of undefined function" at the end of the TU. Under noemit
   nothing is emitted AND nothing queued (a silent scan that recorded
   a 10-byte site whose bytes were never written would have the
   definition-time patch scribble over LIVE code). Calls through
   values: Primary's IDENT-peek dispatch handles `fp(3)` (args pushed
   first, target loaded LAST into RAX — the landmine: CCVarAddr does
   NOT write the pending-lvalue state, the caller does; without setting
   is_addr/ety/aty/sty/esz2 by hand, CCVal used the LAST ARGUMENT'S
   state, skipped the slot load, and `call rax` executed the stack
   slot — SIGSEGV with the return address looking like code). Chain
   results (`n.fn(2)`, `(*fp)(3)`, `(bp)(1)`) flow through a new
   Postfix `(` arm → CCIndirectCallAt: normalize RAX to the TARGET
   (is_addr==1+TY_PTR: CCVal loads the slot — the member case;
   is_addr==1+TY_INT: use as-is — `*fp` left a pending-load marker
   but a function HAS no further load; is_addr 0/2: already a value);
   spill to a hidden local (lofs grows; frame imm32 patched at function
   end — switch's trick), evaluate args with RAX free, reload, CALL
   RAX, pop. Struct fp members (`int (*fn)(int,int);`) ride the member
   machinery unchanged: TY_PTR member, '.' yields is_addr=1 member
   address, the `(` arm loads it. Landmines this milestone stepped on,
   all probe-pinned: the peek-path state bug above; arity-mismatch
   probes that passed the compiler and failed the MATHS (`runvia`
   taking `int(*)(int)` around a two-arg `add` — the compiler is right,
   C is UB); a fixture referencing a global before its file-scope
   declaration (C agrees); expected-sum traps (twice(later)=33+44=77,
   not 78 — fifth such catch). Saved objects rebase flawlessly: fp
   values survive dump/reload at 27/27/27/27. 69 cases (fptr.c: 26
   checks + closer — member/global/typedef/param fps, `(*fp)`/`(fp)`/
   `(fp=expr)(...)` forms, forward-address resolution, zero-arg side-
   effect counting, NULL sentinels, address equality, sizeof, anon
   declarators), green x3, saved-object round trips 27/27/27/27,
   error paths pinned (undefined-label-style diagnostics for int-call,
   prototype-only address use, fp arrays), clean `CC TESTS: 0 failed`.
 - [x] **M19** qualifiers + unsigned/signed char + unions + escapes +
   literal indexing. **Fold table rewrite** (CCAcceptBase and its
   silent twin CCTrySpec, kept as exact mirrors): every qualifier is a
   no-op consume; alone it PROVISIONALLY promises TY_INT (C's
   default-int) with a `tent` flag; `had_int` (int/long/short ONLY —
   never bare unsigned) and `had_char` implement C's combining rules,
   so `unsigned char`/`char unsigned` fold to the byte model (char IS
   the zero-extending byte type here), bare `unsigned` is the int
   model, `int char`/`char int`/`char char` diagnose. Landmine, caught
   ONLY by the suite (every fresh probe stayed green because none of
   them wrote `const void *`): the qualifier's provisional TY_INT made
   the void and struct arms' `ty>=0` combine-checks fire on
   `const void *v` — the headers' favorite parameter — breaking the
   hdr fixture with "void cannot combine". The `tent` flag is the fix:
   a bare-qualifier promise YIELDS to void/struct/typedef (which
   overwrite it), while a REAL base (unsigned/int/char so far) still
   combines illegally. Same-day expectation traps the compiler beat
   three more times: `signed char sb=-3; sb==-3` is FALSE in the byte
   model (zero-extend: 253 — now pinned AS the documented behavior);
   `*v.p=*v.p+1000` mutates the POINTED-TO variable while the union
   slot keeps the POINTER quad (`v.i==&ip_value`, not ==tot); and
   3+100=103. Unions ride the struct machinery end-to-end: `union`
   joins the tag arms (ONE shared tag namespace — `struct S`/`union S`
   collide, a deliberate simplification), CCStructDef takes an
   is_union flag (all member offsets 0, size exactly one slot — true
   because every member type the model admits IS a slot), and the
   Postfix `.` arm chains nested value members through msty for free.
   The new acceptance — by-value members of ONE-SLOT tags — is exactly
   C's union-member semantics, and a tag mid-definition still has
   size 0 there, so C's illegal `struct S { struct S s; }` rejects
   itself with the pointer message. Block-scope `struct T {...};`
   definitions: peeked in CCDeclStmt with the full CCSnap/CCRestore
   pair (the USE form `struct L l;` must rewind and run the ordinary
   CCAcceptBase path — def-before-use in HolyC again needed an extern
   for CCStructDef's new 3-arg signature before CCDeclStmt could call
   it). Escapes: \a \b \f \v \e added to both literal kinds, \xHH
   (1-2 hex digits, error on none) and \ooo octal (up to 3 digits —
   `\17z` stops at two because z is non-octal, exactly like C) join
   them; `\0` in a string lands a NUL, which the pool stores as the
   terminator (strlen("a\0b")==1 — C-truth by construction). String
   LITERALS now enter Postfix: `"ab"[1]`, `("ab"+1)[0]` — byte scale
   from TY_CHARP, zero new machinery. Python anchor discipline (count==1
   asserts) caught a duplicated declaration block between the two fold
   functions before it could edit the wrong one. 70 cases (unionc.c:
   39 checks + closer — every qualifier position, qualified function
   defs, unsigned/signed/char-unsigned decls + sizes, full union alias
   matrix int/char/pointer incl. arrays + params + nested chains +
   block-scope defs, 14 escape forms, literal indexing, for-init
   declarations), green x2, saved-object reload 40/40 identical, error
   paths pinned (int-char conflicts, duplicate char, tag redefined,
   by-value self-member, shared-tag collision), clean
   `CC TESTS: 0 failed`.
 - [x] **M20** enums + compile-time constant folding + literal
   concatenation. The folder is a mini recursive-descent parser over
   the SAME precedence ladder as the code-emitting chain (Prim/Unary/
   Mul/Add/Shift/Rel/Eq/BitAnd/BitXor/BitOr/LogAnd/LogOr), but it
   produces I64 values instead of code: enum bodies (`BLUE=GREEN*2`,
   chained auto-increment from the previous value), case labels
   (`case GREEN:`, `case 1+1:` — replacing the old `[-]TK_NUM`-only
   parse), global initializers (`folded=PLAIN+FANCY*10` — literals,
   constants, folded expressions; non-constants still diagnose), array
   sizes (all four dim sites + the negotiable param 2nd-dim), and any
   live expression via the Primary constant branch (a constant is an
   immediate, not storage). Div/mod by zero is a LOUD diagnostic: the
   host JIT would fault, and a compiler crash on edge input is never
   acceptable. `~` folds as `-v-1` spelled to avoid host-~ dependence.
   Enum machinery: constants in a flat table (enumerator names must be
   TU-unique — C's shared namespace — and CCConstFind refuses any name
   a variable/function owns, so locals shadow correctly); tags in a
   separate 16-entry table (`enum Color` folds to the int model, the
   tag only catches redefinition; struct/union tags live in THEIR own
   table, so `struct S` + `enum S` coexist C-faithfully); definition
   shape (tag OPTIONAL: `enum { A } x;`) hooked at top level and in
   CCDeclStmt behind the full CCSnap/CCRestore pair; use form falls
   through the CCAcceptBase enum arm (blind tag consumption — C-legal
   for incomplete tags; `enum x;` misreads x as the tag: documented).
   Adjacent string literals concatenate: scratch-buffer collect (the
   HolyC unit has NO StrCat — hand-rolled byte copy) then ONE pool
   allocation — appending into the first literal's entry would scribble
   on the pool's next placement, and dedupe could hand back an unrelated
   address anyway.
   **The binding landmine — strangest of the compiler so far:** with
   only `CCConstExpr` extern-declared (needed for Prim's upward call —
   without even that the HOST refuses to compile the unit), every call
   edge INSIDE the new chain executed NOTHING — no value, no token
   consumed, no error, no callee-entry print (proven by a DBG print at
   Prim's top: dead from inside Unary, alive when the same call came
   from a probe-file wrapper). The file compiles clean; the edges are
   just silently dead. Adding extern declarations for the WHOLE new
   chain at the top — the pattern every existing parser chain function
   already carries — made it deterministically correct (5/5 + suites).
   Rule pinned: any new function group in this unit gets its extern
   declarations, even ones defined before their callers. (Secondary
   debugging trap: single-token sources make `pc`-advance an unusable
   consumption signal — pc is already past the token after the first
   lex; use multi-token sources or value observation.) Expectation
   trap #8 (compiler right, author wrong): `PLAIN+FANCY*10` is 40,
   pinned 41 in the probe. A shadowing probe (inner `int x` aliasing
   the outer slot, rc=15) put scoped symbols on the backlog. 71 cases
   (enums.c: 36 checks + closer — auto-increment chains, anonymous
   enums, tag-typed globals/locals, constants as case labels/subscripts/
   sizes/global inits, the full live expression fold set, three concat
   forms), green x3, saved-object reload 37/37/37 identical (constants
   bake as immediates; folded globals ride the arena snapshot), error
   paths pinned (non-constant case, enumerator/variable conflict,
   constant div0, enum tag redefined, function address in a constant),
   clean `CC TESTS: 0 failed`.
 - [x] **M21** block scopes + `static` block objects. Scope mechanism:
   every construct that C gives a scope (function body, `{ }` block,
   `for`-init clause, switch body) records a waterline for each table
   the parser appends to — sym_cnt/sym_base, the frame-slot count lofs,
   typedefs (tn), enum constants + tags (ncnst/netag), struct tags
   (nst) — and restores them at the closing brace. Reverse-scan
   CCSymFind already resolves to the newest binding, so ERASURE alone
   gives correct shadowing, correct "undefined again" behaviour, and
   releases frame slots; `cc->lofs_peak` remembers the deepest point
   the function reached so `SUB RSP` at function end still pays for a
   block that peaked deep and then rewound. CCSymNew gains the same-
   scope dup check (scan bounded by sym_base ⇒ nested scopes redeclare
   freely), and function bodies bind params UNDER the function symbol
   registration so a param may shadow the function itself (e21a) but
   not another param (e21c: C99 agrees, "redeclaration in this scope").
   `static` block objects: CCMakeStatic hands back the frame slot and
   takes an ARENA slot (name stays block-scoped through the table,
   storage outlives the call) — counters, arrays (arena ascent, global
   layout), structs, pointers through `&`, and fps all work; the
   initializer is folded by the M20 constant evaluator and written as
   ARENA BYTES at compile time (never an emitted store ⇒ "exactly
   once" is structural, and a saved object carries the initial value).
   **Landmine #1 — function symbols must outlive the brace:** the first
   waterline version truncated the table to the mark blindly, and a call
   to a not-yet-declared function auto-creates its stub INSIDE whatever
   window the call sits in. Erasing that stub means the later definition
   creates a SECOND symbol, and the queued rel32 patch never lands —
   `mutual` (is_even/is_odd, no prototypes) silently returned garbage.
   C makes such a function a file-scope entity, so CCEraseScope (the
   single place that truncates the table) compacts SC_FUN entries down
   onto the mark instead of dropping them. The related ordering trap:
   registering the function symbol itself after the mark erased the
   function's OWN name at `}` — every later reference re-queued.
   **Landmine #2 — recycled entries:** truncation makes indices reusable,
   so a fresh symbol inherited a dead one's dim/persist/patch queues;
   CCSymNew now MemSets the entry before filling it (any NEW CSym field
   must be reset there). **Landmine #3 (my own C-code, host-visible):**
   the first draft called `MemSet(s,0,…)` BEFORE `s` was assigned — every
   compile in the process died with a host FAULT inside CCCompile, which
   reads like a compiler bug but is a garbage-pointer wipe. **Landmine
   #4:** the dup check first returned NULL, and callers of CCSymNew
   dereference the result unconditionally — one clean diagnostic became
   a fault storm; it now reports the error and hands back the EXISTING
   symbol (errs is set, the translation unwinds anyway). Expectation
   trap #9, compiler right again: two calls to a stateful function in
   ONE expression sum 103+104=207, not 206 (and C leaves even the order
   unspecified — stateful calls go on separate fixture lines). New
   verification habit for this milestone: the fixture was cross-checked
   against `gcc -std=gnu99` on a copy with only the model-specific
   `sizeof` numbers adjusted — both answer 49. 72 cases (scoped.c: 48
   checks + closer — nested shadowing chains, static counters with
   folded inits and per-function twins, static arrays/structs/pointers/
   fps, statics declared in loops and nested blocks, a static shadowing
   a global, for-init + switch-body scopes with shadowing inside,
   params shadowing globals and their own function, an in-block
   function stub patched at definition, pointer access to a frame slot
   and shadowed `sizeof`), green x2, saved-object reloads 49/49 for
   scoped and 27/37/40 unchanged for fptr/enums/unionc, error paths
   pinned (out-of-scope use, same-scope redeclaration, duplicate params,
   static fp initializer), clean `CC TESTS: 0 failed`.
 - [x] **M22** aggregate initializers. Collection is a separate pass
   (`CCInitCollect`) that folds each element through the M20 evaluator
   and records, per value, WHICH brace group it came from (element-level
   groups get fresh ordinals; deeper braces inherit the current one;
   flat values report -1). `CCArrayInit` then maps values to ELEMENT
   indices with C 6.7.9's rule: element arity is 1 for a 1-D array and
   dim2 for a row; a group opens the next element and fills it in order;
   flat values keep filling and roll over when the element is full —
   which is exactly why `int m[2][3]` accepts both `{{1,2,3},{4,5,6}}`
   and `1,2,3,4,5,6`. Size deduction reads the same mapping (highest
   element touched), so `int a[]`, `int m[][3]` and `char s[]="ab"` all
   deduce like C does, including a partial final row. Landing: arena
   bytes for globals and `static` objects (compile-time, so a saved
   object carries the initial state and every reload starts from it),
   and one emitted store per element for frame objects — walking the base
   pointer up by the element stride (1 for char arrays, 8 otherwise, the
   SAME scale the index path uses). Elements the initializer leaves are
   zero-filled, which for frame objects is REQUIRED (that memory is not
   pre-zeroed) and is what makes `int c[6]={1,2}` C-correct.
   **Landmines, all three caught by probes or by the gcc cross-check:**
   (1) flat values originally shared one group id (-2 for all of them),
   and "same group" is how the mapper recognizes "still filling the same
   element" — so `{10,20,30}` looked like three values in one group and
   the first global array in the fixture died with "excess elements";
   flat values must be positionally distinct. (2) The group ordinal was
   RESET on every group close, so `{{1,2,3},{4,5,6}}` handed group 0 back
   a second time and the second row overflowed the first — groups need
   monotonically increasing ordinals from a separate counter. (3) The
   byte store's ModRM: `MOV [RAX],RDI`-as-byte was encoded 88 38, and
   without REX.R the reg field 111 names **BH** (the high byte of RBX),
   so every char-array initializer byte landed as 0 while every INT
   array worked perfectly (quad stores take REX.W and a legal reg field);
   fixed by routing values through RCX and storing CL. Also back-to-back
   HOST-dialect lapses (PLAN documented both and I still hit them):
   `continue` in the new collector loop (does not exist in this dialect —
   the arms now fall through and the loop re-tests the current token,
   which is what a continue would have done anyway) and the ternary
   operator in four places (host parse errors). The gcc cross-check paid
   for itself twice over before the compiler even ran: my hand-written
   expectations for `{{1},{2,3},{4}}` and `{{1,2},{3}}` assumed
   FLATTENING, which C does not do — the fixture was rewritten to
   row-group forms and the flatten-then-error semantics pinned instead.
   Error paths pinned: excess elements (both flavors), initializer string
   too long (with C's "NUL only if it fits" trim — `char s[2]="ab"` is
   legal and unterminated), empty initializer with deduction, `int a[];`
   with none, `int m[][ ]`, variable elements, a string on a non-char
   array, designators. 73 cases (arrinit.c: 26 checks + closer — local/
   global brace lists, deduction three ways, tail zeros, `{}`, nested row
   groups, enum-folded elements, braced scalars (local, static, global),
   char byte arrays incl. the unterminated form, arrays sized by enum
   expressions, static arrays persisting across calls, pointer access to
   an initialized array), green x2, saved-object reloads 27/27 and
   scoped 49/49 identical, cross-checked against `gcc -std=gnu99` on a
   copy with only the model `sizeof` numbers adjusted — 27 with the same
   per-check increments, clean `CC TESTS: 0 failed`.

## HolyC dialect gotchas (all verified in this build)

- **There is NO `continue` keyword in this HolyC dialect.** The kernel
  source never uses it (grep T/*.HC: effectively zero hits), and
  `continue;` anywhere in host-side `.HC` code — even inside a plain
  `while` — dies with "Invalid lval"/"Undefined identifier at ";"",
  sometimes reported at a LATER brace line than the actual culprit.
  Restructure with if/else chains (break exists and works). CC's own
  C language has continue (its parser implements it); only the HOST
  dialect lacks it — M13's prep/type code hit this three times over.
- Classes must end with `};`.
- **`if` and `else` guard EXACTLY ONE statement** (verified by probe:
  `if (0) x=1;x=5;` leaves x=5; `if (1) x=1; else x=2;x=7;` leaves
  x=7). Statements written after a bare single-statement arm — even on
  the same line — run UNCONDITIONALLY. Multi-statement arms need `{}`;
  bare multi-statements *before* an `else` are a parse error
  (`Missing expression at "else"`). This build bit CC twice through
  one pattern, `else CCByte(cc,0x48);CCByte(cc,0x8B);CCByte(cc,0x00);`:
  the MOVZX path emitted `48 0F B6 00` then a stray `8B 00`, a second
  load that dereferenced the just-loaded VALUE (fault addr == the
  stored byte). Audit grep: `grep -n -A1 "else$" CC_*.HC` and look for
  lines with two `);` groups.
- No `for (;;)` — use `while (TRUE)`; no `continue` at all.
- Wrap *every* operand of `&&`/`||` in parentheses.
- Forward references need `extern` prototypes: with plain prototypes
  this build mislinks mutual recursion (calls return garbage —
  verified by probe); `extern` links correctly. The INVERSE also bites:
  `extern`-declaring a symbol that is ALREADY DEFINED in the same
  translation unit (e.g. a helper from an earlier `#include`d file)
  yields `ERROR: Undefined Extern at &Caller+0xNN` at link — declare
  extern only for things defined LATER in the unit.
- The earlier `Missing ';' at "F"` prototype rejections were only the
  missing `};` on the class; prototypes are fine once classes are
  terminated properly.
- Casts need HolyC postfix form — `p=(I64 *)x` is a parse error;
  write `x(I64 *)` or dodge (byte-wise store).
- **Never mix a unary `&` operand into a binary expression**: probe
  `&x.b-p` yields 1 and `&member - structptr` bakes a wrong address
  into JIT'd code (a nil-deref at run time). Pass `&member` as a
  single-operand argument (that form is proven) and subtract inside
  the callee over plain I64 values — plain `I64-I64` subtracts of two
  address values are correct (verified `pb-pa` == 8).
- A HolyC fp-call enters with the **code address in RAX** — a load
  that forgets to set RAX "works" until the leftover address leaks
  into results (found via a `MOV RAX,[RCX]` mis-encoded as modrm 09).
- Exit codes are always 0; grep stdout for `ERROR:`; always end
  scripts with `Exit;`; run under `timeout`.
- **NEVER call `Free()` on anything CC-adjacent.** This port's
  allocator (poopalloc) hands each big MAlloc its own `PROT_RWX` mmap
  chunk and `Free` unmaps at *chunk* granularity — freeing a small
  neighbouring block SIGSEGVs every live neighbour. Symptom that cost
  a whole debugging round: `Free(text)` in `CCRunFile` unmapped the
  `out` arena; a later script read of `cc.out[0]` died with
  `SIGSEGV addr == the printed, perfectly valid out pointer` while
  code inside the include had read the same address microseconds
  earlier. Proven by probe; fixed by removing both `Free` calls
  (`CCSrc`/`CCRunFile` now leak their buffers, like the arenas).
  Host-side `dbg.c` now prints `[3d] FAULT sig=N addr=... rip=...` to
  stderr — that one line is what cracked this bug.
- x86-64 REX trap: `REX.W` alone leaves ModRM.reg / SIB.index `000`
  meaning **RAX**, not R8 — R8 as a destination reg needs `REX.R`
  (`4C`), as a SIB index `REX.X` (`4A`). `49 8B 02` is
  `MOV RAX,[RDX]`, not `MOV R8,[RDX]`; `49 89 02` stores RAX, not R8.
  The wrong-register stores silently zeroed the buffer. Prefer
  legacy-encodable regs (RAX/RCX/RDX/RSI/RCX) with plain `REX.W` —
  `CCEmitPutchar` now uses only those. Also: `CCWord` writes FOUR
  bytes (imm32); pairing it with a stray `CCByte(0)` desyncs the
  instruction stream (`00 88...` decoded as an `ADD [rax],...` and
  wrote through garbage).
- x86-64 byte-opcode trap: **`REX.W` does NOT widen opcodes that the
  Intel map defines only for bytes.** `48 3A C8` decodes as
  `cmp %al,%cl` — a SIGNED BYTE compare — so every C comparison of a
  value >=128 saw its low byte sign-extended (999 -> 0xE7 -> "-25 <
  0" was true!). The 64-bit forms are `39` (cmp r/m, r) / `3B` (cmp r,
  r/m); `CCCompareEmit` now emits `48 3B C8` = `cmp %rax,%rcx`
  (RCX-RAX, left-right — the SETcc direction stays correct). M1-M4
  tests all passed with 3A purely because their values fit a signed
  byte; the trap survived until `printf` negative / 4-digit data.
  Audit every split pair: 84/85 (TEST) and the F6/F7 groups follow the
  same rule; our emitters use `85` and `F7` with REX.W — both correct.
- No ternary operator in this HolyC build: `x=(1)?7:9;` fails with
  `Missing ';' at "?"`. Use `if/else`.
- A `*/` sequence inside a block comment's TEXT terminates the comment
  early: `int*/char*` in a header comment broke compilation with an
  error at the `*`. Write `int * / char *`, or put such text on a
  `//` line.
- C lexer state is `(pc, tk, tk_kw, tk_num, tk_str)` TOGETHER: after
  any CCLex, `pc` points PAST the current token, which lives only in
  the tk fields. Rewinding the source for a re-parse (the for-step
  dance) must save/restore ALL of them — `pc=X; CCLex(cc)` resumes one
  token late and silently Swallows the first token after the rewound
  region. It bit `for (...) body; return x;` — the `return` vanished
  and the M5 tests still passed because the fall-off return shipped
  the last statement's RAX by luck. Any rewind that saves only `pc`
  is a bug waiting for a statement whose first token matters.

## Codegen invariants

- Expression value always lands in RAX; binary ops `PUSH` the left
  half and `POP` it back into RCX.
- Every emit goes through `CCByte`/`CCWord`/`CCQuad` (bounds-checked).
- Initializer stores must use the SAME element scale the index path uses
  (1 for char arrays, 8 otherwise) and a byte store must name a low byte
  (CL/DL) or carry REX.R — ModRM 0x38 without a REX names BH, which
  compiles, runs and writes the wrong byte (M22). Aggregate values are
  landed as arena bytes for globals/statics and as emitted stores for
  frame objects; only CODE sites carry relocations, so anything an
  initializer needs that is a code address cannot live in the arena yet.
- Scope waterlines: CCEraseScope is the ONLY place that truncates the
  symbol table (function body, block, for-init, switch body all call
  it) and it MUST keep SC_FUN entries — a stub created by a call to an
  undeclared name inside any window has to survive to its definition,
  or its queued call patches dangle (mutual recursion). If a new
  per-scope table is added, give it waterline handling at all four
  sites, and remember CCSymNew zeroes recycled CSym entries.
- Hidden locals (`cc->lofs++` spills: switch dispatch slot, fp call
  target) allocated INSIDE a block are released by the block's
  waterline: the frame is still sized for them via lofs_peak, but code
  emitted AFTER that brace must not reference the slot. Anything that
  must live for the whole function is allocated BEFORE the scope mark
  (the switch dispatch slot is, deliberately).
- ANY new function group in the compiler unit gets file-top `extern`
  declarations like every existing chain function (M20 proof): without
  them the host COMPILES the unit but same-unit call edges into the new
  functions execute NOTHING — silently, deterministically wrong, zero
  diagnostics. Check the extern list matches the definitions exactly.
- Absolute code addresses (fp values, M18) relocate through base kind
  CC_B_CODE (CCLoad validates `kind>CC_B_CODE`); forward uses queue
  operand positions on the symbol and resolve AT DEFINITION — both the
  buffer operand and the fixup delta get written there, which is safe
  only because CCInit allocates the code buffer once per compile.
- Type analysis for `sizeof(expr)`/cast decisions runs the REAL parser
  with `cc->noemit=1` (M15): CCByte refuses everything (so CCWord/
  CCHalf/CCQuad inherit the freeze), CCQuadAbs refuses its FIXUP
  RECORD too (a phantom relocation into live code is worse than no
  code), CCCall refuses to emit or queue patches, CCJmp/CCJz hand
  back -1 and CCPatchJmp ignores it. Type state (ety/sty/aty/is_addr)
  is still written normally — that IS the analysis. The expression
  chain's logical tails may normalize ety ONLY when their operator
  fired.
- Entry stub sequence (M2, amended by M12.1): save caller RSP ->
  `cc->save`, `PUSH RBP` (stashes the caller's frame pointer at
  `[cc->save]-8` on the HOST stack), switch RSP to `cc->stack` top,
  then frame. Exit-shaped returns (return-from-entry, `exit`,
  `abort` at any depth) restore RSP from `cc->save` and LOAD
  `RBP,[RSP-8]` — never POP either quad — before `RET`; the HolyC
  caller rebuilds its frame from RBP, so both values must be host-
  valid at the moment of the jump (see the M12.1 landmine).
- Globals: fixed arena `cc->locals` (MAlloc'ed, zeroed); accessed via
  `MOVABS RCX,addr` — the code buffer may be far from the arena, no
  32-bit absolute encodings.
- `CCEmitPutchar` leaves the char in RAX untouched (C putchar returns
  its arg) and touches only RCX/RDX/RSI — all scratch under our
  convention — using exclusively plain-`REX.W` legacy-register forms.
