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
`char s[]="text"` byte init) (M22) and struct/union initializers (M23)
— and their elements may be POINTER values: string literals, a DEFINED
function name, or `&global` (M24: `char *tab[]={"a","b"};`,
`int (*gf)(int)=f;`, `struct L gl={&gi,4};`), and element-faithful array
decay (M25: `char *tab[i][j]`, `struct P *pa[i]->m`, `t[i][j]` params,
one-element arrays, three-level chains on 2-D pointer arrays), and
whole-object struct/union assignment (M27: `v = w;` copies every member,
shallow, same tag required, through objects, array elements, dereferences
and by-value members)
`for`/`while`/`if`/`else`/`break`/`continue`/`switch`/`case`/`default`/
`do-while` (M14), `goto`/labels and the conditional `?:` (M16),
functions (forward refs,
recursion, and M13 prototype declarations `int f(int a);` — undefined
at link time is a compile error), function pointers (M18: declared in
any position, assigned from a name or `&name`, called directly or
through `(*fp)`/`(fp)`/member/typedef/param forms) and ARRAYS of them
(M26: `int (*tab[2])(int)` / `int (*tab[])(int)={f,g}` tables at file,
block and `static` scope, passed as parameters, called via
`tab[i](x)`/`(*tab[i])(x)`/`(tab[i])(x)`), `printf` (literal format:
`%d %s %c %%`), `putchar`, `exit`, and the
M12 stdlib subset — `strlen strcmp strcpy strchr memmove memcpy memset
memcmp malloc atoi abs puts srand rand` (delegated to kernel routines
where they exist; see M12), the M12.1 batch — `strcat strncat strrchr
strstr strncmp strncpy memchr strdup calloc` + `sprintf getchar fgetc
getc fputc putc fputs abort` (see M12.1) — the usual operators —
`++ -- & * []` (M28: both spellings of `++`/`--`, prefix and postfix, on
any modifiable lvalue the chain can produce), comma, short-circuit logic,
bitwise `& | ^`, shifts
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

