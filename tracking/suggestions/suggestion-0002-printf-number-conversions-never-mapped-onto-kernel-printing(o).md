# printf's number conversions have never been mapped onto the kernel's own formatting

## Status

Proposed

**Severity:** P2 — no output is wrong *because the delegation is missing*, and no
build is broken. What is missing is the **mapping**: nobody has written down,
flag by flag, whether the kernel's formatter can produce C's output. The cost is
therefore latent and it is about to be paid for real: M35 (C `double` via
delegated F64 arithmetic) needs a `%f`, and under today's arrangement that means
writing a decimal-float conversion in the prelude — in the C subset that lite
itself compiles — when the host already has one. Measured here, the kernel's
formatter matches C on every explicit-precision `%f` shape tried (1–15
fractional digits, width, sign, ties); a later 255-case sweep (see *A wider sweep,
after M35 landed*) agrees with that for `%f` and adds the shapes that do NOT
survive — the flags `-`, `+`, ` ` and `#`, `inf` and `nan`, and all of `%e`,
`%E`, `%g` and `%G` — so the blanket reason recorded at
`T:/CC/PLAN.md:751-755` is true in general and false for the specific shapes
M35 needs. See *Not claimed* for the one shape measured as wrong **today**,
which is a defect and deliberately not what this file is.

## Finding

`printf` is lowered at compile time into calls into a tiny pure-C runtime that
CC prepends to every module as C source — `CC_PRELUDE`, `T:/CC/CC_EMIT.HC:9`,
prepended in `CCCompile` (`T:/CC/CC.HC:101-107`), described at `PLAN.md:592-604`
(M6b) and reserved in the symbol table at `PLAN.md:499-507`. Decimal digit
generation lives inside that string: `CCItoa` (`T:/CC/CC_EMIT.HC:24-36`) is what
`%d` compiles to (`CCPrintf`, `T:/CC/CC_PARSE.HC:3068-3196`), and a **second**
decimal generator `cc_dig` (`T:/CC/CC_EMIT.HC:386-398`) plus a hex one `cc_hexd`
(400-414) serve the `sprintf` engine (its `%d` arm at 471-484, `%x` arm at
485-495). So "turn a number into decimal text" is implemented twice inside the
compiler, in C (both in one string, `CC_EMIT.HC:9`), and a third time in the host.

The host's is `StrPrintJoin` (`T:/CC/STRPRINT.HC:208`, extern'd at
`T:/CC/KERNELA.HH:5355`, reachable as `StrPrint`/`MStrPrint` at
`KERNELA.HH:4302-4307`), and it is the engine behind HolyC's own `""` print. It
parses C-shaped flags today: `-` at `STRPRINT.HC:239`, `0` at 244, decimal width
at 248, `*` width from an argument at 250 and 261, `.`+precision at
257-267, with the flag bits declared at `KERNELA.HH:4087-4098`; its conversion
letters include `s` 365, `c` 392, `d` 433, `u` 519, `f` 524, `e` 628, `g` 684,
`X` 787, `x` 815, `b`/`B` 843-844 and `%` 867. The kernel already formats
floating point with width **and** precision natively: `T:/KERNELA.HH:3284` is
`F64 x format "\dDA-TRM,A=\"X Spacing:%6.3f\"\d\n"`.

M6c already named this exact open half and left it open
(`PLAN.md:726-755`, note at 751-755): "`%d` digit conversion still runs in
compiled-C (CCItoa); routing conversions through kernel `Prnt` via `cc->ext`
staging is a possible further delegation, deliberately not done here (C's
format-flag semantics would have to survive the mapping)." Console output itself
is already delegated: the flush is the zero-arg HolyC helper `CCFlush`
(`T:/CC/CC.HC:70-84`, bound as relocation base `CC_B_HFLUSH`, `CC.HH:131`) and it
"does what the kernel does for HolyC programs" — `"%s",cc.out+8;` at
`T:/CC/CC.HC:81` — so all console I/O, formatting delegation aside, is the
kernel's own Print path (`PLAN.md:739-742`).

