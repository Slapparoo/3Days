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

Supported: `int`/`char` scalars, pointers and 1-D arrays (locals and
globals), structs (tag definitions, `.`/`->`, struct-pointer params,
self-referential links — see M9), string and char literals,
`for`/`while`/`if`/`else`/`break`/`continue`, functions (forward refs,
recursion), `printf` (literal format: `%d %s %c %%`), `putchar`, `exit`, and the
M12 stdlib subset — `strlen strcmp strcpy strchr memmove memcpy memset
memcmp malloc atoi abs puts srand rand` (delegated to kernel routines
where they exist; see M12), the M12.1 batch — `strcat strncat strrchr
strstr strncmp strncpy memchr strdup calloc` + `sprintf getchar fgetc
getc fputc putc fputs abort` (see M12.1) — the usual operators —
`++ -- & * []`, comma, short-circuit logic, bitwise `& | ^`, shifts
`<< >>`, `~`, all ten compound assignments (`p+=n` scales), chained
`a=b=5` — with C's full precedence chain (shift vs relational vs the
three bitwise levels are pinned by `bitops.c`).

Not supported — each currently fails with a compile diagnostic:

- Multi-declarator globals must not hide a function: `int a,b();` is
  an error — give functions their own declaration. (`int i,s=0;`,
  `int *p,*q;` and `int a=1,b=2;` all work, C's per-declarator star
  typing included.)
- Array initializers of any kind (`int a[4]={1,2,3,4};`).
- `else if` chains parse, but deeply nested control flow is untested;
  `switch`, `do`, `goto` absent.
- `float`/`double`, preprocessor, unions, function pointers in C,
  `sizeof`, casts, multi-dimensional arrays.
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
| `CC_LIB.HC` | M12 zero-arg delegation helpers (spliced into `CC.HC` after `CCCtrl cc;`) |
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

## HolyC dialect gotchas (all verified in this build)

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