Silent deviations were closed at M25: an array carries its ELEMENT TYPE
(`s->ael`), decay and every subscript hand it on, and the subscript arm
sizes/loads through it. What that fixes, all of it silently wrong before:
`char *tab[2]; tab[0]="xy"; tab[0][0]` yielded the string's low 16 bits
(31096) instead of `'x'` (int-tied 4-byte load); a STORE through `tab[0][i]`
wrote eight bytes over the string's neighbours; `struct P *pa[2]` was
indexed by the TAG size (its elements are pointers, not structs) and
`pa[0]->a` was refused outright; `sizeof(c)` for `char c[1]` reported the
pointer size; and `int a[1]` — one-element arrays were never decayed at
all (`dim>1` gated array-ness), so `a[0]=5` loaded the slot's own contents
as the base address and stored through nil (a fault, reproducible at
HEAD). New C-faithful diagnostic in the same arm: subscripting a SCALAR
(`v[0]`, `RED[0]`, `tab[0][0][0]`) is "array subscript requires a pointer
or array" — gcc says the same thing, and the old "lenient: index a plain
int as slots" rule dereferenced the int as an address and faulted.
Residual model choices, both documented below: struct-pointer arithmetic
scales by 8 while `ps[i]` strides by the tag size, and arrays of function
pointers stay unsupported.

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
  (`[0]=x`, `.x=x`) are not supported; struct arrays keep their existing
  rejections; function-pointer arrays initialize since M26
  (`int (*gt[3])(int)={inc,dbl,neg};`); pointer elements work since M24
  (strings, defined function names, `&globals`) because the arena slots
  they fill carry relocations of their own. `extern int a[];` (an incomplete array declared extern) is
  "array size missing". Braced SCALARS are accepted (`int x={5};`).
  Initializer VALUE edges (M24): a pointer element in a non-pointer array
  is accepted (`int a[2]={"ab"};` stores the address where C warns); a
  function name must be DEFINED by then (a bare forward reference is
  "not a compile-time constant", a prototype-only name "initializer refers
  to a function that is not defined yet" — no arena slot means no patch
  queue); the address of a frame LOCAL is accepted in ordinary assignment
  (C-legal, emitted store) but refused inside a brace list, where values
  must fold ("initializer must be a constant, a string, or the address of
  a function or global"). String literals live in the writable data arena,
  so mutating one through any slot works here (`wbuf[0]='Z'`); C would put
  the literal in `.rodata` and the same write is UB there — gcc segfaults
  on it, so a fixture that mutates literal bytes needs a real array.
- Array edges (M25): subscripting a scalar is diagnosed (C agrees — the
  model's old lenient int-indexing rule faulted in practice); 3-D arrays
  are still unsupported (`t[2][2][2]` — the second dimension is the last);
  a `char b[128]` frame object still spends one
  8-byte slot per byte (element COUNTS and strides are byte-true since
  M25, the frame density is not); `sizeof(char c[1])==1` and
  `sizeof(int b[1])==8` follow the model's slot convention while pointer
  and byte element sizes are C-true; and struct-pointer arithmetic
  (`ps+=1`, `ps++`) scales by 8 where `ps[i]` strides by the tag size —
  a pre-existing inconsistency, unchanged here, so prefer `ps[i]`.
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
  members stay pointer-only because members occupy one slot EACH (M19
  layout), not for want of a copy — M27 supplies the copy, which is why
  union assignment (`a=b` between union objects) and whole-tag member
  copies work now;
  `sizeof` a string LITERAL answers the pointer size, not the byte
  count (the silent expression route cannot see pool lengths); a tag
  still mid-definition has size 0, so C's illegal by-value self-member
  rejects itself. `void` combinations follow C once the qualifier
  promise is discounted (`const void *v` folds to the byte-pointer
  model like every void*).
- Function-pointer edges (M18): arity is NOT checked — `int (*f)(int)`
  called with the wrong arg count against a mismatched callee is as
  undefined as the C it mirrors (mismatched types at assignment are
  likewise silent — the declarator swallows params wholesale); ARRAYS of
  function pointers work since M26 (declaration, brace-list init with
  deduced sizes and zero-filled tails, `static`, globals, params
  `int (*t[])(int)`, and calls via `t[i](x)` / `(*t[i])(x)` / `(t[i])(x)`),
  with edges of their own: ONE dimension, and it must sit INSIDE the
  parentheses (`(*t[2][3])(int)` and `(*t)[2](int)` diagnose — C's grammar
  agrees, since a function returning an array is illegal); struct members
  may not be arrays (one slot each, M19); `typedef int (*tab[2])(int);` is
  refused because it DECLARES an array rather than naming a type — the
  supported route is `typedef int (*fp)(int); fp tab[2];`, which works;
  a parameter's written count is ignored (param arrays decay, the caller
  owns the length) and an unsized array with no initializer is "array size
  missing"; calling a HOLYC-JIT function through a C fp is
  unsupported (Callees must be CC-compiled: plain `ret` + caller-side
  pops — HolyC's epilogue differs); `sizeof(*fp)` reports 8 where C
  rejects a function type; a function address used before its
  DEFINITION resolves at definition time (prototype required first,
  like any C use); global and `static` fp initializers work since M24
  (`int (*gf)(int)=f;` — a defined function's address, or 0) and land as
  a relocated arena slot, so the target survives a saved-object load; a
  forward reference diagnoses "not a compile-time constant" and a
  prototype-only name "initializer refers to a function that is not
  defined yet"; a stored fp is a raw code address:
  NULL is the only portable sentinel (`fp==0` / `fp!=0` work). `fp++`/`++fp`
  on a function pointer move it by 8 like any other slot pointer (M28): C
  gives function-pointer arithmetic no meaning at all, and nothing in the
  symbol records fp-ness, so this is the model's choice rather than a rule.
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
  ordinary local (it does not link to a file-scope x); file-scope tentative double
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
  subscripting a string LITERAL works since M19 (`"ab"[1]` is 'b', and
  `*("ab")` too) — and it writes into the WRITABLE data arena, so
  `++"ab"[1]` really does change that pool string for the rest of the run
  (C: undefined behavior; a variable is the portable spelling);
  `++`/`--` on a POINTER-TO-ROW steps by element size, not row size;
  array PARAMETERS discard the first dimension (the caller owns the
  count) and keep the second; struct members may not be arrays.
- **Pointer DIFFERENCES are byte counts, not element counts** (silent,
  measured while verifying M28, pre-existing): `int a[4]; p=a; ++p; ++p;`
  then `p-a` is 16 here where C says 2, because subtraction is the ordinary
  integer subtract and nothing divides by the element scale. `++p`/`p++`
  DO scale (by 8 for slots, by 1 for bytes — `incr.c` pins both), so the
  walk is right and only the difference is byte-shaped. Byte pointers are
  correct by construction (`q-w` on `char *` is the byte count C reports),
  which is why this hides: the common `strlen`-style difference works. Until
  it is fixed, carry an index alongside the pointer when you need an element
  count, and note that `sizeof(*fp)` and struct-pointer steps keep their own
  documented model sizes.
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
- Structs: no by-value passing or return, no array members and no
  by-value members of MULTI-slot tags (one-slot tags — any union, any
  single-member struct — are by-value since M19), tags must be defined
  before any use (no forward tags), and one member may not nest a struct
  by value. Whole-object assignment works since M27 (`v = w;`, `arr[1]=t`,
  `*p=t`, `w.v=z.v`): slot-by-slot, so it is SHALLOW — a pointer member is
  copied and the two objects then alias the same target — requires the same
  tag on both sides (C's constraint, enforced rather than approximated, so
  `u = x;` mixing a union and a struct diagnoses even when the layouts
  match), and it fully overwrites the destination. An assignment's value is
  the left OBJECT, so `(x=y).a` and `x=y=z` chain. Initializers
  (M23) work for objects and 1-D arrays: members fill in declaration
  order, partial initializers zero-fill the rest (mandatory for frame
  objects), a union takes exactly ONE value (its first member — extra
  values in its group are excess, like C), and a by-value one-slot
  member takes the one quad that IS that sub-object. Designators
  (`{.a=1}`, `[0]=x`) are not supported anywhere; a 2-D array of structs
  diagnoses; a function-pointer member initializes from a DEFINED
  function name (M24 arena relocations made arena code addresses
  portable); char members land as full quads (harmless: each member has its
  own 8-byte slot and only its low byte is ever read through it). The
  shared mapper reports overfull targets as "excess elements in
  initializer" (generalized from M22's array-only wording).
- `++`/`--` edges. Prefix `++v`/`--v` arrived at M28 (they had never
  parsed at all — `++v` died at the operator with "expected number, string,
  variable or '('", found while probing M27, which is why every fixture
  written before M28 spells its loops `i++`). The same milestone moved
  postfix into the postfix chain, so `(*p)++` and `(v)++` — the forms PLAN
  told people to rewrite as `*p=*p+1` for a dozen milestones — now work.
  Both spellings share ONE emitter (`CCIncDecAt`), so pointer scaling,
  byte-vs-quad width and what survives in RAX cannot differ between them;
  the difference is only that postfix parks the old value on the stack and
  pops it back. `++ ++ x` parses and then diagnoses, which is what C does
  too (the operand must be a *modifiable lvalue*, and `++x` is a value).
  Aggregates are diagnosed, not approximated (M27): `arr[0]++` and
  `u[0]--` report "++/-- needs a modifiable scalar or pointer" — they reach
  the lvalue test as is_addr==1 elements, and bumping only the first slot
  would be neither C's rule nor a useful approximation; `++m[0]` on a 2-D
  array says "++/-- needs a modifiable object" (its operand is a row, i.e.
  an array in C too); and `v += 1` on a struct or array says "compound
  assignment to an array or struct object".
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
  `CCSave(fn)` serializes (v2, M24): header (magic/version/entry_off/
  code_len/arena_len/nfix/nafx) + nfix CODE relocation triples + nafx
  ARENA relocation triples + code bytes + the *initial* data arena
  (global initializers and the string pool are written to the arena at
  compile time, so the dump is the `.data` snapshot — save BEFORE
  running). The arena is a fresh allocation on every load, so pointer
  slots INSIDE the arena (a string table, a function table, `&global`)
  carry relocations too: an absolute baked into the arena was exactly as
  stale as one baked into code. `CCLoad(fn)` validates sizes and BOTH
  relocation tables (arena records are checked against the ARENA length;
  their fields sit at `arec+0/1/2` because `arec` already counts the
  header and the code table), `CCInit`s fresh arenas, copies, and
  rewrites each site as `newBase+delta`; `CCRunObj`/`CCRunObjFile` then call main. Loaded
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
 - [x] **M23** struct and union initializers. The M22 collector is
   reused unchanged; the element mapping was extracted into `CCInitMap`
   so aggregates share ONE rule with a different target ARITY: 1 for a
   scalar element, `dim2` for a row, `nmem` for a struct, and **1 for a
   union** (its first member — extra values in its group are excess, as
   in C). Tags gained an `isu` flag, written on BOTH definition paths
   because the M21 waterlines RECYCLE tag slots: a flag set only in the
   union branch would inherit a stale value from a dead tag (the same
   class of bug as recycled symbol entries, which M21 had to fix).
   `CCStructInit` walks targets in ascending order like `CCArrayInit`
   does for elements: arena bytes for globals and `static` objects, one
   emitted quad store per member for frame objects. Member addresses are
   a TABLE, not a stride, so the frame walk advances RAX by the
   DIFFERENCE between consecutive `element*size + moff[member]` targets
   (ascending by construction: the next element starts at `size+moff[0]`
   above the previous element's last member), and the arena path just
   recomputes from the tag each time. Everything lands as a full quad:
   each member owns its own 8-byte slot, so writing a char member's
   unused upper bytes is invisible, and a by-value member can only be a
   one-slot tag (M19) so that sub-object IS one quad. Struct ARRAYS work
   too (arity repeats per element, elements step by the tag's size — the
   layout the '.' arm already uses, probed first rather than assumed),
   including size deduction (`struct P gpa[]={{1,2,3},{4,5,6}};`).
   Expectation traps #10 and #11, both caught by the gcc cross-check
   BEFORE the compiler was blamed: a flat 8-value list into `struct P
   pf[4]` fills members in order (so `pf[2].b` is 8 and `pf[3]` is all
   zero — not `pf[3].a==8`), and a deduced 2-element struct array is
   48 model bytes, not 144. Error paths pinned: excess elements (struct
   and union), string initializer on a struct ("apply to arrays only"),
   2-D struct arrays, function-pointer members ("not a compile-time
   constant"). 74 cases (structinit.c: 21 checks + closer — members in
   order, partial zero-fill, grouped and flat struct arrays incl. the
   tail element, enum-folded elements, union first-member semantics and
   union ARRAYS, by-value one-slot members (local and global), NULL
   pointer members, globals/statics incl. a persisting static struct,
   `sizeof` unchanged), green x2, saved-object reloads 22/22 with
   arrinit 27/27 unchanged, cross-checked against `gcc -std=gnu99`
   (only the model `sizeof` number adjusted) — identical check by
   check, clean `CC TESTS: 0 failed`.
 - [x] **M24** pointer-valued initializers + ARENA relocations. Values
   that are absolute addresses (a string literal's pool slot, a DEFINED
   function, `&global`) now initialize anything — scalars, brace lists,
   struct members, `static` and global arrays, function pointers.
   `CCInitOne` is the single value primitive: it returns a folded constant
   (`*pkind=-1`), or a pool/code/arena address with its base KIND and
   DELTA. `CCArenaPtr(cc,off,kind,addr)` (CC_EMIT.HC) stores the 8 bytes
   at an arena offset AND records the triple, and `CCSave`/`CCLoad` gained
   a second relocation table (object format v2, `nafx` records) patched
   with the same `CCBase(kind)+delta` the code sites use. Without it, a
   pointer baked into the arena pointed at the arena the module was
   COMPILED against — the arena is a fresh allocation per load, exactly
   like the code buffer. `extern int a[];` still needs a size (no tentative
   extern), fp initializers take a defined function or 0, and forward uses
   diagnose rather than queue (an arena slot has no patch list).
   **Negative control, and the reason this is worth stating:** with the
   load-side arena patch loop neutralized, `int gi; int *gp=&gi;` +
   `gp[0]=gp[0]+1` saved once and run three times from the object gives
   `3 2 1` — the mutation lands somewhere shared and LEAKS across
   reloads; with relocations it is `1 1 1`, and ptrinit (which mutates a
   global array through a pointer slot and retargets `gtab[0]`) reloads
   25 25 25 identically. The version word went 1→2, and the loader
   refuses an old object rather than misreading it (pinned: patching
   `hdr[1]=1` on a saved file yields "is not a CC object").
   **Landmine #1 — an `extern` prototype AFTER the definition breaks
   LINKING, not parsing.** `CCArenaPtr`'s prototype first went at the top
   of CC_PARSE.HC in the ordinary extern block; the unit compiled clean and
   every call into it died with `ERROR: Undefined Extern at
   &CCArrayInit+0x392` — an extern declared after the definition promises
   the symbol from ELSEWHERE and the host never resolves it. Prototypes of
   cross-file functions belong in CC.HH's forward block (spliced first);
   the same file-ordering class of bug as the M20 rule, different symptom:
   M20 was a missing prototype silently executing nothing, this is a
   too-late prototype refusing to link. See the gotchas list.
   **Landmine #2 — record indexing in the new table.** Arena records sit
   at `arec+0/1/2` (`arec=7+3*nfix` already counts the header AND the code
   table), but the copy-pasted code-table literals made the validator read
   `arec+9` and every saved arena pointer looked "corrupt". **Landmine
   #3:** the fp declarator path created the symbol, ran the static branch
   and RETURNED before `s->ty=TY_PTR` was set, so `sf(41)` on an
   initialized static fp reported "not a function" — type the symbol before
   any early exit. Plus the usual `nafix`/`nafx` typo.
   **Found while verifying (pre-existing, not M24):** the array-decay
   element-type gap above — identical at HEAD on the same probe, so it is
   M25's job, and the two failing probe checks were moved to forms the
   model handles instead of being papered over. Error paths pinned: forward
   function name, prototype-only name, `&local` in a brace list, `&5`,
   string on an fp, a mutated literal's portability. 75 cases (ptrinit.c:
   24 checks + closer — global/local/static string tables, `&global`
   scalars and arrays, `int (*gf)(int)=f` global/static/struct-member/
   frame-member forms, a `char *tab[]` param walked by strlen, enum-folded
   members, retargeting a table slot, writing through a pointer slot),
   green x2, saved-object reloads 25/25/25 with structinit 22/22,
   arrinit 27/27, fptr 27/27 unchanged, fixture cross-checked against
   `gcc -std=gnu99` (25, after the fixture was changed to a writable
   array), clean `CC TESTS: 0 failed`.

 - [x] **M25** element type through array decay. Every array symbol now
   records what its ELEMENTS are (`s->ael` + `s->aelsty`, set by
   `CCSetArrTy` from the declarator immediately BEFORE array-ification —
   which overwrites `ty` with the decayed pointer shape and destroys
   exactly that information), params through `pael`/`paelsty`, and the
   parser hands the value on through decay (`cc->celem`/`cc->celebsty`)
   and every subscript (`CCArrEltSize` sizes the index by the element:
   1 for bytes, 8 for slots/pointers, the tag size for whole-struct
   elements; `CCArrNext` derives the NEXT level's element type, so
   `tab[0][1][2]` chains). Before this, the subscript arm GUESSED the
   element type from the base's scale, which was right exactly once per
   array: `tab[0]` produced TY_INT, so `tab[0][0]` loaded a dword and a
   store through it wrote 8 bytes, and `struct P *pa[]` — whose ty and
   tag look identical to `struct P ps[]`'s — was indexed by the tag size
   and could not reach `pa[0]->a` at all.
   **Landmine #1 — `TY_INT` is 0.** The first version stored the element
   type raw and used 0 for "absent", so an INT element WAS the sentinel:
   `int a[1]` looked like a non-array and kept its nil-deref bug, and
   sizeof regressed. Element types are therefore stored +1 (encode at the
   three write sites, decode once at consumption). A `CC_AEL(ty)` macro
   looked tidier and does not exist in this dialect: the HOST
   preprocessor has no FUNCTION-LIKE macros either (the very limitation
   PLAN documents for CC's own prep-lite), and `#define CC_AEL(ty)
   ((ty)+1)` broke the umbrella with `Invalid lval at "ty"`.
   **Landmine #2 — array PARAMETERS are not decayed objects.** Their
   storage is the argument slot, which HOLDS the pointer, so they need
   the load-on-value path (`is_addr=1`); routing them through the decay
   arm because they are arrays made `t[0]` read the frame slot itself:
   silently wrong element values through every `char *t[]`/`struct P *q[]`
   param (`t[0][0]` answered 96 for "hi"). Params still need `ael` — the
   element type and the decay decision are independent questions.
   **Landmine #3 — the 2-D hijack branch must RE-WRITE the element type.**
   Parsing the index expression clears `celem` (that is what keeps stale
   residues out of bases), and the row branch inherits its element type
   from the base rather than computing it, so it has to restore the saved
   value; skipping that cost `char *m2[2][2]` its third level.
   **Pre-existing hole found and fixed by the fixture:** `dim>1` gated
   array-ness at three sites (decay, `sizeof(ident)`, the call-through-an-
   array check), so one-element arrays behaved like scalar variables and
   `a[0]=5` stored through nil — reproduced at HEAD before being attributed
   to M25. `s->ael` is now the array marker at all three, which also makes
   `sizeof(char c[1])` 1 (C truth) and makes `a=5` on `int a[1]` the
   "assignment to array" error it always should have been.
   **Deleted rule, with evidence:** "lenient: index a plain int as slots",
   justified by `RED[0]` commuting — gcc rejects that program ("subscripted
   value is neither array nor pointer nor vector") and our lenient path
   dereferenced 97 as an address and faulted. Before deleting it the suite
   was run with int bases DIAGNOSED (76/76 green ⇒ nothing indexed a
   scalar), so the change was proven safe rather than assumed. Error paths
   pinned: scalar/byte-base subscripts, `pa[0].a` (C-correct "'.' needs a
   struct object" — an array of pointers yields pointers), 1-element array
   assignment. 76 cases (decay.c: 27 checks + closer — byte strides at both
   indices, NULL elements, `sizeof` of element and byte, `strlen` through a
   table, `char *t[]` walked by a param, `->` and `(*p).m` and `p[i][1]` on
   struct arrays, struct-pointer params, int-array identity through `+=`/`-=`,
   char-pointer `++`/`--` scaling by 1, byte stores through an element with
   the neighbour intact, quad stores through a struct member, 2-D pointer
   arrays at three levels, frame and static arrays), green x2, saved-object
   reloads 28/28/28 with ptrinit 25/25, scoped 49/49, arrinit 27/27
   unchanged, fixture cross-checked against `gcc -std=gnu99` — 28 with the
   same per-check increments and NO size adjustments needed this time
   (pointer and byte sizes are C-true here), clean `CC TESTS: 0 failed`.
 - [x] **M26** arrays of function pointers. `CCParseFunPtrDecl` learned
   the ONE bracket pair C puts INSIDE the parentheses — `(*tab[N])(int)` —
   because that is where the declarator grammar binds the array to `*tab`
   before the parameter list; the shape it already accepted is therefore
   exactly the no-brackets case. The count surfaces as `cc->fpdim` (reset
   at EVERY entry: callers peek with a snapshot and rewind on FALSE, so
   residue from a failed attempt is a lie to the next one), where -1 means
   `[]` — size from the initializer, or unknown because it is a parameter.
   Everything after the declarator is machinery that already existed:
   `CCReserveArray` lays the slots out (frame descending, arena ascending),
   `CCSetArrTy` records "the elements are pointers" (M25 — that is what
   makes `t[i]` a callable value instead of an int), `CCArrayInit` lands
   brace lists with deduced sizes, zero tails and excess-element errors
   (M22/M23), M24 records an arena relocation per code-address slot or an
   emitted store whose MOVABS operand relocates, and M18's
   `CCIndirectCallAt` performs the call. Calling itself did not change:
   an fp array is one type declaration getting the same treatment in three
   storage classes. The array PARAMETER needed one extra line (pael)
   because its brackets are consumed inside the declarator, so the ordinary
   array-parameter branch never sees them.
   Landmines, both small and both real: the empty-`[]` branch set `fpdim`
   without CONSUMING the `]`, so an unsized fp PARAMETER died one token
   later at `)` with "expected ')'"; and my first C cross-check of this
   milestone proved nothing because I wrote `%d` rather than `%%d` inside
   the SHELL printf that built the C file — the shell substituted zeros, so
   gcc "agreed" with an empty format, and the same file then printed
   `H=23376` on one run and `H=25213` on the next: not a difference between
   engines but an UNINITIALIZED `c` in my own probe (C leaves it garbage;
   our zeroed arenas make it look deterministic). Two rules for next time:
   build C probes with a heredoc, and when a number changes between runs in
   ONE engine, suspect UB in the probe before believing a compiler bug.
   Verification: 30 checks cross-run against `gcc -std=gnu99` with ZERO
   model adjustments (pointer sizes are C-true) and 0 mismatches check by
   check. The reload protocol is the point of this milestone: an fp table
   is the first arena object whose bytes are CODE addresses, so fptab was
   saved once and run three times — 31 31 31, with decay 28/28, fptr 27/27
   and ptrinit 25/25 unchanged (a stale entry would have called into the
   previous session's code buffer, which is a different address every load).
   Error paths pinned: `(*t[2][3])(int)`, `(*t)[2](int)`, size 0, `(*t[])`
   with no initializer, `typedef int (*tab[2])(int);`, fp-array struct
   members, excess elements, `b=a` on arrays. 77 cases (fptab.c: 30 checks
   + closer — sized and deduced frame tables, global sized and deduced, a
   `static` table that persists across calls and keeps a retargeted element,
   zero-filled tail, retargeting an element at run time, all three call
   spellings, loop dispatch through a table, `int (*t[])(int)` and
   `int (*t[2])(int)` parameters, a table element feeding a scalar fp and a
   struct member, element and array `sizeof`, NULL-vs-address identity,
   the same function twice yielding the same address, and a typedef'd
   `binop tab[3]`), green x2, clean `CC TESTS: 0 failed`.
 - [x] **M27** whole-object assignment. `v = w;` copies a struct or union
   slot by slot, and so does every aggregate-lvalue form the parser can
   already produce: an array element (`arr[1]=t`), a dereference (`*p=t`,
   `*p=*q`), a by-value member (`w=v`, `w.x=z.x`), a `static` or global
   object, and either direction of a frame<->arena copy. All of it lives in
   `CCAssignTail`: by the time it runs, BOTH sides are addresses (a struct
   lvalue IS its address — Primary never loads one — and the pending-address
   convention already has the destination on the stack), so the copy is a
   straight quad walk between RCX and RAX with nothing else live, unrolled
   at emit time because a tag's size is a compile-time constant. The guard
   is where the real insight is: `is_addr==2` covers arrays AND struct
   objects, because both are names that yield their own address. Pre-M27
   that meant `struct P x; x=y;` was refused with "assignment to array",
   while `*p=y` and `v=s` silently stored the struct's ADDRESS — read back
   as a huge number, so the program looked like it worked. The array case
   is now excluded BY TYPE, which is exactly the distinction C draws: an
   array is not modifiable, a struct is.
   **Deliberate non-delegation:** the obvious move was to emit a call to the
   `memcpy` builtin, and inline is better here. The builtin needs the
   argument-push sequence and a runtime length; the walk is 8 bytes per slot,
   records no relocation, scales by nothing that can be mis-scaled, and costs
   one instruction per slot. Delegating to the kernel is the rule when the
   kernel has the harder problem (sorting, printing, the FFI); copying two
   known quads is not it.
   **Landmine — `MOV RAX,[RAX]` destroys the source pointer.** That is the
   shorter encoding and the form every other load in this compiler uses, and
   it was wrong: iteration 1 replaces the source address with the value it
   loaded, so iteration 2 dereferences *that* (address 8, because an
   uninitialized first member is 0). It hid behind the tag size — a ONE-slot
   union copy ran perfectly, so the failure looked like a bug about multi-slot
   tags rather than about register lifetime. RDX is the accumulator instead.
   The safety argument, not the luck: live C values in this model live in
   RAX, on the stack, or in hidden frame slots; the only other RDX consumer
   is the CQO/IDIV pair behind `%`, which completes before the assignment
   tail runs. Grep the register (`grep -n "RDX" CC_*.HC`) before claiming it
   as scratch.
   **Fault mapping, since generated code needs it:** compile inside a driver
   that prints `cc.code` and `cc.entry_off`, dump the function's bytes, then
   run THE SAME image in the same session — a fault line's `rip` maps back as
   `rip - cc.code - entry_off`, a byte offset into the dump, which capstone
   decodes to the offending instruction. An attempt that dumped one source's
   bytes and then ran a different one pointed at a `push` two statements away
   from the truth: right method, wrong pairing.
   Also: the walk finishes on the LAST slot, and an assignment's value is the
   LEFT object, so RCX must step back `ADD RCX,-8*(ns-1)` before
   `MOV RAX,RCX` hands it over — `CCWord` truncates, so the negative immediate
   emits as a correct 2's-complement imm32.
   **C rules enforced rather than approximated:** both sides must carry the
   SAME tag (C makes differing tags a constraint violation even for identical
   layouts, so `union U u; struct P x; u=x;` diagnoses), a struct into a
   non-aggregate diagnoses where it used to store the address, and compound
   assignment and `++`/`--` on aggregates diagnose (the old messages called a
   struct an array, and `arr[0]++` passed the is_addr==1 lvalue test and would
   have bumped one slot of three). The copy is shallow, like C: two objects
   share the pointee, and retargeting one leaves the other pointed at the
   original — the fixture pins both directions of that. Reading slot i before
   writing it is what makes `x=x` and `*p=*q` safe without a temporary.
   **Found while probing (documented, not fixed): prefix `++`/`--` do not
   exist.** `++v` dies at the operator with "expected number, string,
   variable or '('", identically at HEAD — postfix only. It surfaced while
   testing the compound-assignment wording, and PLAN had never said so.
   Verification: 27 checks cross-run against `gcc -std=gnu99` with 0
   mismatches and ONE declared model adjustment (check 7 asserts
   `sizeof(x)==24` — `{int,char,int}` as three slots — where C reports 12;
   aggregate target sizes are the M15 slot convention, and the fixture says
   so in its comment). Error paths pinned: differing tags both ways, struct
   into a scalar, scalar into a struct, `x=y=5`, `x=v` (struct from an int
   array), `arr[0]++`, `u[0]++`, `x+=1`, `x++`, and the self-copy/aliasing
   forms that must NOT diagnose. 78 cases (agcopy.c: 27 checks + closer — the
   assignment-expression value `(x=y).a`, every member type including a char
   and a negative int, full overwrite, value semantics in both directions,
   copy of a copy, array elements and self-copy, three dereference shapes,
   pointer members with shallow aliasing and retargeting, union copies that
   alias a byte, frame<->global both ways, `static` objects assigned across
   calls, copying inside a loop), green x3, saved-object reloads 28/28/28 with
   fptab 31/31, decay 28/28, fptr 27/27, ptrinit 25/25 and structinit 22/22
   unchanged, clean `CC TESTS: 0 failed`.
 - [x] **M28** prefix `++`/`--` (and the postfix forms they exposed). M27
   was documented an hour before I noticed that `++v` does not parse at all:
   the probe batch that pinned M27's compound-assignment wording used `++v`
   as a control, and it died at the operator with "expected number, string,
   variable or '('" — identically at HEAD. Since `for (i=0; i!=n; ++i)` is
   the most common loop spelling in C, that is a bigger compatibility wall
   than anything else on the list, so it became this milestone. Two changes:
   a PREFIX arm at the top of `CCExprUnary` (recursing like the `-`/`!`/`~`/
   `&`/`*` arms already do, then checking that the operand left an ADDRESS
   pending), and the POSTFIX block MOVED OUT of Primary's identifier arm into
   `CCPostfix`'s loop, where the operator applies to whatever the chain
   actually produced. The move is what recovered `(*p)++` and `(v)++` — forms
   PLAN had told readers to rewrite as `*p=*p+1` since before M17 — with no
   new code at all: the paren group already leaves is_addr==1 for `(*p)=x` to
   work, and the increment simply stopped being restricted to ident roots.
   Both spellings then call ONE emitter, `CCIncDecAt(cc,inc,prefix)`, whose
   only difference is that postfix parks the old value on the stack and pops
   it back after the store. That is the structural win: the two spellings
   cannot drift in pointer scale, in byte-vs-quad width, or in what they leave
   in RAX, which is exactly the trio that has caused every increment bug in
   this compiler's history.
   **Landmine #1 — my own documented gotcha, new disguise.** I compressed
   `if (byte_aty) { CCByte(cc,0x88); CCByte(cc,0x01); }` into
   `if (byte_aty) CCByte(cc,0x88);CCByte(cc,0x01);`, which binds the guard to
   the FIRST statement only and leaves the following `else` with no matching
   if: "Missing expression at else", reported where the else is, not where the
   damage is. The audit I documented (`grep -n -A1 "else$"`) does not catch
   this shape, because the two-statement line sits BEFORE the else. Use
   `grep -n -B2 "^[[:space:]]*else$" CC_*.HC` and flag any line above an else
   that carries two `);` groups — or brace every guarded branch, which is what
   the emitter does now (including the ADD/SUB pair, emitted once for both
   spellings).
   **Landmine #2 — a splice that ended mid-line ate a statement, and the host
   segfaulted instead of telling me.** My python index-surgery cut the old
   postfix block by searching for its terminating `return;`/`}` and joined a
   replacement whose last line was a `//` comment with NO trailing newline, so
   the next line of the file — `if (CCAccept(cc,'('))`, the head of Primary's
   parenthesized-expression arm — was appended INTO the comment. Nothing was
   missing at the syntax level: the arm simply became a bare block and the
   umbrella compiled. Then every run died with a host core dump (exit 139),
   not a `[3d] FAULT` banner — including the control `int main(){return 1;}`,
   which is the tell that the compiler itself, not the test program, was
   broken. `git stash push -q -- T/CC/CC_PARSE.HC` reproduced a clean run at
   HEAD, and `diff -u` against the pre-edit snapshot showed the glued line
   immediately. Two rules: a splice's removed region AND its replacement must
   both end at a line boundary, and diff the whole file against the snapshot
   after any index surgery (that diff has now caught a bad splice and a bad
   revert anchor).
   **Semantics pinned by the fixture:** prefix yields the NEW value, postfix
   the OLD one, and the object moves either way; the result KEEPS the
   operand's type (the M15 rule, untouched), so `(*(p++))==22` and `(*++p)==22`
   both read the right slot and `++p` steps a whole 8-byte slot while
   `++w2[0]` steps one byte; `while(n--)` tests the old value and `while(--n)`
   the new (both pinned with the final value of `n`, so the two cannot be
   confused); `sizeof(v++)` neither evaluates nor stores (`CCByte` is gated by
   `noemit`, so the emitter emits nothing and `v` is unchanged afterwards);
   and `a[++i % 4]` survives the modulo's CQO/IDIV pair, which clobbers RDX
   and RCX AFTER the emitter has finished with them.
   **Found while verifying, pre-existing:** pointer DIFFERENCES are byte
   counts (`p-a` after two `++p` gives 16 where C gives 2) because subtraction
   is the plain integer subtract and nothing divides by the element scale —
   documented as a silent deviation rather than papered over, and deliberately
   not fixed here (it changes a binary-operator rule, and the fixture's whole
   point is that the WALK scales correctly). `int (*p)[3]`, which is legal C
   (pointer to array), was rejected with M26's function-pointer wording; the
   diagnostic now names both shapes and the supported alternative. Two helpers
   I added in M25 (`CCArrNext`, `CCSetArrTy`) took a `CCCtrl *` they never
   read, warning in every build; dropped rather than left.
   Error paths pinned: `++5`, `++f()`, `++(v=1)`, `++(v?u:v)`, `++--v`,
   `++v++`, `x++` on a struct object, `++arr[0]`, `arr[0]--`, `++a` and `a++`
   on arrays, `++m[0]` on a 2-D row, and the fp-array declarator forms. 79
   cases (incr.c: 41 checks + closer — prefix/postfix on int and char
   scalars, values old-vs-new, pointer steps in both directions, `*p++` /
   `*++p` / `++*p` / `(*p)++` / `(v)++`, char elements and byte elements
   reached through a `char *` table, 2-D elements, struct members through a
   pointer (including a byte member), union aliasing, a global, a `static`
   counter across calls, `while(n--)` vs `while(--n)`, a modulo index, a call
   argument, a condition, `sizeof` non-evaluation, and loops in both update
   spellings), cross-checked against `gcc -std=gnu99` with 0 mismatches and
   ONE declared adjustment (`sizeof(v++)` is 8 here, 4 in C), green x3,
   saved-object reloads 42/42/42 with agcopy 28/28/28, fptab 31/31, decay
   28/28, fptr 27/27, ptrinit 25/25 and structinit 22/22 unchanged, clean
   `CC TESTS: 0 failed`.

