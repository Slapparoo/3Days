# printf's number conversions have never been mapped onto the kernel's own formatting

## Status

Implemented

**What landed, and it is shipped now, not ruled.** M36 (`6b1b04c`) took `%d` and `%x`
and answered this file's direction (b) with an **argued refusal**: the walkers stayed
in the prelude, on a cost comparison (`T:/CC/PLAN.md`'s M36 entry — the host route would
need a destination `StrPrintJoin` takes no capacity argument for, and a count sprintf's
return value needs and pointer subtraction cannot supply), now exact over the whole
signed quad. M37 took `%f` and delegated it **whole**, and that is what the code does.
`CCXFmt` (`T:/CC/CC_LIB.HC`, its own comment block "M37: printf's %f, delegated WHOLE"
opens the section) reads `ext[0]` = destination, `ext[1]` = the value's bit pattern and
`ext[2]` = **the directive TEXT**, calls `CCIClap` on that text, calls
`StrPrintJoin(dst, cc.ext[2], 1, &v)` and returns `StrLen(dst)`; `CC.HH`'s `CC_LIB_FMT`
comment documents the slot as `cci_fmt(dst, value_bits, directive_text)` and `fmtbuf` is
1024 bytes. The prelude's `cc_sprintf` rebuilds the directive as text for `%f` and hands
it over, writing `.6` into the text when the source stated no precision and scanning `*`
without copying it; `CCPrintf`'s float arm is now the catch-all `else` — it scans flags,
width, precision and `*`, builds the directive with `CCStrAlloc`, pushes it through
`CC_B_LOCALS` with `cc->fmtbuf` through `CC_B_CTRL`, and refuses anything whose letter is
not `f` with the message it always had, `printf: unsupported format (only %d %s %c %% and
%f)`. Whatever the kernel writes is what lite emits. *(T:/CC/ line numbers are omitted on
purpose and PLAN's are given with an as-of caveat: the code and PLAN were being rewritten
while this pass ran, and the anchors moved twice.)* The precision ceiling, the `cci_p10`
exact-quad table, the 2^52/2^63 magnitude gates and the doubling digit walker are **gone
from the tree** — that design was built, measured at 0.16% of answerable cells against
glibc, and abandoned; `grep -n "cci_p10|cci_big|precision over 15"` over `T:/CC/` returns
nothing.

**What this file recorded, and is now moot.** The rule this file offered as its answer —
*state the precision and keep it at or below 15* — is dead on both counts. It was wrong
as a rule about the engine: the kernel's `%.15f` of `1.5` prints `1.499999999999998`
where C prints `1.500000000000000`, for a value that is exactly representable and whose
C rendering needs two significant digits, so there was no rounding decision to get wrong
at all; and a ceiling only ever bound a caller that owned the scale factor, and no
caller of the kernel's `%f` owns it any more. The cause is in the host and not in the
precision: the arm scales the value by `Pow10I64(p)`, whose table `T:/KEND.HC:13` fills
with `Pow(10.,i)`, a transcendental, and `Str2F64` multiplies by the same table at
`T:/STRB.HC:574` and `:592` — that is
`tracking/suggestions/suggestion-0004-pow10-table-built-from-pow-not-exact-integers(o).md`,
which this file cross-links and which carries the measurements and the proposed host
fix. See *What M37 measured, what shipped, and what the three open questions came to*
below.

**Status decision, and why this one is closed rather than still open.** `Implemented`,
so the file is renamed to `(c)` with the slug unchanged and is kept — a suggestion is
never deleted, and this one is additionally the only `tracking/` record of `BUG-0001`'s
closure (first bullet of *Not claimed*; the bug file was deleted at `6b1b04c` per the
lifecycle and reads back from `6fed45c`). The reasoning: this file asked for three things
and all three now exist. (i) **The flag-by-flag mapping record** exists for every
conversion and modifier lite accepts — the accept side is measured (11 directive shapes
through both sinks, byte-identical to the kernel) and the refuse side is measured (15
directives, 15 compile errors), and what the engine does for a flag it does not implement
is measured too. (ii) **The delegation** landed for the conversion the file pointed at,
whole, with nothing float-related left in the prelude. (iii) **Where the mapping does not
hold, the C path was kept and said so per flag** — M36's cost comparison for `%d`/`%x`,
and this file's own sweep for `%e`/`%g`, which is the recorded reason those two are
refused rather than delegated. That third leg is why the surviving integer walkers
do not make the finding partly-open: direction (c) sanctioned exactly that outcome, so the
duplication is now a *decision on the record* rather than the unexamined state the file
filed it as. What is genuinely left is new work for new conversions — `%e`, `%g`, `%E`, `%G`
(refused today, on this file's own sweep measurement), and `%i`, `%p`, `%a`, plus `%n`, which
the engine implements and printf refuses, so that it is a decision lite can take back if it ever
wants what the host offers — and each of those owes its own decision when lite first wants it,
which is this file's own closing sentence, not an open finding. **The alternative reading,
recorded rather than hidden:** the pass before this one argued `Partly resolved` with the marker
staying `(o)`, on the ground that the integer walkers had not moved. That reading was reasonable
then (the delegation was a ruling, not yet code, and three questions were still open) and it is
superseded here by the shipped state. It was never committed, so it is not recoverable from git:
what git holds is `(o)`/`Proposed` at both of its commits — the as-filed text at `6fed45c`, and a
citation-and-context revision of that same `Proposed` file at `6b1b04c` — so whoever wants the
`Partly resolved` middle version has it only in this section's and *Repo notes*' sentences. A
reader who still thinks the walkers keep this file open should re-file the walking question as
`suggestion-0005` rather than reopen this id: numbers are not reused, and the cost comparison
that decided it is in PLAN's M36 entry.

**What survives as a second implementation, stated plainly.** `%d`, `%s` and `%c` are
still lowered by lite into prelude C — `CCPrintf` looks up `CCItoa`, `CCStr` and
`CCOutByte` by name and calls them, and none of the three reaches the kernel's formatter;
`%x` and everything `sprintf` does are still interpreted at run time inside `CC_PRELUDE`
(`cc_dig`, `cc_hexd`), and the runtime engine is still a runtime engine — a variable
format is scanned by lite's own C, and only a `%f` inside it is handed onward. None of the
four asks `StrPrintJoin` to format anything; the console flush remains the kernel's own
`"%s"` print, which is a different kernel call doing a different job. The
kernel's formatter is reached for one conversion and one letter. So the
three-implementations condition that opened this file's *Finding* is now two-and-a-half:
the kernel's, and lite's, with the float half of lite's handed through. Nothing in this
file asks to change that; M36 argued it, and the argument is the record.

**Severity:** P2 — no output is wrong *because the delegation is missing*, and no
build is broken. What is missing is the **mapping**: nobody has written down,
flag by flag, whether the kernel's formatter can produce C's output. The cost is
therefore latent and it is about to be paid for real: M35 landed C `double` as
the delegated F64 value model and shipped no printing, so `%f` is now M36's
first item (`T:/CC/PLAN.md:839-841`), and under today's arrangement it means
writing a decimal-float conversion in the prelude — in the C subset that lite
itself compiles — when the host already has one. Measured here, the kernel's
formatter matches C on every explicit-precision `%f` shape tried (1–15
fractional digits, width, sign, ties); a later 255-case sweep (see *A wider sweep,
after M35 landed*) agrees with that for `%f` and adds the shapes that do NOT
survive — the flags `-`, `+`, ` ` and `#`, `inf` and `nan`, and all of `%e`,
`%E`, `%g` and `%G` — so the blanket reason recorded at
`T:/CC/PLAN.md:834-838` is true in general and false for the specific shapes
M36 needs. See *Not claimed* for the one shape measured as wrong at filing time
(which the post-fix tree no longer reproduces), which is a defect and
deliberately not what this file is. **[M36 and M37 have since rewritten that
paragraph's state, and the sentence "the kernel's formatter matches C on every
explicit-precision `%f` shape tried (1–15 fractional digits …)" is the part that
turned out to be a measurement of the values tried rather than of the mapping: it
still holds for 1.5, 0.0, −1.25, 2.5, 0.125, 1/3 and 2/3 at p = 1..15 — where row 8
of the float table measured 1/3 and not 1.5 — and it is false for `1.5` itself at
p = 15, which M37's probe found (`1.499999999999998` against C's
`1.500000000000000`, see *What M37 measured*), for 1.0, 10.0, 100000.0, 1e9, 1e10
and 2.5, and for every magnitude needing more than the engine's own truncation. `%f` is no longer the
pending item — M37 landed it for both sinks and shipped it WHOLE: lite rebuilds the
directive as text (spelling `.6` when the format states no precision, because the engine
defaults to 0), passes the width and the flags through, and prints what the kernel
returns, `i` and `-i` included; the suite's own compile-time line for 1.5 is
`ffmtP|1.500000|1.500|1.499999999999998|     1.500|`, which shows both halves at once —
C's default precision restored, and the kernel's digits where the kernel's digits differ
from C's. There is no lite-side precision clamp and no lite-side magnitude route any more:
a fraction-scaling design that had both was measured at 0.16% of answerable cells against
glibc and abandoned, so what lite emits is the host's whole behaviour, and that is a
documented non-standard of the host rather than a lite defect (PLAN's `getchar` bullet is
the precedent, and PLAN's own M37 heading and policy paragraph now say so). What remains
of this paragraph's claim is the part that survived: the mapping had to be written down
flag by flag before the delegation, and now it is.]**

## Finding

*(The `T:/CC/` line numbers in this section are the tree as this file was filed and as M36 left
it, except where the sentence itself says "post-fix". The pass that closed this file re-measured
the four that had moved again and lists them under **Citation drift** at the end of this section —
read that note before quoting any number below.)*

`printf` is lowered at compile time into calls into a tiny pure-C runtime that
CC prepends to every module as C source — `CC_PRELUDE`, `T:/CC/CC_EMIT.HC:9`,
prepended in `CCCompile` (`T:/CC/CC.HC:101-107`), described at `PLAN.md:674-686`
(M6b — `592-604` as filed) and reserved in the symbol table at `PLAN.md:581-588`
(`499-507` as filed). Decimal digit generation lives inside that string: the
walker behind `%d` is `CCItoa` (`T:/CC/CC_EMIT.HC:24-36` as filed — post-fix
`CCItoa` is 41-49 and the walk is `CCItoaNeg` at 34-40), which `%d` compiles to (`CCPrintf`,
`T:/CC/CC_PARSE.HC:3679-3803`, comment at 3674-3678; `3068-3196` as filed), and
a **second** decimal generator `cc_dig` (`T:/CC/CC_EMIT.HC:386-398` as filed —
post-fix 403-416) plus a hex one `cc_hexd` (printed as 400-414, though it ends at
421 — post-fix 423-450) serve the `sprintf` engine (its `%d` arm at 471-484 —
post-fix 500-516 — and `%x` arm at 485-495 — post-fix 517-523). So
"turn a number into decimal text" is implemented twice inside the compiler, in C
(both in one string, `CC_EMIT.HC:9`), and a third time in the host.

The host's is `StrPrintJoin` (`T:/STRPRINT.HC:208`, extern'd at
`T:/KERNELA.HH:5355`, reachable as `StrPrint`/`MStrPrint` at
`KERNELA.HH:4302-4307`) — this filing wrote the first two paths with a `T:/CC/`
prefix, and neither `T:/CC/STRPRINT.HC` nor `T:/CC/KERNELA.HH` exists; the line
numbers were right — and it is the engine behind HolyC's own `""` print. It
parses C-shaped flags today: `-` at `STRPRINT.HC:239`, `0` at 244, decimal width
at 248, `*` width from an argument at 250 and 261, `.`+precision at
257-267, with the flag bits declared at `KERNELA.HH:4087-4098`; its conversion
letters include `s` 365, `c` 392, `d` 433, `u` 519, `f` 524, `e` 628, `g` 684,
`X` 787, `x` 815, `b`/`B` 843-844 and `%` 867. The kernel already formats
floating point with width **and** precision natively: `T:/KERNELA.HH:3284` is
`F64 x format "\dDA-TRM,A=\"X Spacing:%6.3f\"\d\n"`.