The delegation mechanism is built and proven. `CCLibCall` stages arguments left
to right into `cc->ext[]` and then emits `MOVABS RAX,&cc->lib[slot]; CALL
[RAX]` (`T:/CC/CC_PARSE.HC:1672-1692` → `CCEmitLibCall`,
`T:/CC/CC_EMIT.HC:859-866`); the helper is a zero-arg HolyC function that reads
`cc->ext`, calls the kernel, and returns in RAX (`T:/CC/CC_LIB.HC:1-30`, table
installed by `CCSetLib`, `CC_LIB.HC:74-82`). M12 runs five stdlib functions on
it and `PLAN.md:764-770` records the saved-object round trip as proven
(`CCSetLib` rebinds; `objstdlib`). So the machinery is not the open question.
The open questions are (i) whether C's flags survive the mapping — never probed,
and (ii) the staging width, which is a real number: `cc->ext` is `I64 ext[4]`
(`CC.HH:358`; `CCExtStub` refuses "more than 4 staged args", `CC.HC:322`) and
`cc->lib` has `CC_LIB_SLOTS 8` with slots 0-5 used (`CC.HH:140-147`), i.e. two
free slots and room for two staged quads per call — a per-conversion call fits,
one call carrying a format string plus 8 arguments does not.

What has changed is the rule. The owner agreed, during M34, that **"Delegation
is the default, because this is an adapter layer, not a standalone C
implementation"** is a Correctness-policy rule, to be written into
`T:/CC/PLAN.md`'s `## Correctness policy` section (`PLAN.md:44-85`, before
`## C subset limitations` at 86), and that a non-delegation is allowed only with
an argument, in two cases: the host cannot do the job, or the delegation costs
more than the thing it replaces. **As committed, that paragraph is not yet in
the file** — see *Not claimed* — so this filing cites the decision, not a PLAN
line.

## The cost today

- **Three implementations, one record.** `CCItoa` (prelude, for `printf`),
  `cc_dig` (prelude, for `sprintf`) and the kernel's `StrPrintJoin`. Only the
  first two are covered by the suite's fixtures, and their expected values come
  from C's documented `printf` behaviour; `gcc -std=gnu99` cross-checking — the
  habit PLAN records from M21 on (`PLAN.md:1269`, and the run of entries at
  1340, 1381, 1445, 1509, 1543, 1622, 1716, 1790, 1989) — appears in no entry
  before M21, so the `printf`/`sprintf` fixtures from M6b/M12.1 have no recorded
  gcc-derived value either (the same gap `suggestion-0001` files for `unionc.c`).
  The kernel's formatter has **no** comparison against C's flags at all, in
  either direction.
- **C's flags are not expressible in lite's `printf` today.** `printf("%8d|",5);`
  is refused: `CC ERROR: printf: unsupported format` (measured; the conversion
  set is `%d %s %c %%` and the refusal is `CC_PARSE.HC:3183`). So the widths and
  precisions M35's `%f` needs — C's `%w.pf`, and C's implicit precision of 6 —
  are net-new work in the prelude regardless, while the kernel has all of them
  already.
- **And the prelude cannot currently spell a float at all**: `float`/`double` are
  listed as unsupported (`PLAN.md:253`), so a C-side dtoa waits on M35's own
  arithmetic landing, then has to be written correctly (digit extraction across
  a decimal point, rounding, subnormals, inf/NaN, magnitudes past 2^53) inside
  the subset lite compiles. The kernel path already has an inf arm
  (`STRPRINT.HC:534`), a scaled-integer path for magnitudes the 64-bit digits
  cannot hold (`sp_out_f:` 555, `n=Log10(d)` 558) and its own
  documented limit ("In float formatting, do not exceed 18-digits before or
  after the decimal point because the numbers … are stored in 64-bits",
  `STRPRINT.HC:210-217`). A new C dtoa starts behind that.