## HolyC dialect gotchas (all verified in this build)

- **Prototypes of functions defined in ANOTHER CC_*.HC belong in CC.HH's
  forward block**, not in the consumer's extern list: an `extern` that
  arrives after the definition is a promise from elsewhere and never
  resolves (see the rule below — M24 hit it as `Undefined Extern at
  &CCArrayInit+0x392` for `CCArenaPtr`, which is defined in CC_EMIT.HC,
  the file the umbrella splices first). Missing prototype → a call that
  silently runs nothing (M20); too-late prototype → a link failure (M24).
  When a brand-new call site misbehaves, grep its output for "at &".
- **There is NO `continue` keyword in this HolyC dialect.** The kernel
  source never uses it (grep T/*.HC: effectively zero hits), and
  `continue;` anywhere in host-side `.HC` code — even inside a plain
  `while` — dies with "Invalid lval"/"Undefined identifier at ";"",
  sometimes reported at a LATER brace line than the actual culprit.
  Restructure with if/else chains (break exists and works). CC's own
  C language has continue (its parser implements it); only the HOST
  dialect lacks it — M13's prep/type code hit this three times over.
- **A `//` comment swallows everything after it on that line**, so a
  spliced-together line silently DELETES a statement: my edit ended a
  replacement with a comment and no newline, `if (CCAccept(cc,'('))` landed
  on the comment line, Primary's paren arm became a bare block, the umbrella
  still compiled, and every run — including `int main(){return 1;}` — died
  with a host core dump instead of a HolyC FAULT. When EVERY probe fails,
  including your controls, suspect the umbrella and diff against the pre-edit
  snapshot; a splice's old region and new text must each end at a line
  boundary.
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
  frame objects. Since M24 both tables carry relocations (arena pointer
  slots included), so a code address CAN initialize an arena object — a
  function table is the case that needs it most.
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
- `++`/`--` is emitted by `CCIncDecAt` and BOTH spellings call it (M28):
  prefix from `CCExprUnary`, postfix from the arm inside `CCPostfix`'s loop,
  because that loop is where every chain result (subscript, member, deref via
  parens, group) lands. A new postfix arm is therefore incrementable for
  free, and a new operator that produces a modifiable target must leave the
  EMITTER'S PRECONDITION — is_addr==1, RAX holding the address, aty/ety
  describing what it points at — or ++/-- will diagnose it (correctly) rather
  than emit. Never re-inline the load/add/store sequence: the one copy is
  what keeps pointer scale, byte width and RAX's final contents identical.
- The M27 aggregate copy reads slot i BEFORE writing it (MOV RDX,[RAX] /
  MOV [RCX],RDX, then both pointers step), which is what makes `x=x` and
  `*p=*q` correct without a temp, and it uses RDX as the accumulator
  because `MOV RAX,[RAX]` would overwrite the SOURCE pointer with the
  value just loaded (the shorter form faults on the second slot). Before
  treating any register as scratch, grep its consumers: RDX belongs to the
  CQO/IDIV pair behind `%`, which finishes before an assignment tail runs.
  The copy's result state is is_addr==2 with TY_STRUCT (the lhs address),
  so `(x=y).m` and `x=y=z` chain; is_addr==2 therefore means "array OR
  struct object", and code that equates it with arrays must test the type
  (M27 fixed the assignment guard; compound assignment/`++` deliberately
  reject both).
- Any POINTER value written into the arena (a global, a `static`, or an
  initializer slot) goes through `CCArenaPtr(cc,off,kind,addr)`, never raw
  bytes: the arena is a fresh allocation on every `CCInit`, so an absolute
  stored without a record points at the compile-time arena and reads as
  stale-or-foreign data after a reload (M24: leaks across loads instead of
  resetting). Raw-byte stores stay correct for folded CONSTANTS, which is
  why the two paths are kept separate by `kind<0`. `CCOBJ_VER` is 2; old
  v1 objects are refused, not misread, so any change to the record layout
  means bumping it and re-checking the loader's validation of BOTH tables
  (code positions against clen, arena positions against alen).
- Element types (M25): `s->ael`/`cc->celem` are stored +1 (TY_INT is 0, so
  0 is free as the "unknown/not an array" marker) and `cc->celem` is only
  meaningful immediately after a Primary or a subscript arm — every
  expression entry clears it, so a NEW producer of a subscriptable base
  must write it, and the 2-D hijack branch must RESTORE the saved value
  because the index parse cleared it. Array-ness is tested with `s->ael`,
  never `dim>1` (dim is 1 for one-element arrays AND for every array
  parameter, whose slot holds the pointer and therefore keeps `is_addr=1`
  instead of decaying). New cross-file helpers of this unit get their
  prototypes in CC.HH's forward block, and no function-like macro exists
  in the host dialect to shorten the encoding.
- `cc->fpdim` (M26) is transient declarator OUTPUT, reset at the top of
  CCParseFunPtrDecl: every caller peeks with a snapshot and rewinds on
  FALSE, so residue from a failed attempt is a lie to the next one (the
  same discipline as `noemit`/`spec_static`). An fp-array symbol gets its
  element type from CCSetArrTy like any other array — an fp is TY_PTR, so
  `ael` is what makes `t[i]` callable — and its storage comes from
  CCReserveArray, never from a hand-rolled offset bump.
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