M6c already named this exact open half and left it open
(`PLAN.md:808-838`, note at 833-838): "`%d` digit conversion still runs in
compiled-C (CCItoa); routing conversions through the kernel's format engine
(`StrPrintJoin`, T:/STRPRINT.HC, extern'd in KERNELA.HH) via `cc->ext` staging is
a possible further delegation, deliberately not done here (C's format-flag
semantics would have to survive the mapping)." As filed the quoted middle read
"through kernel `Prnt`"; M35 rewrote that line to name `StrPrintJoin`, and the
same commit appended a bracketed addendum to this note (`PLAN.md:839-855`) whose
answers are in *A wider sweep* below. Console output itself is already delegated:
the flush is the zero-arg HolyC helper `CCFlush` (`T:/CC/CC.HC:70-84`, bound as
relocation base `CC_B_HFLUSH`, `CC.HH:165`) and it "does what the kernel does for
HolyC programs" — `"%s",cc.out+8;` at `T:/CC/CC.HC:81` — so all console I/O,
formatting delegation aside, is the kernel's own Print path (`PLAN.md:820-822`).

The delegation mechanism is built and proven. `CCLibCall` stages arguments left
to right into `cc->ext[]` and then emits `MOVABS RAX,&cc->lib[slot]; CALL
[RAX]` (`T:/CC/CC_PARSE.HC:1964-1987` → `CCEmitLibCall`,
`T:/CC/CC_EMIT.HC:887-893` — `859-866` as filed, the same function before the
in-flight printing fix added 28 lines above it); the helper is a zero-arg
HolyC function that reads `cc->ext`, calls the kernel, and returns in RAX
(`T:/CC/CC_LIB.HC:1-30`, table installed by `CCSetLib`, `CC_LIB.HC:294-315`).
M12 runs five stdlib functions on it and `PLAN.md:864-870` records the
saved-object round trip as proven (`CCSetLib` rebinds; `objstdlib`). So the
machinery is not the open question. The open questions were (i) whether C's flags
survive the mapping — never probed at filing time, and probed since, here and by M37,
with per-flag answers in *What M37 measured, what shipped, and what the three open questions
came to* — and (ii) the staging width, which is a real
number: `cc->ext` is `I64 ext[4]` (`CC.HH:493` as of this pass, `474` when this sentence was
written; `CCExtStub` refuses "more than 4 staged args", `CC.HC:322`) and `cc->lib` is
`U8 *lib[CC_LIB_SLOTS]` (`CC.HH:388`, was `381`) with `CC_LIB_SLOTS 32` (`CC.HH:207`, was `CC.HH:381` when this was written). M35 grew it from the
8 this filing printed (`CC.HH:140-147` then, slots 0-5) and added the float family at slots
6-18, `CCSetLib` installed 19, and M37 registered the `%f` helper at slot 19
(`CC_LIB_FMT`, `CC.HH:203`), so the working tree installs 20 and 12 are free: `CCSetLib` has
exactly 20 `c->lib[…]` assignments and the comment on `CC.HH:207` says "20 installed: 0-5
stdlib (M12), 6-18 the float family (M35), 19 the float printer (M37)", which matches. Slots
are therefore not the binding constraint; four staged quads per call are — a per-conversion
call fits, one call carrying a format string plus 8 arguments does not.

What has changed is the rule. The owner agreed, during M34, that **"Delegation
is the default, because this is an adapter layer, not a standalone C
implementation"** is a Correctness-policy rule, to be written into
`T:/CC/PLAN.md`'s `## Correctness policy` section, and that a non-delegation is
allowed only with an argument, in two cases: the host cannot do the job, or the
delegation costs more than the thing it replaces. **As filed, that paragraph was
not yet in the file** — see *Not claimed* — so this filing cited the decision
and not a PLAN line. It is in the file now: `6fed45c` committed it, the section ran
`PLAN.md:44-117` at that commit and `## C subset limitations` followed at 118; in the M37
working tree the policy section runs 44-127 and `## C subset limitations` is at 128, because
M37 rewrote the seam paragraph in place — it now states the WHOLE delegation and puts the
by-parts design in the past tense. Those PLAN numbers moved twice during the pass that closed
this file, so they are given as of that pass.

**Citation drift, recorded rather than chased.** The same pass re-measured the anchors this
*Finding* states as current and found four of them have moved again, because the M37 code was
being rewritten while this file was being updated: `CCPrintf` is `T:/CC/CC_PARSE.HC:3694` (was
3679-3803, and its refusal is now the `fmt[j] != 'f'` test at 3857, not 3789); `CCSetLib` is
`T:/CC/CC_LIB.HC:460` (was 294-315); `CCEmitLibCall` is `T:/CC/CC_EMIT.HC:965` (was 887-893);
`CCLibCall` is `T:/CC/CC_PARSE.HC:1971` (was 1964-1987, close enough to be called unchanged).
What did NOT move: `T:/CC/CC_EMIT.HC:9` (`CC_PRELUDE`), `T:/CC/CC.HC:70` (`CCFlush`) and its
`"%s",cc.out+8;` at `:81`, `T:/CC/CC.HC:322` (`CCExtStub`'s 4-slot refusal), and the whole
`T:/STRPRINT.HC` conversion-letter table quoted above — `s` 365, `c` 392, `d` 433, `u` 519,
`f` 524, `e` 628, `g` 684, `X` 787, `x` 815, `b`/`B` 843-844, `%` 867 — re-verified line by
line against the current file. Treat the drifted four as as-of-this-pass, not as stable.

## The cost today

**[As filed, at M35 — and three of the five bullets below are now retired, so read
them with the bullets in *Status* and in *What M37 measured, what shipped, and what the three
open questions came to* rather than as current state. Bullet 3 ("nothing in the prelude can
print a float yet", "a
C-side dtoa is therefore still unwritten") is retired outright: M37 delegated the whole
`%f` conversion to the host's printer and NO decimal-float dtoa was written in the
prelude at all, which is the outcome this bullet was hoping for. Bullet 4 ("the
exception is recorded as a blanket, not per-flag … the flag-level answer does not
exist") is retired: PLAN's M36 entry carries an argued non-delegation for the integer
walkers and its M37 entry the delegation for `%f`, and this file's own tables plus
M37's sweeps are the flag-level answer. Bullet 2 is retired for `%f` and stands for the
integers: a width on a `%f` is no longer net-new prelude work because the directive is
passed to an engine that already has widths, while `printf("%8d|",5);` is still a
compile-time refusal. Bullet 1 stands and is the residue: the three implementations are
still three, since `CCItoa`/`cc_dig` stayed, but its "the kernel's formatter has NO
comparison against C's flags at all, in either direction" is now false — this file is
that comparison for the integers and float shapes, and `suggestion-0004` extends it.
Bullet 5 stands, and is stronger than it was: the divergence is no longer merely
measurable between the implementations, it is what lite prints for `%f`.]**

- **Three implementations, one record.** `CCItoa` (prelude, for `printf`),
  `cc_dig` (prelude, for `sprintf`) and the kernel's `StrPrintJoin`. Only the
  first two are covered by the suite's fixtures, and their expected values come
  from C's documented `printf` behaviour; `gcc -std=gnu99` cross-checking — the
  habit PLAN records from M21 on — `PLAN.md:1369` now, and every milestone-log
  number in this file from M12's entry (`756` as filed) onward has moved by the
  same +100, so the 1269 and the run at 1340, 1381, 1445, 1509, 1543, 1622, 1716,
  1790 and 1989 that this filing printed all read 100 low — appears in no entry
  before M21, so the `printf`/`sprintf` fixtures from M6b/M12.1 have
  no recorded gcc-derived value either (the same gap `suggestion-0001` files for
  `unionc.c`).
  The kernel's formatter has **no** comparison against C's flags at all, in
  either direction.
- **C's flags are not expressible in lite's `printf` today.** `printf("%8d|",5);`
  is refused: `CC ERROR: printf: unsupported format` (measured; the conversion
  set is `%d %s %c %%` and the refusal is `CC_PARSE.HC:3789`). So the widths and
  precisions M36's `%f` needs — C's `%w.pf`, and C's implicit precision of 6 —
  are net-new work in the prelude regardless, while the kernel has all of them
  already.
- **And nothing in the prelude can print a float yet.** M35 landed the values:
  `float`/`double` are supported — one width, every value from the host's F64
  (`PLAN.md:190-201` in "C subset limitations", and the bullet "Floating point
  (M35)" at `PLAN.md:301-335`) — so the thing missing from C's `printf` is not
  `double`, it is `%f`: that bullet ends "`printf` has no `%f`"
  (`PLAN.md:333-334`) and puts the whole list on M36 (`PLAN.md:335`). A C-side
  dtoa is therefore still unwritten, and has to be written correctly (digit
  extraction across a decimal point, rounding, subnormals, inf/NaN, magnitudes
  past 2^53) inside the subset lite compiles. The kernel path already has an inf
  arm (`STRPRINT.HC:534`), a scaled-integer path for magnitudes the 64-bit digits
  cannot hold (`sp_out_f:` 555, `n=Log10(d)` 558) and its own
  documented limit ("In float formatting, do not exceed 18-digits before or
  after the decimal point because the numbers … are stored in 64-bits",
  `STRPRINT.HC:210-217`). A new C dtoa starts behind that.
- **The exception is recorded as a blanket, not per-flag.** `PLAN.md:837-838`
  says "format-flag semantics would have to survive the mapping" and stops. M27
  shows what the policy now asks for instead: a named, argued non-delegation
  with the cost spelled out ("Delegating to the kernel is the rule when the
  kernel has the harder problem (sorting, printing, the FFI); copying two known
  quads is not it", `PLAN.md:1678-1685`). For `printf` nobody can currently tell
  whether the exception is earned, because the flag-level answer does not exist.
  Under the new default, an unargued blanket is the shape that is out of policy.
- **The divergence is not hypothetical.** It is already measurable between the
  two prelude generators and the kernel, and one shape was already wrong in lite
  at filing time (see *Not claimed*: `%d` of the most negative quad; the post-fix
  tree no longer reproduces it). Float row 13 below shows a
  magnitude where the kernel path is badly wrong and the C path is right; float
  rows 1-8 show where the kernel is right and a fresh C dtoa would have to earn
  it.

## Already decided?

Partly, and this filing does not reopen it:

- **M6c considered this exact delegation and declined it**
  (`PLAN.md:833-838`), for the flag-semantics reason. That decision stands as
  far as it goes: nothing here says M6c should have done it in that milestone.
- **M27 declined a different delegation with an argument** (`PLAN.md:1678-1685`),
  and M12 records both directions in one entry — five functions delegated to
  kernel routines (`strlen memcpy memset memcmp malloc`, `PLAN.md:857-860`) and
  the rest kept in pure-prelude C because "the kernel has no equivalent or the
  semantics are C-specific" (nine named there: `strcmp strcpy strchr memmove atoi
  abs puts srand rand`, `PLAN.md:869-872`). The pattern of *arguing* the
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
  **[Answered since, three of the four, by M36 and M37 — the answers went first into
  `T:/CC/PLAN.md`'s milestone entries, and they are transcribed into this file now (see
  *What M37 measured, what shipped, and what the three open questions came to*), which is why
  the file closes rather than reading as partly resolved. Per flag: the mapping holds for
  `%d`, `%u`, `%x`, `%s`, `%%`, for field width and `0`-padding, and for the host's
  half-even tie rule, and it fails for absent precision, for `-` on the numeric arms,
  for `%c` of a NUL or wide value, for `h`/`,`/`t`/`/`, for the `%` before an unknown
  letter, and for the whole-value float scaling at precisions the floor this file
  offered called safe. Where the fix belongs: the absent-precision question was
  repaired in lite at compile time (as this paragraph predicted) by spelling `.6`;
  the float scale factor was NOT repaired in lite — under M37's ruling lite delegates
  the whole `%f` directive and inherits the host's rendering, deviation included, on
  the `getchar` precedent; and the remaining host-side gaps were left as host-side
  gaps, which after the delegation ruling means they are visible in lite's `%f` output
  and have to be documented there. Whether `cc->ext` should be widened: not for `%f` —
  what is staged per conversion is one directive's worth of quads and the four-slot
  ceiling has not bound it, so a delegated call per conversion remains the working
  shape and `CCStr`/`CCOutByte` still carry the literal runs either way. What is
  genuinely still undecided is the residue this file cannot close: whether
  `%d`/`%s`/`%c` and sprintf's engine ever move to the kernel's formatter — M36 argued
  they should not, today, on a cost comparison a future change in the host's engine
  could flip — and whether `%e`/`%g` get delegated, refused or invented when someone
  wants them.]**

## Evidence (measured at filing time; the drivers were host scratch under `/tmp` and are gone)

**Durability, as of the pass that closed this file.** The heading once read "all of it
reproducible", and that is no longer true of the filing-time probes: they lived under
`/tmp/opencode/` (see *Reproduction traps*), the host `/tmp` is wiped on reboot, and they were
not moved into the tree. What IS reproducible from the repo is the M37 battery — `T:/CC/GATES/`,
whose `README.txt` gives the command for each gate, the verdict each must print, and the
generator that rebuilds each program (`genm37.c`, `genffmt.c`, `mkrefuse.c`) — plus
`T:/CC/GATES/rows.txt`, the 342 kernel renderings every float expectation in the tree is derived
from, and `T:/CC/GATES/host.HC`, the driver that reproduces it. The shapes, commands and numbers
below are a record, not a re-runnable suite.

Host: gcc 13.3.0 (Ubuntu 13.3.0-6ubuntu2~24.04.1) on x86-64 Linux, LP64. Kernel
side measured inside `./3d_loader -t T -c` from the repo root; each shape
formatted into a buffer with `MStrPrint` and dumped as hex bytes, so padding and
NUL bytes are visible (see *Reproduction traps* 5 — console text hides both).
"C" is gcc's own `printf` on the same shape. lite's `int` is one quad
(`PLAN.md:138-141`), so the C column for wide values uses gcc's `long long`
spelling, which is the quad-width reading. Every `PLAN.md:<lines>` citation in
this file is as of filing time, when `T:/CC/PLAN.md` was clean against `68e4f34`
(see *Not claimed*); `6fed45c` then added lines in nine places and a
review-time correction added six more, so the filing-time numbers now read low
by a fixed amount that depends on where in PLAN they sat: 0 up to 77, +6 for
82-85, +32 for 86-157 (`## C subset limitations` 86 → 118), +44 for 158-240,
+46 for 242-251, +82 for 254-752 (its `518` → 600, `551` → 633, M11 at
`687-725` → 769-807), +100 for 756-2298 (M12 `756-795` → 856-895, M14
`907-937` → 1007-1037, the M21 habit line `1269` → 1369, M34 `2182` → 2282) and
+369 below M35's entry, which is new at 2399. Where a paragraph was rewritten at
review time its PLAN numbers were re-derived against the current tree and are
not filing-time; that is *Severity*, the *Finding*'s delegation, staging and
`StrPrintJoin` paragraphs, the `%f` bullet in *The cost today*, the gcc-habit
entry cited above, the *Already decided?* paragraphs naming M6c, M12 and M27,
and the *Not claimed* bullet about the delegation rule; everywhere else they are
as filed and the mapping above converts them. The milestone names and section
headings are the durable handles throughout. Every cell in both tables is a run,
not a recollection. The two runs use different field delimiters (`|` on the kernel
side, `[]` on some gcc runs), so the columns are compared on the field content,
not on the punctuation.

**1. Integer / char / string shapes.**

| # | shape | kernel (`StrPrintJoin`, measured) | C (gcc, measured) | verdict |
|---|---|---|---|---|
| 1 | `%d` −5 | `-5\|` | `-5\|` | ✓ |
| 2 | `%8d` −5 | `      -5\|` | `      -5\|` | ✓ width incl. sign |
| 3 | `%08d` −5 | `-0000005\|` | `-0000005\|` | ✓ 0-pad after sign |
| 4 | `%03d` 7 | `007\|` | `007\|` | ✓ |
| 5 | `%d` −(2^63−1)−1 | `-9223372036854775808\|` | `-9223372036854775808\|` | ✓ kernel and C agree; **lite printed `-(`** (filing time; the post-fix tree prints the C string) |
| 6 | `%x` 255 | `ff\|` | `ff\|` | ✓ lowercase |
| 7 | `%X` 255 | `FF\|` | `FF` | ✓ hex case available |
| 8 | `%x` −1 (quad) | 16 × `f` | `ffffffffffffffff` | ✓ kernel matches the quad reading; lite's `sprintf` printed `1` at filing time (documented then as `%x` prints the magnitude — `PLAN.md:518`, `CC_EMIT.HC:485`); the post-fix `cc_hexd` (`CC_EMIT.HC:423-450`) prints the unsigned reading, so lite matches now, and the bullet that documented the magnitude rule (`PLAN.md:600` as cited above) was rewritten in the same change to state the unsigned reading (`## C subset limitations`, `PLAN.md:610-613`) |
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
| 23 | `"50% done\|%d\|"` | `50done\|1\|` | `50 1one\|<garbage>\|` (C reads `%<sp>d` as a conversion) | ✗ three-way; and lite **refuses** this format at compile time (`CC_PARSE.HC:3789`) while the kernel silently drops the `%` |

`%-8d` and `%-8x` fail for one structural reason that a fix in one place would
remove: `PRTF_LEFT_JUSTIFY` is consulted only inside `OutStr`
(`STRPRINT.HC:29-40`), which the string-ish arms use; the numeric paths fall to
`sp_out_comma_num:` (457) and `sp_out_num:` (493), which pad before the value
(468-487, 499-514) and never look at the flag. Row 23 is the shape that should
decide how much weight the policy puts on "silently different": lite's policy has
no warning channel and treats a compile-time refusal as the tool (`PLAN.md:58-64`),
and delegation moves that refusal into the runtime engine, which drops output
instead.

**2. Float shapes (M36's `%f`; measured as M35's target at filing time).** "C"
is gcc's `printf` on the same value.

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
more than the 18 digits the engine documents (float row 13 is one). M36's `%f`,
whose C default precision is 6, lands in the good zone. By rows 9-13 the honest
answer is "delegable for the common range, with the precision spelled explicitly
and kept at or below 15, and the
out-of-range magnitudes either refused at compile time or routed to `%e`" — which
is an **argument**, the thing the new rule requires, where M6c's note had only a
reason not to. **[Measured since, and this is the sentence that did not survive:
M37 ran the sweep this answer was predicting would settle the question, and the
"good zone" is not one. The kernel's `%f` disagrees with C in 91 of 228 direct
host cells at explicit precisions 0..15, and the failures include values with no
rounding decision at all — `%.15f` of 1.5, of 1.0, of 2.5, of 10.0, of 100000.0 and
of 1e9. A precision ceiling cannot express the real limit, because the engine
multiplies the whole value by a factor that is itself wrong: see *What M37 measured, what
shipped, and what the three open questions came to* and `suggestion-0004`. The parts of this
sentence that survive
are the absent-precision repair, the half-even tie behaviour (which the host does
get right and which lite therefore inherits) and the magnitudes past the engine's
own digit limit — what lite does with that limit after M37 is neither refuse those
values nor route them to `%e`: it hands them to the engine and prints what comes
back, so `%.0f` of 1e19 is `10000000000000001600` and `%.6f` of 1e300 is a run of
zeros in lite's output as well as the host's, which makes them the host's documented
non-standard rather than C's. (A route that printed those magnitudes exactly — the
significand's digits doubled off the exponent field, measured exact against C on 3020
magnitudes up to DBL_MAX — was built and priced in the same session and abandoned when
the conversion was ruled to delegate whole.)]**

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
   value (13 significant digits) is identical. **[Corrected by M37: this is not a
   significant-digit limit. The `%f` arm scales the VALUE by `Pow10I64(precision)`
   (`T:/STRPRINT.HC:555-599`, the multiplies at 564/567/572) and that table is
   filled by `Pow(10.,i)` at `T:/KEND.HC:13`, so the factor is a few ULP off before
   any rounding happens; that is why 1e10 fails at p = 6 while 1e10 at a
   smaller-scale shape survives, and it is why the same engine fails at `%.15f` of
   1.5, where C's answer needs two significant digits. A significant-digit rule
   predicts neither. Filed as `tracking/suggestions/suggestion-0004-...`, which
   also records the direct 342-cell host re-measurement (91 of 228 cells at explicit
   precisions 0..15) and the `inf`→`i`, `nan`→a-scaled-decimal-number spellings this
   sweep's cause 2 bucketed without naming the table. And after the delegation ruling
   these are no longer facts about a host routine lite avoids: they are what lite's
   `%f` prints, so the `i`/`-i` and NaN spellings and the engine's digit behaviour
   belong in lite's documented-deviation list alongside the `getchar` semantics, not
   only in this file.]**

For the shape M36 needs, the answer is unchanged and now has a floor: `%f` with
the precision **spelled** and **at or below 15**, on a value whose integer part
fits, with `inf`/`nan` handled by lite, and with no `-`/`+`/` `/`#` until someone
implements them — which is a stated mapping, i.e. the argument M6c's note lacked,
and it says plainly which parts of the conversion lite would still have to own.
**[Rewritten by M37's ruling, because the delegation this paragraph recommended is
what landed and the floor is what did not. What landed: the precision is spelled
(a missing one is compiled as `.6`, which is what row 10 and sweep cause 1 asked
for), the directive with its field width goes to the engine, and lite prints what
comes back — so `inf` prints `i`, a NaN prints a scaled decimal number, and cause
3's flag gap becomes an inherited deviation for `%f` rather than something lite
avoids: `-`, `+`, ` ` and `#` reach the engine, whose numeric arms ignore the first
and do not implement the others, exactly as rows 13 and 14 measured. What did not
land is this paragraph's floor: a precision ceiling only binds a caller that owns the
scale factor, and lite no longer owns any part of the rendering. A design that DID own
it — scale only the fraction by a factor from exact integer quads, cap the precision
at 15 because 10^16 is past 2^53, print magnitudes ≥ 2^63 by doubling the
significand's digits — was built in the same session and measured at 2253 of
1,396,624 answerable cells (0.16%) against glibc where the whole-value route scores
22.74%, and it was then abandoned in favour of delegation. So the honest version of
this paragraph is: the delegation went in whole, the mismatch it inherits is the
host's and is documented as such, and the 0.16% alternative is the priced cost of
having chosen compatibility with the kernel over compatibility with C. See the next
section, and `suggestion-0004` for the table behind the 22.74%.]**

**3. What the mechanism would carry.** A conversion helper added the M12 way
costs, per call site: `CCSetScratch` + `MOVABS RAX,&lib[slot]` + `CALL [RAX]`
(`CC_EMIT.HC:887-893`) against today's plain `CCCall` into the module's own code
(`CC_PARSE.HC:3775-3783`), i.e. ~14 extra bytes and an RDI-scratch touch per
conversion, plus 2 staged quads (the value, and either the pointer to a
per-directive format quad the compiler already builds or the dst pointer) out of
`ext[4]`. It buys: one fewer prelude function, no recursion on the module's
private C stack (`CCItoa` recurses ~20 deep, `CC_EMIT.HC:31-33` as filed —
post-fix the recursion is `CCItoaNeg` at 36-38), and free
rebinding for saved objects through the `CC_B_LIB` slot indirection — the
property M12 verified with `objstdlib` (`PLAN.md:864-870`, `CC_TEST.HC:196-201`).
It does **not** change console semantics for NUL bytes or DolDoc markup: both
paths end at the kernel's `"%s"` print (`CC.HC:81` for mid-run flushes,
`CC.HC:119-130` at exit), so markup in output is interpreted today exactly as it
would be after delegation.

## What M37 measured, what shipped, and what the three open questions came to

M37 is the milestone this file's *Directions (a)* predicted would settle the `%f`
question, and it settled it: the delegation direction (b) recommended is what shipped,
while the precision floor this file attached to that delegation is what did not. Citations
to `T:/CC/PLAN.md` below are by milestone and section name first, because PLAN was being
rewritten alongside this pass and its line numbers moved twice; where a number is given it
is as of this pass, against the uncommitted M37 working tree (there the M36 entry begins at
`2749`, the M37 entry at `2833`, the M6c addendum bracket runs `907-935`, and the
delegation-rule paragraph of `## Correctness policy` sits inside `105-127`). PLAN's own float
numbers are now correct as printed here: its policy paragraph says 143,569 of 631,424 cells /
22.74% and its M6c addendum adds "and on 187 of the 342 cells of the direct 38-value probe,
91 of them at precisions stated at or below 15", which is what the harness data gives.

**The measurement.** Two runs, on two different cell sets, and they must not be conflated
— this file's own successor report had to correct the pairing, and PLAN now carries the
corrected version:

- **Priced rate** (pure python, no loader): a faithful re-implementation of the `%f` arm's
  arithmetic — `F64 × Pow10I64(p)`,
  the table entry modelled as the same transcendental family `Pow(10.,i)`, then
  `Round`, then the peel, with the engine's 17-significant-digit truncation — scored
  against glibc on 39,464 values × 16 precisions = **631,424 cells: 143,569
  disagree = 22.74%**. (934,272 is a DIFFERENT run's cell count — 58,392 values ×
  16 precisions — and it is the run that priced the fraction-scaling
  alternative, at 0.1733% there, with PLAN carrying 2253 of 1,396,624 = 0.16% from the
  larger sweep that followed; that alternative has since been abandoned, so neither of
  those figures describes what ships now. PLAN paired 22.74% with 934k in an earlier
  draft of the same entry and no longer does: as of this pass its
  `## Correctness policy` paragraph prints "143,569 of 631,424 cells, 22.74%", its M6c
  addendum adds "and on 187 of the 342 cells of the direct 38-value probe, 91 of them at
  precisions stated at or below 15", and the only 934,272 left in the file is in the
  pricing paragraph, where it is the denominator of the abandoned model's own rate. The
  correction `suggestion-0004`'s *Corrections* records is therefore applied in both
  places.) **Durability, stated plainly: these two runs were host python scripts
  (`m37design.py` and `m37price.py`) under `/tmp/opencode/`, the host `/tmp` is wiped on
  reboot, and neither script was moved into the tree — so the 22.74%, the 0.1733% and both
  cell counts are RECORDED here and cannot be re-derived from the repo.**
- **Direct host probe** (`T:/CC/GATES/host.HC`, committed, whose run produces
  `T:/CC/GATES/rows.txt`, also committed): 38 values handed to `StrPrintJoin` as raw quads of
  bits, across `%f`, `%.0f`, `%.1f`, `%.3f`, `%.6f`, `%.15f`, `%.16f`, `%10.3f`,
  `%.20f` = 342 cells. Run it as gate 4 of `T:/CC/GATES/README.txt` and the 342 `K` rows it
  prints are `rows.txt` byte for byte — verified on this tree — so **this measurement IS
  reproducible from the repo, which is the reason the fixture, the sweep and `CCProbeFloat`
  can all be regenerated from it.** The scoring of those rows against glibc was done by a
  host python helper (`m37fprobe.py` generating the driver, `m37map.py` doing the comparison)
  under `/tmp/opencode/` and gone with `/tmp`, so the **187 disagree with C** count and the
  **91 of them at stated precision ≤ 15** breakdown (per format: `%.0f` 10, `%.1f` 13, `%.3f`
  14, `%10.3f` 14, `%.6f` 15, `%.15f` 25, out of 38 values each) are recorded, and a reader
  wanting them re-derived can diff the committed `rows.txt` against glibc themselves. The
  discriminating
  cells include `%.15f` of 1.5 → `1.499999999999998`, and `%.15f` of 1.0, 10.0,
  100000.0, 1e9 and 2.5 — all exactly representable values whose C renderings need
  1–3 significant digits. `%.20f` of 1e18 is NOT one of them: the host prints C's
  answer there, which is one place where the priced model over-predicts failure and
  the probe is the authority.

**So the rule this file recorded is moot, and the measurement is what made it moot
twice.** "State the precision and keep it at or below 15" bounds the wrong quantity:
the engine multiplies the whole value by a factor that is itself a few ULP off
(`T:/STRPRINT.HC:564`, `:567`, `:572` reading a table filled by `Pow(10.,i)` at
`T:/KEND.HC:13`), so the error scales with the value's magnitude and appears at
precisions the ceiling calls safe; and there is a second, independent limit — after
scaling, the product has to stay integral-representable below 2^53 for `Round` to mean
what the peel assumes — which a better table does not remove. But the ceiling was
never lite's to enforce either: it existed because a lite-side design owned the scale
factor and needed 10^prec to stay exact, and under M37's ruling lite owns no part of
the rendering, so there is nothing for the ceiling to bind. Filed as
`tracking/suggestions/suggestion-0004-pow10-table-built-from-pow-not-exact-integers(o).md`,
which names the cause, lists every kernel caller of the table (the `%f`, `%e` and `%g`
arms, `Str2F64`, and the HolyC compiler's own float scanner at `T:/LEX.HC:579`,
`:598`, `:600`), prices the fix's blast radius and proposes building the exact range
exactly.

**What landed: the whole conversion, delegated.** Lite composes the directive text and
hands it over; the host renders. In detail, per the shipped code (`CCXFmt` and `CCIClap`
in `T:/CC/CC_LIB.HC`, `cc_sprintf` in `T:/CC/CC_EMIT.HC`, `CCPrintf` in
`T:/CC/CC_PARSE.HC`) and the measurements that pin it:

| part of C's `%f` | whose it is now | measured basis |
|---|---|---|
| the directive text | **lite** — rebuilt as a string: `%`, the flag letters, the width digits, `.` + the precision digits, with C's default 6 written in as `.6` when the source stated none, and `*` scanned but not copied | needed because the engine defaults to **0**: `%f` of 1.5 measured `2`, float row 10 and sweep cause 1. The effect is measurable: the filing-time probe scored the bare-`%f` column bad on 38 of 38 values because it compared the engine's precision-0 answer against C's precision-6 answer, and with `.6` written in the same column is bad on 15 — the same 15 cells the probe measured at `%.6f` |
| field width and `0`-padding | **host**, passed through — with the numeric runs capped by lite, on the text | `%10.3f` of 1.5 measured `     1.500`, byte-identical to C; `%010.3f` of pi → `000003.142`, `%10.3f` of −0.0 → `-00000.000`, all identical to gcc; `%01500.3f` of 1.5 measured a 1000-character field (fixture cell 57 pins all 1000 characters of it) and `%999.999f` measured 999 columns |
| the `+`, ` ` and `#` flags | **host**, and the host's rule for them is to print the REST OF THE DIRECTIVE as text | `%+f` → `f`, `% f` → `f`, `%#f` → `f`; `%+10.2f`, `% 10.2f` and `%#10.2f` → `10.2f`, five characters, no number. Through lite, `printf("%+f",1.5)` emits `.6f`. See *Question 1* below — this is output loss, and it is the kernel's own behaviour, not lite's |
| the integer→decimal walk, the fraction digits, the decimal point | **host** (`StrPrintJoin`'s `%f` arm, `T:/STRPRINT.HC:524-599`) | lite implements no radix conversion for a double. The walk stops at the host's own scratch (`k < TMP_BUF_LEN-SLOP`, `T:/STRPRINT.HC:205-206`, the loops at `:579` and `:593`) and the measured body saturates at **250 characters**: `%.600f` of 1.5 → 250 characters, and the body of `%999.999f` is those same 250 |
| the rounding, incl. half-to-even ties | **host** (`Round()` inside that arm) | `Round(1.5)=2`, `Round(2.5)=2`, `Round(3.5)=4`, `Round(-2.5)=-2`; `%.0f` of 1.5 and of 2.5 both `2`, `%.0f` of −2.5 `-2` |
| the scale factor | **host** — `Pow10I64(p)`, i.e. `Pow(10.,i)` | the 22.74% and 91-of-228 measurements above; inherited, not repaired |
| `inf`, `-inf`, `nan` | **host**, and so printed as the host spells them | `i`, `-i`, and a decimal number that is itself scaled (`%.6f` of a NaN measured `9223372036854.775808`); C says `inf`, `-inf`, `nan` |
| large magnitudes | **host**, truncating by its own rules | `%.0f` of 1e19 measured `10000000000000001600` (C: `10000000000000000000`); `%.6f` of 1e300 measured as a run of zeros |
| the count sprintf returns, and the destination | **lite**: `StrPrintJoin` returns its destination and not its end (measured over 342 calls) and takes no capacity argument, so `CCXFmt` returns `StrLen(dst)` and `CCIClap` is the bound — a width run above 1000 becomes 1000, a precision run above 600 becomes 600, against `cc->fmtbuf[1024]` | See *Question 2* below. Nothing overflowed: the measured worst case is a 1000-character field into a 1024-byte buffer |

So `%f` did take this file's direction (b) mechanism — a lib slot (`CC_LIB_FMT`, whose
`CC.HH` comment documents it as `cci_fmt(dst, value_bits, directive_text)`), a zero-arg
HolyC helper beside `CCXStrLen`, a line in `CCSetLib`, and the ~14-bytes-per-conversion
and RDI-scratch tax its "what the mechanism would carry" paragraph priced. Both sinks
reach that one helper: `CCPrintf` emits the staged call for a compile-time directive, and
the runtime `sprintf` engine calls the same `cci_fmt` by name. What did not come out of
`CC_PRELUDE` is `CCItoa`/`cc_dig`/`cc_hexd`: M36 argued that exception on the two costs
named in direction (b) and it stands, so the number-to-text duplication *The cost today*
names is still there, one conversion narrower than before.

**The alternative that was priced, and the documents that moved with the code.** Earlier
in the same session a lite-side design was built and measured: scale only the
fraction, by 10^0..10^15 as exact INTEGER quads (F64←I64 assignment is correctly
rounded; the decimal literal `1e15` is 12 ULP low for the reason in `suggestion-0004`),
cap precision at 15 because 10^16 is past 2^53, and produce the digits of magnitudes
≥ 2^63 by doubling the significand's digits off the exponent field. It scored 2253 of
1,396,624 answerable cells (0.16%) against glibc — every one of them ONE in the last
printed digit, created by its own multiply landing on an exact `.5` where the true
value was never at a tie (a Dekker product-residual tie-break was priced for those and
scored worse, 0.30%, because its own terms round) — where the host route scores
22.74%. That design has now been **abandoned**, so the 0.16% is not a deviation lite
carries: it is the measured price of the road not taken, and lite's `%f` deviation set
IS the host's. The earlier version of this paragraph said the tree had not caught up with
the switch, and named four documents that still described the abandoned design; that
sentence is now **false and is reversed**. The code and the documents moved in the same
change this file is being updated against: `T:/CC/CC_LIB.HC`'s M37 section comment opens
"M37: printf's %f, delegated WHOLE"; `T:/CC/PLAN.md`'s M37 heading reads "delegated WHOLE:
lite rebuilds the directive as text, hands it to the kernel's float printer, and emits what
comes back. Every divergence from C that follows is a documented non-standard, not a gap to
route around"; its `## Correctness policy` paragraph, which used to say "the radix walk,
the left padding and the rounding are the host's, and the scaling is the helper's", now
says the delegation "is taken WHOLE" and puts the by-parts design in the past tense with
its 0.16% against the 22.74% it replaced; and `## C subset limitations` describes the
shipped accept set — `%f` with "flags, field width and a precision of any size, all of them
the KERNEL's rules … handed to the kernel's float printer as text (M37); any other
directive, `%x` among them, is a refusal and not a silent narrowing". And
`T:/CC/TEST/ffmt.c` is regenerated: it pins the **kernel's** renderings, its header says so
and names where C differs ("Where the kernel and C disagree, the comment says so and the
kernel wins here, because lite hands the kernel the directive and emits the answer"), and
**nothing in the tree now diffs a float line against gcc's stdout**. The header's other
rule survives the switch untouched: no decimal float literal is a test value, because the
host's parser drifts — every value here is an exact significand scaled by an exact power of
two (`p2e`), which is how the fixture, the sweep and `CCProbeFloat` agree on one bit pattern
without a `double *`, a type lite refuses. That rule is this file's neighbour's finding,
`suggestion-0003`.

**Question 1, and its answer: the flags cost the conversion, and that is the kernel's
doing.** `%+f` prints `f`, `% f` prints `f` and `%#f` prints `f` — the number is gone. The
mechanism is in the engine's scan, not in anything lite writes: `StrPrintJoin`'s flag scan
(`T:/STRPRINT.HC:239-267`) recognises only `-` and `0` before the width run, then `*`, then
`.` and the precision run, and its modifier loop (`:270-317`) recognises only `\d / , t l h
?`; anything else falls to `sp_arg` (`:317`, `:321`), whose conversion switch (`:323-868`) has
cases for
letters and digits and **no `default:`**, so the unrecognised character is consumed by the
`switch (*fmt++)` and the REST OF THE DIRECTIVE is emitted by the outer loop as ordinary
text. `+`, space and `#` are in none of those case lists, which is why they produce text
rather than a number, and it is also why no `argv` slot is popped on that path — the code
that pops is inside the conversion cases. `%+10.2f`, `% 10.2f` and `%#10.2f` therefore all
print `10.2f`, five characters, in all three readings measured by ONE run of
`T:/CC/GATES/rfdriver.HC`: the runtime `sprintf` engine (`acc.c`'s A04-A06 rows), printf
(B04-B06, byte-identical), and the kernel called directly on the same bit pattern (its K04-K06
rows, also byte-identical, and produced in the same session, which is why that gate needs no
external C compiler). The A/B/K transcripts are run output and are deliberately not committed —
the drivers `acc.c`, `r01.c`..`r15.c` and `rfdriver.HC` are, and
`T:/CC/GATES/README.txt` gate 3 states the expectation (`ACC rc=0`, and `diff a.txt k.txt` /
`diff b.txt k.txt` both silent). The bare flag forms come from a different driver,
`T:/CC/GATES/hostcap.HC`: its run has `[f]` for `%+f`, `% f` and `%#f` on one bit pattern
(`K 0 9`, `K 0 10`, `K 0 11`) and stops at format 23 because format 24 is `%s` with an integer
argument, which threw and dropped into the debugger at `FL:T:/STRPRINT.HC,27` — so the composed
forms, not that probe, are the authority for the width-and-precision shapes. (`hostcap.HC` is
committed but is not listed in that README's gate inventory, so it is the one driver here whose
how-to-run is undocumented.)
Because lite injects C's default precision *after* the flag letters, `printf("%+f",1.5)`
emits `.6f`: the `+` is scanned and copied, `.6` is written in, and the kernel consumes
`.6` as a precision and prints the letter `f` where the number should be. Fixture cell 54
pins that string and `CCProbeFloat` pins the kernel's answer for the directive lite actually
sends (`StrPrintJoin(out,"%+.6f",1,&q)`). So: yes, **output loss rather than a formatting
deviation**, recorded as such — and it is the kernel's own fallback for a modifier it does
not implement, so lite passes it through rather than refusing. `+`, space and `#` are not in
the accept set as useful conversions; they are accepted, and what they yield is the kernel's
text. Do not call this a lite defect and do not tidy it away: refusing a flag the kernel
consumes would make lite stricter than the thing it delegates to, and lite refuses letters,
not flags.

**Question 2, and its answer: the buffer is not under-sized, because lite caps the numbers
it hands over.** `StrPrintJoin` takes no length argument, so the cap had to live on lite's
side of the seam, and it does: `CCIClap` rewrites the directive TEXT, turning a width run
above **1000** into 1000 and a precision run above **600** into 600, and it keeps a leading
`0` when a run begins with one (the kernel reads that zero as its pad flag, not as a digit)
so `%01500f` becomes `%01000f` and still pads with zeros, while a run of nothing but zeros
carries no width and is left alone. Rewriting the text is the only option a runtime format
leaves, and the rewrite is done through a 64-byte copy of the source because the destination
is `dir` itself; it is idempotent, which is what makes it safe for printf's sites, whose
directive lives in the string arena and can execute in a loop. The measured consequences, all
from one run of `T:/CC/GATES/rfdriver.HC` (`acc.c`'s A-lines are the runtime engine, its B-lines
are printf, its K-lines are the kernel rendered in the same session): `%01000.3f` of 1.5 writes a
1000-character field (A09 = B09 = K09, all three identical), `%01500.3f` writes that same
1000-character field and fixture cell 57 pins all 1000 characters of it, `%999.999f` writes 999
columns, and the BODY saturates at **250 characters** — `%.600f` of 1.5 measures 250 characters
(`0.000…`, A07 = B07 = K07) and the unpadded part of the 999-column row is those same 250 —
because the kernel's digit walks stop
at its own scratch (`k < TMP_BUF_LEN-SLOP`, `T:/STRPRINT.HC:205-206`, the loops at `:579` and
`:593`). Padding therefore dominates the buffer arithmetic, and the worst case is a
1000-character field into `cc->fmtbuf[1024]`, measured, with nothing overflowing: the 1026-cell
delegation sweep reports 0 failures and all 10 chunks `rc=0`, and the 22 acceptance rows above
match the kernel byte for byte. One figure in the milestone note is off and is corrected here
rather than repeated: `CCIClap`'s comment justifies the 600 by saying a `%f` body "never passes
247 characters", and `TMP_BUF_LEN - SLOP` is 248, but the measured saturation is **250**; the
margin is on the safe side either way. One claim in the handover I could not confirm against a
committed driver: `%.10000000000000000000f` → 600 fraction digits. No program in
`T:/CC/GATES/` uses that directive; what the gate battery does contain is a precision run of 600
(`acc.c`'s `.600f`, 250 characters because the body truncates) and the code path that clamps a
19-digit precision run to 600, which I read but did not run.
Also read and not measured: the directive text `CCPrintf` builds is a subset of the format
string, so `dir[CC_TOK_MAX+8]` against `fmt[CC_TOK_MAX]` is sized to fit the source plus the
two injected characters — the numeric cap is `CCIClap`'s, the array bound is not what stops a
huge precision.

**Question 3, and its answer: that fixture is gone, and what replaced it pins the kernel.**
`T:/CC/TEST/ffmt.c` is regenerated and pins the KERNEL's renderings — its header says so and
names where C differs ("Every rendering pinned below is what `T:/STRPRINT.HC`'s `case 'f':`
writes for these bit patterns — measured in TempleOS, not derived from what C would print …
Where the kernel and C disagree, the comment says so and the kernel wins here"). It carries
63 cells. Alongside it `T:/CC/CC_TEST.HC` gained `CCProbeFloat()`, 58 `StrPrintJoin(out,…)`
calls that hit the kernel directly on the same bit patterns with the directive lite SENDS —
`%.6f` where the source wrote `%f`, `%+.6f` for the `%+f` cell, `%01000.3f` for the cell whose
source says `%01500.3f` — so the fixture pins what C-source means and the probe pins what the
kernel answers for what lite sends, and the two are the same rendering by construction.
`CCM10` gained an object case: `PASS objfloat = 63` compiles the fixture, saves it, reloads the
object and runs it, which is the case that proves BOTH addresses a lowered float directive
bakes rebase — `cc->fmtbuf` through `CC_B_CTRL` and the arena-resident directive string through
`CC_B_LOCALS` — and that the helper's in-place rewrite of that string survives save and load.
Nothing in the tree now diffs a float line against gcc's stdout. Two runs of the in-tree suite
(`timeout 1500 stdbuf -oL ./3d_loader -t T -c T:/CC/CC_TEST.HC </dev/null`, gate 1 of
`T:/CC/GATES/README.txt`) agree: 89 `PASS` lines, identical PASS
sets, `CC TESTS: 0 failed`, `TOT ffmt 63 63`, `PASS objfloat = 63`,
`PASS float kernel = 58 cells`, no FAIL and no FAULT line, and the compile-time line the
fixture itself prints shows both halves of the design at once:
`ffmtP|1.500000|1.500|1.499999999999998|     1.500|` — C's default precision restored, the
kernel's digits where the kernel's digits are not C's, and C's own padding for `%10.3f`. The
transcripts of those two runs were host-side files under `/tmp` and are gone with it; the gate
that produces them is in the tree, and so is its expected verdict.

**The same deviation, counted from the C side, and how the two counts reconcile.** The sweep
(`T:/CC/GATES/gs0.c`..`T:/CC/GATES/gs9.c`, run in one session by `T:/CC/GATES/gsd.HC`, 38 bit
patterns × 9 directives × 3 sinks = 1026 cells) is
a delegation check, not a C check: its expectations ARE the kernel's renderings, its 684
`sprintf` cells self-check inside the program and report 0 failures in every chunk, and its 342
`printf` cells match the kernel text 342/342 — re-reading the sweep's own output and comparing
each `P` row's two halves against the golden list `genm37.c` derives from `T:/CC/GATES/rows.txt`
(gate 2 of `T:/CC/GATES/README.txt` spells out the `want.txt`/`got.txt` diff that decides this),
and all 10 chunks returned `rc=0`. Run the SAME generated programs through gcc
instead and 328 of those 684 `sprintf` cells fail: 164 left-hand and
164 right-hand lines, i.e. **164 distinct (value, directive) cells, 48.0% of the 342**. The
gcc transcripts were transient host files and are gone, but the programs they ran are committed,
so that count is re-derivable on any host with a C compiler. That is
the C-side deviation reached from the other direction, and it agrees with the 187-of-342
(54.7%) figure scored from `rows.txt` once the one column that is not comparable is taken out:
187 − 164 = **23**, and those 23 are exactly the bare-`%f` cells the probe scored bad because it
compared the engine's precision-0 answer against glibc's precision-6 answer, while the sweep
compares like with like after `.6` is written in. Of the bare-`%f` column's 38 probe-bad cells,
the 15 that remain bad in the sweep are precisely the 15 the probe measured bad at `%.6f`. The
containment runs one way only: every sweep-bad cell is probe-bad, and no cell good at the probe
went bad in the sweep. Per-directive, both counts agree exactly on the other eight columns
(`%.0f` 10, `%.1f` 13, `%.3f` 14, `%10.3f` 14, `%.6f` 15, `%.15f` 25, `%.16f` 28, `%.20f` 30).

**One behaviour change outside `%f`, recorded as measured.** The runtime `sprintf` engine now
scans a directive's flag and width run for EVERY conversion, so that run reaches the output as
text and the conversion letter is recognised instead of falling through as text too.
`sprintf(b,"%5d %d",5,7)` writes `55 7` where it wrote `5d 7`; `sprintf(b,"%-2d",7)` writes
`-27` where it wrote `-2d`; `sprintf(b,"%*d",10,7)` writes `10` where it wrote `*d` — the
measured rows are ARITY1-3 of the run of `T:/CC/GATES/rfdriver.HC` (`acc.c`'s three ARITY cells),
with return counts 4, 3 and 2
equal to the string lengths, and all three shapes are pinned by cells in the regenerated
`ffmt.c`; PLAN's M37 entry and its `## C subset limitations` carry the same three pairs. The
mechanism is readable in `cc_sprintf`: the A-part is buffered (`cc_a0` marks where it ends), and
for any letter that is not `f` the engine emits that buffered run — from index 1, so the percent
sign is not repeated — and then dispatches on `d`, `x`, `c`, `s`; `*` is scanned but deliberately
not copied, so `%*d` prints the width-less conversion of the FIRST argument (10) and the second
(7) is never consumed. Argument consumption is therefore unchanged: one argument per `%`, and
ARITY3 is the visible proof of it. None of the three is C's answer, which is why `printf` refuses
them instead: its scan requires the letter after the run to be `f`, and a `d` is not. So the two
sinks now disagree on these shapes — printf is a compile error, a variable `sprintf` format prints
the kernel-shaped text — and the disagreement is the engine's, because the engine is a directive
scanner that forwards only `%f`. Recorded and not filed as a defect: neither of these shapes was
C-exact before the change either (`%5d` printed `5d`, which is not C's `    5`), so nothing moved
from right to wrong, and an unrecognised letter in a variable format is emitted as text because
the dispatch has no final `else` — the letter simply falls to the outer copy loop.

**What is delegated, and what is still neither.** For `%f`, width and the flags now travel to
the engine, so this file's integer-table rows 13 and 14 (the numeric arms ignoring
`PRTF_LEFT_JUSTIFY`) and sweep cause 3 (`-`, `+`, ` `, `#`) are **inherited** rather than
avoided: a `%-10.3f` reaches the engine and right-justifies anyway, which fixture cell 53 now
pins, and C's `-`, `+`, ` ` and `#` spellings are not what lite prints. That is the
documented-deviation posture, and PLAN does take it: its `## C subset limitations` bullet hands
`%f` "flags, field width and a precision of any size, all of them the KERNEL's rules … as text",
its M37 entry says every divergence that follows is a documented non-standard, and its
`## Correctness policy` paragraph cites the `getchar` precedent for that posture. What lite
still does not delegate is the rest of the float family, and *Directions (a)*'s gaps come due
the moment that changes: `%e`, `%g`, `%E` and `%G` are refused by printf. Three of the four are
measured — `r01.c`..`r15.c`, run by `T:/CC/GATES/rfdriver.HC`, carry `%e`, `%g` and `%.2e` among
their 15 cases, and the gate's verdict is 15 `Rnn rc=-1` lines with one `CC ERROR:` per case
(gate 3 of `T:/CC/GATES/README.txt`) — while `%E` and `%G` follow from the code rather than from
a run: the test at the
end of the scan is `fmt[j] != 'f'`, and an uppercase letter is not `f`. The reason none of the
four is delegated is sweep cause 4: the host's `%e` default precision is 8 where C's is 6,
its exponent is spelled `e0`/`e-1` rather than `e+00`/`e-01`, `%g` behaves like `%e`, and `%E`
and `%G` print nothing; delegating them today would import four divergences rather than borrow a
rendering. The same battery records the other refusals — `%x % c %n %ld %u %5s`,
`%5d %010d %10.3d %.2d`, `%hd` and `%lf` — 15 for 15, every one a compile error and none a
silent narrowing, which is direction (c)'s refusal shape applied to letters and modifiers alike.
`%i`, `%p`, `%a`, the `*`-width argument order (the engine pops `argv` for `*` at
`T:/STRPRINT.HC:250-255` and `261-266`, which is not C's left-to-right conversion order), and
whether a bounded-format primitive exists in the kernel that a helper could use instead of a
capped-text rewrite remain unprobed; `%n`, `%p`, `%u` and `%c`-as-the-kernel-reads-it are
accepted by the engine and refused or handled by lite, which is a divergence in lite's accept
set and not in the kernel's.

## Directions (recorded, not implemented)

**(a) Finish the probe before touching code.** Shape by shape, against C, the
set `printf` actually accepts (`%d %s %c %%`, up to 8 args, `CC_PARSE.HC:3674`
and the 8-arg ceiling at 3704-3707) plus whatever M36 adds. Rows 1-23 and the
float table above are the first batch, done at filing time. Still unprobed:
`%+d`, `%<space>d`, `%08.3f`-style combinations, `%-` on `%f`, precision 19 and
above (1..15 measured exact for 1/3, 16/17/18 measured divergent), `inf`/`NaN`,
`%e` and `%g`, `%i`, `%n`, `%p`, the `*`-width argument
consumption order (the kernel pops `argv` for `*` at 250-255 and 261-266, which
is not C's left-to-right conversion order), and what a `%` before an unsupported
letter does to a *variable* `sprintf` format. Cost: about a dozen loader runs and
no code change. Output to write down: which flags survive, per flag.
**[Run, and it is what retired this file's own answer. `%f` was swept at nine
precisions over 38 values directly against the engine, and at 16 precisions over
39,464 values in the priced model; `inf`/`-inf`/`NaN` are measured (`i`, `-i`, and a
scaled decimal number); precisions 16..20 measured divergent; `%10.3f` measured as the
shape that agrees. `%-` on `%f` and the `+`/` `/`#` flags stopped being open questions the way this file
expected: under the shipped delegation the directive goes to the engine with its flags, and
the engine's answer for them is measured three ways — directly on the bit pattern, through the
runtime `sprintf` engine and through printf — and all three agree (see *Question 1* above). What
this paragraph hedged on is therefore settled, and settled in the blunt direction: for a flagged
`%f` it IS **output loss rather than a formatting deviation**, and it is the kernel's own fallback
for a modifier its scan does not recognise. The accept set that shipped is `%d %s %c %%` plus any
directive whose letter is `f`, with `%e`, `%g`, `%E`, `%G` and every modifier-flagged integer
refused at compile time. `%i`, `%p`, `%a`, the `*`-width argument order and the multi-argument
ceiling under delegation are STILL unprobed; `%n` is a special case worth a line, because the
engine implements it (`T:/STRPRINT.HC`, `case 'n':`) and printf refuses it anyway, so lite's
accept set is narrower than the host's there, by choice, and that refusal is measured
(`r11.c`, one of the 15 cases `T:/CC/GATES/rfdriver.HC` runs). A `%` before an unsupported letter in a *variable* `sprintf` format is answered
by reading `cc_sprintf` rather than by running it: the buffered A-part prints as text, the letter
finds no dispatch case and prints as text too, and one argument is consumed either way —
`%*d` measured through the engine shows the one-per-`%` rule holding, but no cell on disk runs a
variable `%e`, so that half stays read-and-not-run.]**

**(b) If the mapping holds, delegate and delete the duplicate.** New slot
(`CC_LIB_FMT 19` — the float family took 6-18, so the 6 this filing proposed is
`CC_LIB_FADD` now), a zero-arg HolyC helper in `CC_LIB.HC` beside `CCXStrLen`, a
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
come free for M36 instead of a new C dtoa (and row 10's divergence is cured by
spelling the precision), and the exception list shrinks to the
flags that genuinely fail. **[Taken, for `%f`, and in the whole form this paragraph
did not contemplate: the slot, a zero-arg HolyC helper beside `CCXStrLen`, the
`CCSetLib` line and `CCPrintf` emitting the staged call — the helper composes the
directive with the precision written out and hands the whole thing to `StrPrintJoin`,
so no decimal rendering of a double is left in the prelude at all, and the prelude's
`sprintf` engine reaches the same helper by name. Not taken: `%d`, so
`CCItoa`/`cc_dig`/`cc_hexd` are still in `CC_PRELUDE`, on M36's argued exception built
from the two costs named in this paragraph (no capacity argument for a byte-stream
sink; sprintf's return value not obtainable by pointer subtraction across re-based
arenas) plus the per-call-site tax — which means the duplication this file's *The cost
today* names survives, one conversion narrower. "Rows 1-8 come free" turned out to be
true and to be the whole bargain, good and bad: the free part is the radix walks, the
field width, the `0`-padding and the half-even tie rule (all measured identical to C),
and the price is everything else the engine does — the inexact scale factor
(`suggestion-0004`), the absent default precision, `i`/`-i` and the NaN spelling, and
its own digit truncation. A lite-side alternative was built and measured against that
price in the same session (fraction-only scaling by exact integer quads, precision
capped at 15, magnitudes ≥ 2^63 printed by doubling the significand's digits: 0.16% of
answerable cells against glibc where the host route scores 22.74%) and then abandoned;
the delegation won on the rule, not on the number, and the number is recorded here so
the next reader knows what was given up. The cost this paragraph hedged on is now
answered, and the hedge was the right thing to hedge: `StrPrintJoin` bounds its own DIGIT
walk (`k < TMP_BUF_LEN-SLOP` at `T:/STRPRINT.HC:205-206`, the loops at `:456`, `:492`, `:579`,
`:593`, `:616`, `:625`, `:804`, `:812`, `:832`) but its PADDING loops are driven by the field
width with no such bound — `for (;i<len-k;i++)` at `:473`, `:483`, `:507`, `:510` and `:546`,
guarded only by `if (len<0) len=0` — and the destination takes no capacity argument, so a
`%5000f` walks 5000 characters through a 1024-byte buffer. The fix is on lite's side of
the seam and is shipped: `CCIClap` caps a width run at 1000 and a precision run at 600 on
the directive text before the call, keeping a leading `0` so the pad flag survives.
Measured, both directions: `%01500.3f` produces a 1000-character field (fixture cell 57
pins every character of it) and `%.600f` produces a 250-character body, so padding, not
digits, is what the buffer is sized for. See *Question 2* above for the buffer arithmetic
and for the one figure in the shipped comment that is off by three.]**

**(c) If it does not hold for a flag, keep the C path for that flag and say so
per flag** — the shape M27's note has (`PLAN.md:1678-1685`), not M6c's blanket.
The three failures measured here have three different homes, which is why the
record has to be per-flag and not per-function: `%-` on numeric arms is a
10-line fix in the kernel's two padding blocks (457-487, 493-514) that
also makes the kernel better for HolyC programs, so "the host cannot do the job"
is a statement about today's kernel, not forever; `%c` of a value whose low byte
is NUL, or whose high bytes are non-zero, is a semantic difference the kernel
should keep (HolyC wants the string-shaped reading) and C should not, so those
cases stay in C — or, cheaper, `CCPrintf` can mask the argument to one byte and
emit `CCOutByte`, which it already does for `%%` (`CC_PARSE.HC:3761-3767`);
`%h` is not a numeric-shape problem at all, it is a **letter-collision** problem,
and the answer there is a rule about the format string, not about the helper: the
compiler must reject or rewrite the modifier letters the kernel engine gives a
different meaning (`h` 292-294, `,` 280, `t` 283, `/` 277, `?`, and the
`*` slots). Write whichever answer is taken into `## C subset limitations`
(`PLAN.md:118`) beside the existing `printf` bullet, since that is the section a
reader of a `%h` surprise will be sent to. **[This direction landed in two of its
three shapes, and what shipped added a fourth this paragraph did not contemplate.
**Non-delegation with a per-flag argument**, exactly as written here: `%d` (M36's cost
comparison, and `CCItoa`/`cc_dig`/`cc_hexd` stay), and `%e`/`%g`/`%E`/`%G`, which printf
**refuses** rather than delegating — `%e`, `%g` and `%.2e` measured among the refusals,
`%E`/`%G` following from the `!= 'f'` test — because sweep cause 4 measured what delegating
them would import: the host's `%e` default precision is 8 where C's is 6, its exponent is
spelled `e0`/`e-1`, `%g` behaves like `%e`, and `%E` and `%G` print nothing. **Refusal**, for
the letters and modifiers lite does not implement, now measured rather than inferred: 15
directives produce 15 `printf: unsupported format (only %d %s %c %% and %f)` compile errors
(the drivers are committed: `T:/CC/GATES/r01.c`..`T:/CC/GATES/r15.c`, run by
`T:/CC/GATES/rfdriver.HC` — `%e %g %lf %hd %5d %010d %10.3d %.2d %x % c %n %.2e %ld %u
%5s`), and PLAN's `## C subset limitations` states the rule in exactly those terms — "any other
directive, `%x` among them, is a refusal and not a silent narrowing". The runtime engine's
separate behaviour is recorded beside it (a precision is parsed for every directive and only
`%f` acts on it; an unrecognised letter prints as text and still consumes one argument), and so
is the shape change the new A-part scan brought: `sprintf(b,"%5d %d",5,7)` writes `55 7`,
`"%-2d"` writes `-27`, `"%*d"` writes `10`. **Inheritance**, the fourth shape: `%f`'s `-`, `+`,
` ` and `#` are neither refused nor non-delegated — they travel to the engine, whose conversion
switch has no `default:`, and what it writes is the rest of the directive. That is this
paragraph's hoped-for "keep the C path for that flag" inverted: a host gap taken into lite's
output on purpose, with *Question 1* above as its measurement (a flagged `%f` loses the
conversion outright) and fixture cell 53 as the pinned left-justify case. Documenting it is
therefore the whole job, and the section named here is where it lives: PLAN's
`## C subset limitations` hands `%f` "flags, field width and a precision of any size, all of
them the KERNEL's rules", beside the `getchar` bullet this file cited as the precedent. The
three homes are still three homes: `%c`'s NUL and wide-byte semantics were never delegated, so
that divergence is avoided rather than resolved, and the numeric arms ignoring
`PRTF_LEFT_JUSTIFY` (`T:/STRPRINT.HC:457-487`, `493-514`) remains a ten-line host-side fix that
lite now inherits for `%f` — `suggestion-0004`'s neighbourhood, not this file's to close.]**

(a) is worth doing whether (b) or (c) follows: under the new rule the thing that
has to exist is the argument, and today it does not. **[Written since, in three shapes at
once, and the last of the three turned out to be an answer rather than a residue: for `%d` and
`%x` the argument is a cost comparison (PLAN's M36 entry); for `%f` it is the delegation rule
itself, on the measurements this file supplied for it — the host has the conversion, lite owns
none of it, and the divergence from C is documented as the host's rather than repaired in lite
(PLAN's M37 heading and its `## Correctness policy` paragraph, which cites the `getchar`
precedent there); and for everything lite does not implement there is a refusal, measured 15
for 15 and written into `## C subset limitations` in the same change that shipped it. A refusal
IS the per-flag answer direction (c) asked for once it is recorded per directive — "say so per
flag" is a record, not a converter — so the residue this paragraph named is closed, and it is
the residue the pass before this one used to keep the file `(o)`. What is genuinely unprobed —
`%i`, `%p`, `%a`, the `*`-width argument order, the multi-argument ceiling under delegation — is
work for whichever conversion first wants it, which is this file's own closing sentence and not
an open finding.]**

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
   It is `#include`d into `CC.HC` immediately after `CCCtrl cc;` (`CC.HC:16`, and
   the layout row at `PLAN.md:633`) because the helpers reference `cc` directly;
   its own header calls them "zero-arg HolyC helpers" (`CC_LIB.HC:1`). The
   **pure C** half is the `CC_PRELUDE` string in `CC_EMIT.HC:9`, which lite
   compiles itself and which cannot call a kernel routine. The handover for this
   filing described `CC_LIB.HC` as "pure C … NOT a HolyC file", which is the
   wrong way round; the distinction is load-bearing, because the helpers are
   JIT-compiled HolyC callees and that is precisely why `CCSetScratch` exists
   (the RDI frame-zeroing landmine, `CC.HH:384-395`, `CC_LIB.HC:6-8`). A change
   in `CC_LIB.HC` therefore needs the HolyC image rebuilt (`HCRT.BIN`), while a
   change in `CC_PRELUDE` is recompiled with each module.
3. **`-c` hands its file to the HolyC compiler, not to lite.** Running a `.c`
   fixture through `./3d_loader -t T -c x.c` measured here as
   `ERROR: Undefined identifier at "main"`. To exercise lite's side, write a
   HolyC driver that names the `.c` file by a path the VFS can open — the M37 battery's
   driver is `T:/CC/GATES/gsd.HC`, which calls `CCRunFile("T:/CC/GATES/gs0.c")` ..
   `gs9.c` (`-c` also mounts the host root at `Z:` and reads work, per `AGENTS.md`; the
   filing-time probes used a `Z:/tmp/...` path, and that scratch directory is gone with the
   host `/tmp`).
4. **Run from the repo root.** `HCRT.BIN` is resolved from the process cwd; a run
   started in the `/tmp` scratch directory booted a different image and produced no
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
   half-even) was **not** tried. **[Tried since, by M37's probe: the host prints `0.2`
   for `%.1f` of 0.25, which is glibc's half-to-even answer and not the
   half-away-from-zero `0.3`. So the engine's tie rule is C's at that shape too, and it
   is worth being precise about which half of the float behaviour is wrong: the rounding
   decision is C's, the scale factor that feeds it is not (`suggestion-0004`). The cell
   that would have caught this filing's own optimism about 1.5 was `%.15f` of a
   terminating value, and that is the cell M37 found.]**

Working scripts: the filing-time probes lived under `/tmp/opencode/delegprobe/`
(`k1.HC` … `k11.HC` — kernel shapes: ints, widths, justification, hex, `%c`, modifiers, floats,
the precision sweep at `k9.HC`/`k10.HC`/`k11.HC`), `c1.c`/`c2.c`/`c3.c` with
`drv1.HC`/`drv2.HC` (lite's side), `g1.c`…`g17.c` (the gcc witness, including the
`%.*f` precision sweep at `g13.c` and the integer/float baselines at `g11.c`,
`g12.c`, `g16.c`, `g17.c`). **None of those was moved into the tree, and the host `/tmp` is
wiped on reboot, so they no longer exist** — what is durable is the commands-and-shapes record
above, plus `T:/CC/GATES/`, which carries the M37 battery and, in its `README.txt`, how to run
each gate and how to regenerate each program.

## Not claimed, and one thing deliberately not filed

- **`%d` of the most negative quad printed `-(` in lite at filing time**
  (measured: `printf("%d|",-9223372036854775807-1);` → `-(|`, and the same via
  `sprintf` → `-(`, count 2) where both C and the kernel print
  `-9223372036854775808` (measured, 20 digits). The mechanism follows from the
  prelude source: `cc_v=-cc_v` overflows back to the same negative
  (`CC_EMIT.HC:26-29`), so `if (cc_v>=10)` (`:31`) skips the recursion and
  `48+cc_v-(cc_v/10*10)` (`:33`) evaluates to 40, `(`. `cc_dig` has the same
  shape (`389`, `397`). That is wrong output, so by the tracking rule it is
  a defect and belongs in a `BUG-NNNN(o).md`, not here; this filing was scoped to
  one suggestion, so it is recorded as evidence and left unfiled. There was no
  `tracking/bugs/` directory in this checkout at filing time. Those four line
  ranges and the negate-first shape they describe are the tree as filed; in the
  post-fix `CC_PRELUDE` the walker is `CCItoaNeg` (`CC_EMIT.HC:34-40`) and
  `cc_dig` is 403-416, and neither negates the value it walks — so the measurement
  above no longer reproduces, and is kept because it is what the filing measured.
  **[Filed since, and cross-linked both ways while the report existed:
  `tracking/bugs/BUG-0001-printf-d-of-the-most-negative-quad-prints-minus-paren(o).md`
  — `Open`, severity P1, when filed, and fixed by M36 now: per the lifecycle a
  fixed bug is deleted rather than renamed to `(c)`, so that path is now gone.
  It re-ran the measurement, added the
  kernel-formatter byte witness and the `sprintf` return-count witness (2
  against C's 20), and pinned the mechanism line by line; it also corrected one
  line of this bullet's evidence — the host's `%d` of a `long`-typed argument
  prints `0`, not the 20-digit string, because `%d` converts an `int`
  (`gcc -std=gnu99 -Wall` names the mismatch outright), so the sound host
  comparisons are `%ld`/`%lld` on the same value. Of the directions it recorded,
  (b)'s shape and (e)'s answer are the ones that landed: the sign is chosen by
  the caller, by negating the non-negative side, and the walker takes values `<=
  0` (`CCItoaNeg`, `CC_EMIT.HC:34-40`, with `cc_dig` at 403-416 as its
  buffer-and-count twin), and `%x` prints the quad's unsigned reading instead of
  a magnitude. Direction (d) — this file's direction (b): delegate `%d` to
  `StrPrintJoin` and delete `CCItoa` — was not taken, so the conversion is still
  a second implementation inside the adapter, now a correct one, and what this
  file actually asks remains open. A report is the only record of its own
  finding, so with the file deleted the record is `T:/CC/PLAN.md`: the M6c
  addendum's "filed separately as a bug and closed by M36" (`PLAN.md:868`), the
  M36 milestone entry (`PLAN.md:2684`+), and the `%x` bullet of `## C subset
  limitations`, rewritten in that same change (`PLAN.md:610-613`). **Those three PLAN numbers
  are the tree as of the pass that wrote them and they moved again with M37: as of this pass the
  "filed separately as a bug and closed by M36" sentence is `T:/CC/PLAN.md:932` (inside the M6c
  addendum bracket at 907-935), the M36 milestone entry begins at `2749`, and the `%x` clause of
  `## C subset limitations` is at `666-671`. Cite those, or cite by section name.** The deleted
  report reads back from `6fed45c` — the only commit that ever held it — and the
  deletion is now COMMITTED, at `6b1b04c`, so `6fed45c` is that commit's parent and
  the read-back is not conditional any more: `git log --diff-filter=A --
  tracking/bugs/` names `6fed45c`, `--diff-filter=D` names `6b1b04c`, and
  `git ls-tree HEAD tracking/bugs/` is empty (this bullet once said the last of those
  listed the file, which was true while the deletion was unstaged and is false now).
  What reads back is the text as of
  `6fed45c`, 731 lines: the citation corrections a later review pass made in that file were
  never committed, so they survive only as a scratch copy at
  `/tmp/opencode/BUG-0001-deleted-2026-11-05.md` — outside the repo, on a host `/tmp` that is
  wiped on reboot, and therefore gone: the uncommitted review-pass citations in that report are
  unrecoverable, and the committed 731-line text is the whole durable record.
The measurement above stays the strongest available argument, for the
  tree it measured, that the two implementations disagreed; what M36 removed was
  the disagreement, not the argument.]**
- **The "Delegation is the default" paragraph was not in the file at filing
  time.** `T:/CC/PLAN.md` was then clean against `68e4f34` ("CC: M34 …"), its
  `## Correctness policy` section (44-85) ended at the gcc-as-witness paragraph,
  `grep -rn "Delegation is the default"` and `grep -rn adapter` over the tree
  returned nothing, and the last milestone entry was M34 (`PLAN.md:2182` then)
  with no M35 entry at all. So the rule was cited here as the owner's M34
  decision as relayed, and it is marked as such; the measurements do not depend
  on the wording. **Superseded at review time:** `6fed45c` wrote both — the
  paragraph is `PLAN.md:92-116` and the M35 entry is `PLAN.md:2399` — and
  *Severity* and the *Finding* above were re-derived to match. The naming point
  stands, and is now PLAN's own: the same commit rewrote M6c's note from
  "through kernel `Prnt`" to "through the kernel's format engine (`StrPrintJoin`,
  T:/STRPRINT.HC, extern'd in KERNELA.HH)", and `grep -rn Prnt T/` now returns
  two lines — `PLAN.md:98` and `PLAN.md:113`, both in the new paragraph — neither
  of them a routine that exists under that name in this tree. The engine to name
  is `StrPrintJoin`.
- **Not probed:** precision 19 and above (1..15 measured exact for 1/3, 16..18
  measured divergent); `inf`/`NaN`; `%e`, `%g`, `%i`, `%n`, `%p`, `%a`; `%+d` and
  `%<sp>d` beyond the
  one row that fell out by accident (row 23); `%-` on `%f`; `*`-width arg order; the
  8-argument `printf` ceiling under delegation; whether a bounded-format
  primitive already exists in the kernel that a helper could use instead of
  scratch-plus-copy. The mechanisms behind float rows 9 and 13 (the trailing-digit
  difference, and the flipped sign on a 20-digit magnitude) were not chased.
  **[Updated by M37. Now probed: `inf`, `-inf` and NaN (`i`, `-i`, and a decimal
  number that is itself scaled); precisions 16 through 20 on 38 values; `%10.3f`;
  `%.1f` of 0.25 (see trap 8). **Updated again by M37 as shipped:** `%-` on `%f` is no longer
  unprobed — the engine pads before the value and the numeric arms never consult
  `PRTF_LEFT_JUSTIFY`, and fixture cell 53 pins the result: `%-10.3f` of 1.5 renders right
  justified, byte-identical to `%10.3f`. `%n` is no longer unprobed either, in the direction
  that matters for lite — printf refuses it (it is one of the 15 measured compile errors) even
  though the engine implements `case 'n':`. And "does a bounded-format primitive already exist
  in the kernel" is answered by `StrPrintJoin`'s signature: `U8 *StrPrintJoin(U8 *dst,U8
  *fmt,I64 argc,I64 *argv)` — four parameters, no capacity — so none exists, and what shipped
  instead is lite capping the two numeric runs on the text (`CCIClap`: 1000 and 600 against
  `cc->fmtbuf[1024]`). `%+d` and `%<sp>d` are still unmeasured as OUTPUT shapes: printf refuses
  them (a `d` after a flag is not `f`), so the refusal is the answer, and nobody has measured
  what a flagged `%d` does in a variable `sprintf` format — the engine's A-part rule predicts it
  prints the run as text and then converts, and that prediction is untested for `+` and space,
  while `%5d`, `%-2d` and `%*d` ARE measured. Now chased, for float row 9's shape: the
  trailing-digit difference is attributable to the scale path — `T:/KEND.HC:13` fills the table
  `T:/STRPRINT.HC:564`/`:567`/`:572` multiply with `Pow(10.,i)`, and the scaled product
  also has to stay integral-representable for the peel to mean what it assumes — which
  is an attribution from a measurement plus a source read, recorded with its cause in
  `suggestion-0004`, not a chased blow-by-blow. Float row 13's flipped sign on a
  20-digit magnitude is STILL unexplained and, worse, not even reproducible on demand:
  M37's probe used precisions 0, 1, 3, 6, 15, 16 and 20 and never `%.2f`, so the exact
  shape that produced `-567041950362118272.00` for a positive input in the filing-time
  probe has not been re-run, while the same value at the precisions that WERE re-run
  comes back unsigned and wrong in a different way (`%f` of 1.2345678901234567e19 →
  `12345678901234569600`, against C's `12345678901234567168`). `%i`, `%p`, `%a`,
  the `*`-width argument order and the multi-argument ceiling under delegation are all
  still unprobed; `%n` left that list when the shipped accept set was measured, because printf
  refuses it.]**
- **"The kernel already matches C on 1..15 fractional digits"** is a claim about
  the values tried (1.5, 0.0, −1.25, 2.5, 0.125, 1/3, 2/3) and one 17-digit integer
  value; it is not a proof for all doubles, and subnormal and near-tie inputs at
  high precision were not exercised. **[Not a claim any more: it is false as stated,
  and the counterexample is one of its own values. `%.15f` of 1.5 prints
  `1.499999999999998` and of 2.5 `2.499999999999996`; `%.15f` of 1.0, 10.0, 100000.0
  and 1e9 are wrong the same way; 187 of the 342 cells M37 probed disagree with C and
  91 of them are at stated precisions ≤ 15. What survives of the sentence is the
  narrower thing it was measuring: at precisions 1..15 the engine's DIGIT COUNT and its
  tie rule are C's, and its half-even behaviour on 1/3 and 2/3 is exact. The caveat
  "not a proof for all doubles" was the right warning and this filing did not weight it
  enough.]**
- **Nothing here says M36's `%f` should be delegated.** It says the question has
  a cheap answer, records a first batch of that answer, and says which flags the
  batch says do and do not survive. **[It was delegated, whole, by M37 — the lite-side
  fraction-scaling alternative having been built, measured at 0.16% of answerable
  cells against glibc and then abandoned — and the answer that came with the delegation
  is that the host's rendering is the standard lite asserts for this conversion, with
  the divergence from C documented rather than repaired. What this paragraph got right
  is the part that mattered: the question was cheap to answer, the answer was worth
  writing down before the code, and it is now written down here, in PLAN's M37 entry and
  in `suggestion-0004`.]**

## Repo notes

`tracking/` is **not** in this repo's `.gitignore` (`git check-ignore tracking/`
prints nothing; rc=1), and the precedent suggestion is committed at `68e4f34`, so
once this file is committed a deletion here would be recoverable from history at
the deletion commit's parent — unlike the assumption the tracking skill carries
elsewhere. Per the lifecycle a suggestion is never deleted regardless: whichever
direction lands, this file stays, renamed to `(c)` with status `Implemented` or
`Declined`, and keeps its reasoning. Filed on `bugfix/build-isssues` in the
primary checkout `/mnt/d/Source/3Days` and committed at `6fed45c` (M35), which
also committed `tracking/bugs/` for the first time.

**Where the evidence lives now.** The gate battery that decides every `%f` claim in this file is
in the tree at `T:/CC/GATES/` — 35 programs and tables plus `README.txt`, which documents how to
run each gate, what verdict each must print, and how to regenerate each program from its
generator — because the host `/tmp` is wiped on reboot and the probes this file was filed on lived
there. Run logs are deliberately NOT committed: attribute a measurement to its driver
(`T:/CC/GATES/gsd.HC`, `rfdriver.HC`, `host.HC`, the `CC_TEST.HC` suite) and not to a log path.

**Status decision on this pass, and the one it supersedes.** The M37 measurement pass set this
file to `Partly resolved` with the marker staying `(o)`, on the ground that the integer walkers
had not moved and that several *Directions (a)* shapes were unprobed. That reading is superseded
by the shipped state: the status is `Implemented` and the marker moves with it, so the file is
`suggestion-0002-printf-number-conversions-never-mapped-onto-kernel-printing(c).md` — slug
unchanged except for the marker — and it is KEPT with its reasoning, because a suggestion is
never deleted and because this file is additionally the only `tracking/` record of `BUG-0001`'s
closure (first bullet of *Not claimed*; that bug file was added at `6fed45c`, deleted at
`6b1b04c`, and reads back in full from `6fed45c`, which is the deletion commit's parent). The
substantive reasons are in *Status* and in the three answers above; the lifecycle reason is that
`Implemented` is a decided state, and `Partly resolved` would misdescribe a finding whose three
asks all have answers on the record: the mapping (measured acceptance, measured refusal, and
measured inheritance), the delegation (`%f`, whole, both sinks), and the per-flag answers where
the mapping does not hold (M36's cost comparison for `%d`/`%x`, 15 measured refusals, and this
file's own sweep as the reason `%e`/`%g`/`%E`/`%G` are refused and not delegated). Renaming is
safe with respect to everything that cites the file: `grep -n "suggestion-000[0-9]"
T/CC/PLAN.md` returns five references to this id, none carrying a path or an `(o)` suffix, and
`grep -rn "tracking/suggestions" T:/CC/` returns nothing at all. Recoverability, since this repo
does **not** gitignore `tracking/`: the as-filed `(o)`/`Proposed` text is durable at `6fed45c`;
the `Partly resolved` intermediate named above was never committed, so it survives only as the
sentences in *Status* and in this paragraph, and not in git; and everything this pass wrote is
uncommitted until the user commits it — it sits in the index and the working tree, and a
deletion before that commit would lose it outright, which is why this file was renamed and not
removed even though it is now closed.