- **The exception is recorded as a blanket, not per-flag.** `PLAN.md:754-755`
  says "format-flag semantics would have to survive the mapping" and stops. M27
  shows what the policy now asks for instead: a named, argued non-delegation
  with the cost spelled out ("Delegating to the kernel is the rule when the
  kernel has the harder problem (sorting, printing, the FFI); copying two known
  quads is not it", `PLAN.md:1578-1585`). For `printf` nobody can currently tell
  whether the exception is earned, because the flag-level answer does not exist.
  Under the new default, an unargued blanket is the shape that is out of policy.
- **The divergence is not hypothetical.** It is already measurable between the
  two prelude generators and the kernel, and one shape is already wrong in lite
  (see *Not claimed*: `%d` of the most negative quad). Float row 13 below shows a
  magnitude where the kernel path is badly wrong and the C path is right; float
  rows 1-8 show where the kernel is right and a fresh C dtoa would have to earn
  it.

## Already decided?

Partly, and this filing does not reopen it:

- **M6c considered this exact delegation and declined it**
  (`PLAN.md:751-755`), for the flag-semantics reason. That decision stands as
  far as it goes: nothing here says M6c should have done it in that milestone.
- **M27 declined a different delegation with an argument** (`PLAN.md:1578-1585`),
  and M12 records both directions in one entry — five functions delegated to
  kernel routines (`strlen memcpy memset memcmp malloc`, `PLAN.md:757-760`) and
  the rest kept in pure-prelude C because "the kernel has no equivalent or the
  semantics are C-specific" (nine named there: `strcmp strcpy strchr memmove atoi
  abs puts srand rand`, `PLAN.md:769-772`). The pattern of *arguing* the
  exception is established practice.
- **What changed is the default.** As of the M34 agreement the rule is
  delegation-first, and the exception has to be argued in one of the two named
  cases. `suggestion-0001`'s precedent applies: this is not a reopened question,
  it is the open half of a decision PLAN already flagged, asked again under the
  new rule — and asked with the cheap part of the probe already done
  (*Evidence*), because "would the flags survive?" is answerable shape by shape
  without touching code.
- **What is NOT decided:** whether the mapping holds per flag; whether the fix
  for the failing flags belongs in the kernel, in lite (the format string is a
  compile-time constant, so much of the padding and all of the default-precision
  question can be spelled out by the compiler instead), or in neither; and
  whether `cc->ext`/`cc->lib` should be widened (both are documented as the
  constraint, not blockers).

## Evidence (measured at filing time; all of it reproducible)

Host: gcc 13.3.0 (Ubuntu 13.3.0-6ubuntu2~24.04.1) on x86-64 Linux, LP64. Kernel
side measured inside `./3d_loader -t T -c` from the repo root; each shape
formatted into a buffer with `MStrPrint` and dumped as hex bytes, so padding and
NUL bytes are visible (see *Reproduction traps* 5 — console text hides both).
"C" is gcc's own `printf` on the same shape. lite's `int` is one quad
(`PLAN.md:138-141`), so the C column for wide values uses gcc's `long long`
spelling, which is the quad-width reading. Every cell in both tables is a run, not
a recollection. The two runs use different field delimiters (`|` on the kernel
side, `[]` on some gcc runs), so the columns are compared on the field content,
not on the punctuation.

**1. Integer / char / string shapes.**

| # | shape | kernel (`StrPrintJoin`, measured) | C (gcc, measured) | verdict |
|---|---|---|---|---|
| 1 | `%d` −5 | `-5\|` | `-5\|` | ✓ |
| 2 | `%8d` −5 | `      -5\|` | `      -5\|` | ✓ width incl. sign |
| 3 | `%08d` −5 | `-0000005\|` | `-0000005\|` | ✓ 0-pad after sign |
| 4 | `%03d` 7 | `007\|` | `007\|` | ✓ |
| 5 | `%d` −(2^63−1)−1 | `-9223372036854775808\|` | `-9223372036854775808\|` | ✓ kernel and C agree; **lite prints `-(`** |
| 6 | `%x` 255 | `ff\|` | `ff\|` | ✓ lowercase |
| 7 | `%X` 255 | `FF\|` | `FF` | ✓ hex case available |
| 8 | `%x` −1 (quad) | 16 × `f` | `ffffffffffffffff` | ✓ kernel matches the quad reading; lite's `sprintf` gives `1` (documented: `%x` prints the magnitude, `PLAN.md:518`, `CC_EMIT.HC:485`) |
| 9 | `%08x` 255 | `000000ff\|` | `000000ff\|` | ✓ |
| 10 | `%u` −1 | `18446744073709551615\|` | `18446744073709551615` (`%llu`) | ✓ |
| 11 | `%8s` "ab" | `      ab\|` | `      ab\|` | ✓ |
| 12 | `%-8s` "ab" | `ab      \|` | `ab      \|` | ✓ |
| 13 | `%-8d` 5 | `       5\|` | `5       \|` | ✗ **numeric arms ignore `-`** |
| 14 | `%-8x` 255 | `      ff\|` | `ff      ` | ✗ same |
| 15 | `%-4c` 65 | `A   \|` | `A   ` | ✓ the `c` arm justifies (`c` reaches `OutStr`, the only consumer of `-`: `STRPRINT.HC:29-40`) |
| 16 | `%c` 65 | `A\|` | `A\|` | ✓ |
| 17 | `%c` 0 | `\|` (0 bytes written) | 1 NUL byte (`sprintf` count = 1 on gcc **and** on lite, measured) | ✗ the kernel writes nothing (`STRPRINT.HC:392-396` stores 8 bytes then NUL-terminates; `OutStr` measures with `StrLen`) |
| 18 | `%c` 321 | `A`,0x01 (2 bytes) | `A` (1 byte) | ✗ wide `%c` |
| 19 | `%c` 0x4141414141414141 | 8 × `A` | `A` | ✗ same cause |
| 20 | `%%` | `\|%d\|` from `"%d\|%%d\|"` → `1\|%d\|` | `1\|%d\|` | ✓ |
| 21 | `%ld` −5 | `-5\|` | `-5\|` | ✓ (`l` is inert: `STRPRINT.HC:286`) |
| 22 | `%hd` 7 | `7.000000000 \|` | `7\|` | ✗ **`h` is not C's `hh`** — it sets `PRTF_AUX_FMT_NUM` (engineering notation), `STRPRINT.HC:292-294` |
| 23 | `"50% done\|%d\|"` | `50done\|1\|` | `50 1one\|<garbage>\|` (C reads `%<sp>d` as a conversion) | ✗ three-way; and lite **refuses** this format at compile time (`CC_PARSE.HC:3183`) while the kernel silently drops the `%` |

`%-8d` and `%-8x` fail for one structural reason that a fix in one place would
remove: `PRTF_LEFT_JUSTIFY` is consulted only inside `OutStr`
(`STRPRINT.HC:29-40`), which the string-ish arms use; the numeric paths fall to
`sp_out_comma_num:` (457) and `sp_out_num:` (493), which pad before the value
(468-487, 499-514) and never look at the flag. Row 23 is the shape that should
decide how much weight the policy puts on "silently different": lite's policy has
no warning channel and treats a compile-time refusal as the tool (`PLAN.md:58-64`),
and delegation moves that refusal into the runtime engine, which drops output
instead.

**2. Float shapes (M35's target).** "C" is gcc's `printf` on the same value.

| # | shape | value | kernel (measured) | C (gcc, measured) | verdict |
|---|---|---|---|---|---|
| 1 | `%6.3f` | 1.5 | ` 1.500\|` | ` 1.500\|` | ✓ |
| 2 | `%8.3f` | 1.5 | `   1.500\|` | `   1.500` | ✓ **width+precision combination works** |
| 3 | `%.3f` | 1.5 | `1.500\|` | `1.500` | ✓ |
| 4 | `%.3f` | −1.25 | `-1.250\|` | `-1.250` | ✓ |
| 5 | `%6.3f` | 0.0 | ` 0.000\|` | ` 0.000\|` | ✓ |
| 6 | `%.0f` | 2.5 and 1.5 | `2\|`, `2\|` | `2`, `2` | ✓ ties round half-even on both |
| 7 | `%.1f` 2.5; `%.2f` 0.125; `%.3f` 2/3 | | `2.5\|`, `0.12\|`, `0.667\|` | `2.5`, `0.12`, `0.667` | ✓ |
| 8 | `%.1f` … `%.15f` sweep | 1/3 | digit count equals the precision and the digits match C in **every** one of p = 1..15 (measured: `%.1f`,`%.2f`,`%.3f`,`%.4f`,`%.5f`,`%.6f`,`%.7f`,`%.8f`,`%.9f`,`%.10f`,`%.11f`,`%.12f`,`%.13f`,`%.14f`,`%.15f`) | same | ✓ |
| 9 | `%.16f`, `%.17f`, `%.18f` | 1/3 | `0.3333333333333326\|`, `0.33333333333333252\|`, `0.333333333333332520\|` | `0.3333333333333333`, `0.33333333333333331`, `0.333333333333333315` | ✗ **the divergence starts at p=16, not at 17**: C is still exact at 16 for this value, the kernel is not. The engine's own doc comment caps digits-before-plus-after-the-point at 18 (`STRPRINT.HC:210-217`); mechanism not chased |
| 10 | `%f` (no precision) | 1.5, 0.0 | `2\|`, `0\|` | `1.500000`, `0.000000` | ✗ **the kernel has no default precision** — but C's default is 6, and 6 is in the exact zone of row 8, and lite knows the format at compile time, so this is fixable by spelling `.6` |
| 11 | `%f` | −1.0×0.0 (−0.0) | `-0\|` | `-0.000000` | sign preserved ✓, precision ✗ (row 10 again) |
| 12 | `%f` | 1.2345678901234567e16 | `12345678901234568\|` | `12345678901234568` (`%.0f`) | ✓ at 17 significant digits with precision 0 |
| 13 | `%.2f` | 12345678901234567890.0 | `-567041950362118272.00\|` | `12345678901234567168.00` | ✗ badly wrong **and the sign flips** (the input is positive): this value has 20 integer digits, above the 18 the engine documents (`STRPRINT.HC:210-217`); mechanism not chased |

Read as a mapping answer rather than as a list of bugs: the kernel's `%f` is
C-faithful for `%w.pf` with 0 ≤ p ≤ 15, for negative values and for
half-even ties, and it breaks for absent precision (cheap for lite to repair at
compile time), for p ≥ 16 (measured), and for magnitudes whose integer part needs
more than the 18 digits the engine documents (float row 13 is one). M35's `%f`,
whose C default precision is 6, lands in the good zone. By rows 9-13 the honest
answer is "delegable for the common range, with the precision spelled explicitly
and kept at or below 15, and the
out-of-range magnitudes either refused at compile time or routed to `%e`" — which
is an **argument**, the thing the new rule requires, where M6c's note had only a
reason not to.

**A wider sweep, after M35 landed (255 shape×value cases: 17 format shapes ×
pi, 1.0, 0.1, 123456.7891, 0.000123456, 1e-5, -2.5, -0.0, +inf, -inf, NaN, 0.0,
42.0, 1e10, 1e-3; each value reaching the formatter as the raw IEEE quad, which
is the shape a delegated helper hands it).** 49 of the 255 are byte-identical to
`gcc -std=gnu99`'s output. Per shape (agreeing/total): `%10.3f` and `%010.3f`
12/15, `%.3f` 12/15, `%.6f` 11/15, `%10f` 0/15, `%f` 0/15, `%-10.3f` 2/15 (the two that agree are values
already at least 10 characters wide, where justification cannot show),
`+.3f` 0/15, ` .3f` 0/15, `#.3f` 0/15, `#.4e` 0/15, `%e` 0/15, `%8.2e` 0/15,
`%E` 0/15, `%g` 0/15, `%G` 0/15, `%12.5g` 0/15. The disagreements sort into
five distinct causes, and only one of them is a rounding limit:

1. **absent precision**: the kernel's `%f` defaults to 0 fractional digits
   (`%f` of pi gives `3`), C's defaults to 6 — confirmed here and at row 10
   above, and cheap for lite to repair because the format string is a
   compile-time constant.
2. **`inf` and `nan`**: the kernel prints `i`, `-i` and the decimal number
   `9223372036854775808` respectively; C prints `inf`, `-inf`, `nan`. A
   delegated `%f` needs its own arm for these two (the kernel's `i` is not a
   C spelling in either direction).
3. **`-`, `+`, ` ` and `#` are not flags the numeric arms honour**: `%-10.3f`
   right-justifies anyway (`     3.142`), and `%+.3f`, `% .3f` and `%#.3f` print
   `.3f` — the conversion does not happen at all. Width and `0`-padding DO work
   (`%10.3f` → `     3.142`, `%010.3f` → `000003.142`, `-0.0` → `-00000.000`, all
   identical to gcc), and `-` works for `%s` (integer table row 12) but not for
   `%d` (row 13) or `%x` (row 14). So padding and sign-flags for floats are
   either lite-side work or kernel-side work, not a free inheritance.
4. **`%e`/`%g` are not C's**: the kernel's `%e` default precision is 8 and its
   exponent is spelled `e0`/`e-1` (C: precision 6, `e+00`/`e-01`), its `%g`
   behaves like `%e`, and `%E` and `%G` print nothing. C's `%e`/`%g` are
   therefore not delegable today without kernel-side change.
5. **the 15-significant-digit limit, now measured at a magnitude rather than
   only at a precision**: `%.6f` of exactly-1e10 gives
   `9999999999.999998` where glibc gives `10000000000.000000` — 16 significant
   digits needed, matching rows 9-10's boundary — while `%10.3f` of the same
   value (13 significant digits) is identical.

For the shape M36 needs, the answer is unchanged and now has a floor: `%f` with
the precision **spelled** and **at or below 15**, on a value whose integer part
fits, with `inf`/`nan` handled by lite, and with no `-`/`+`/` `/`#` until someone
implements them — which is a stated mapping, i.e. the argument M6c's note lacked,
and it says plainly which parts of the conversion lite would still have to own.

**3. What the mechanism would carry.** A conversion helper added the M12 way
costs, per call site: `CCSetScratch` + `MOVABS RAX,&lib[slot]` + `CALL [RAX]`
(`CC_EMIT.HC:859-866`) against today's plain `CCCall` into the module's own code
(`CC_PARSE.HC:3169-3177`), i.e. ~14 extra bytes and an RDI-scratch touch per
conversion, plus 2 staged quads (the value, and either the pointer to a
per-directive format quad the compiler already builds or the dst pointer) out of
`ext[4]`. It buys: one fewer prelude function, no recursion on the module's
private C stack (`CCItoa` recurses ~20 deep, `CC_EMIT.HC:31-33`), and free
rebinding for saved objects through the `CC_B_LIB` slot indirection — the
property M12 verified with `objstdlib` (`PLAN.md:764-770`, `CC_TEST.HC:196-201`).
It does **not** change console semantics for NUL bytes or DolDoc markup: both
paths end at the kernel's `"%s"` print (`CC.HC:81` for mid-run flushes,
`CC.HC:119-130` at exit), so markup in output is interpreted today exactly as it
would be after delegation.

## Directions (recorded, not implemented)

**(a) Finish the probe before touching code.** Shape by shape, against C, the
set `printf` actually accepts (`%d %s %c %%`, up to 8 args, `CC_PARSE.HC:3068`
and the 8-arg ceiling at 3098-3101) plus whatever M35 adds. Rows 1-23 and the
float table above are the first batch, done at filing time. Still unprobed:
`%+d`, `%<space>d`, `%08.3f`-style combinations, `%-` on `%f`, precision 19 and
above (1..15 measured exact for 1/3, 16/17/18 measured divergent), `inf`/`NaN`,
`%e` and `%g`, `%i`, `%n`, `%p`, the `*`-width argument
consumption order (the kernel pops `argv` for `*` at 250-255 and 261-266, which
is not C's left-to-right conversion order), and what a `%` before an unsupported
letter does to a *variable* `sprintf` format. Cost: about a dozen loader runs and
no code change. Output to write down: which flags survive, per flag.

**(b) If the mapping holds, delegate and delete the duplicate.** New slot
(`CC_LIB_FMT 6`), a zero-arg HolyC helper in `CC_LIB.HC` beside `CCXStrLen`, a
line in `CCSetLib`, and `CCPrintf`'s `%d` arm emitting `CCPushExt` +
`CCEmitLibCall` instead of `CCCall(hi)`; then `CCItoa` comes out of
`CC_PRELUDE`, and `cc_dig`/`cc_hexd` follow for `sprintf` once the same rows are
probed for the runtime engine. Costs, concretely: the ~14 bytes and scratch touch
per conversion above; the helper needs a bounded destination, because
`StrPrintJoin(dst,…)` takes **no capacity argument** (226-229: with a non-NULL
`dst` it writes into it and only the caller's own arithmetic bounds it), so
either the helper bounds the write against `CC_OUT_SIZE` itself or it formats
into `cc->libscr`-class scratch and the compiler copies; and two staged quads
per call means the whole `printf` cannot become one call, so `CCStr`/`CCOutByte`
stay for the literal runs no matter what. Benefits: rows 1-8 of the float table
come free for M35 instead of a new C dtoa (and row 10's divergence is cured by
spelling the precision), and the exception list shrinks to the
flags that genuinely fail.

**(c) If it does not hold for a flag, keep the C path for that flag and say so
per flag** — the shape M27's note has (`PLAN.md:1578-1585`), not M6c's blanket.
The three failures measured here have three different homes, which is why the
record has to be per-flag and not per-function: `%-` on numeric arms is a
10-line fix in the kernel's two padding blocks (457-487, 493-514) that
also makes the kernel better for HolyC programs, so "the host cannot do the job"
is a statement about today's kernel, not forever; `%c` of a value whose low byte
is NUL, or whose high bytes are non-zero, is a semantic difference the kernel
should keep (HolyC wants the string-shaped reading) and C should not, so those
cases stay in C — or, cheaper, `CCPrintf` can mask the argument to one byte and
emit `CCOutByte`, which it already does for `%%` (`CC_PARSE.HC:3155-3161`);
`%h` is not a numeric-shape problem at all, it is a **letter-collision** problem,
and the answer there is a rule about the format string, not about the helper: the
compiler must reject or rewrite the modifier letters the kernel engine gives a
different meaning (`h` 292-294, `,` 280, `t` 283, `/` 277, `?`, and the
`*` slots). Write whichever answer is taken into `## C subset limitations`
(`PLAN.md:86`) beside the existing `printf` bullet, since that is the section a
reader of a `%h` surprise will be sent to.

(a) is worth doing whether (b) or (c) follows: under the new rule the thing that
has to exist is the argument, and today it does not.

## Reproduction traps

1. **HolyC refuses to pass arguments through a zero-arg function-pointer type.**
   Verified at filing time: with `I64 (*fp)();` in scope, all three of
   `fp(&answer)`, `fp(p)` and `fp(answer)` fail with `ERROR: Missing ')' at …`
   (and a follow-on `ERROR: Compiler Parse Error`), while `fp();` parses and
   runs. That is the reason behind this compiler's rule that generated code calls
   only zero-arg helpers, and the reason arg-taking C calls go through
   `CCFindCallN` with `cc->ext` staging (`T:/CC/PLAN.md:9-42` "Usage", the M11
   analysis quoted at `CC.HC:150-152`, `CCExtStub` at `CC.HC:304-330`, and the
   milestone entries M11 at `PLAN.md:687-725`, M12 at 756-795, M14 at 907-937).
   A probe that reaches for
   `fp(arg)` to invoke a helper will hit a parse error and mistake it for a
   calling-convention problem.
2. **The helper bodies live in `T:/CC/CC_LIB.HC`, and that file is HolyC, not C.**
   It is `#include`d into `CC.HC` immediately after `CCCtrl cc;` (`CC.HC:17`, and
   the layout row at `PLAN.md:551`) because the helpers reference `cc` directly;
   its own header calls them "zero-arg HolyC helpers" (`CC_LIB.HC:1`). The
   **pure C** half is the `CC_PRELUDE` string in `CC_EMIT.HC:9`, which lite
   compiles itself and which cannot call a kernel routine. The handover for this
   filing described `CC_LIB.HC` as "pure C … NOT a HolyC file", which is the
   wrong way round; the distinction is load-bearing, because the helpers are
   JIT-compiled HolyC callees and that is precisely why `CCSetScratch` exists
   (the RDI frame-zeroing landmine, `CC.HH:288-299`, `CC_LIB.HC:6-8`). A change
   in `CC_LIB.HC` therefore needs the HolyC image rebuilt (`HCRT.BIN`), while a
   change in `CC_PRELUDE` is recompiled with each module.
3. **`-c` hands its file to the HolyC compiler, not to lite.** Running a `.c`
   fixture through `./3d_loader -t T -c x.c` measured here as
   `ERROR: Undefined identifier at "main"`. To exercise lite's side, write a
   HolyC driver: `#include "T:/CC/CC.HC"` then
   `CCRunFile("Z:/tmp/opencode/delegprobe/c1.c")` — `-c` mounts the host root at
   `Z:` and reads work (`AGENTS.md`).
4. **Run from the repo root.** `HCRT.BIN` is resolved from the process cwd; a run
   started in `/tmp/opencode/delegprobe` booted a different image and produced no
   script output at all.
5. **Console text hides padding and swallows NULs — measure bytes.** Both the
   C path and the HolyC path end in `"%s"`, so an embedded NUL truncates whatever
   was buffered, and the printed text is rendered as DolDoc: printing a bare `$`
   made the kernel parse `T:/Tmp.DD.Z` (`ERROR: Expecting ',' at "T:0"
   T:/Tmp.DD.Z,5`). Dump shapes as hex instead: format with `MStrPrint`, then
   `"%02X ",s[i]` over `StrLen`. `T:/Tmp.DD.Z` is scratch and is untracked in
   this checkout (`?? T/Tmp.DD.Z` in `git status` before this filing started);
   these runs rewrite it.
6. **Still `timeout` + `stdbuf -oL`, still rc=0.** The engine does throw —
   `throw('StrPrint')` fires on a NULL format (`STRPRINT.HC:224-225`) and on a
   missing argument (326-327, 434-435, and both `*` arms) — and a throw in `-c`
   mode drops into the fault handler and waits for input; the exit code is 0
   either way, so grep stdout for `ERROR:`. What the engine does **not** do is
   object to an unknown conversion letter: the conversion switch (`sp_arg:` /
   `switch (*fmt++)`, `STRPRINT.HC:321-323`) has no `default:` arm, so a `%` and
   its letter are consumed or dropped in silence — which is exactly why row 23
   came out silent rather than diagnostic. Ordering the risky shapes last in a
   script keeps the earlier measurements.
7. **Probe the engine through a buffer, not through a print statement.** The
   hex-dump shape in trap 5 is what makes padding and NULs visible. Printing the
   same shape with HolyC's own `""` operator hides both, and it hides a third
   thing: `%f` with no precision prints the rounded integer, so `"%f",1.5` prints
   `2` and reads like a rendering bug when it is the engine's absent default
   precision (float row 10).
8. **A terminating value exposes no rounding.** 1.5, −1.25 and 0.125 print exactly
   what C prints at every precision tried, and `%f` never rounds up on them, so a
   probe built only on those three values would report "the kernel's `%f` matches
   C" without ever testing rounding. The discriminating shapes are the half-way
   ones — measured so far: `%.0f` of 2.5 and of 1.5, where **both** engines print
   `2` (half-even), and `%.3f` of 2/3, where both print `0.667` — and the
   near-tie ones, of which `%.1f` of 0.25 (the shape that separates half-up from
   half-even) was **not** tried.

Working scripts, if `/tmp` survives: `/tmp/opencode/delegprobe/k1.HC` … `k11.HC`
(kernel shapes: ints, widths, justification, hex, `%c`, modifiers, floats, the
precision sweep at `k9.HC`/`k10.HC`/`k11.HC`), `c1.c`/`c2.c`/`c3.c` with
`drv1.HC`/`drv2.HC` (lite's side), `g1.c`…`g17.c` (the gcc witness, including the
`%.*f` precision sweep at `g13.c` and the integer/float baselines at `g11.c`,
`g12.c`, `g16.c`, `g17.c`). The commands and shapes above are the durable record.

## Not claimed, and one thing deliberately not filed

- **`%d` of the most negative quad prints `-(` in lite today** (measured:
  `printf("%d|",-9223372036854775807-1);` → `-(|`, and the same via `sprintf`
  → `-(`, count 2) where both C and the kernel print
  `-9223372036854775808` (measured, 20 digits). The mechanism follows from the
  prelude source: `cc_v=-cc_v` overflows back to the same negative
  (`CC_EMIT.HC:26-29`), so `if (cc_v>=10)` (`:31`) skips the recursion and
  `48+cc_v-(cc_v/10*10)` (`:33`) evaluates to 40, `(`. `cc_dig` has the same
  shape (`389`, `397`). That is wrong output today, so by the tracking rule it is
  a defect and belongs in a `BUG-NNNN(o).md`, not here; this filing was scoped to
  one suggestion, so it is recorded as evidence and left unfiled. There was no
  `tracking/bugs/` directory in this checkout at filing time.
  **[Filed since, and cross-linked both ways:
  `tracking/bugs/BUG-0001-printf-d-of-the-most-negative-quad-prints-minus-paren(o).md`,
  status `Open`, severity P1. It re-runs the measurement, adds the
  kernel-formatter byte witness and the `sprintf` return-count witness (2
  against C's 20), and pins the mechanism line by line; it also corrects one
  line of this bullet's evidence — the host's `%d` of a `long`-typed argument
  prints `0`, not the 20-digit string, because `%d` converts an `int`
  (`gcc -std=gnu99 -Wall` names the mismatch outright), so the sound host
  comparisons are `%ld`/`%lld` on the same value. The bug's direction (d) is
  this file's direction (b): delegating `%d` to `StrPrintJoin` deletes
  `CCItoa`, and with it the cause.]** It remains the strongest available
  argument that the two implementations already disagree.
- **The "Delegation is the default" paragraph was not in the file at filing
  time.** `T:/CC/PLAN.md` is clean against `68e4f34` ("CC: M34 …"), its
  `## Correctness policy` section (44-85) ends at the gcc-as-witness paragraph,
  `grep -rn "Delegation is the default"` and `grep -rn adapter` over the tree
  return nothing, and the last milestone entry is M34 (`PLAN.md:2182`) with no
  M35 entry at all. So the rule is cited here as the owner's M34 decision as
  relayed, and it is marked as such; the measurements do not depend on the
  wording. Nothing outside `tracking/` was touched by this filing, so the
  paragraph and the M35 entry are still to be written by whoever has PLAN.md in
  flight. Also: `Prnt` — the routine M6c's note names (`PLAN.md:753`) — does not
  exist under that name in this tree; `grep -rn Prnt T/` returns only that PLAN
  line. The engine to name is `StrPrintJoin`.
- **Not probed:** precision 19 and above (1..15 measured exact for 1/3, 16..18
  measured divergent); `inf`/`NaN`; `%e`, `%g`, `%i`, `%n`, `%p`, `%a`; `%+d` and
  `%<sp>d` beyond the
  one row that fell out by accident (row 23); `%-` on `%f`; `*`-width arg order; the
  8-argument `printf` ceiling under delegation; whether a bounded-format
  primitive already exists in the kernel that a helper could use instead of
  scratch-plus-copy. The mechanisms behind float rows 9 and 13 (the trailing-digit
  difference, and the flipped sign on a 20-digit magnitude) were not chased.
- **"The kernel already matches C on 1..15 fractional digits"** is a claim about
  the values tried (1.5, 0.0, −1.25, 2.5, 0.125, 1/3, 2/3) and one 17-digit integer
  value; it is not a proof for all doubles, and subnormal and near-tie inputs at
  high precision were not exercised.
- **Nothing here says M35's `%f` should be delegated.** It says the question has
  a cheap answer, records a first batch of that answer, and says which flags the
  batch says do and do not survive.

## Repo notes

`tracking/` is **not** in this repo's `.gitignore` (`git check-ignore tracking/`
prints nothing; rc=1), and the precedent suggestion is committed at `68e4f34`, so
once this file is committed a deletion here would be recoverable from history at
the deletion commit's parent — unlike the assumption the tracking skill carries
elsewhere. Per the lifecycle a suggestion is never deleted regardless: whichever
direction lands, this file stays, renamed to `(c)` with status `Implemented` or
`Declined`, and keeps its reasoning. Filed on `bugfix/build-isssues` in the
primary checkout `/mnt/d/Source/3Days`, staged not committed.
