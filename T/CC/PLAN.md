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

## Correctness policy

**The target is C's behaviour, not gcc's.** "Correct" means: the machine code
does what C99 requires for a program C accepts, and a program C does *not*
accept is not silently compiled into something that looks like it worked.
Three things are easy to conflate and are kept apart deliberately:

- **C's contract** — what a legal program computes. Not negotiable; a
  difference here is a bug, always.
- **lite's own model** — the arena, one slot per element, `sizeof` in slots,
  one translation unit, no section a definition could live in. These are
  design choices. Where one means lite cannot lay a legal program out the way
  C would, the choice and its consequence are written down below; that is not
  conformance, and it is not claimed to be.
- **gcc's diagnostics** — almost entirely quality of implementation. gcc
  compiles in `-std=gnu99` mode, has a warning channel, and C99 leaves the
  boundary between "warning" and "constraint violation" partly to the
  implementation. lite has **no warning channel**: a program is either
  compiled or refused. So lite neither matches nor aims to match gcc line for
  line on *which* messages appear, and a difference in that direction is
  recorded as a scope note rather than defended as conformance.

**What the verification actually measures.** Fixture VALUES against
`gcc -std=gnu99` — gcc builds and runs the same source, lite compiles and runs
it, and the exit codes plus the per-check marks are compared (see M14's
verification ritual) — and the preprocessor's output against `gcc -E -P` token
for token, because there a translation unit's token sequence IS the specified
behaviour. Error paths are exercised, but a refusal is judged by whether
silent acceptance would have produced wrong code, not by whether gcc printed
the same words.

**Why gcc is a witness and not the spec.** Its verdicts carry GNU extensions
and its own inline link semantics; its warnings are numerous and move between
releases; and on C's undefined behaviour its answer is one of several legal
ones. `T:/CC/TEST/unionc.c` scores 40 here; gcc gives it 36, but only on a
copy: as written the file does not build at all — `auto` and `register` at file
scope are errors (`:14`, `:12`) and C99 `inline` emits no definition for `ifunc`
(`:29`), so the link fails — the 36 needs those two lines rewritten plus
`-fgnu89-inline`, and the 4 points it loses are checks 8, 10, 15 and 29: the
byte model, the slot-sized `sizeof` of the int family, the union one-slot pun
and literal `sizeof` — four model deviations recorded below, none of them a
file-scope construct (`suggestion-0001` carries the per-check run). `int a[3];
a[5]=1;` is UB, which is why the two engines agree on it only by accident.
Where lite and gcc disagree about a program C considers legal, it is
either a bug or a paragraph in "C subset limitations" below, and the test for
telling those apart is: **does the difference change what the program
*computes*, or only what gets *printed* while compiling it?**

**Delegation is the default, because this is an adapter layer, not a standalone
C implementation.** Anything the host can already do, lite *should* get by
asking the host to do it, not by reimplementing it in generated C or in emitted
bytes. The mechanism is established and cheap: a zero-argument HolyC helper
(compiled by the host JIT, sitting in `CC_LIB.HC` or the `CC_PRELUDE`'s helper
array) reads `cc->ext[]` slots, uses whatever the host already has — F64
arithmetic, `StrLen`, `Prnt`'s formatting, `Sqrt` — and returns the answer in
RAX; generated code reaches it through `MOVABS RAX,&lib[slot]; CALL [RAX]`, a
relocatable call (`CC_B_LIB`) with no calling-convention reversal. Where a
host behaviour lite inherits, the host's behaviour *is* then lite's behaviour,
and any divergence from C that comes along with it is recorded as such instead
of being papered over by writing a second implementation.

Delegation is not mandatory in exactly two cases, and both must be argued in
the milestone entry rather than assumed: the host genuinely cannot do the job
(`qsort`'s comparator contract, `strdup`'s allocation contract, C's
initialiser ordering — the kernel has no equivalent, which is why M12's other
half is pure C), or the delegation costs more than the thing it replaces, as
M27's whole-struct copy argued (the builtin needed an argument-push sequence
and a runtime length; the inline walk is one instruction per slot and records
no relocation). Where the host does it *almost* right but the mapping needs
care — M37's `%f`, where the host's own conversion scales the value by a table
it builds with `Pow(10.,i)` and so disagrees with C on 143,569 of 631,424
cells, 22.74% — the delegation is still the first choice, and it is taken
WHOLE: lite rebuilds the directive as text, hands it to the kernel's engine
and prints what comes back, because a host that is a non-conforming
implementation of floating point is delegated to and then described, not
repaired piece by piece; the `getchar` bullet below already states that rule
for keyboard input ("documented deviation, not emulation").  An earlier pass
of M37 found the seam INSIDE the conversion — the scaling — and split the
work by step to keep the host's digits and take over that one step, which is
the shape this paragraph was written for; it measured 0.16% against that
22.74%, and was abandoned.  That is still the difference between an adapter
with documented seams and a compiler with an undocumented second
implementation hidden inside it.

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
`char s[]="text"` byte init) (M22), struct/union initializers (M23) and
DESIGNATORS (M30: `int a[8]={[0]=1,[3]=5};`, `int m[][3]={[1][2]=9};`,
`struct P p={.c=7};`, `struct P ps[2]={[0].a=1,[1]={.a=2,.c=3};`,
`struct W w={.v.i=5};`, `union U u={.ch='x'};`, out-of-order and mixed with
sequential items, indices folded as constant expressions)
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
`%d %s %c %%` lowered, and `%f` — flags, field width and a precision of any
size, all of them the KERNEL's rules — handed to the kernel's float printer
as text (M37); any other directive, `%x` among them, is a refusal and not a
silent narrowing), `putchar`, `exit`, and the
M12 stdlib subset — `strlen strcmp strcpy strchr memmove memcpy memset
memcmp malloc atoi abs puts srand rand` (delegated to kernel routines
where they exist; see M12), the M12.1 batch — `strcat strncat strrchr
strstr strncmp strncpy memchr strdup calloc` + `sprintf getchar fgetc
getc fputc putc fputs abort` (see M12.1) — the usual operators —
`++ -- & * []` (M28: both spellings of `++`/`--`, prefix and postfix, on
any modifiable lvalue the chain can produce), comma, short-circuit logic,
bitwise `& | ^`, shifts
`<< >>`, `~`, all ten compound assignments, chained
`a=b=5` — with C's full precedence chain (shift vs relational vs the
three bitwise levels are pinned by `bitops.c`), and C's pointer-arithmetic
rules whole (M29: `p+n`/`n+p` scale by the element step, `p-q` is an element
count, struct tags and 2-D rows step whole, and `p+q`/`n-p`/`p+=q` diagnose).
M15 adds `sizeof`
(C-truth sizes — see M15) and C casts (`(char)` truncates, `(char*)`/
`(int*)` retype pointers, `(struct T *)` keeps the tag for `->`).
M13 adds the type
keywords `const` (no-op qualifier), `unsigned`/`long`/`signed`/`short`
(fold to the single 64-bit int — no width distinction; note C-truth
`sizeof(short)` is 2, ours is 8), `void` (folds to the
byte model: `void *` is a byte pointer like memcpy's contract;
`f(void)` is the zero-arg idiom; anonymous parameters and `...` in
prototypes parse), `typedef` for scalar/pointer aliases (`typedef
unsigned long size_t;`), a line-oriented preprocessor-lite (function-like
`#define` with arguments, `#`, `##`, `...` and `__VA_ARGS__` since M31, and
`#if`/`#elif` expressions with `defined()`, `#elif`, `#error` and `#warning`
since M32, line continuations and a `#include "x.h"` that is searched for
beside the file that writes it since M33), and the
adapted-musl (MIT) headers in `T:/CC/INC/`: stdio.h, stdlib.h,
string.h, stddef.h (guarded, include each other by plain name).
M34 adds C's file-scope redeclaration rules: `extern int g;` DECLARES an object
without defining it, a declaration with no `extern` is a TENTATIVE definition
whose zero initializer is the zeroed arena it already sits in, every declaration
of one file-scope name binds ONE object at ONE address, only one declaration may
carry an initializer, and the length of an `extern int a[]` may arrive later —
from brackets or from an initializer — without the object moving.
M35 adds C floating point: `double` and `float`, literals (`1.5`, `.5`, `2.`,
`1e10`, the `f`/`F`/`l`/`L` suffixes folded), `+ - * /`, unary minus, all six
comparisons, `(double)`/`(int)` casts crossing the two models, conversion at
every boundary C converts at (assignment, an argument going through a prototype,
`return`, an initializer — aggregate, member, array and `static` ones included),
compound assignment, `++`/`--`, arrays of double with their element type
surviving subscripts and 2-D rows, `double` struct and union members,
`double t[]` parameters, functions returning `double` (and recursing through
that prototype), and `sizeof` — ONE floating width, every value computed by the
host's F64 operators, and by the same helpers at compile time whenever the
expression is constant. What is still not supported about floating point is the
bullet in this section named for M35.

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
Residual model choices, documented below: the slot-size convention for
`sizeof` of aggregates and `int`, ONE pointer code for every non-byte
pointee (`struct P **` is indistinguishable from `int *`), and fp arrays
having arrived at M26 with their own edges. Arithmetic and indexing stopped
being two different rules at M29.

Not supported — each currently fails with a compile diagnostic:

- Multi-declarator globals must not hide a function: `int a,b();` is
  an error — give functions their own declaration. (`int i,s=0;`,
  `int *p,*q;` and `int a=1,b=2;` all work, C's per-declarator star
  typing included.)
- Initializer edges (M22): every element must be a **foldable constant**
  — C permits variable elements in a FRAME array (`int a[2]={x,y};`),
  this model diagnoses "not a compile-time constant" there and requires
  constants everywhere else anyway. Brace groups follow C's SUBOBJECT walk
  (M30 restated it correctly; M22's "a group opens the next element" was
  right only when the cursor sits at an element boundary): an enclosure takes
  the next subobject in order, which is a whole element at an element
  boundary and the NEXT SINGLE SLOT partway through one — `int m[2][3]`
  accepts `{{1,2,3},{4,5,6}}` and the flat `1,2,3,4,5,6` alike, `{{5},9}` puts
  9 in m[1][0] (the closed group finished row 0) and `{1,{5}}` puts 5 in
  m[0][1] (the group took the next slot of the row being filled). Over-full
  targets ERROR: `{{1},{2,3},{4}}` on a 1-D array and `{{5},{9},8}` on a
  `[2][3]` are excess elements — C warns and drops, we ERROR (the same stance
  M22 took, and C's "initializer-string for char array is too long" for a
  too-long row literal is a third case). A 512-value cap (CC_INIT_MAX) bounds
  one initializer. A 2-D tag array DECLARS since M25 but its initializer
  still diagnoses ("one dimension only"); 1-D tag arrays initialize as
  ordinary aggregates.
  function-pointer arrays initialize since M26
  (`int (*gt[3])(int)={inc,dbl,neg};`); pointer elements work since M24
  (strings, defined function names, `&globals`) because the arena slots
  they fill carry relocations of their own. An array whose length no
  declaration stated (`extern int a[];`, and `int a[];` with no initializer) is
  ONE SLOT, silently, since M34 — gcc warns "assumed to have one element" and
  compiles, and lite has no warning channel; see M34. A brace list longer than
  the slots that exist is refused where gcc warns.
  Braced SCALARS are accepted (`int x={5};`).
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
  and byte element sizes are C-true. The struct-pointer inconsistency this
  bullet used to describe (`ps+1` stepped 8 while `ps[i]` strode the tag)
  is closed: since M29 arithmetic and indexing read the SAME element step,
  so `sp+1`, `sp+=1`, `sp++` and `sp[1]` all move a whole tag.
- `else if` chains parse; nested `switch` works (M14 — but nested
  switches exposed the sharing bug: per-statement parse state must be
  CALL-LOCAL, the ctrl-array version let an inner switch overwrite its
  parent's case table mid-body).
- `switch` case values must be compile-time integer constants — M20's
  folder handles literals, enum constants and folded expressions, and M35 added
  the KIND check (`CCConstWantInt`): a floating case value, and a floating
  `switch` controlling expression, are diagnosed rather than compared as quads;
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
  types and struct tags so `(c?&p1:&p2)->x` works — except that since M35 a
  pair with ONE floating arm is refused rather than folded, because the middle
  arm's conversion has to happen inside the middle arm's own path.
- **Floating point (M35): ONE width, delegated whole to the host's F64.**
  `double` and `float` are the same thing here — the host has no single precision
  at all — so `sizeof(float)` is 8 where C's is 4 and a single-precision
  computation keeps more bits than C would. Every float VALUE comes from the host,
  so the host's differences from C are lite's and are listed here instead of being
  papered over by a second implementation. Measured in ULPs of the value C requires
  (the spacing of doubles at that value, so the errors are whole): plain decimals
  are exact except `0.15` (+1), and a scaled form lite could not fold to a plain
  spelling drifts with its exponent — `1e10` -23, `1e19` -16, `1e100` -117, `1e308`
  -160, `3.141592653589793` +9 — and the host's own literal parser drifts the same
  way, so `0.1 + 0.2 == 0.3` is TRUE here and FALSE in C. A scaled form lite COULD
  fold (`1e2`, `1e10`, `100e-2`) is exact, because the fold moves it onto the
  accumulate-then-convert path. A decimal integer literal with no dot and no
  exponent (`double x = 12345678901234567890;`) is not a float spelling at all and
  takes the INTEGER path, which sums into the same unchecked I64: lite holds
  -6101065172474983726 where C holds 12345678901234567890, which matches C for
  integer use (6.4.4.1/5 gives such a constant `unsigned long long`, whose bits are
  that quad) and differs only on the way to a double — so this file is -6.1e18 here
  and 1.2345678901234567e19 in C. Fixing that means converting the quad as unsigned
  when the destination is a double, and it is M38's (M37 took `%f` instead). The
  truthiness of a double is
  "the quad is nonzero", so `-0.0` is truthy here and falsy in C; a hex float
  literal (`0x1p3`) is a syntax error because the kernel answers 0 for one; `%`,
  `~`, `& | ^`, `<< >>` and `switch` on a floating operand are refused, as are
  `case` labels, array lengths, enumerators and designation indices with a floating
  value — C forbids all of them, and the integer path would have computed the bit
  pattern of no answer. **`double *` is refused at every declarator site** (local,
  global, parameter, struct member, typedef and cast): the pointer model records no
  pointee type, so `*p` would be the integer model applied to a bit pattern —
  `double a[4]` with `a[i]`, and a `double t[]` parameter, are how several doubles
  travel today. A function pointer may not return a double, and its parameter list
  may not contain one (M18 records neither an fp's return type nor its argument
  types); a conditional with one floating and one integer arm is refused (the
  middle arm's conversion cannot be inserted after its jump was emitted); and
  there are no math builtins, so `sqrt(x)` is a call to nothing yet. All of that
  is on M38's list; `printf`'s and `sprintf`'s `%f` arrived in M37.

  The 19-digit spelling this bullet is about is not merely imprecise, it is a
  DIFFERENT number, and one measurement shows how far: with `a` holding 0.0, the
  test `a >= 9223372036854775808.0` answered TRUE, because the literal on the
  right is not 2^63.  The host says the limit out loud — `T:/STRB.HC:529-534`,
  "Does not allow more than 18-digits before or after the decimal point because
  the numbers before and after the decimal point are stored in 64-bits" — and has
  no error path for breaking it, so the overflow is silent and the answer is
  whatever the wrapped accumulator says (measured: -6101065172474984 for
  `12345678901234567.891`, whose true integer part is 12345678901234567).  That is
  also why no value in M37's fixture or sweep is spelled as a decimal float
  literal: the constructions build a significand and an exact power of two
  (`T:/CC/TEST/ffmt.c:27-45`) so that lite, its harness probe and the kernel
  all format one and the same bit pattern.  **[M37's first pass read the same
  fact as two magnitude gates inside `cci_fmt`, testing the exponent FIELD
  (1086 for 2^63, 1075 for 2^52) rather than comparing against a literal,
  because an exponent field is an integer and a 19-digit literal is not. Those
  gates shipped with that abandoned design and are not in the code now; see
  M37.]**
- Integer literals have NO suffixes here: `12u`, `12ul` and
  `12345678901234567LL` are syntax errors (the number token ends at the digits
  and the suffix is then an identifier), while the FLOAT suffixes M35 folds are
  accepted. A C-legal spelling that is refused beats the same spelling read
  wrongly, but `1LL << 40` has to be written `1 << 40` in this dialect. The
  neighbouring fact, measured in the same run: a literal that does not fit the
  SIGNED quad wraps into it instead of taking C's unsigned literal type, so
  `9223372036854775808` is -9223372036854775808 here and `0xFFFFFFFFFFFFFFFF`
  is -1, where C's literal is `unsigned long long` and both values are positive
  (6.4.4.1/6).
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
  File-scope redeclaration is C's rule since M34 (see M34): `extern` declares
  and does not define, a declaration without `extern` is a tentative definition,
  one name is one object at one address, and `int g; int g;` costs ONE arena
  slot. What survives of the old scope-deviation list: a block-scope
  `extern int x;` becomes an ordinary local (it does not link to a file-scope
  x), and `int`, `long` and `unsigned` are ONE type in this model, so a
  redeclaration that swaps one for the other cannot be seen, let alone
  diagnosed.
- File-scope redeclaration edges (M34), pinned by a 44-row verdict matrix
  grouped by the two questions the correctness policy asks: 24 programs C
  accepts match VALUE, all 11 programs C refuses are refused (3 of them at
  compile time where C's linker would say it, because lite is one translation
  unit), and 9 rows are choices lite makes on purpose, listed just below, with
  nothing unexplained. Those choices: an `extern` object that nothing in the
  file defines is an error when something reads it and is silent when nothing
  does, which is exactly the question a linker asks; **an array whose length no
  declaration ever stated is one slot, silently** — `int a[];` with no
  initializer and `extern int a[];` — because a file-scope object that IS used
  needs a real object somewhere, lite has no warning channel to explain an
  incomplete type, and one slot is the same assumption lite already makes about
  a subscript past the declared length (`int a[3]; a[5]=1;` compiles and
  writes); consequently `sizeof` of such an array answers its one slot where C
  measures bytes, and a brace list of more elements than the slots that exist
  is refused where gcc warns (`int (*gt[])(int); ... = {f,g};`); a late array
  length is REFUSED outright when another global was declared between the
  incomplete declaration and the definition — C lays such an array out anywhere,
  lite bakes `locals + offset` into everything compiled in between, so it says
  so rather than put one object's elements on top of another's, and that is an
  IMPLEMENTATION LIMIT, not a C rule; `static` after `extern`, and `int g;`
  then `long g;`, are accepted where C6.7.5 and C6.2.7 diagnose — one int type
  and one translation unit make the difference unobservable here — while
  `char g; int g;` is refused, because byte elements really are a different
  object; `struct` and `union` tags share one namespace and the kind must match
  (C6.7.2.3), so `union pt` after `struct pt {…}` refuses; an unbraced
  function-pointer array redeclaration is treated as the one-slot object its
  first declaration laid out; a struct or union BODY behind a storage-class
  keyword (`extern struct q { int a; } v;`) is not a path lite knows — define
  the tag on its own line, then declare the object; and a block-scope
  `extern int x;` is still an ordinary local, the one divergence here that
  changes a VALUE rather than a message.
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
  does not read); elements of pointer-arrays (`char *ap[2]`) became
  element-faithful at M25 — `ap[1][0]` strides and loads BYTES through a
  `char *` element (what used to answer 31096 for "xy"), and a pointer
  ELEMENT is still a slot, which is C's size for one; the model keeps one
  pointer CODE (`struct P **` and `int *` are both TY_PTR, so `**pp` needs
  the temp-variable form), just not one element size;
  subscripting a string LITERAL works since M19 (`"ab"[1]` is 'b', and
  `*("ab")` too) — and it writes into the WRITABLE data arena, so
  `++"ab"[1]` really does change that pool string for the rest of the run
  (C: undefined behavior; a variable is the portable spelling);
  `++`/`--` on a 2-D array BASE diagnoses (arrays are not incrementable,
  M27/M28) while `m+n` steps a whole ROW and `(m+1)[0][0]` indexes it
  (M29); `int (*p)[3]` (pointer to array) is a diagnostic declarator;
  array PARAMETERS discard the first dimension (the caller owns the
  count) and keep the second; struct members may not be arrays.
- Pointer arithmetic follows C's rules since M29 (`ptrmath.c`, 32 checks
  cross-checked against gcc): `p+n` and `n+p` scale by the ELEMENT step in
  either operand order, `p-q` is an element COUNT in units of the left
  operand's step (not a byte count, and the result is an int, so `a[p-a]`
  and `(p-a)/2` behave), `p+q`, `n-p`, `p+=q`/`p-=q` and any arithmetic on a
  struct OBJECT diagnose as C's constraint violations. The step is
  `CCStepOf`/`CCPtrStep`, one answer shared by `+ - += -= ++ --` and the
  subscript scale: 1 for bytes, 8 for slots and untagged pointers, the TAG
  SIZE for `struct P *p` and for `struct P st[]`, and the ROW size for a 2-D
  array. Two model choices survive: mixing a tagged pointer with an untagged
  one is ACCEPTED (a slot pointer cannot be told from `int *`/`void **`
  here) and divides by the left operand's step; and `sizeof(*fp)` still
  reports 8 where C rejects a function type.
- `sizeof`/cast edges not supported: `sizeof` needs parentheses (and
  its expression form decays arrays to pointer size there — `sizeof
  f(x)` and a bare `sizeof x` are diagnostics, and the bare-identifier
  form `sizeof b` is the only route reporting array element counts);
  `sizeof(...)` inside a global initializer (or any constant-expression
  site: case label, array size, `static` init) is "not a compile-time
  constant" — M20's folder folds literals, enum constants and arithmetic,
  but not `sizeof` or address values; `(struct T)` non-pointer casts
  yield the address as an int; `sizeof(function)` is a diagnostic.
- Preprocessor pieces that lite skips — M31 closed the function-like
  `#define F(a)` gap (arguments, nesting, variadic tails, `#` and `##`) and
  M32 closed the conditional gap (real expressions in `#if`/`#elif`,
  `defined()`, `#elif`, `#error`, `#warning`), and M33 closed the two shape
  gaps that were left: `\` at the end of a line now splices the next one in,
  and a quoted `#include` name is searched for in the directory of the file
  that contains the directive before the header home is tried (a
  drive-qualified or leading-`/` name is still opened exactly as written, and
  `<name>` still goes only to `T:/CC/INC/`). What is left of M13's list is
  `#line` and `#if` interacting inside block comments (prep is line-shaped;
  keep conditionals and block comments disjoint, as real code does). Three
  function-like edges stay open on purpose, all three pinned against
  `gcc -E`: an
  argument LIST that runs off its line is an error here and fine in C
  (line-shaped again); a name that reaches the scanner only by
  SUBSTITUTION is not rescanned, so `#define APPLY(f,a) f(a)` invoked
  with a MACRO name for `f` leaves `TWO(3)` as text and the C front end
  says "call to undefined function" — loud, not a wrong value; and a
  `#` or `##` in an OBJECT-like body is left as written (gcc pastes
  `#define OB a##b` to `ab`). Four conditional edges stay open on purpose,
  all four pinned against `gcc`: a `sizeof` in a condition is not an operator
  here (it reads as a name, so the expression then has trailing tokens — same
  verdict as C's rejection, different wording); a `defined` written in a macro
  BODY is a portability warning in C and is accepted in silence here; `#pragma`
  is CONSUMED by lite while `gcc -E` forwards it to the compiler proper, so a
  file containing one cannot be diffed token for token; and `#warning` is a
  note line here where C99 has no such directive at all (gcc calls it an
  extension, and `-pedantic-errors` turns it into an error). Three
  continuation edges stay open on purpose, all three pinned against `gcc`:
  lite splices where C99 says a splice needs the `\` and the new-line adjacent
  and gcc only warns (`backslash and newline separated by space`), it splices a
  `\` + new-line that ends the FILE (gcc warns `backslash-newline at end of
  file`) while the backslash that ends a file with NO new-line stays an ordinary
  byte and is a stray-character error in both engines, and it never says
  anything, so all three costs are silence where gcc prints a warning — never a
  different translation unit. Two path limits bound where a relative include can
  reach, and neither is new with M33: only `CC_TOK_MAX-1` characters of an
  includer's directory are kept, so a chain of directories deeper than that stops
  resolving relative names, and the file layer caps a NAME at 37 characters —
  probed through `FileFind` and `FileRead` on both drives, a 37-character final
  component opens and a 38-character one is simply not there, while a 153- and an
  182-character PATH of short components opens happily. A header whose own name is
  38 characters or longer therefore cannot be included at all, from any rule.
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
  values in its group are excess, like C) and since M30 a member
  DESIGNATOR names which member that value belongs to, though every union
  member still lands on the one slot the tag occupies, and a by-value
  one-slot member takes the one quad that IS that sub-object (M30: a
  designation written on a scalar tag object names a MEMBER, so
  `{.b={5}}` writes b, and a group inside such an object takes ONE member
  slot rather than the tag — `struct Box gb={{77},8}` puts 8 in `gb.t`,
  which M23's fixture pins). Only a 2-D tag array's INITIALIZER diagnoses (the
  declaration itself works since M25); a
  function-pointer member initializes from a DEFINED function name (M24
  arena relocations made arena code addresses
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
- Builtin names bind BUILTINS: `cci_fmt strlen memcpy memset memcmp malloc
  getchar` (like `printf`/`putchar`/`exit`) are dispatched by name
  before the symbol table, so a user function of the same name is
  shadowed.  `cci_fmt` is in that set although no C library has that
  name: it is lite's `%f` DELEGATION (M37) — it formats nothing, it hands
  the directive and the value's bits to the kernel's float printer — and
  name dispatch is the only route a prelude function has to a lib slot:
  the runtime sprintf engine has to reach the same helper printf's
  compile-time lowering emits, so one function serves both sinks.  It is a lite
  extension of the namespace and not a C name, so a program that
  spells its own `cci_fmt` gets lite's helper and never its own,
  with no diagnostic.  The pure prelude half (`strcmp strcpy strcat strncat
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
- `sprintf` interprets its format AT RUNTIME (the format may be a variable — the
  point of the engine): `%d %s %c %x %f %%`, at most 4 conversion arguments (extra
  user args are a diagnostic; missing ones read as 0). What follows the `%` —
  flags (`- + space #`), a width, a precision — is BUFFERED AND REBUILT AS TEXT
  (M37), and `%f` hands that text to the kernel's float printer, so width,
  padding and precision there are TempleOS's rules and not the engine's; every
  other letter gets the flag/width run as TEXT and keeps its own arm, which is
  why `("%5d %d",5,7)` now writes `55 7` and `("%*d",10,7)` writes the first
  argument. A precision IS READ for every directive and only `%f` acts on it, so
  `%.2d` prints the number where it used to print `.2` as text — the `.2` is now
  dropped rather than printed — and it is not C's minimum-digit padding; an
  unrecognised directive character after the `%` is emitted as text and CONSUMES
  a conversion argument, as it always has. `%f` takes a precision of ANY size and
  caps it at 600 SILENTLY, with the field width capped at 1000, because
  `StrPrintJoin` takes no buffer length and a runtime sink has nowhere to report
  (`CCIClap`, `T:/CC/CC_LIB.HC:358-434`); printf, which reads its format at
  compile time, shares those caps and keeps its own refusal for a directive
  whose letter is not `f`.
  `%x` prints the QUAD'S UNSIGNED READING, which is what C and the kernel both
  print for a 64-bit `int` (M36; it used to print the magnitude, which answered `1`
  for -1 and a single `0` for the most negative quad, a value with no positive
  magnitude to print), and the return is the written-character count — never
  pointer subtraction (arena pointers are relocated per session; subtraction across
  a saved-object reload is a different arena). `snprintf` deferred: per-write bound
  guards would bloat every conversion site; the engine writes unguarded into the
  caller's buffer (caller's responsibility, same as C UB territory).
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
| `CC_LEX.HC` | lexer, `CCError`, and the shared escape table (M32) |
| `CC_EMIT.HC`| machine-code emitters, prologue/return, patches  |
| `CC_SYM.HC` | symbol table, variable load/store                |
| `CC_PARSE.HC`| parser, direct emission, M34 merging, M35 float routing |
| `CC.HC`     | umbrella include, `CCInit`/`CCSrc`/`CCRunFile`, `CCSave`/`CCLoad` |
| `CC_PREP.HC`| M13 preprocessor-lite + M31 macros + M32 conditionals + M33 continuations and relative includes (`#include`/`#define`/`#undef`/`#if`-family), runs on user text before the prelude |
| `CC_LIB.HC` | M12 + M35 delegation helpers and `CCFold` (in `CC.HC`) |
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
  through the kernel's format engine (`StrPrintJoin`, T:/STRPRINT.HC,
  extern'd in KERNELA.HH) via `cc->ext` staging is a possible further
  delegation, deliberately not done here (C's format-flag semantics
  would have to survive the mapping).

  **[The delegation rule in "Correctness policy" now asks that question again, and
  `%f` is where it comes due. M35 turned out to be the F64 value model rather than
  printing, so the conversion became M36's subject: its integer half landed (`%d`
  exact at the most negative quad, `%x` reading the quad instead of the magnitude),
  and `%f` is the part M36 did not reach; the kernel formats F64 natively
  (`A="%6.3f"` in a DolDoc string, KERNELA.HH), so writing a decimal-float dtoa
  from scratch in the prelude would be the second implementation the rule warns
  about. suggestion-0002 carries the mapping probe. M37 ran the measurement that
  probe was predicting would settle the question, and the prediction did not
  survive it: routing a double through the kernel's `%f` disagrees with C on
  143,569 of 631,424 (value, precision) cells — 22.74% — and on 187 of the 342
  cells of the direct 38-value probe, 91 of them at precisions stated at or
  below 15. So it fails at precisions the "15 significant digits" rule called
  safe: `%.15f` of 1.5, a value with no rounding problem at all, comes back
  `1.499999999999998`. The cause is in the host and not in the precision:
  `T:/STRPRINT.HC:555-599` scales the value by `Pow10I64(p)` (`:564`, `:567`,
  `:572`), and `T:/KEND.HC:13` fills that table with
  `pow10_I64[i+309]=Pow(10.,i)`, a transcendental (suggestion-0004 files it).
  So the rule the probe offered — state the precision, keep it at or below 15,
  own the special values — is not sufficient, and neither is the workaround
  M37 first built from it: scaling only the FRACTION on an exact integer-quad
  factor, which measured 0.16% and was abandoned. M37 as shipped delegates the
  conversion WHOLE and records what the host does instead of engineering around
  it — see the M37 entry.
  That suggestion also records a DEFECT it found (lite's `%d` of the most negative
  quad printing `-(`), filed separately as a bug and closed by M36: the walker no
  longer negates its own input, and 144 values across the signed-quad range were
  pinned twice each -- written length and exact content -- with 288/288 matching
  `gcc`'s `%lld` and `%llx` output.]**
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
   **[M31 replaced the depth-bounded rescan with a per-expansion disable
   mask (blue paint) and added function-like macros: arguments, `#`,
   `##`, `...` and `__VA_ARGS__` — see M31.]**
   `#ifndef`/`#else`/`#endif` (three-bit frames: emit/chain-taken/
   parent-emit — an inner `#else` cannot resurrect an outer-skipped
   region), `#if 0`/`#if 1`, `#pragma`; anything else is a loud error.
   **[M32 grew the frames to four bits (it adds "an #else has already run at
   this level"), replaced the literal-only `#if` with a real expression
   evaluator plus `defined()`, and added `#elif`, `#error`, `#warning` and the
   unterminated-`#if` check — see M32. What the evaluator cannot fold is still
   rejected: no `sizeof`, no string literals, and a name that survives
   expansion is 0, exactly as C says.]**
   **[M33 put a line-shape step in front of the line loop: a trailing `\`
   splices the next physical line into the one the scanner sees, so a
   directive, a comment and a literal are all recognised on the JOINED line,
   and it made `"x.h"` resolve against the directory of the file that contains
   the directive before `T:/CC/INC/` (while `<x.h>` still goes only to
   `T:/CC/INC/`) — see M33. Skipped regions still emit one newline, now per
   PHYSICAL line consumed, so a splice shifts nothing.]**
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
   them; **[the table was written out TWICE in CC_LEX.HC here and the two
   copies had already drifted — one handles `\'`, the other `\"`. M32 replaced
   them with ONE `CCEscape`, which the `#if` character-constant reader uses
   too; the quote escapes stayed per-site because they differ by context, and
   `CIsHexCh` came out of the same change.]** `\0` in a string lands a NUL,
   which the pool stores as the terminator (strlen("a\0b")==1 — C-truth by
   construction). String
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
   and `1,2,3,4,5,6`. **[M30 restated this mapping as C's subobject walk:
   "a group opens the next element" holds only at an element boundary of
   an array, and a closed group resumes AFTER the element. The two rules
   coincide for arity 1, which is why every M22 fixture agreed with C
   without proving the general form — see M30.]** Size deduction reads
   the same mapping (highest element touched), so `int a[]`, `int m[][3]`
   and `char s[]="ab"` all
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
   array, and the designator forms M22 rejected (all of them implemented
   by M30, where the same shapes became C-truth checks). 73 cases
   (arrinit.c: 26 checks + closer — local/global brace lists, deduction
   three ways, tail zeros, `{}`, nested row
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
   in C; M30 adds member designators, which validate the name and then all
   land on the union's single slot). Tags gained an `isu` flag, written
   on BOTH definition paths because the M21 waterlines RECYCLE tag slots:
   a flag set only in the
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
   **[M34 added the tentative definition: `extern int a[];` is completed by a
   later declaration of the same name in this file, or — if none ever states a
   length — becomes the ONE slot it already owns, and an `extern` object that
   nothing in the file defines is a diagnostic when something uses it; see M34
   and the limitations bullet. "An arena slot has no patch list" is unchanged,
   and so is why a late array length can only extend the arena above its own
   slot, and never into slots somebody else claimed.]**
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
   PLAN documents for CC's own prep-lite) **[M31 lifted that for CC's OWN
   prep-lite; the HOST dialect still has no function-like macro, which is
   the part that bit here.]**, and `#define CC_AEL(ty)
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
 - [x] **M29** pointer arithmetic gets C's rules. The deviation M28 had just
   documented ("pointer differences are byte counts — until it is fixed,
   carry an index") became this milestone, and fixing it turned out to be one
   question, asked in one place: HOW MANY BYTES IS ONE STEP OF THIS VALUE.
   `CCPtrStep` (for a pointer value) and `CCStepOf` (which also takes the
   residue fields: row scales first, then the M25 element marker) answer it,
   and every consumer now goes through them: `p+n`, `n+p`, `p+=n`, `p-=n`,
   `++`, `--` and the subscript scale. `CCPtrScale`, which answered from the
   TYPE CODE alone, is deleted. C's operand rules went in beside them in
   `CCExprAdd`: `p+q`, `n-p`, `p+=q`/`p-=q` and arithmetic on a struct OBJECT
   are constraint violations and diagnose; `p-q` yields an element COUNT,
   typed TY_INT with its residue cleared so the next subscript cannot scale
   it twice. What was silently wrong before, all of it: `p-a` on `int *` was
   a byte count (16 where C says 2) so `a[p-a]` indexed 16 elements out;
   `2+a` added two BYTES to an array address; `st+1` on a struct array added
   a byte to a whole-tag stride so `st+1 == &st[1]` answered FALSE; `m+1` on
   a 2-D array moved one element instead of a row; `p+q` produced an address
   SUM typed as a pointer; `s+1` did arithmetic on a struct's own address;
   and `sp+=1` on a struct pointer moved one byte because the compound path
   multiplied only when the scale was exactly 8 (ptrmath checks 22/23 — gcc
   passed them, we failed them). `p++`/`p+=1` on a `struct P *` now step a
   whole tag, which retires the inconsistency PLAN had carried since M25
   between struct arithmetic (8) and struct indexing (the tag): subscripting
   already strode the tag, and the two rules now read the same number.
   **A step is not an element size.** `CCArrEltSize` (M25) answers "how big is
   one element" and `CCPtrStep` answers "how far does one step go", and a
   pointer gives two different numbers: `struct P *pa[2]` has 8-byte elements
   (they ARE pointers) while `struct P *p` steps a whole tag. Keeping them as
   two functions is what makes `pa[i]` and `p+i` both right; merging them, or
   answering both from a type code, is the trap `CCPtrScale` was in — a type
   code cannot answer either question once tags exist.
   **Landmine — residue only means something to a value that can have a
   shape.** My first `CCStepOf` consulted `celem`/`esz2` unconditionally.
   Those fields are written by the arm that PRODUCES a base and cleared by
   only SOME of the layers that combine values, so a parenthesized comparison
   came back carrying its last operand's element marker: `tot + (fp[0]==fq[0])`
   looked like int + pointer, took the reversed-operand branch, multiplied
   `tot` by 8 and added a pointer to it. fptab answered 1487 instead of 31 and
   sizeof 117 instead of 26 — 79 green cases dropped to 77 on the first run.
   The fix is a type-first guard: only TY_PTR/TY_CHARP/TY_STRUCT can have a
   step, and a scalar's residue is simply ignored. The same discipline moved
   the scalar-subscript diagnosis AHEAD of the element-marker branch, so `5[0]`
   is refused whatever the residue says. Clearing residue at every producer
   was the tempting alternative and it is worse: there are a dozen of them,
   new operators get written, and the authority should be the TYPE. Two
   residue-carrying values survive on purpose: `*pp` keeps its pointee's
   marker so `*pp+1` and `(*pp)[0]` work through the flattened `int **`, and
   `&st` keeps the array marker because a pointer to an ARRAY is not
   representable here (its step is approximate, and `p - &st` mixes units).
   **Landmine — the ModRM byte names the DESTINATION in its reg field.**
   `IMUL r64,r/m64,imm32` with RCX on both sides is `48 69 C9`, and my first
   try wrote `C1`, which is `IMUL RAX,RCX,imm32`: it multiplied the POINTER,
   stored into RAX, and left the UNSCALED integer in RCX for the ADD that
   followed, so `2+a` became `a*8+2` and faulted on the next load. Every other
   IMUL in this file targets RAX with `0xC0` and copies without complaint,
   which is why the wrong encoding looked like it worked. Capstone named the
   bug the moment the bytes were decoded (`rip - cc.code - entry_off` maps a
   fault into the dumped bytes); the bit arithmetic `mod 11 | reg<<3 | rm` is
   worth writing out whenever a register other than RAX is the destination.
   **Landmine — a fixture comment missing its `*/` deletes an assertion, and
   both engines delete it identically.** Polishing check 12's comment dropped
   its terminator; check 13's assertion then lived inside the comment, gcc and
   CC agreed on everything, and only the mark count (31 printed for 32
   assertions) showed a line had vanished. Two rules: check that a `/*`-style
   comment is closed on the same line or ends with `*/`, and COUNT THE MARKS —
   a cross-check compares engines against each other, never against the
   fixture's intent, so it is blind to a source both of them misread.
   Verification: full suite re-run after each edit (the two regressions came
   from a rule every existing fixture touches, and only the full run could see
   them), then the marked-copy diff against `gcc -std=gnu99` — 32 checks,
   0 mismatches, NO model adjustment needed (element semantics never touch
   byte sizes), and the two checks that first failed were ours, not the
   oracle's. Error paths pinned: `p+q` for slot pointers and for byte
   pointers, `2-p`, `p+=q`, `p-=q`, `s+1`, `1+s`, `p-s`, `int *` minus
   `char *`, `struct P *` minus `struct Q *`, `p-[1]`. 80 cases (ptrmath.c:
   32 checks + closer), green x3, saved-object reloads 33/33/33 with agcopy
   28/28/28, incr 42/42/42, fptab 31/31, decay 28/28, fptr 27/27, ptrinit
   25/25 and structinit 22/22 unchanged, clean `CC TESTS: 0 failed`.
 - [x] **M30** initializer designators, and the brace-elision rule they
   exposed. `CCInitDesign` parses one designation chain (`[0]`, `[0][1]`, `.a`,
   `.v.i`) and resolves it against the TARGET SHAPE the writer owns — a 6-slot
   bundle (`CC_KI_SCALAR`/`ARR`/`TAG`/`UNION`, outer and element descriptors
   plus `charr` and the mapper's arity) — because whether `[i]` is legal depends
   on the type of the object the value sits in, and a chain crosses from one kind
   to the other exactly as the declarator does: `[1].a` needs the array to pick
   the element and the tag to pick the member. Two levels travel because a brace
   group has already descended one (C 6.7.9/20), so a designation inside a group
   can only choose a slot inside the element the group opened. Coordinates stay
   -1 when the chain does not set them, which is what lets an undesignated value
   FOLLOW a designated one (`{[2]=1, 7}` → a[3]) as C specifies; `=` after a
   designation is optional in C and consumed here. A designation is
   **pending until a value is recorded**, and `dfl[]` records WHETHER the slot
   position was named outside the enclosure that owns the value: `[0]={...}`
   moves the cursor before the group classifies its target, `.b={5}` does too,
   `{[1]=5}` does not — the brace has already chosen. **The insight: the mapper
   had to be restated as C's subobject walk.** An enclosure takes the NEXT
   subobject, which is a whole element only at an element boundary of an ARRAY
   and otherwise the ONE slot under the cursor; a closed element group resumes
   at the NEXT element even with slots left empty. M22's "a group opens the next
   element" was correct only for arity 1, where a group can target nothing else
   — which is why no fixture from M22 through M29 ever reached the divergence.
   `int m[2][3]={{5},9}` puts 9 in m[1][0] and `{1,{5}}` puts 5 in m[0][1];
   `struct Box gb={{77},8}` (Box=`{union v; int t;}`) puts 8 in gb.t while
   `struct P ps[2]={{1},5}` puts 5 in ps[1].a, because there the group took a
   whole element. Designators move the SAME cursor, so an approximate cursor
   made `{[1]={5},9}` — excess in C — look like a legal position. C's element
   COUNT (`narr`) is passed in so a group may only take a whole element, and a
   flat value may only roll, if another element exists: a scalar tag object is
   ONE aggregate, and filling its last member ends the walk rather than rolling
   into a phantom second element (`struct L gl={.k=7,.p="seven"}` works for this
   reason). Writers walk targets ASCENDING, so `CCInitOrder` insertion-sorts the
   collected list whenever any designation appeared — without it a value that
   falls behind the walk never matches again and every later element silently
   zero-fills — and the sort makes duplicate detection free, which is where the
   C-permissive override shapes (`{1,[0]=2}`, `.i=1,.ch=2`) get diagnosed instead
   of silently taking the last value. The excess test moved from "last value
   written" to `hw` (highest index touched) because designators make the last
   source value the lowest target. A string literal now contributes ONE ROW for
   byte arrays with arity > 1 (`char t[2][3]={"ab","cd"}`, `{[1]="hi"}`) by
   taking a fresh group id and closing it, retiring M22's whole-object-only
   literal path for that shape; a literal aimed at a single char subobject is
   diagnosed where the old code stored its address truncated to one byte.
   **Landmine — group classification happens after a designation written BEFORE
   the brace and before one written INSIDE it.** Inverting that order is what
   made `struct P gp[2]={[0]={.a=9,.c=2}}` report `.c` as excess: the group was
   classed as a one-slot leaf group using the cursor the previous value had left,
   so `.c` looked like a second value crammed into `.a`. `dfl[]` exists to keep
   the two orders straight, and neither fixture shape is obvious enough to catch
   it without the other (`{[1]=5}` needs the inside-order).
   **Landmine — a designation may be followed by a brace group or a row literal,
   not only by a value.** The first version read a value anyway, which sent `{`
   to `CCInitOne` and reported "expected '}' after initializer" about an ordinary
   designated sub-aggregate; the second version routed `{` but not a literal, and
   `{[1]="hi"}` on a sized `char t[2][3]` stored 144 where C says 'h' — the
   literal's address truncated to one byte, a WRONG VALUE with no diagnostic.
   The same shape on a DEDUCED `char t[][3]` took a different path and happened
   to work, which is why the bug survived the first probe round.
   **Landmine — the descriptor descent must follow the STEP, not the resulting
   state.** Deciding it from `pe[0]>=0` after the branch is also true when an
   EARLIER component set `pe`, so `[0][1]` left the descriptor pointing at the
   element type instead of its elements and a third bracket would have been
   accepted against a scalar (gcc: "array index in non-array initializer" for
   `[0][0][0]`). Same class: `gcap` describes the OPEN subobject and must return
   to A when a group closes — leaving it at a leaf group's cap made the next flat
   value look over-full, which is how `{{77},8}` failed on its first run.
   **Landmine — a `//` comment on the `{` line that follows a multi-line
   `if`/`while` condition makes the HOST fail to parse the condition** ("Missing
   ')' at '{'"), and so does a comment inside such a condition when a brace
   follows; a comment after `{` on a ONE-line condition, or on a `}` line, is
   fine. Verified by four probe files. The roll guards are two-term and long, so
   the prose lives above them and the condition stays on one line.
   **Landmine — a splice whose replacement re-includes text still present in the
   file duplicates it.** A comma tail I re-emitted beside the copy the file
   already had made ordinary `int a[8]={1,2};` fail with "expected ',' or '}' in
   initializer" for the rest of the session; the diff against the pre-milestone
   snapshot is what named it. Divergences from C, all deliberate: an initializer
   that OVERRIDES an earlier one (`{1,[0]=2}`, `.i=1,.ch=2` — gcc accepts, last
   wins, `-Woverride-init` warns) and a union given two named members are
   diagnosed rather than silently resolved, and over-full targets ERROR where C
   warns ("excess elements", "initializer-string for array of chars is too
   long") — M22's stance, kept. gcc's `-Wmissing-braces` has no analogue here and
   changes no value. Error paths pinned against `gcc -std=gnu99
   -pedantic-errors -Wall`: 18 of 21 shapes reach the SAME verdict (index on a
   non-array at both chain levels, `.name` on a non-record, unknown member at
   both levels, `[-1]`, `[5]`, third bracket on a row, `[0]=` bare, a string
   aimed at a char subobject, excess in a scalar/struct/array initializer, an
   over-long row literal), and the 3 remaining are the two override shapes above
   plus a cosmetic `-Wmissing-braces` on `{{5},9,8,7` whose VALUES agree.
   Verification: desig.c is 42 checks + closer, and unlike every earlier fixture
   it needed **no model adjustment at all** — all 42 cross-checked increments
   match `gcc -std=gnu99` exactly, marks counted on both sides (42 = 42). Suite
   81 PASS x3, clean `CC TESTS: 0 failed`, no new `WARN: Unused var` entries;
   saved-object reloads 43/43/43 with agcopy 28/28/28, ptrmath 33/33/33, incr
   42/42/42, fptab 31/31, decay 28/28, fptr 27/27, ptrinit 25/25 and structinit
   22/22 unchanged.
 - [x] **M31** function-like macros: arguments, `#`, `##`, `...`, and the
   paint that makes recursion stop. A `(` following the name with NO space
   makes a macro function-like (`#define F (x)` is an object-like macro whose
   body is `(x)`, exactly as C says); whitespace before `(` at the CALL site
   IS allowed, and a function-like name on its own is just an identifier, so
   a variable may be named `AD2` in the same file as `#define AD2(a,b)`. An
   argument is expanded ONCE, in the context of its INVOCATION rather than of
   the macro being expanded, and is then substituted verbatim: `F1(F1(5))`
   gives `((5))`, and with `#define A A+1` plus `#define F(x) x`, `F(A)` gives
   `A+1` and NOT `A+1+1`. `#param` stringizes and `param ## x` pastes, with
   BOTH operands substituted unexpanded (with `#define ONE 1`, `CAT(ONE,x)`
   is `ONEx`), but the token the paste FORMS is looked up: with `#define code
   5`, `CAT(co,de)` is 5, while with `#define Z 3`, `a##Z` is the token `1Z`.
   **The insight: arity is decided by the SUBSTITUTED stream, not by the body
   text as written — which forces three passes over every invocation.**
   `#define VP(...) Q(__VA_ARGS__)` against a two-parameter Q is a TWO-argument
   call, and `#define T(x) S(x)` hands S the argument's text, not its value.
   So CCPrepIdent first scans each argument with `subonly=1` (substitute the
   enclosing macro's parameters, expand nothing, leave `#` and `##` as text),
   splits the result on its top-level commas, and only then expands each
   piece: an argument's expanded text can carry commas that were never
   separators (`S(P2(a,b))` stringizes `P2(1,2)`), and substituting first is
   also what makes a parameter shadow a macro of the same name (`#define X 9`
   with `#define FX(X) [(X)]` gives `FX(1)` = `[(1)]`). **The second insight:
   blue paint replaces M13's rescan depth limit.** `CMacScan.mask` is a
   bitmask of the macros whose expansion is open on this path, threaded into
   every nested scan — an argument scan gets the CALLER's mask, a body scan
   gets the caller's mask plus the bit of the macro being expanded — so `SELF`
   inside `SELF+1` stays a name, `AA`/`BB` mutual recursion stops exactly
   where gcc's output stops, and no legitimate chain is truncated. **Landmine
   — raw argument text has to be COPIED.** The `#` and `##` operands are the
   arguments AS WRITTEN, and the region an invocation was found in is NOT the
   region its body is scanned in: reading a raw span against the body buffer
   is how `GC(1,2,3)` emitted scratch garbage where C puts `2,3`. Both
   argument forms therefore live in the scratch so that ONE base addresses
   them, and every span is an offset because a put can move the buffer.
   **Landmine — the scratch is FIXED capacity on purpose.** Every open scan
   caches a region pointer, so a growing scratch leaves the outer ones reading
   a freed buffer; growth went back out and overflow became "macro argument
   text too large". **Landmine — `#__VA_ARGS__` stringizes the tail AS
   WRITTEN**, its commas and their spacing included (`SV(1, 2)` is `"1, 2"`),
   and that text exists only in the source: per-argument spans, trimmed and
   copied one at a time, lose the separator, which is how the first version
   produced `"12"`. The tail is therefore copied as ONE span, and it has to be
   copied BEFORE the invocation's scratch mark, because the compaction that
   ends an invocation slides the body down over everything from the mark
   onward and would erase it. **Landmine — the paste flag must be cleared on
   EVERY path that consumes an operand.** The `## __VA_ARGS__` path does not
   go through CCPrepFuse, so leaving it set made the NEXT body token look like
   a second operand and `GC(1)` came out `(1 ,)`: the paste re-fired and
   pasted the held comma onto the closing paren. **Landmine — a paste operand
   that became empty is not an error, and the deletion rule is narrower than
   "delete the left side".** C deletes a token only for `, ## __VA_ARGS__`
   when the tail matched nothing; any other empty operand leaves the OTHER
   operand standing (probed: `P(,2)` gives 2, `P(1,)` gives 1, and
   `x##__VA_ARGS__` with no extras keeps `x`). The first version deleted the
   left side in all three, losing values with no diagnostic. **Landmine — half
   of these diagnostics must fire at DEFINITION time because that is where C
   fires them.** `#` not naming a parameter of this macro, `##` at either end
   of a body, `__VA_ARGS__` in a body whose macro cannot supply it, a trailing
   comma in a parameter list, a duplicate parameter name, a digit-leading
   macro name: gcc reports all six even when the macro is never used, so
   CCPrepCheckBody walks the replacement text when the macro is defined,
   stepping over literals and comments whole (`#define S(x) ["#x"]` is clean).
   Checking at use time instead is silence for an unused macro. The one rule
   that cannot be checked there is `__VA_ARGS__` written in a CALL argument
   (`V(__VA_ARGS__)` at top level: V's definition is perfectly legal), and
   there it must be limited to text that arrived FROM THE MACHINERY
   (`CMacScan.isarg`), because this prep scans a line at a time and the
   continuation lines of a multi-line block comment are ordinary text —
   M31's own fixture header comment mentioning `__VA_ARGS__` was a compile
   error before that restriction existed. Divergences from C, all deliberate
   and all pinned against `gcc -E`: an argument list that runs off its line;
   a name that arrives only by SUBSTITUTION is not rescanned, so passing a
   macro name to `#define APPLY(f,a) f(a)` leaves `TWO(3)` as text (loud:
   "call to undefined function"); `#` and `##` in an OBJECT-like body stay as
   written; the pasted result is not validated as a single preprocessing token
   (`a##+##b` yields `1+2` where gcc errors) because CCPrepTail returns either
   a whole identifier or one byte, and name+punct could not be rejected either
   — `1` pasted with `.` is the pp-number `1.`; a redefinition replaces a
   macro's kind in silence (gcc warns); and where C is permissive-with-a-
   warning (trailing comma in a parameter list, duplicate parameter names) lite
   errors, keeping M22's stance. Verification: the prep oracle compares our
   expanded text with `gcc -std=gnu99 -Wall -E -P` token for token (literals
   atomic, comments dropped, whitespace collapsed) over 63 shapes — 62
   identical, and the 63rd (`BR({1,2})`, braces splitting an argument list)
   reaches the SAME diagnostic wording in both engines. Diagnostic parity over
   50 further shapes in five sets: 17/18 on the invocation matrix (the miss is
   the multi-line argument list), 11/11 and 7/7 on the definition-time checks,
   8/9 on the object-like-`##` set, 5/5 on empty paste operands. macros.c is
   44 checks + closer driven through real C code paths (array sizes,
   initializers, case labels, a for header, a static's initializer, string
   compares) rather than `int a[...]` shapes; cross-checked against
   `gcc -std=gnu99` with a mark after every check, all 44 increments identical
   and both engines returning 45. Suite 82 PASS x3, clean `CC TESTS: 0
   failed`, no new `WARN: Unused var` entries; saved-object reloads 45/45/45
   with agcopy 28/28/28, desig 43/43/43, ptrmath 33/33/33, incr 42/42/42,
   fptab 31/31, decay 28/28, fptr 27/27, ptrinit 25/25 and structinit 22/22
   unchanged.

 - [x] **M32** the conditional family: `defined()`, real expressions, `#elif`,
   `#error` — and one escape table instead of two. `#if` and `#elif` carry a C
   constant expression now: full precedence (`?:` loosest, then `||`, `&&`,
   `|`, `^`, `&`, `==`/`!=`, the four relational forms, `<<`/`>>`, `+`/`-`,
   `*`/`/`/`%`, then unary `+ - ~ !`), parentheses, and the rule that a name
   which survives expansion is ZERO (6.10.1/3), so `#if NOTAD == 0` takes.
   Literals are hex, decimal and `0b` with a `u`/`l` suffix read and ignored,
   and character constants decode through the SAME table the literals under the
   `#if` use, so a condition and the code it guards cannot read `'\x41'`
   differently. `defined` exists in all four written forms — `defined(X)`,
   `defined (X)`, `defined X`, `!defined(X)` — and `defined` is no longer an
   acceptable macro name. **The insight: `defined` is a SCAN-time operator, not
   a parse-time one, and its scope is the directive line plus any BODY rescan
   but NOT an argument expansion.** Its operand is never macro-expanded (with
   `#define HAVE 1`, `#if defined(HAVE)` is 1 and not `defined(1)`), yet a
   PARAMETER in that position stands for its argument's EXPANDED text, because
   by the time a body is rescanned the substitution has already happened. Both
   ways of getting the scope wrong are visible: were argument scans
   defined-aware, `#if K(defined(A))` with `#define A 1` and `#define K(x) x`
   would answer 1, where C expands the argument to `defined(1)` and reports
   "operator defined requires an identifier"; were body scans not,
   `#define ISDEF(x) defined(x)` would leave `defined` as a name and the answer
   would depend on which tokens happened to follow. A `defined` written in a
   body is a portability warning in C and is accepted here in silence.
   **The second insight: a condition is evaluated only when it can still change
   the answer.** `#if` and `#elif` evaluate solely when the enclosing region
   accepts AND no earlier branch of this chain was taken — which is C's own
   behaviour, and is what makes `#if 1` / `#elif 1/0` / `#elif ))` clean
   (probed), just as `#error` and `#include "no-such-file.h"` inside `#if 0`
   are inert. Short-circuiting rides the same channel: `&&`, `||` and `?:` parse
   both arms but mark the dead one, and division by zero is a diagnostic only
   for a live operand (`#if 0 && 1/0` and `#if 1 ? 0 : 1/0` are not errors in
   C, and faulting them would stop compiles C completes). `#elif` after `#else`
   is the one conditional error that fires even when nobody will read the
   branch, so the frame needed a fourth bit for "an #else has run at this
   level", gated on the enclosing region so a skipped outer region still
   tolerates junk. `#error` prints the rest of its line UNEXPANDED (probed:
   `#error MSG` prints `MSG`) and fails the compile; `#warning` prints the same
   shape as a note and continues. Every file must close the conditionals it
   opened, checked at the end of each CCPrepText, so an included file cannot
   leave an `#if` open and silently swallow the rest of its INCLUDER.
   **Landmine — `==` is not the relational level's operator.** The first
   CCPrepExRel ate `==`, which belongs to its caller CCPrepExEq, and then fell
   through to the `>` case, so `#if SEVEN == 7` asked whether 7 was greater than
   7: every equality condition answered wrong in the same direction, and the
   prep oracle caught it because `(2+3)*4 == 20` came back false. ExRel now
   leaves anything that is not `<` or `>`. **Landmine — the `#error` and
   `#warning` arms were inverted,** because `StrCmp` returns 0 on equality:
   `#error` printed a note and let the compile continue while `#warning` failed
   it. **Landmine — the condition text goes into the same fixed-capacity scratch
   as macro arguments.** It is released after evaluating (`pp->scpos = off`),
   and that is safe only because every invocation the expansion opened compacts
   its own region back below this one's start; the scratch still never moves, so
   a condition that expands past the end reports "macro argument text too
   large". **Landmine — the escape table had been written twice in CC_LEX.HC
   and the two copies had drifted** (`\'` in one, `\"` in the other — which is
   also why each literal kind keeps its own quote rule). They are now one
   `CCEscape` used by both lexers, plus a `CIsHexCh` predicate; checked against
   gcc on 19 escape shapes across both literal kinds. Divergences, all
   deliberate and all pinned against `gcc`: no octal in a condition, because
   `int x = 017;` is 17 to this front end — so `0x10 > 017` is false here and
   true in C, and `08` is accepted where C errors, a literal meaning one thing
   in a condition and another in the code under it being worse than the
   divergence; a decimal literal past I64 wraps with no warning; `'ab'` packs to
   the value C gives but without C's warning; `sizeof` and string literals are
   not operators in a condition; and `#pragma` is consumed rather than forwarded
   to the parser. Verification: the prep oracle compares our expanded text with
   `gcc -std=gnu99 -Wall -E -P` token for token over 122 shapes — 105 identical,
   14 reaching the SAME diagnostic where gcc recovers and prints something while
   we abort, and 3 being the literal rules above. Verdict parity over 83
   conditional shapes: 79 identical, the 4 misses being C's octal error and
   three C warnings (an oversized decimal, and a multi-character constant twice)
   that lite has no notion of; end to end through `CCRunFile`, all nine M32
   diagnostics abort with `rc=-1` while `#warning` alone continues at 42.
   cond.c is 42 checks + closer whose ENTIRE preprocessed text matches
   `gcc -E -P` token for token — so every directive decision in the file is
   compared, not only its answers — and it was then cross-checked against gcc
   per check (42 identical increments, both engines 43). The cross-check earned
   its keep twice more: two of my own expectations were wrong (the value a `?:`
   gives the directive, and whether `ALIAS` or its expansion is the name
   `defined` asks about) and both engines disagreed with me in the same way.
   Suite 83 PASS x3, clean `CC TESTS: 0 failed`, no new `WARN: Unused var`
   entries; saved-object reloads cond 43/43/43 with macros 45/45/45, agcopy
   28/28/28, desig 43/43/43, ptrmath 33/33/33, incr 42/42/42, fptab 31/31,
   decay 28/28, fptr 27/27, ptrinit 25/25 and structinit 22/22 unchanged. Prep
   and the lexer live under `T/`, so no HCRT.BIN rebuild is involved.

 - [x] **M33** line continuations, and `#include "x.h"` resolved against the
   file that wrote it — the last two gaps on M13's own list of what
   preprocessor-lite skips. A backslash ending a physical line now splices the
   next one into it, and it happens where the text is READ: `CCPrepText` asks
   `CCPrepLogical` for one logical line before deciding whether it is a
   directive, so a `#define` BODY, its PARAMETER list, its NAME, the right
   operand of a `##`, an `#if` condition, an `#ifdef` name, an `#include` name,
   a macro call's ARGUMENT LIST, one argument of it, a call in ordinary text, a
   string literal, a character constant, an identifier in a declarator, an
   initializer list and a `for` header all cross the splice through one call,
   and the C parser downstream never learns that anything happened. The same
   order is what makes a `//` claim the following line, makes a block comment
   pick its terminator up from the line below, and makes a body swallow a whole
   line even when that line was a directive — with `#define EATS \` above
   `#define INSIDE2 1`, INSIDE2 is NOT defined, because the directive that
   would define it became the tail of the body. **The insight: the observable
   rule is about whitespace, not about the backslash.** The newline and the
   backslash are both deleted and every other byte keeps its place, so the space
   BEFORE a backslash survives (`STR(1 ` + splice + `2)` stringizes `1 2`, two
   tokens, not `12`) while whitespace AFTER it goes with the newline (`12` +
   backslash + two spaces + newline + `34` is the one number 1234, and `12 \` +
   newline + `34` is `12 34`, two numbers). C99 requires the `\` and the new-line
   to be adjacent, gcc splices anyway with `backslash and newline separated by
   space`, and lite follows gcc's result and stays silent — a divergence in
   noise, never in a translation unit. A `\` that ends a file WITH a new-line is
   still a splice (warning in gcc, silence here); a `\` that ends a file with NO
   new-line is not a splice at all and stays an ordinary byte, which is why both
   engines call `int a[1];\` at EOF a stray character. One output newline is
   emitted per PHYSICAL line consumed, so M13's rule that the parser sees the
   source's line structure survives a splice instead of being broken by it.
   **The second insight: the includer's directory is per include DEPTH, not a
   fact about the main file.** `CPrep.dir[depth]` is filled from the path that
   was opened at that depth, so `contaux.h` in `T:/CC/TEST/` reaches
   `sub/leafh.h` from the main file's directory while `leafh.h` reaches
   `leafb.h` from ITS OWN — and the `leafb.h` lying beside the main file, same
   macro with a different value, is never opened. Proving that needed two
   probes that a token diff cannot supply: headers with the SAME name in the
   includer's directory and in the header home with DIFFERENT bodies, so the
   VALUE that comes out says which directory answered, and a decoy in the main
   file's directory that only a header-relative rule would skip. `<name>` keeps
   going straight to the header home, the split gcc makes between the two forms.
   `FileFind` is the existence probe, not `FileRead`: a fallback chain that
   asked `FileRead` would print a kernel `File not found` line for every
   home-resident header, on the attempt that was always going to fail. The name
   of the file being compiled travels in `CCCtrl.fn` — `CCRunArgv` records it,
   `CCInit` carries it across the zeroing (a fixed-size `MemCpy`, because it is
   an array and nobody promised a NUL inside it), `CCPrep` seeds `dir[0]` from
   it — so a driver that reads text itself has to name the file too: the reload
   driver's stale name was correct only because all twelve of its fixtures share
   one directory, which is the worst kind of passing test. **Landmine: the
   scan-back's "did not move" index is the COMMON case, not the failure.**
   `CCPrepCont` finds the backslash by stepping back over trailing spaces, so a
   line that ends directly in the backslash leaves the index where it started;
   reading that as "no continuation" spliced nothing, and every shape then
   failed with its own unrelated message (`bad parameter name in #define`,
   `unterminated argument list in a macro invocation`, `unexpected token in a
   #if expression`) so it looked like four bugs. The corpus diff named it in one
   line: our output still contained the backslash. **Landmine: a helper that
   returns text from either of two buffers must return the length with it.** A
   caller that kept computing the line end from the original text paired a
   pointer into one allocation with a length from another, a directive scanned
   unrelated bytes, and the diagnostic was `unknown directive '#'` in a file
   with no stray `#`. **Landmine: the joined path is longer than either of its
   parts.** An includer directory of up to `CC_TOK_MAX-1` characters joined with
   a name of up to `CC_TOK_MAX-16` needs more than `CC_TOK_MAX` to hold it, and
   the winner of the search is copied back into the caller's `path` — sized for
   the pair now, or it would have written off the end of the HolyC stack frame.
   **Landmine: a fixture has to be RUN.** My `for (j=0;j!=3;j++) j=j+1;` check
   incremented `j` twice per lap, so it went 0, 2, 4 and never equalled 3: an
   infinite loop, in BOTH engines, in perfect agreement. A token diff against
   `gcc -E -P` cannot see that, and agreement between two compilers about a
   wrong loop is not evidence. Verification: the continuation corpus is 30 shapes
   compared with `gcc -std=gnu99 -Wall -E -P` token for token — 29 identical,
   including `//` swallowing a line, a block comment ending on a continued line,
   a `##` operand and an `#ifdef` name arriving from the next line, a chain of
   three splices and the four whitespace shapes above — and the 30th (`junk`
   pulled into a skipped `#if`) reaches the same VERDICT in both engines with
   different wording. The include harness is 13 cases against `gcc -std=gnu99
   -Wall -I T:/CC/INC` and all 13 agree, six of them because a twin header in
   both directories makes the answer observable; the verdict matrix is 20 shapes
   and 15 agree, the five misses being warnings lite does not have (`backslash
   and newline separated by space` twice, `backslash-newline at end of file`
   three times — warnings even under `-pedantic-errors`) where the translation
   unit is identical. `cont.c` is 29 checks + closer whose ENTIRE preprocessed
   text matches `gcc -E -P`, cross-checked against gcc per check (29 identical
   increments, both engines 30) after the loop bug above was fixed, and it pins
   the path rules with real files: `contaux.h` reached by a plain name, its own
   sibling include beating the decoy, and `contaux3.h` beside the main file
   reachable only if the directory stack unwound. `preproc.c` no longer has to
   spell its includes with a drive letter and is still 12. The suite is 84 PASS
   x3, clean `CC TESTS: 0 failed`, with the `WARN: Unused var` set unchanged, and
   the whole 153-shape prep oracle re-ran with exactly ONE new divergence — the
   both-error wording case — so replacing the line loop did not move anything
   else about prep. Saved-object reloads give cont 30/30/30 with cond 43/43/43,
   macros 45/45/45, agcopy 28/28/28, desig 43/43/43, ptrmath 33/33/33, incr
   42/42/42, fptab 31/31, decay 28/28, fptr 27/27, ptrinit 25/25 and structinit
   22/22 unchanged. Prep and the drivers live under `T/`, so no HCRT.BIN rebuild
   is involved.

 - [x] **M34** file-scope redeclaration: C's tentative definitions, and what
   `extern` actually means. A file-scope name is now ONE object with ONE
   address and ONE type, however many times it is declared. `extern int g;`
   DECLARES: it claims the base slot (lite has no way to name storage it does
   not own) but is not a definition, so it says nothing until something USES it,
   and then it says "declared extern but never defined". A declaration with no
   `extern` IS a definition — C's tentative one — and its zero initializer is
   not code, it is the zeroed arena the object already sits in, which is why
   `int g; int g;`, `extern int g;` + `int g;` and `int g=7;` +
   `extern int g;` all merge into one slot and cost nothing. **The insight: in
   a compiler that bakes absolute arena addresses, the promise a declaration
   makes is its OFFSET, not its symbol.** A function compiled between
   `extern int g;` and `int g;` has already emitted `locals + s->offset`, so the
   definition must reuse that slot instead of claiming its own — that is the
   whole reason the first declaration owns the storage and every later one
   merges into it, why only one declaration may carry an initializer, and why
   the only thing a redeclaration may still ADD is length above slots nobody
   has taken. `CSym.def` / `CSym.has_init` record what the declarations have
   promised; `CCVarAddr` sets `CSym.refs` — the one place a name lookup turns
   into storage — so the "never defined" report asks the question a linker asks
   and no more, and a silent `sizeof` scan (`cc->noemit`) deliberately does not
   answer it. The merge check compares what THIS declarator *would have* stored,
   decay included (an array symbol holds the decayed type plus `ael`, so
   comparing raw `ty` calls every array redeclaration a type conflict).
   **Two silent bugs surfaced because the merge check refused to paper over
   them.** `extern struct pt g;` does not take `CCParseTop`'s struct branch
   (that branch requires the statement to START with `struct`), it falls through
   to the keyword path, which received the tag id from `CCAcceptBase` and then
   dropped it passing `-1` to the declarator: the object was TY_STRUCT with no
   tag, so `sizeof` answered one slot and every member reference failed. Until
   the merge check, the shape was caught only by the downstream "global
   redefined" refusal — right verdict, wrong reason — and merging removed the
   refusal. The sibling was `union pt u;` after `struct pt { … };`: C puts
   struct and union tags in ONE namespace and requires the kind to match, and
   lite looked the tag up without asking which kind it was, silently giving the
   object the STRUCT's layout. `CCStagUse` now asks the kind at both tag-USE
   sites (the definition form was already refused outright by `CCStructDef`) and
   `CCParseTop` carries the tag to the declarator. **A third bug the fixture
   caught by accident:** `char gg[4]="ab";` saved to an object and reloaded came
   back 24 where it compiled 25. The collector's whole-object string arm filled
   `vals[]` with the literal's bytes and never touched `vk[]`, so the arena
   writer read a per-value "kind" out of an uninitialised stack slot, took its
   pointer branch, and called `CCArenaPtr` per byte — an 8-byte write plus a
   relocation record at a BYTE offset. In place that is invisible (each quad's
   low byte is the character and the next element overwrites the byte after it),
   so only a reload exposed it: the loader patches eight bytes at each recorded
   position, rewriting the string's neighbours with addresses. `nafx` for that
   test file went 3 → 0. The M30 nested-string arm already set `vk[n]=-1`; only
   the older arm had missed it.
   **One rule is lite's, not C's, and is written as such:** an array whose
   length no declaration ever stated (`int a[];` with no initializer,
   `extern int a[];`) is ONE SILENT SLOT. C calls it an incomplete type and
   diagnoses only the uses that need a size; gcc warns "assumed to have one
   element" and compiles. lite has no warning channel, and an object something
   reads needs a real object somewhere, so it takes the assumption gcc makes —
   the same one it already makes for `int a[3]; a[5]=1;`, which compiles and
   writes. The `dim == 0` state that expresses "nobody has said" is resolved at
   the END of `CCParse`, because C's rule is about the file and not about any
   one declaration: `int a[];` then `int a[3];` is a legal pair, and a check
   welded to the first declaration refused it.
   **Landmine: a guard owns only the statement under it — reformatting can move
   a return inside one.** A comment reflow I did while rewriting the
   function-pointer tail left `return FALSE;` as the BODY of
   `if (!cc->spec_extern)`. For every `extern` function-pointer declaration the
   guard was false, the return was skipped, and the parser fell through into the
   plain-declarator path and reported "expected name after type" — a message
   about a name where the source has a `(`. Ten shapes, including one
   `fptr.c` had used since M18, went red. The cause was not the fp rewrite at
   all: it was doing comment formatting and code motion in the same edit. **And
   the deeper one: three of the five things I had built here were diagnostics I
   invented.** The deferred "no known length" report, the arena-contiguity
   refusal dressed up as conformance, and the fp-array length-completion
   machinery existed only to match gcc's pedantic warnings; a read of the
   milestone against the correctness policy cut all three, and the fall-through
   bug above had been introduced by the cut's own precursor while chasing them.
   **Landmine: an invariant belongs where the question is meaningful, not in the
   helper that happens to do the work.** My first draft of the top-up check went
   INSIDE `CCReserveArray` as "the watermark must still be one slot past this
   symbol's offset". That holds at every first declaration, but a block-scope
   `static int (*t[3])(int)` reserves its slots BEFORE `CCMakeStatic` claims the
   arena slot, so there `s->offset` is a FRAME number and the question cannot be
   asked; a shape that had worked since M26 (fptab) started erroring. **Landmine:
   a U0 helper that reports an error is not a guard** — the caller has to be able
   to decline, and only the fixture noticed. **Landmine: file order is
   load-bearing in this host.** `CCGlobalTopUp` and `CCStagUse` first went below
   their callers and the host rejected the unit with `ERROR: Invalid lval at
   "CCGlobalTopUp"` at the CALL site — not "unknown function" — because an
   unresolved identifier followed by `(` parses as something else entirely.
   **Landmine: a multi-file test driver attributes diagnostics to the wrong
   row.** A driver that runs 12 shapes in one process reports green for shapes
   that fail, because each run's diagnostics precede its marker line and my
   parser gave them to the previous row; `extern` function-pointer declarations
   looked fine for an hour. Re-prove any claim from a multi-file run with one
   file per process. **Landmine: an existing fixture can be the thing that is
   wrong, not the code.** `T:/CC/TEST/unionc.c` declared `extern int eg;` and
   then WROTE eg — legal C to compile, a link failure in C, which is what lite
   now says at compile time. The fixture got `int eg;` beside its declaration
   rather than a weaker compiler.
   **Verification:** `decls.c`, 24 checks + closer = 25, identical in gcc and
   lite and cross-checked check by check (24 identical increments in both
   engines); the six identity checks (`pa()==&ga`, `pf()==&gf[0]`, …) put a
   function between the declaration and the definition so a moved slot shows as
   a FALSE check rather than as a leak. The 44-row matrix, grouped by the
   policy's two questions, gives 24/24 value matches on C-legal programs and
   refuses all 11 C-illegal ones (3 at compile time where C's linker would),
   with 9 rows that are the choices listed in limitations and nothing
   unexplained; a 35-shape probe compares VALUES (30 identical); a 12-shape
   function-pointer-array probe gives 11 identical. The suite is 85 PASS ×3,
   clean `CC TESTS: 0 failed`, `WARN: Unused var` set unchanged; the saved
   object for decls reloads 25/25/25 and the 13-fixture reload stress is
   unchanged (cont 30, cond 43, macros 45, desig 43, agcopy 28, ptrmath 33,
   incr 42, fptab 31, decay 28, fptr 27, ptrinit 25, structinit 22, decls 25).
   The 153-shape prep oracle re-ran with an UNCHANGED divergence set (18, the
   same 18) — M34 touches no prep code. `unionc.c` 40 and `fptab.c` 31 and
   `fptr.c` 27, the three fixtures these fixes forced, are unchanged.
   Everything lives under `T/`, so no `HCRT.BIN` rebuild is involved.

 - [x] **M35** `double`, delegated whole: the F64 value model, where storage needed
   nothing at all and only computation and literals had to be routed. A double in
   this model is ONE slot holding the IEEE-754 binary64 pattern — which is what a
   TempleOS `F64` already is (measured: a `1.0` literal arrives as
   4607182418800017408, and a `quad -> F64 -> quad` postfix cast round-trips
   exactly) — so globals, locals, statics, parameters, returns, struct and union
   members, arrays with their subscripts and element types, the arena and its
   relocations all already moved the right number of bytes, and `sizeof(double)`
   answered 8 before anyone asked. Everything M35 adds is therefore about VALUES:
   13 zero-argument HolyC helpers in `CC_LIB.HC` (`CCXFAdd/Sub/Mul/Div/Neg/Lt/Gt/
   Le/Ge/Eq/Ne`, `CCXFItoD`, `CCXFDtoI`) read `cc->ext[]`, evaluate one line of
   host F64 work, and answer in RAX. lite implements no float arithmetic, no
   rounding mode, no conversion algorithm and emits no SSE encoding; a float value
   is not a register class here, it is a kind of quad. `CCFold(cc,slot,a,b)` gives
   the constant folder the same route, so `double g = 1.5 + 2.5;` and the same sum
   computed at run time are evaluated by THE SAME FUNCTION — agreement by
   construction, instead of a second float evaluator that has to be kept in step,
   which is what the delegation rule is for. Two things are deliberately not
   delegated and both are argued rather than assumed: where C's answer needs a
   distinction the host's operators do not make (`==` on a pair that is
   bitwise-different and numerically-equal), lite COMPOSES it from the host's other
   operators; and where the host's own reading of a literal is wrong in a way the
   program could never see, lite changes the SPELLING it hands over (below).
   One slot per operation rather than one slot plus an opcode quad, for the same
   reason: staging an opcode would put THE OPERATION in memory, where a nested
   float call could overwrite it between staging and execution, and
   `CC_LIB_SLOTS` went 8 -> 32 so the family can grow without an object-format
   change (a slot is one quad in `CCCtrl`, reached through the `CC_B_LIB` table
   base, so a saved object rebinds to the loading session's helpers).

   **The whole representation is one distinction, measured on both sides before it
   was leaned on.** Assignment CONVERTS and the postfix cast REINTERPRETS: `r=9` on
   an `F64` gives 9.0 exactly, `a=qa(F64)` gives the double whose bits are `qa`,
   `ri=b` where `b` is an `F64` truncates toward zero (7.875 -> 7, -7.875 -> -7,
   C's answer), and out of quad range the host lands on 0x8000000000000000 as
   `cvttsd2si` does where C leaves it undefined. Casts, arguments, returns,
   assignment, initializers, `++` and the constant folder all cross that boundary
   in one of those two spellings and in nothing else.

   **Host semantics, measured rather than assumed.** On reinterpreted quads the
   host's `+ - * /` and unary minus are BIT-EXACT against gcc over nan, -nan, inf,
   -inf, +-0.0, 1e308, 5e-324, 0.1, 1+2^-52, -7.875 and pi. Its ordered compares
   are IEEE for every finite pair — proven with the pairs a quad compare must get
   backwards (two negatives: -7.875 vs -1.0, and -2.0 vs -4.0, both answering C's
   way) — and for a NaN pair `< > <= >=` all answer 0, which is what C defines. Its
   `==`/`!=` are BITWISE, and that is where the divergence lives: `-0.0 == 0.0`
   answers 0 where C says 1, and a NaN equals itself, so `CCXFEq`/`CCXFNe` compose
   C's equality from the ordered compares — measured over 16 pairs (both zeros,
   four NaN forms, both infinities, subnormals, equal and unequal finite values)
   with 0 disagreements against C where the host's own `==` had 3, at the price of
   one extra compare and no new mechanism. The `ext[]` staging ABI was proved safe
   for NESTED evaluation before it was used for arithmetic (10 shapes of nested
   `strlen` calls with runtime operands, all equal to gcc): a partially evaluated
   operand lives on the machine stack, not in `ext[]`, and a conversion helper
   touches only `ext[0]` as its scratch, so a staged `ext[1]` cannot be clobbered
   underneath it.

   **The kernel's decimal parser has two silent wrong-value modes, and both are
   repaired in the lexer rather than passed through.** Measured with `Str2F64`
   direct and again through compiled C:
   - It STOPS at an exponent's `+`. `Str2F64("1e+2")` answers 1.0 — the mantissa
     alone, where C says 100 — and `1E+2` and `1.5e+2` answer 1.0 and 1.5 the same
     way, while `1e2` and `1e02` both parse. That is a wrong VALUE two orders of
     magnitude off, not a rounding difference, and the only symptom a program
     would get is an answer.
   - It sums the significant digits into a SIGNED 64-BIT INTEGER and checks
     nothing: `12345678901234567890` came back -6.1e18 and
     `0.12345678901234567890` came back -0.061, each with its end-pointer argument
     reporting that the whole text was read, so the conversion does not even know
     it failed. Leading zeros reach the accumulator on neither side
     (`000000000000000000001` is 1 in both), and 20 significant digits are always
     past 0x7FFFFFFFFFFFFFFF. The scale is applied AFTER that sum, in floating
     point, which is why the error survives being scaled correctly:
     `1234567890123456789012345e-6` answers 1096246371337.5599 where C says
     1.2345678901234568e+18 — the same wrapped accumulator, divided by 1e6. And
     m35exp2.py spelled ONE value six ways: the five whose digits fit returned
     the same quad every time (0.7 ULP below C's), and the 20-digit spelling that
     does not fit is the odd one out. It is the digit count that decides, not the
     exponent — which is why the repair is a digit test and not a rounding fix,
     and why every spelling lite can build from digits it has checked is as good
     as the host gets.
   `CCNumFloat` now does DECIMAL bookkeeping — no rounding, no binary, no float
   arithmetic — and checks the answer. `CCDigOvf` is the test the host omits; the
   `+` is dropped from the text, because C's exponent is `e [sign] digits` and a
   `+` exponent IS the unmarked one; a literal whose written digits overflow goes
   as the shortest spelling lite can build from digits it has already checked fit;
   and `Str2F64`'s end pointer is compared against the text handed over, so a
   spelling the host cannot read is a diagnostic instead of a number. Verified:
   `1e+2`, `1E+2`, `1.5e+2`, `0e+5` and `1e+02` each compare equal to the same
   literal spelled without the `+`, in BOTH engines — an assertion that holds
   whichever way the decimal parser rounds.

   **A positive exponent is folded into the digits, because that path is exact.** A
   positive exponent is applied by multiplication and drifts — `1e2` comes back
   99.99999999999997 and `1e10` 9999999999.9999561, where C99 6.4.4/6 requires
   exactly 100 and exactly 10000000000 — so `1e2 == 100` was false here and true in
   C, a wrong answer about an ordinary literal. Writing the same rational number
   without an exponent puts it on the accumulate-then-convert path, which is exact
   twice over: the accumulator holds the integer (the overflow test is what makes
   that certain) and an `I64 -> F64` conversion is correctly rounded. Measured over
   40 exponent spellings against the value python's correctly-rounded parser gives:
   every shifted form whose digits fit came back exact (`1e1`->`10`,
   `1e10`->`10000000000`, `1.5e3`->`1500`, `100e-2`->`1`), and lite uses that
   spelling when it fits. A NEGATIVE exponent gains nothing from it, and that is a
   measurement and not an assumption: m35neg.py spells 19 such values both ways and
   every pair returned the IDENTICAL quad (`1e-5` and `0.00001` are both 12 ULP
   high, `1e-7` and `0.0000001` both 4), so there is no better spelling to choose
   and lite attempts no shift. A plain spelling for `1e19`, `1e100` or `1e308`
   would not fit the accumulator either, so those keep the spelling the program
   wrote, and their drift is recorded here rather than papered over. In ULPs of the
   value C requires — the spacing of doubles there, which is why every one of these
   is a whole number (m35drift.py prints the table): `1e2` -2, `1e10` -23, `1e18`
   -22, `1.5e18` -14, `1e19` -16, `1e100` -117, `1e308` -160,
   `1.7976931348623157e308` -225, `1e-308` +64, `3.141592653589793` +9, `1e-5` +12,
   `1e-7` +4, `0.15` +1 and `5e-324` -1, the kernel having underflowed that one to
   zero. Plain decimal spellings came back exact across the measured set (`1.5`,
   `2.5`, `.5`, `2.`, `0.1`, `100`, `1000.0`, `123456.789`, `123456789012345678`),
   with `0.15` the one ULP-high exception. Overflow and underflow agree with C:
   `1e400` is infinity and `1e-400` is zero. The host's OWN literals drift the same
   way — its `0.3` is one ULP high, which is why `0.1+0.2==0.3` answers 1 here and
   0 in C — so a program comparing a decimal LITERAL for exact equality with a
   computed value can differ from gcc, and closing that would mean writing a
   correctly-rounded decimal->binary converter: the second implementation the
   delegation rule exists to avoid. `0x1p3`-style hex floats stay unsupported (the
   kernel answers 0 for them, so the lexer keeps the digits an INTEGER and the
   program gets the syntax error a compiler without hex floats reports, rather than
   the value 0 with no comment).

   **What the host does differently from C, and what lite does about it.** C's
   truthiness of a floating value stays the bits-nonzero test the integer model
   uses (`!`, `&&`, `||`, `if`), so `-0.0` is truthy here and falsy in C: one
   divergence, chosen over adding a second way to test truth, because the folder
   and the program agreeing about it is worth more than one zero's sign. `float`
   folds to `double` because the host has no single precision at all (no `F32`, no
   single-precision operator), which agrees with C wherever C itself promotes
   (arithmetic, varargs) and differs visibly only in `sizeof(float)` (8 here, 4 in
   C) and in the last bits of a single-precision computation.

   **Refusals, every one of them "the integer path would have computed a
   number".** `%`, `~`, `& | ^`, `<< >>` and `switch` on floating operands — for
   `switch` the dispatch chain compares QUADS, so a double would report "no match"
   for a label C says matches, which is wrong control flow rather than a wrong
   number; `case` labels, array lengths, enumerators and designation indices whose
   value is floating, which is why `CCConstWantInt` exists: once the folder folded
   floats for `double g = 2.0 * 3.0;`, "is this the right KIND of constant
   expression" stopped being a property of the expression and became a property of
   the CONTEXT, so it is phrased in one place. `double *` is refused at every
   declarator site — local, global, parameter, struct member, typedef and cast —
   because a pointer slot records no pointee type and `*p` would be the integer
   model applied to a bit pattern; `double a[4]` indexed with `a[i]`, and a
   `double t[]` parameter, are how several doubles are passed around today, because
   an array symbol DOES carry its element type (M25's `ael`). A function pointer
   cannot return a double and its parameter list cannot contain one, because M18
   records neither an fp's return type nor its argument types: `double (*f)(int)`
   would type `f(2)`'s result as an integer, and `g(3)` through `int (*g)(double)`
   would hand the callee the denormal whose bits are 3 where C gives 3.0. A
   conditional with one floating and one integer arm is refused because C converts
   BOTH arms to their common type, and the middle arm's conversion belongs inside
   the middle arm's own path — but that path's jump is emitted before the third
   arm's type is known, so lite cannot go back and insert it; the fix is the silent
   look-ahead parse `sizeof` and the cast arm already use. Typing that pair as the
   integer model instead (what M15 through M34 did for every pair) leaves a
   double's bit pattern in a slot C says holds a number, which is a wrong value
   with no message. Every one of these is written in the limitations list as the
   thing the next milestone owes. M37 took the `%f` half of that list.

   **Six bugs in lite, two defects in the host and four errors in my own probes —
   every one of them found by running something, and the probe errors are listed
   because a wrong expectation is the harder kind to notice.** `CCFloatBin` popped
   with `0x59`, which is POP RCX, not POP RAX: the left operand landed in the
   register the very next instruction was about to overwrite, and the failure only
   showed up in the program that used the value. **Inserting an `if` above an
   `if`/`else if` ladder does not skip the ladder's arms** — each arm emits its own
   operation — so `CCExprAdd`'s new float branch emitted the add, fell through, and
   the ladder's final `else` emitted it a second time with a second POP, which ate
   the destination address `CCAssignTail` was about to store through: `u = 1.0 +
   1.0` answered 0 and `a = a + 1.0` on a local faulted. Making the float case the
   ladder's FIRST arm is not cosmetic. `<=`/`>=` had been composed as `!(a>b)` and
   `!(a<b)`, and **a `!` written directly on a parenthesized FLOATING comparison
   answers 0 for every pair in this HolyC build** (measured: `!(1.0 > 1.0)` gives 0
   where C wants 1, and `!(1.0 < 2.0)` gives 0 where C wants 1, while `r = a > b;
   return !r;` gives the right 1 both times), so both helpers reported false for
   every input; the visible symptom was not a wrong value but THE LOADER
   SEGFAULTING, because a `<=` that never becomes true turns a bounded recursion
   into an unbounded one and the C program runs out of its private stack — a
   runaway recursion in generated code looks like a compiler crash, not like a
   wrong comparison, so check the guard before blaming the codegen. The fix
   delegates further rather than composing around the bug: the host's own
   `a<=b`/`a>=b` are correct for every pair in the measured set, NaN included.

   **The conversion map had to be widened to the comparisons, and only a probe
   found that it had not been.** `CCFloatOp` asked for the int->F64 conversion of a
   mixed pair on `+ - * /` and not on `== != < > <= >=`, so a mixed comparison put
   the F64 BITS of one side against the NUMBER on the other and was right only
   where they happened to agree: `x == 100` on a double holding exactly 100.0
   answered 0 and `1.5 < 2` answered 0, while `x == 0` on 0.0 and `-2.5 < -2`
   answered 1 by luck (the bits of 0.0 ARE 0, and the bits of -2.5 are a negative
   quad). Measured over 25 shapes against gcc, 15 of them wrong, in both operand
   orders and in the constant folder as well as in the emitted code. That the
   fixture missed it through 49 green checks is the more useful finding: every
   float in it was observed through `(int)… == n`, which makes BOTH sides integers
   and hides the operand type entirely. A value model that is only ever observed by
   converting the value away exercises the conversion, not the model, so checks
   50-53 of `TEST/float.c` are deliberately unwrapped comparisons, one of them
   against a variable. **A symmetric rule has to be tested in both directions**: I
   gated the new initializer conversion on "the destination wants F64 bits", which
   made `double a[] = {1, 2}` right and left `int a[] = {1.5}` storing a bit
   pattern; the gate is the MISMATCH. Initial global and aggregate initializers
   needed the same treatment for a different reason — C converts an initializer to
   the type of the object being initialized (6.7.9/19), and storing the integer 3
   into a double slot is the denormal whose bits are 3 — which is why a value's own
   floatness now travels in the `vk[]` slot that already carried its relocation
   base, and why `CCInitConv` is the one place the five initializer writers
   convert. And a hand-written IEEE constant nearly cost an hour: `-(bits of 1.0)`
   is NOT bits of -1.0 (setting the sign bit ADDs 2^63, it does not negate), so a
   probe meant to compare -2.0 with -1.0 had been comparing -2.0 with -4.0 and came
   back looking like a host bug. Print the value a quad actually holds — `%12.5f`
   on an `F64` loaded from the quad prints it (verified: the bits of pi print
   `3.142`) — and generate test constants with a tool. The other three were mine in
   the same way: two sweep cases I "expected" to be 1 where C says 0 (`1e400` and
   `1e399` are both infinity, and infinity is not greater than itself); the
   leading-0 case, where lite's 17-for-017 is a deviation M32 already records and I
   asserted 15 as though it were new; and a stress harness that compared the word
   `direct` against a number and reported ten correct results as ten mismatches.
   Two more were wrong TOOLS: I re-ran `prepcmp.py` (a host tool, its /tmp
   path gone because /tmp does not survive a reboot) with no arguments
   and called its 63 built-in cases the prep oracle, when the oracle is that script
   with its seven case files and 153 shapes (see M33/M34); and I compared against a
   HEAD worktree without copying `./3d_loader` into it — the binary is a build
   product, not tracked, so the comparison ran nothing and said so with exit 127,
   which is the right answer to the wrong question.

   Verification: `TEST/float.c`, 53 checks + closer, THE SAME VALUE from gcc
   (-std=gnu99) and from lite, and bisected to the check: 82 prefixes of `main`,
   each compiled and run by both engines (m35bisl2.py) — 0 prefixes where lite
   disagrees with C, 0 crashed, 0 prefixes gcc refused. 28 mixed floating/integer
   comparison shapes (m35mix.py): 0 disagreements after the conversion fix, 15
   before it, over both operand orders, all six operators, a `char`, a `static` and
   a subscripted operand, a value past `int` range, and the constant-folded form of
   each. The literal corpus separates what must agree from what need not
   (m35lex.py, 53 spellings in five classes): 33 exact-value cases and the 6 `e+`
   rewrites agree with gcc to the bit, 7 differ as the measured drift says they
   should, 5 C-illegal spellings are refused, and 2 are recorded deviations (`017`,
   and the 20-digit integer constant above). Host-side evidence, one probe per
   question: m35end.py (the end-pointer report, and the digit-sum wrap),
   m35cliff2.py (exact at 9223372036854775807, negative at 9223372036854775808),
   m35shift.py and m35exp2.py (spellings of one value: the positive-exponent fold
   is exact and the exponent path is not), m35neg.py (19 negative-exponent pairs,
   every one the identical quad), m35trunc.py (409 long literals: the re-spelling
   changed the double in none), m35drift.py (the ULP table quoted above) and
   m35huge.HC (1e1000000 is infinity, 1e-1000000 is zero). Suite: 3 runs, 86 PASS
   and `CC TESTS: 0 failed` in each and identical across them, and the kernel's
   five compile notices (`mS`, `max_ona`, `min_ona` in `Noise`; `t` in
   `CCParseFun`; `total` in `CCLoad`) are the same set a HEAD worktree produces —
   none added, none removed. A/B against that worktree, five runs each: HEAD 85
   PASS, this tree 86 (the extra PASS is float.c), 0 FAULT lines in either; that
   comparison is in here because a SIGSEGV partway through one suite run appeared
   once while an edit was in flight, and ten clean runs on both trees is what says
   the edit raced the run rather than the code being flaky. Saved-object stress: 10
   fixtures x 4 modes (compile-run, save+reload, reload twice, direct), 0
   mismatches, float.c at 53 in every mode — the F64 constants this milestone
   writes into the arena and into MOVABS operands survive a reload. Operator and
   refusal sweep: 37 shapes, 37 as expected, the 12 refusals still refusing; fold
   and initializer sweep: 23 shapes, 0 needing attention. The 153-shape prep
   oracle, run with the seven case files that make it 153 shapes and compared with
   the same grep and sort that produced the M34 baseline list: 18 divergences, none
   new, none gone. All of it is `T:/` content, so there is no `HCRT.BIN` rebuild
   and no host recompile — the loader is the binary M34 was tested with — and the
   suite is the gate.

 - [x] **M36** the integer conversions `printf` and `sprintf` do, made C-exact.
   Two wrong values, one of them a filed bug, both caused by the same habit: a
   conversion that NEGATES ITS OWN INPUT before working on it. `%d` of the most
   negative quad went through `if (cc_v<0) { putchar(45); cc_v=-cc_v; }`, and
   `-cc_v` is the same value when `cc_v` is -2^63, so the walker was handed a
   negative number it believed positive, its `cc_v>=10` recursion test failed, and
   it printed one digit — `48+(-8)` is `(`, so the answer was `-(` for a
   twenty-character number (BUG-0001, and sprintf's own `cc_dig` had the identical
   shape, so the same program produced `-(` there too, with a length of 2 where C
   returns 20). The fix is a domain rule rather than a special case: THE SIGN IS
   CHOSEN BY THE CALLER, by negating the side where negating is safe (the
   non-negative one, whose magnitude always fits), and the walker takes values `<=
   0` and never negates. `cc_v-cc_v/10*10` then lies in [-9,0] for every input
   because `/` truncates toward zero (the emitted `IDIV`), so the digit needs no
   sign fix and -2^63 needs no exception. `CCItoaNeg` and `cc_dig` are the same
   rule with two sinks — `putchar` for printf's byte stream, buffer-plus-count for
   sprintf, whose return value is a documented API and is why one function cannot
   serve both — and both comments state the rule because `%f` was the third sink
   this change had to consider, and turned out not to be one at all: M37 hands
   the whole rendering to the kernel's engine, so no new digit walk joins these
   two. That is the point of having written the rule down twice — the alternative
   could be taken against a stated rule rather than by accident. `%x` was the same
   habit one
   step away: it negated, so it printed the MAGNITUDE of a signed value, which was
   lite's documented deviation (`1` for -1) and had no answer at all for -2^63,
   where it printed a single `0`. Hex needs no division, so it does not need a
   domain at all: `cc_hexd` now walks the sixteen nibbles from the top with `>>`
   and a mask and skips leading zeros, and masking AFTER the shift makes it
   irrelevant that lite's `>>` is arithmetic. That is C's `%x` for a 64-bit `int`,
   and it is also what the kernel prints, so this removed a documented divergence
   instead of adding one: `ffffffffffffffff` for -1 and `8000000000000000` for the
   most negative quad. The `## C subset limitations` bullet that promised the
   magnitude was rewritten in the same change, because a document that describes a
   wrong value has to stop describing it when the value stops being wrong.
   **Delegation was asked and answered, not assumed.** The host prints this value
   correctly through both of its routes (`StrPrintJoin` and its own `print`,
   measured on 12 values including the edge), so the delegation rule asks why the
   walker stayed in the prelude. The answer is a cost comparison, and it is narrow:
   the host route replaces ~35 lines of prelude C with a helper, a slot and a
   registration, AND buys a per-call-site tax suggestion-0002 measured — about 14
   bytes and an RDI touch per conversion, two of the four `ext` slots — plus two
   things the prelude does not need: a destination, because `StrPrintJoin` takes no
   capacity argument and printf's sink is a byte stream, so `%d` would format into
   a scratch buffer and then be copied out by `CCStr`; and a count, because
   sprintf's return value cannot come from pointer subtraction in generated code (a
   saved object re-answers arena addresses per session). The walker, by contrast,
   is now exact for every value the model can hold, and that is a measurement
   rather than a belief (below). `%f` needed the same question asked, and the
   answer came out whole: lite owns no decimal-float algorithm, and the host's
   `%f` is the kernel's own, so M37 delegates the conversion WHOLE — directive
   text in, characters out — and records the divergences instead of correcting
   them. **[M37 first answered this split, delegating the radix walk, the
   padding and the rounding while keeping the scaling; that design was measured
   and then abandoned, as its entry says.]** Verification:
   144 values across the signed-quad range — the edge and its neighbours, every
   power of ten that fits, powers of two, the 32-bit edge, and 60 random quads —
   each pinned TWICE through `sprintf`: the written length (the return value, which
   an approximately-right walk cannot fake) and the exact content via `strcmp`,
   against python's own arbitrary-precision spelling of the same value. **288 of
   288 checks pass in lite, and 288 of 288 in `gcc`** on the equivalent program,
   which is the control that says the oracle is C's answer and not mine. The
   `printf` path was checked separately against the same reference, and that
   comparison needed TWO transforms rather than one: `%d` to `%lld` (C's `%d` is
   32-bit and lite's `int` is a quad — untransformed, gcc answered `0` for the most
   negative quad) and unsuffixed literals to `LL` (an unsuffixed `-11` passed to
   `%lld` is undefined, and gcc printed `4294967285` for it). Getting either half
   wrong produces a reference that is quietly not the same program. `sprintf.c`
   gained checks 12-14 (the quad edge, its two neighbours, and the `%x` reading),
   every one of them expanded and verified TRUE under C on its own, and its
   assertion moved 12 to 15; check 9's comment — "widest %d" over a 32-bit value —
   was corrected, since that label is what let a 19-digit failure read as covered.
   Suite: 86 PASS with `CC TESTS: 0 failed`, identical across two runs, with no
   FAULT line in either. Saved objects: the prelude is compiled into every module,
   so 13 fixtures (the whole printing family plus the M35 set) were saved and run
   three times each — obj/obj/obj/direct, 0 mismatches, sprintf 15 15 15 15. Two
   host facts came out of the work and are recorded rather than fixed: the host's
   own `%d` is correct where lite's was not (so this was the second implementation
   disagreeing with the thing it duplicated), and lite accepts NO integer literal
   suffixes — `12345678901234567LL`, `12u` and `12ul` are syntax errors, while the
   float suffixes M35 folds are accepted — a C-legal spelling lite refuses, so the
   sweep's lite source spells every literal unsuffixed while its C reference spells
   the same values `LL` — the two programs differ in spelling and not in value,
   which is the only reason the reference is trustworthy.

 - [x] **M37** `%f` for both sinks, delegated WHOLE: lite rebuilds the directive
   as text, hands it to the kernel's float printer, and emits what comes back.
   Every divergence from C that follows is a documented non-standard, not a gap
   to route around.

   **The ruling, and what it settles.** "Use TempleOS we are just delegating."
   "TempleOS is not wrong, it is a non standard implementation of Float, that is
   why we are just delegating." A host that is a non-conforming implementation
   of a C sublanguage is delegated to and then DESCRIBED, not repaired piece by
   piece, and this file already says exactly that about keyboard input in the
   `getchar` bullet above: "C byte semantics on a console are documented
   deviation, not emulation." So M37's subject was never how to make the host
   agree with C. It is what a C caller still has to be given, and where the
   host's answer has to be recorded instead of corrected. Three things, and no
   arithmetic, is the answer.

   **What lite contributes, in full.** (1) The directive is REBUILT AS TEXT and
   handed over with the value's bits — flags, field width, precision and the
   letter `f` — from printf's compile-time scan (`T:/CC/CC_PARSE.HC:3822-3854`)
   or from the prelude's runtime scan (`T:/CC/CC_EMIT.HC:508-531`), so width and
   padding mean whatever TempleOS says they mean. (2) C's default precision is
   SPOKEN rather than inferred: the host's bare `%f` prints no fraction digits
   at all (its precision defaults to 0, measured: 1.5 gives `2`), so both sinks
   write `.6` into the text whenever the source stated none
   (`T:/CC/CC_PARSE.HC:3860-3866`; `T:/CC/CC_EMIT.HC:554-558`). (3) The two
   numeric fields are capped, because `StrPrintJoin` takes no buffer length and
   a `%5000f` would pad 5000 bytes through `cc->fmtbuf`, which is 1024
   (`T:/CC/CC.HH:391`): `CCIClap` (`T:/CC/CC_LIB.HC:358-434`) takes a width run
   down to 1000 and a precision run to 600, keeps a leading `0` because the
   kernel reads that zero as its pad flag and not as a digit (`%01500f` becomes
   `%01000f` and still pads with zeros), and rewrites the text in place from a
   copy of itself — printf's directive lives in the data arena, the rewrite has
   to be idempotent because a `%f` inside a loop runs more than once, and the
   destination is the source. A `*` field width is dropped by both scans rather
   than refused: the engine takes a `*` width from the NEXT argv entry and
   throws when there is none (`T:/STRPRINT.HC:250-254`, `:261-265`), and the
   only entry lite has is the value. Beyond those three, nothing: no digit
   walk, no scaling, no rounding rule, no sign handling, no special values. The
   helper is `I64 CCXFmt()` (`T:/CC/CC_LIB.HC:444-456`) and its body is a
   clamp, one call and a length.

   **The ABI, and why the value needs a local.** `cci_fmt` is dispatched BY
   NAME (`T:/CC/CC_PARSE.HC:2117-2120`, `CCLibCall(cc,CC_LIB_FMT,3,TY_INT)`),
   which is what lets the prelude's C reach the same helper generated code
   reaches; its three `ext` slots are destination, value bits and directive
   text (`T:/CC/CC.HH:203`), the third now a STRING where the number a precision
   used to be. `StrPrintJoin`'s argv is a COUNT plus an ARRAY of quads
   (`T:/STRPRINT.HC:208`) and `case 'f':` reads one of them as `(F64)` (`:527`),
   so `CCXFmt` moves `ext[1]` into a local quad and passes its ADDRESS
   (`:452-454`): a double's slot value IS its bit pattern (M35), so no
   conversion and no decimal parsing is on the way in. The engine returns its
   DESTINATION and not its end (`:875 return buf;`, `buf = dst` at `:228`), so
   the count comes from `StrLen` (`T:/CC/CC_LIB.HC:455`) — the contract M36
   recorded, and the reason `cc_w` in the prelude is still a running total
   rather than pointer subtraction.

   **The kernel's surface, measured and not derived.** All of it came from
   `StrPrintJoin(buf, dir, 1, &bits)` run in TempleOS on this tree:
   `T:/CC/GATES/hostcap.HC` measures the flag, width and `*` rows, the
   `%e`/`%g`/`%E`/`%G` letters and the inf, `-inf` and nan values, and
   `T:/CC/GATES/rows.txt` holds the 38 bit patterns across 9 directives —
   the expectation source for the sweep and the float fixture. Their run
   logs are artifacts and are not committed; rerun the driver for the rows.
   And lite reproduces every row because lite sends the same text.
   `%f` lives at `T:/STRPRINT.HC:524` and its mechanism is the scaling and
   rounding at `:555-599` — `d*=Pow10I64(...)` at `:564`, `:567` and `:572`,
   then `m=Round(d)` at `:576` — and `Pow10I64` returns a table entry
   (`T:/KMATH.HC:675-683`) that `T:/KEND.HC:13` filled with
   `pow10_I64[i+309]=Pow(10.,i)`, a transcendental. A bare `%f` prints NO
   fraction digits, so 1.5 gives `2`; field width is honored (`%10.3f` of 1.5
   gives `     1.500`) and so is the `0` pad (`%010.3f` gives `000001.500`);
   `-` is parsed and then IGNORED for numbers, because `PRTF_LEFT_JUSTIFY` is
   set at `:240` and read only by `OutStr` (`:29`), never by a numeric padding
   path, so `%-10.3f` comes back right-justified and `%-8f` gives `       2`;
   `+`, space and `#` print the REST OF THE DIRECTIVE as text and format
   nothing (`%+f` gives `f`, `%+.6f` gives `.6f`, `%#f` gives `f`), which is the
   host's fallback for a modifier it does not implement and therefore lite's
   answer too — `printf("%+f",1.5)` prints `.6f`; `%e` and `%g` spell
   `1.50000000e0` (`e0`, not C's `e+00`) and `%E` and `%G` print nothing, none
   of which reach this path because only the letter `f` is ever handed over;
   `inf` arrives as `i` and `-inf` as `-i`; and a quiet NaN prints a DECIMAL
   NUMBER — the default QNaN `0x7FF8000000000000` answers
   `9223372036854775808` at `%f` — because the engine rounds the value into a
   quad and has no nan spelling to fall back on. x87's `0.0/0.0` yields the
   NEGATIVE quiet NaN, which is why the fixture negates it to reach the pattern
   that was measured (`T:/CC/TEST/ffmt.c:10-12`).

   **The cap, and the warning the host already makes about it.** The digit walks
   stop at `k<TMP_BUF_LEN-SLOP` — `:579`, `:593`, `:616` and `:625` inside the
   `%f` arm, with `TMP_BUF_LEN` 256 and `SLOP` 8 at `:205-206` — and the
   measured ceiling is 248 characters of which 247 are digits: `%f` of 1e300 is
   pinned as 241 zeros, the point, and six zeros after it, where C prints 301
   digits (`T:/CC/CC_TEST.HC:679`, `:689`, `:699`, `:709`; the same cells as
   text at `T:/CC/TEST/ffmt.c:228-237`).  `%100.50f` of 1.5 is pinned at exactly
   100 characters space-padded left, and `%01000.3f` at exactly 1000
   (`T:/CC/CC_TEST.HC:911`, `:921`).
   `StrPrintJoin`'s own docstring warns about this before any of it happens
   (`T:/STRPRINT.HC:210-216`: do not exceed 18 digits before or after the
   decimal point, because the two halves are held in 64 bits) and so does the
   parser (`T:/STRB.HC:529-534`, quoted in the floating-point bullet above). A
   sink handed a buffer with no length has to keep its own promises, so
    `CCIClap` is what bounds the write: the engine’s digit walk never
    emits more than its 248-character bound (`TMP_BUF_LEN` less `SLOP`),
    a measured rendering topping out at 250 with its sign and point; and
    the bytes `StrPrintJoin` writes are never more than the directive’s
    own field width — the pad loop at `T:/STRPRINT.HC:507-511` writes
    `len-k-i` spaces and then the `k` buffered characters — so capping
    the width caps the write, which is the worst case `T:/CC/CC.HH:391-402`
   For `sprintf` the same caps are the only ceiling on how far `%f` can write
   into a caller's buffer.

   **The two measurements, and their denominators.** Scored against glibc, the
   kernel's own arithmetic — `F64 * Pow10I64(p)`, then `Round`, then the peel,
   with the engine's truncation at 17 significant digits — disagrees on 143,569
   of 631,424 (value, precision) cells, 39,464 values across 16 precisions:
   **22.74%** (a pure-python reimplementation of the kernel's arithmetic — no
   loader; `suggestion-0002` records the run and its script). The direct
   probe is the stronger evidence because it is the host itself: the 38 bit
   patterns of `T:/CC/GATES/rows.txt` handed to `StrPrintJoin` across
   `%f %.0f %.1f %.3f %.6f %.15f %.16f %10.3f %.20f` = 342 cells, of which
   **187 — 54.7% — disagree with C, and 91 of those are cells whose precision
   is stated and at or below 15** (`rows.txt` scored against a C oracle; per
   directive, out of 38 values each: `%.0f` 10, `%.1f` 13, `%.3f` 14,
   `%10.3f` 14, `%.6f` 15, `%.15f` 25). So "state the precision and
   stay at or below 15" bounds the wrong quantity: the error scales with the
   value because the FACTOR is what is off, and it shows up at precisions the
   ceiling calls safe — `%.15f` of 1.5, a value with no rounding problem at all,
   comes back `1.499999999999998`. Both figures, and the correction to this
   file's own pairing of 22.74% with the wrong cell count, are in
   `suggestion-0002` and `suggestion-0004`.

   **Priced and declined: the fraction-scaling design, which is NOT what
   ships.** An earlier pass of M37 found the seam INSIDE the conversion — the
   host scales the WHOLE value by a table built with `Pow(10.,i)` — and split
   the work by step instead of delegating it whole: the radix walk, the left
   padding and the rounding stayed the host's and only the scaling moved into
   the helper, on a factor built from exact integer quads (`10^0..10^15` as
   I64s, converted by assignment, because `F64<-I64` assignment is correctly
   rounded where a decimal literal is not — `T:/STRB.HC:574` and `:592` are
   where a literal picks the table up). It was measured before it was dropped,
   and the numbers are worth keeping as the price of the alternative: 0.1733%
   for that model on 58,392 values across 16 precisions (934,272 cells; the
   run is recorded in `suggestion-0002`) and 2253 of 1,396,624 = 0.16% on the
   sweep that followed,
   against the 22.74% above. Three rounding rules were priced in the same runs
   — round the scaled fraction 0.16%, half-away-from-zero `trunc(x*10^p+0.5)`
   0.51%, the scaled-remainder tie test 0.38% — and a Dekker two-product
   residual scored WORSE, 0.30%, because its own correction terms round. Two
   other artifacts went with the design and should not be looked for in the
   code: a 15-ceiling on precision (printf refusing `%.16f`, sprintf clamping
   it), and magnitude gates reading the exponent FIELD (`>= 1086` for 2^63,
   `>= 1075` for 2^52) feeding a significand-doubling route for values a quad
   cannot hold. The ruling supersedes all of it: a 0.16% implementation of C's
   `%f` living inside lite IS the second implementation the delegation rule
   exists to prevent, and correcting the host's float is no longer in scope.

   **What a sink that CAN diagnose refuses.** printf reads its format at compile
   time, so it refuses any directive whose letter is not `f` — with the message
   it always gave (`T:/CC/CC_PARSE.HC:3855-3859`). The float arm is now the
   catch-all `else` (`:3807`), which is what catches a directive that BEGINS
   with a flag or a width: `%5d`, `%010d`, `%10.3d`, `%.2d`, `%x`, `% c`, `%n`,
   `%u`, `%5s`, `%e`, `%g`, `%lf`, `%hd`, `%.2e` and `%ld` — 15 directives,
   each a compile error: `T:/CC/GATES/mkrefuse.c` emits one single-directive
   program per refusal (`T:/CC/GATES/r01.c`..`r15.c`) plus the acceptance file
   plus the HolyC driver `T:/CC/GATES/rfdriver.HC`, which runs all 16 through
   lite and prints each return code — 15/15 rc=-1 on the run of that driver.
   A precision now takes any value: there is no ceiling left to breach, and the
   `printf: %f precision over 15` diagnostic is gone. `%n` is refused rather
   than quietly printing nothing. The acceptance file `T:/CC/GATES/acc.c`
   compiled and ran rc=0 carrying 11 flag/width/precision directives through
   BOTH sinks — `%20.15f %010.4f %-10.4f %#10.2f % 10.2f %+10.2f %.600f
   %999.999f %01000.3f %3.1f %-3.1f` — and its 22 renderings (the `A` rows are
   sprintf, the `B` rows printf) are byte-identical to the `K` rows the SAME
   driver run measures from `StrPrintJoin` on the same bits:
   `     1.499999999999998`, `00001.5000`, `    1.5000`, and `10.2f` three
   times over for the `#`, space and `+` forms the kernel renders as text. The
   caps ride along: acc.c spells `%.600f` and `%999.999f`; the driver measures
   the kernel on exactly those directives — K07 renders `%.600f` as 250
   characters, `0.` followed by 248 zeros, the engine's own digit-walk cap
   swallowing the stated precision, and K08 renders `%999.999f` at 999 — and
   lite's renderings, the second of which reaches the kernel as `%999.600f`,
   match both byte for byte. The width cap is pinned INSIDE the committed
   gates: `T:/CC/TEST/ffmt.c:431-452` pins `%01500.3f`, capped to `%01000.3f`
   before the call, to the 1000-character rendering
   `T:/CC/CC_TEST.HC:918-927` measures from `%01000.3f` directly.

   **The arity change the new scan forced, measured.** sprintf's non-`f` letters
   now receive the buffered flag/width run as TEXT and keep their own arms, so
   `sprintf(b,"%5d %d",5,7)` writes `55 7` where it wrote `5d 7`,
   `sprintf(b,"%-2d",7)` writes `-27` where it wrote `-2d`, and
   `sprintf(b,"%*d",10,7)` writes `10` where it wrote `*d` — C prints
   `         7` for the last. One argument is still consumed per `%`
   (`T:/CC/CC_EMIT.HC:507`), and a precision is still parse-consumed for every
   directive but forwarded only by `%f` (`:533-551`, `:552-566`), so `%.2d`
   prints its number and the `.2` is dropped rather than printed. Three cells in
   `T:/CC/TEST/ffmt.c` (`:464-482`) pin all three. This is the half of M37 that
   changes programs which never mention `%f`, and it is a deviation the kernel
   chose rather than one lite invented.

   **Verification.** Suite: 89 PASS with `CC TESTS: 0 failed`, identical across
   two runs of `T:/CC/CC_TEST.HC`, no FAULT line in either. The float fixture
   reports `TOT ffmt 63 63` and `PASS ffmt = 63`
   (`T:/CC/CC_TEST.HC:402`; 63 cells in `T:/CC/TEST/ffmt.c`) — fixture and
   probe are both emitted from the `T:/CC/GATES/rows.txt` cell list by
   `T:/CC/GATES/genffmt.c`, the C view taking each directive as C SOURCE
   spells it and the probe body taking the one lite WILL send. The fixture's
   printf line — `ffmtP|1.500000|1.500|1.499999999999998|     1.500|` — is
   the compile-time path, diffed against the same four cells of the probe.
   `PASS float kernel = 58 cells` comes from `CCProbeFloat`
   (`T:/CC/CC_TEST.HC:445-929`), which calls `StrPrintJoin` directly on the
   same bit patterns with the directives LITE SENDS — `.6` written in where
   the source stated none, `*` dropped, an over-long field or precision
   capped — so the pair pins both halves of the contract: lite sends what C
   means, and the kernel's rendering is the answer. `PASS objfloat = 63`
   (`T:/CC/CC_TEST.HC:214-233`) compiles the fixture, saves it, reloads the
   object and runs it: a lowered float directive bakes TWO addresses, the
   `cc->fmtbuf` field through `CC_B_CTRL` (`T:/CC/CC_PARSE.HC:3885`, `:3896`)
   and the arena directive string through `CC_B_LOCALS` (`:3890-3892`), and
   the helper then REWRITES that string in place, so this is the case that
   proves both rebase. Delegation sweep: 38 bit patterns x 9 directives x 3
   sinks = **1026 cells**, emitted by `T:/CC/GATES/genm37.c` from the
   `rows.txt` renderings into `T:/CC/GATES/gs0.c`..`gs9.c` and run by
   `T:/CC/GATES/gsd.HC`. The 684 sprintf cells self-check INSIDE the program
   — literal format and variable format, against the kernel text — and each
   chunk prints its own cell total and failure count, so a truncated run
   cannot masquerade as a pass; the driver's run sums to 684 cells and 0
   failures across the 10 chunks. The 342 printf cells print their expected
   text in the same line and are compared outside the program: **0
   differences** against the kernel's answers, with the expectation field
   itself matching the text `genm37.c` read out of `rows.txt`, 342/342. Run
   the SAME generated programs under gcc instead: **328 of the 684 sprintf
   cells fail**, and that failure set is exactly the 164 matrix cells whose
   printf lines disagree with the same expectations — a cell's two sprintf
   sinks and its printf line fail together or not at all. The mapper's 187
   is the same finding on the directives as SOURCE spells them: the 23-cell
   difference is the bare-`%f` column, where the engine prints no fraction
   digits and C prints six — precisely what the `.6` composition exists to
   erase, and lite never trips it. Either way the C-versus-kernel
   disagreement is most of the matrix, and it is the EXPECTED state of
   affairs now rather than a defect — the same finding as the `%.15f`-of-1.5
   and `%.6f`-of-1e10 cells, reached from the program side.
   M36's integer sweep is still 288/288 in both engines and the M35
   mixed-comparison sweep still 0 differences. No test VALUE in the fixture or
   the sweep is a decimal float literal: values are built from a significand
   and an exact power of two (`T:/CC/TEST/ffmt.c:27-45`), because a literal
   picks up the table's error (`T:/STRB.HC:574`, `:592`) and a formatter checked
   against drifted inputs proves nothing. No line over 83 characters added.

   **Two traps in the harness itself, kept because both could have hidden a
   wrong answer.** (1) The first sweep died on `code buffer overflow` and still
   printed zero failures, because a partial program that never reaches its count
   line is indistinguishable from a passing one — lite compiles one file into
   `CC_CODE_SIZE` = 64 KiB (`T:/CC/CC.HH:137`; `CCByte` enforces it at
   `T:/CC/CC_EMIT.HC:632`), so the harness chunks by emitted weight and each
   chunk reports its own cell total. (2) The refusal probe passed six
   conversions to one sprintf call, overrunning lite's 4-argument frame and
   reporting a parse error instead of the format behaviour it was there to
   witness.

- **A HolyC identifier can collide with a KERNEL macro.** `pi` is
  `#define pi (0x400921FB54442D18(F64))` in KERNELA.HH (with `eps`, `inf`,
  `ans`, `ON`, `OFF`, `RED`), so a local named `pi` inside the CC unit was
  substituted before the host parsed it, and the diagnostic was `ERROR:
  Expecting '*' at "INT:400921FB54442D18"` pointing at a F64 cast — nothing
  that names the problem. The kernel headers define 2194 macro names; check a
  new identifier against them (M31's `pi` was the only collision in this
  unit) before hunting through the parser.
- **A check you add can be the first thing that tells the truth about a bug
  upstream of it — read a new failure as evidence, not as the bug.** M34's
  "redeclaration has a conflicting type" fired on `extern struct pt g;` followed
  by `struct pt g;`, both legal C. The check was right and the OLD declaration
  was wrong: a qualified tag spec does not take `CCParseTop`'s struct branch (it
  requires the statement to START with `struct`), it falls through to the keyword
  path, which received the tag id from `CCAcceptBase` and then passed `-1` to the
  declarator — so the object was TY_STRUCT with no tag, `sizeof` answered one
  slot, and every member reference failed. Before the check, the shape was caught
  only by the downstream "global redefined" refusal: the right verdict for the
  wrong reason, and merging declarations removed the refusal. The sibling bug was
  `union pt u;` after `struct pt { … };` — struct and union tags share ONE
  namespace in C and the kind must match, so looking the tag up without asking
  its kind silently gave the object the STRUCT's layout. Ask the kind
  (`CCStagUse`) at every tag USE; the definition form is already refused outright
  by the redefinition check, which is why that path never needed it.
- **A guard owns exactly the statement under it — reformatting a comment can
  move a return inside one.** `if (cond)` followed by `return X;` is one
  statement; if a comment reflow makes a DIFFERENT line the body, the return
  becomes conditional and the control flow changes without changing any token
  that looks suspicious. It happened here: a tail rewrite left
  `return FALSE;` as the body of `if (!cc->spec_extern)`, so every `extern`
  function-pointer declaration skipped the return, fell through into the
  plain-declarator path and reported "expected name after type" pointing at a
  `(`. Ten shapes went red, one of them a fixture case that had worked since
  M18, and the fp code I had just rewritten was blamed for all of it. Do not
  reflow comments and move code in the same edit; when a guard's body is a
  single statement, put the comment ABOVE the guard, not between it and the
  statement.
- **A driver that runs several programs in one process attributes diagnostics
  to the wrong one.** A compiled C program's `printf` output is produced while
  its call is evaluated — i.e. BEFORE the harness marker line that reports its
  exit code — so in a multi-file run the diagnostics for case N appear above
  case N-1's marker. A collector that files "the lines before each marker"
  under that marker reports green for shapes that fail, and it did: extern
  function-pointer declarations looked fine across a 12-shape run while every
  one of them was failing. Either run one file per process when a verdict
  matters, or attribute the lines AFTER a marker to it and say which convention
  you used.

- **Clear a flag where every path passes, not where one path sets it.**
  `spec_extern` and `spec_static` were reset inside `CCAcceptBase`, but a tag
  declaration with declarators and an enum body with declarators bind their
  names without calling `CCAcceptBase` at all, so the PREVIOUS statement's
  promise leaked into the next: `extern int e;` anywhere earlier in a file made
  `struct pt g;` read as a non-definition, and the fptab fixture failed on a
  line three functions away from either declaration — which read like a
  function-pointer bug and was a statement-boundary bug.
- **An invariant belongs where the question is meaningful, not in the helper
  that happens to do the work.** The late-array-length check first went INSIDE
  `CCReserveArray` as "the arena watermark must still be one slot past this
  symbol's offset". That is true at every first declaration, but a block-scope
  `static int (*t[3])(int)` reserves its slots BEFORE `CCMakeStatic` claims the
  arena slot, so at that call `s->offset` is a FRAME number and the arena
  invariant cannot be asked at all; a shape that had worked since M26 began
  erroring. Move it to the two places where a length is genuinely late.
- **A parallel array read without being written is the PREVIOUS call's data,
  and that looks plausible.** `CCArrayInit`'s `vk[]` (per-value relocation kind)
  was read for every initializer value, but the whole-object string arm of the
  collector wrote only `vals[]`. Bytes came out right in place, so a direct run
  passed; a saved object reloaded wrong, because the bogus kinds sent byte
  stores down the pointer path and filed an 8-byte patch at a byte position.
  When a callee reads two arrays in parallel, grep for every writer of each.
- **A helper that can return text from either of two buffers must return its

  LENGTH, and the caller must pair them.** M33's `CCPrepLogical` hands back the
  line to scan: unchanged when nothing was spliced, a copy when something was.
  A caller that still computed the line end from the ORIGINAL text paired a
  pointer into one allocation with a length from another, and a directive then
  scanned unrelated bytes — the symptom was `unknown directive '#'` in a file
  with no stray `#` anywhere, in a function that had compiled for ten
  milestones. Same shape in C, same trap: `ptr` and `len` are one value here.
- **"Nothing moved" is not the same as "not found" — check which of the two is
  the COMMON case before writing the guard.** `CCPrepCont` finds the splicing
  backslash by scanning back over trailing spaces and tabs; when a line ends
  directly in the backslash the scan does not move at all, so my first version
  read the unchanged index as "no continuation" and spliced nothing. Every
  construct that needs a splice then failed with a DIFFERENT message (`bad
  parameter name in #define`, `unterminated argument list in a macro
  invocation`, `unexpected token in a #if expression`), which is what made it
  look like four bugs instead of one: the corpus diff said "our output still has
  the backslash in it" in a single line.
- **HolyC block comments NEST.** A C example inside kernel prose may itself
  contain a comment, and `/* ... P2(1 /*c*/, 2) ... */` compiles because the
  inner `*/` closes only the inner `/*` (probed). The mirror is the hazard: an
  inner `/*` with no matching `*/` leaves the OUTER comment open, and the host
  swallows the rest of the file — the statements below it never compile and
  the only symptom is the loader booting anyway (`ERROR: File not found:
  "Doc"`). Balance every `/*` you quote inside a comment, or write the example
  without one. M33 nearly shipped exactly that: the comment above
  `CCAssert(30,...,"cont")` quoted a `/*` to describe what the fixture pins, and
  a HolyC comment that never closes eats every assert below it. Caught by
  reading the inserted comment rather than by a run: as the paragraph above
  says, the file would still have compiled, and the only sign was a PASS count
  that had stopped growing.
- **A fixture mark line must carry exactly ONE `%d`, fed by `tot`.** The gcc
  cross-check puts `printf("@N=%d\n",tot);` after every check; if the line is
  built by substituting N into a format that still held two `%d`, the
  surviving `%d` is fed the line NUMBER, so every mark reads +1 whatever the
  program computed. That hid a wrong expectation in macros.c (check 3:
  `ADD(1,TWO(2))` is 5, not 6) until the marks (44) disagreed with `tot` at
  the return (43). Compare the mark TOTAL against the check count, not just
  mark against mark.
- **Prototypes of functions defined in ANOTHER CC_*.HC belong in CC.HH's
  forward block**, not in the consumer's extern list: an `extern` that
  arrives after the definition is a promise from elsewhere and never
  resolves (see the rule below — M24 hit it as `Undefined Extern at
  &CCArrayInit+0x392` for `CCArenaPtr`, which is defined in CC_EMIT.HC,
  the file the umbrella splices first). Missing prototype → a call that
  silently runs nothing (M20); too-late prototype → a link failure (M24).
  When a brand-new call site misbehaves, grep its output for "at &".
- **A wrapped `if`/`while` condition is the hazard by itself; a comment in it is
  only one way to hit it.** The bullet below blames a `//` on the `{` line, and
  that is right, but M32 hit the same failure with no comment anywhere: a
  condition split over lines whose closing `)` sits alone on its line, followed
  by `{`, fails as `ERROR: Missing ')' at "{"` pointing at the line AFTER the
  brace. Keep the whole condition on one line whenever the body is a
  brace-enclosed block — compute a long test into a local first.
- **A same-file forward prototype must be `extern`, and the symptom of forgetting
  is a warning far from the cause.** M32 wrote `I64 CCPrepExQ(CCCtrl *cc,
  CPrepExpr *pe);` without `extern` ahead of its definition and the build gained
  `WARN: Unused var 'cc' in 'CCPrepExQ'` and `'pe' in 'CCPrepExQ'`: the host had
  made a SECOND function whose parameters nobody passes, and reported it as
  unused-variable noise in a file that compiled. Diff the warning list against
  the baseline on every run, not just the PASS count.
- **A `//` comment on the `{` line that follows a multi-line `if`/`while`
  condition breaks the host parse**, reported as `ERROR: Missing ')' at "{"`
  pointing at the line AFTER the brace. A comment inside such a
  condition, on the line that closes its parentheses, is fatal the same way.
  Comments after `{` following a ONE-line condition, and comments on a `}`
  line, are fine (all four shapes probed). So: put the prose ABOVE a
  multi-line condition and keep the brace line bare — a long two-term roll
  guard with an inline comment is enough to trigger it, which cost two host
  parse failures during M30.
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
- **An x86 ModRM reg field is the DESTINATION** in the `r64,r/m64,imm32`
  forms: `IMUL RCX,RCX,imm32` is `48 69 C9`, while `48 69 C1` is
  `IMUL RAX,RCX,imm32` — which multiplies the other operand and leaves the
  scaled result in a register nothing reads (M29 turned `2+a` into `a*8+2`
  that way). Every other IMUL in CC_PARSE.HC targets RAX with `0xC0`, so a
  wrong reg field is silent. When a new encoding misbehaves, decode it: the
  map from a fault line is `rip - cc.code - entry_off` into the dumped bytes.
- **A fixture comment that does not end in `*/` deletes the next line, and
  deletes it for gcc too.** Polishing a check marker dropped one terminator
  and the following assertion became part of the comment: engine and oracle
  then agreed on everything and the only evidence was 31 marks printed for 32
  checks. Count the marks against the checks; a cross-check compares two
  engines, never an engine against the fixture's intent.
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

- **A `!` written directly on a parenthesized FLOATING comparison answers 0
  for every pair in this build** (verified by probe): `!(a > b)` on two `F64`s
  gives 0 when the compare gives 0 AND when it gives 1, while
  `r = a > b; return !r;` returns the right answer both times. `<=`/`>=` first
  shipped as `!(a>b)`/`!(a<b)` and were therefore always false, and the symptom
  was a SEGFAULT of the loader, not a wrong number: a `<=` that never becomes
  true turns a bounded C recursion into an unbounded one and the generated
  program runs out of its private stack. Negate through an `I64` temporary, or
  delegate the comparison to the host's own `<=`/`>=` (measured correct for
  finite pairs, for signed zero and for NaN pairs). The same hazard does NOT
  apply to `!(a<b || b<a)`, where `||` has already produced an integer.
- **On an `F64` lvalue, assignment CONVERTS and only a POSTFIX cast
  reinterprets** (verified by probe): `a = 4616189618054758400;` puts the
  NUMBER 4.6e18 into `a`, while `a = qa(F64);` puts the double whose bits are
  `qa` into it. A probe that assigned bit patterns to `F64` locals without the
  cast spent an hour reporting arithmetic that was never the arithmetic under
  test. Related: comparing an `F64` against a literal (`x != 0.0`) gets folded
  into a sign test and mis-classifies `-0.0`, so operands whose zero-ness
  matters must arrive from memory.
- **`-(bits of x)` is not `bits of -x`.** Setting the sign bit ADDS 2^63;
  negating the quad flips the whole two's-complement value. A hand-written
  constant for -1.0 (`-4607182418800017408`) is the pattern of -4.0, and a
  "two-negative pair" built from it tests something else entirely. Generate
  these constants with a tool, and print what a quad actually holds: `%10.3f`
  formats the double whose bits the quad carries (verified: the bits of pi
  print ` 3.142`), which is how the mismatch was found.
- **A runaway recursion in GENERATED code looks like a compiler crash.** An
  emitted C program that recurses without a reachable base case exhausts its
  private stack and the loader dies with SIGSEGV and no `[3d] FAULT` line,
  because the fault is in the code buffer, not in HolyC. Before suspecting the
  codegen, check whether the loop/recursion guard is the thing that is wrong.
- [x] **The tree could not compile itself, and HCRT.BIN has been frozen since
  2022 because of it (2026-11-05).** `Cmp("FULL_PACKAGE.HC",…)` died at
  `T:/AMATH.HC:19` with `Invalid lval at <E3>` and `Errs:2 Code:0 Size:0` — the
  compiler emitted NOTHING, so the rebuild never produced an image and every
  edit to a `#include`d kernel file (including the merged `KERNELA.HH`
  prototype fixes) stayed inert, because `FULL_PACKAGE.HC:29,32` BINFILEs those
  headers into the image and boot parses the EMBEDDED copy. Measured rule
  (`sh T:/CC/GATES/abprobe.sh` re-derives it in ~60 s, six compiles of one
  construct): a raw 0xE3 standing ALONE as a token is rejected; the same
  character as KERNELA's own TEXT escape is accepted; 0xE3/0xE8/0xE9 inside a
  longer identifier is accepted; plain ASCII is accepted; and adding
  `#define \E9 e` does not rescue a lone raw 0xE3, so this is a token rule and
  not a missing macro. `KERNELA.HH` spells every one of these characters as
  text (`#define \E3 pi` :360, `F64 \E8` :4582, `F64 \E9_x` :5421) and holds no
  raw high byte at all, while the IMAGE stores them raw
  (`Mat4x4RotX(I64 *m,F64 <E8>)` is text inside HCRT.BIN): text-in-source is the
  supported spelling. Walking all 155 files reachable from FULL_PACKAGE.HC, five
  definition files held the 25 offending tokens — `AMATH.HC` (12, the only one
  that stopped the build), `DOLDOC/DocGr.HC` (6), `GR/GrMath.HC` (9),
  `GR/GrSpritePlot.HC` (6), `GR/SpriteEd.HC` (3) — and all are now text or
  ASCII with no semantic change: the new `Wrap(F64 e,F64 base=-pi)` is
  byte-identical to `KERNELA.HH:4340`'s own prototype. A whole-image build is
  ~3 minutes here (measured: consecutive full builds 16:57 and 17:00), not tens.
  The rebuilt image (1117808 bytes, `Errs:0 Code:105445 Size:110E70`) was gated
  before it was staged, and `T:/CC/GATES/host.HC`'s 342 kernel renderings diff
  EMPTY against the committed `rows.txt`: the new kernel prints every float cell
  exactly as the 2022 one. Two traps this cost, now in AGENTS.md: a `-c` run
  Cd()s into the HOST cwd mounted at Z: (`main.c:167-176`), so a relative name
  in `Cmp` is not `T:/`; and a failed `Cmp` OVERWRITES its `out_name` with an
  empty module (48 bytes here) after which every boot SIGSEGVs silently, because
  `tos_aot.c:250` dereferences an unchecked symbol lookup before
  `SetupDebugger()` at `:253` — the same failure that produced the reported
  "window border, no content". `HCRT_BC.BIN` and `T:/HCRT_BC.DBG.Z` are still
  the 2022 pair, so `-b` still boots the old bounds-checked image.
- [ ] **M38** `<math.h>` builtins, delegated whole like `%f`: `sqrt→Sqrt`,
  `fabs→Abs`, `exp→Exp`, `log→Ln`, `log10→Log10`, `pow→Pow`, `floor→Floor`,
  `ceil→Ceil` (+ the trig that exists: `Sin Cos Tan ATan ASin ACos Sinh Cosh`).
  The mechanism is PROVEN, so this is now writing, not research: a `CC_LIB`
  helper that makes one direct HolyC call binds an asm-`_extern` math entry
  (`Exp` `Floor` `Pow`) exactly as well as an `_intern`-backed opcode
  (`Sqrt` `Abs`) — no dispatch table and no `CallExtStr`; the AOT `Cmp`+`Load`
  route is dead for other reasons (an AOT unit's scope chains to
  `cmp.asm_hash`, not `Fs->hash_table`, so kernel names are invisible to it).
  End-to-end shape already runs: 5 new slots + 5 one-call helpers + 5
  name-dispatch arms + `math.h` produced a fixture whose 8 cells matched the
  kernel oracle cell for cell, and the SAVED object reproduced all 8 in a NEW
  process, `rc=0`, no fault. 20 slots installed of `CC_LIB_SLOTS 32`. Two gaps
  `CCLibCall` must close: no int→double conversion (`sqrt(2)` reinterprets the
  quad 2 as a denormal and prints 0.00000000000000000) and interception of a
  user's own `sqrt`/`pow`. Record-not-correct divergences: `pow(-2,0.5)` is
  +1.414 (C: NaN), `pow(10,3)` is 999.99999999999872, `acos` is 1 ulp low,
  `exp(1000)` saturates to DBL_MAX rather than +inf, kernel `Round` is
  nearest-even, a NaN renders with a negative sign. Also M38's, from the float
  bullet above: converting an overflowing integer literal as UNSIGNED when the
  destination is a double.
- [ ] **M39** the TinyCC `tests2` port, as its own milestone and not an
  appendage to M38. Verified against repo.or.cz/tinycc.git: the real suite is
  `tests/tcctest.c` (4538 lines) + `tests/tests2/` (141 `.c` with 141 `.expect`
  goldens, BSD-3 via picoc) + `tests/pp/`; there is NO `run_test.c`, no `.tc`
  files and no `libctest` in that history. Its goldens are glibc text produced
  by gcc — the WRONG oracle for a compiler whose `%f` answer is the TempleOS
  kernel, which disagrees on 328 of our own 684 sprintf cells — so take the
  CASES and discard the goldens, converting each into our shape (a
  self-checking fixture returning a pass count, pinned by `CCAssert`). ~70 of
  141 use nothing we lack, ~40 are realistically convertible. The glue is the
  first unit of work: a `cccheck.h` `CK()` macro and a fixture-directory loop
  in `CC_TEST.HC`. Shortlist chosen for where our fixtures are thinnest:
  `11_precedence`, `13_integer_literals` (our 19-digit defect),
  `26_character_constants`, `19_pointer_arithmetic` (feeds `double *`),
  `90_struct-init` (the non-computed-goto half). Excluded as stated: 32-bit
  shapes, `long double`/`%Lf`/`%g`/`%a`, computed goto, bitfields, `__asm__`,
  VLAs, GCC case ranges, stdio-to-a-file, `assert`, argv games, link/bcheck
  machinery, and `46_grep.c` (DECUS "not for profit"). Vendoring would need
  `T:/CC/TEST/tcc/LICENSE.txt` plus attribution; paraphrasing the 15-40-line
  bodies and citing filenames avoids the question.
## Codegen invariants

- Expression value always lands in RAX; binary ops `PUSH` the left
  half and `POP` it back into RCX.
- **A `TY_DOUBLE` value is one quad holding F64 bits, and the only thing that
  may be done with it is hand it to a delegated helper** (M35). `CCFloatOp` is
  the single place an operator chooses between the integer and the floating
  model, and it answers `-1` when the integer model handles the operator, so a
  float branch is additive to every layer rather than a rewrite of one; the
  constant folder folds through the SAME map (`CCConstFoldBin`) and the SAME
  helpers (`CCFold`), which is why the compiler and the compiled program cannot
  disagree about `1.5 + 2.5`. Crossings between the models are explicit and
  there are exactly five: cast, assignment, argument, `return` and initializer.
  The first four are emitted calls (`CCConvRax` while the value is still in
  RAX); the fifth is compile-time (`CCInitConv`, with the value's own floatness
  travelling in `vk[i]` as `CC_VK_FLT`) because initializers are folded into
  arena bytes and MOVABS operands before anything runs. Refusing a crossing is
  always allowed; letting a double's bits flow into the integer model is not:
  it produces a number, which is the one outcome the correctness policy
  disallows. The SAME map decides WHICH operand is converted (`lconv`/`rconv`),
  and C converts both operands before every comparison (6.5.8/6.5.9) as well as
  before the arithmetic — so a delegated operator that wants F64 bits in both
  slots has to say so HERE, not in the caller, or it compares the bits of one
  side against the number on the other. `ext[0]`/`ext[1]` hold a delegated
  helper's two operands, and a conversion helper writes only `ext[0]` — that is
  what makes it safe to convert one operand while the other is staged in
  `ext[1]`.
- Every emit goes through `CCByte`/`CCWord`/`CCQuad` (bounds-checked).
- Initializer stores must use the SAME element scale the index path uses
  (1 for char arrays, 8 otherwise) and a byte store must name a low byte
  (CL/DL) or carry REX.R — ModRM 0x38 without a REX names BH, which
  compiles, runs and writes the wrong byte (M22). Aggregate values are
  landed as arena bytes for globals/statics and as emitted stores for
  frame objects. Since M24 both tables carry relocations (arena pointer
  slots included), so a code address CAN initialize an arena object — a
  function table is the case that needs it most.
- Initializer TARGETS are produced by one walk, not by two rules (M30): the
  cursor `CCInitMap` maintains moves over SUBOBJECTS in flattened order and
  BOTH a brace enclosure and a designation move it — an enclosure takes a whole
  element only when the cursor is at an element boundary OF AN ARRAY (`narr`
  says how many elements exist; -1 for a scalar tag, 0 while a length is being
  deduced) and otherwise takes the single slot under the cursor, and a closed
  element group resumes AFTER the element, never in its empty slots. Any new
  initializer syntax must move that cursor, not compute a position beside it,
  or the two disagree for arity > 1 (which is how `{{5},9}` landed in m[1][0]
  while `{[1]={5},9}` looked legal). A designation written BEFORE its brace
  moves the cursor before the group classifies its target and one written
  INSIDE it moves the cursor after (`dfl[]` carries the difference); both orders
  are load-bearing. Because both writers take values by ASCENDING target index,
  a designated list must pass `CCInitOrder` first, and the excess test is the
  HIGHEST index touched, not the last value in the source.
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
- Expression SHAPE beats residue (M29): `CCStepOf` and the subscript arm read
  `celem`/`celebsty`/`esz`/`esz2` ONLY for a pointer-typed value, because
  those fields are written by whichever arm produced a base and cleared by
  only some of the layers that combine values — a comparison or a `sizeof`
  hands back an integer still carrying its last operand's marker, and trusting
  it turns `tot + (p==q)` into a scaled pointer add. Every step consumer goes
  through `CCPtrStep`/`CCStepOf` so `p+n`, `n+p`, `p+=n`, `++` and `p[n]`
  cannot drift apart, and `CCPtrStep` (step) stays a SEPARATE function from
  `CCArrEltSize` (element size): a pointer is an 8-byte element that steps
  sizeof(pointee). A COUNT result is typed TY_INT with esz/esz2/celem cleared,
  or the next subscript scales it a second time.
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
- Macro expansion has ONE recursion channel and ONE scratch (M31):
  `CMacScan.mask` is the set of macros whose expansion is open on this path
  (blue paint) and it is threaded into every nested scan — an argument scan
  gets the CALLER's mask, a body scan gets the caller's mask plus the bit of
  the macro being expanded. That pairing is what makes `F1(F1(5))` give
  `((5))` while `#define SELF SELF+1` expands once; do not add a second
  guard, and do not bring back a depth limit (M13's `xdepth<4` is gone, and a
  limit truncates a legitimate chain in silence). The argument scratch is
  FIXED capacity (CC_PREP_SCRA) because every open scan caches a region
  pointer — growth would leave the outer scans reading a freed buffer, so
  overflow is a diagnostic, not a realloc. Raw and expanded argument text
  BOTH live in the scratch so that one base addresses them; a raw span read
  against the region a body is being scanned in is garbage. Offsets, never
  pointers, for anything that outlives a put. `CMacScan.isarg` says whether a
  region came from the machinery (a body or an argument) or from a source
  line, and the `__VA_ARGS__` rule depends on it because the continuation
  lines of a multi-line block comment are ordinary text to this scanner.
  A macro's replacement text is validated when it is DEFINED
  (CCPrepCheckBody), and only for a group that is ACCEPTED — a skipped
  `#if 0` region never defines, and so never validates.
- Conditionals have ONE frame stack with FOUR bits, and `defined` is decided at
  scan time (M32): `frm[i]` bit0 = this branch emits, bit1 = this chain has
  taken a branch, bit2 = the enclosing region accepted when the directive ran,
  bit3 = an `#else` has run at this level. Every writer keeps all four
  consistent: `#else` sets bit3, `#elif` requires bit3 clear and takes the chain
  only when bit2 is set and bit1 is clear, and a new `#if` pushes bit3 = 0.
  A condition is expanded AND evaluated under that same gate, so a decided chain
  never reaches CCPrepExprLine; making it unconditional there would invent a
  diagnostic C does not make. `CMacScan.incond` says whether a region is a
  controlling expression or a rescan of one: it is what makes `defined` an
  operator, it is propagated into BODY scans and cleared for ARGUMENT scans, and
  it never reaches the C parser because the expanded condition is evaluated and
  dropped. The evaluator's `live` flag is the short-circuit channel — both arms
  of `&&`, `||` and `?:` are parsed, and only a live operand may report division
  by zero. `CPrepExpr.bad` stops the walk after the first diagnostic so one
  malformed condition reports once. Condition text shares the fixed
  CC_PREP_SCRA scratch with macro arguments and is released after use.
- Prep reads LOGICAL lines and knows ONE directory per include depth (M33):
  `CCPrepText` asks `CCPrepLogical` for the line to scan, and that helper
  either hands back `in` unchanged or a copy in its own buffer — so the length
  and the terminator it reports must be used WITH the pointer it returned, never
  mixed with the original text (`e = txt + len`, `in = term + 1`). Joining
  happens before a directive, a comment or a literal is recognised, which is the
  only reason one mechanism covers a body, a name, a condition and a call's
  argument list; the C parser is downstream and knows nothing about it. One
  output newline per physical line consumed keeps that line structure.
  `CPrep.dir[depth]` is the directory of the file being scanned at that depth
  (`""` when the name says nothing about a directory), seeded from `cc->fn` —
  the file whose text is being compiled, which the caller that read the text is
  responsible for setting, `CCInit` deliberately carrying it across the
  zeroing — and pushed per include from the path that was opened. A quoted name
  searches `dir[depth]` first and the header home second; an angle-bracket name
  skips the first, and `FileFind` is the probe because `FileRead` prints when
  the answer is no.
- ONE file-scope name is ONE object at ONE address with ONE type (M34): the
  first declaration of a name claims the arena slots and every later
  declaration merges into them, because `CCSymFind` scans newest-first over a
  table whose file-scope entries are never erased, and `CSym.def` /
  `CSym.has_init` record what the declarations have promised. Code compiled
  between two declarations has already baked `locals + s->offset` as an absolute
  address, so a redeclaration that re-claims storage leaves one half of the
  program pointing somewhere else and nothing downstream can see it. A later
  declaration may therefore add NOTHING except the length an `extern int a[]`
  left open, and that only by extending `cc->goff` above the slot the object
  already owns (`CCGlobalTopUp` — an implementation limit, not a C rule).
  `CSym.refs` is set only by `CCVarAddr` — the one place a name lookup turns
  into storage — and deliberately NOT by a silent `sizeof` scan
  (`cc->noemit`), so the "declared extern, never defined" report answers the
  question a linker asks and no more.
  `s->dim == 0` on a global array means "no declaration has stated a length",
  in both declarator arms, and it is a PARSING state, not a storage state: the
  end-of-`CCParse` pass resolves it to 1 (lite's stated assumption, see the
  limitations bullet), so nothing outside those two places may read it, and a
  brace list may deduce only on the declaration that owns the layout (`grow` in
  the function-pointer arm) — a later one fits the slots that exist.

- Every producer of an initializer VALUE must also produce its KIND (M34):
  `CCArrayInit` and `CCStructInit` read `vk[i]` beside `vals[i]` to decide
  whether a value is a plain constant or a pointer needing an arena relocation,
  so an arm of the collector that fills `vals[]` without filling `vk[]` inherits
  whatever the previous initializer left in that stack slot. Arena relocation
  positions are therefore 8-byte, slot-aligned quads — a byte-strided array must
  not take the pointer branch, because the patch the loader applies is eight
  bytes wide at a one-byte position.
- A storage-class promise is scoped to ONE statement (M34): `CCAcceptBase` sets
  `cc->spec_static` / `cc->spec_extern`, and `CCParseTop` CLEARS both at the top
  of every top-level statement — because two statements bind their names without
  calling `CCAcceptBase` at all (a tag definition or declaration WITH
  declarators, and an enum body with declarators) and would otherwise inherit the
  previous statement's promise.



