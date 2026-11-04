# unionc.c's expected 40 has never been derived from gcc

## Status

Proposed

**Severity:** P2 — nothing is wrong today. The 40 the suite asserts is what the
model deviations add up to, and this filing shows it is *derivable*; what is
missing is the evidence, so the risk is latent:
`CCAssert(40,"T:/CC/TEST/unionc.c","unionc")` (`T:/CC/CC_TEST.HC:289`) is the
suite's only PASS with no recorded independent verification behind it, and a
future change to keyword folding, the char/unsigned fold, union layout or
`sizeof` of the int family can move that value in the wrong direction with only
a hand-written number to notice against.

## Finding

`T:/CC/TEST/unionc.c` is the fixture whose VALUES PLAN records as never having
been cross-checked against `gcc -std=gnu99`, per check, the way the later
fixtures were. It is not gcc-buildable as it stands, and the three things that stop
gcc are the fixture's own subject matter — the storage-class and `inline` shapes
M19 exists to fold. So no one has ever recorded, per check, which of the 39
checks C agrees with and which it does not, and why.

This is a suggestion, not a defect: no output is wrong. Measured here, on a copy
whose C-forbidden lines are rewritten, **gcc returns 36, and the 4-point gap
between C and the fixture's 40 is exactly four checks — the fixture's own 8, 10,
15 and 29 — each false under C for a reason PLAN already documents as a model
deviation** (below). Nothing in the tree records that mapping. The 40 is PLAN's
recorded value, not a number this filing re-ran (see *Evidence 6*); the gcc half
was re-run in full. See also *Not claimed*, at the end.

## The cost today

- **Deriving the number is manual.** A change that touches the byte model, the
  `int`/`unsigned` slot size, union layout, literal `sizeof` or the qualifier
  fold has to re-read 39 checks and re-derive 40 by hand, because there is no
  marked copy for this fixture while every post-M21 fixture has one. (The
  habit is recorded at `T:/CC/PLAN.md:1218-1220`, M21: "the fixture was
  cross-checked against `gcc -std=gnu99` on a copy with only the model-specific
  `sizeof` numbers adjusted".)
- **A failure carries no attribution.** `CCAssert` prints one number per fixture
  (`T:/CC/CC_TEST.HC:9-19`): `FAIL unionc got 37 want 40` says nothing about
  *which* checks moved, and `T:/CC/TEST/unionc.c` is 134 lines and 39 checks
  spanning the six feature groups its own header comment lists, so the first
  move after such a FAIL is to bisect by hand — against a fixture in which four
  checks are *supposed* to be C-false, so "make gcc agree" is the wrong reflex
  for those four.
- **The expected value is easy to adjust to whatever the compiler now does.** A
  regression is a total, so the tempting fix is a total edit. Without a marked
  comparison there is nothing to say whether the new total is C-faithful or just
  what the broken compiler happens to print — which is the failure mode the
  cross-check habit exists to prevent.
- **The ritual costs three rounds, not one.** See *Reproduction traps*: the
  blockers are revealed one at a time, and one of them is a LINK error, so
  `-c` alone does not even show the whole list.

## Already decided?

Partly, and this filing does not reopen it:

- The discipline (every fixture cross-checked against `gcc -std=gnu99`, per
  check) is recorded in `T:/CC/PLAN.md` and is not disputed here.
- The reason unionc.c lacks it — and the 36-vs-40 gap itself — are already
  written in the M34 milestone entry (`T:/CC/PLAN.md:2232-2246`), which says
  outright that the gap "is filed as a suggestion rather than glossed over".
  This file is that filing. It adds the per-check mapping, which PLAN does not
  have.
- The "adjusted copy" technique (M21, and again at `PLAN.md:1290`) is the
  precedent for how to close the gap without pretending the fixture is C-clean.
- What is NOT decided is which of the two directions below to take, and whether
  `## C subset limitations` should name the inline-semantics choice (see
  *Direction (b)*).

## Evidence (re-run at filing time; all of it reproducible)

Host: gcc 13.3.0 (Ubuntu 13.3.0-6ubuntu2~24.04.1) on x86-64 Linux, LP64 — the
same ABI the C host uses.

`T:/CC/PLAN.md` was being edited during this filing, so every `PLAN.md:<lines>`
citation below is as of filing time; the bullet and milestone NAMES are the
durable handles.

**1. The file as it stands does not build, and the blockers surface one at a
time.**

```
gcc -std=gnu99 -Wall -c T:/CC/TEST/unionc.c       # rc=1
  error: file-scope declaration of 'ag' specifies 'auto'      unionc.c:14
```
(Each command below ran on a byte copy in `/tmp/opencode/verify`, so the tree was
not written to; the line numbers quoted are the fixture's own.) Fix line 14
(`auto int ag;` -> `int ag;`) and the SAME command now reports the one it hid:
```
  error: register name not specified for 'rg'                 unionc.c:12
```
Fix line 12 too, and drop `-c` (link stage), a third appears:
```
  undefined reference to `ifunc'   ...   collect2: error: ld returned 1 exit status
```
so line 29 (`inline int ifunc(int x)`) follows C99 inline semantics: the
definition is not externally emitted, and a one-TU program has no definition to
link. All three shapes are what M19 pins (the limitations bullet that opens
"M19 edges: qualifier keywords (`extern static register auto volatile inline`,
added to `const`) are accepted and folded", `PLAN.md:211-217`), so the fixture is
intentionally not gcc-buildable. `volatile`, `static` and `extern` at file scope
are NOT blockers: the errors name only `ag` and `rg`.

**2. On a copy with those lines rewritten, gcc builds it and answers 36.**

Rewrites: line 14 `auto int ag;` -> `int ag;`, line 12 `register int rg;` ->
`int rg;`, and no source change at line 29 — instead `-fgnu89-inline`, so the
`inline` definition is emitted:

```
gcc -std=gnu99 -w -fgnu89-inline -o uc_gcc uc_base.c   # rc=0
./uc_gcc                                               # exit status 36
```

**3. Check by check: 35 of the 39 checks are C-true, four are false.**

39 `tot=tot+` lines, each followed by `printf("@n=%d\n",tot);` (and
`int printf(const char *,...);` prepended) gives 39 marks; gcc's last reads
`@39=35`, so 35 checks contributed 1 and 4 contributed 0. That is as far as the
engine-to-engine comparison got in this filing — the lite half is *Evidence 6* —
but it is enough to recover lite's per-check verdicts from its recorded total:
a check can only contribute 1, so a lite total of 40 means all 39 contributed 1
there. The mark index and the fixture's own `/* n */` labels coincide 1:1 (the
two-line check 26 has only one `tot=tot+` line). Deltas:

| check | unionc.c | the check | why C answers 0 |
|---|---|---|---|
| 8 | :66 | `sb=-3; tot+=(sb==253);` | `signed char` keeps `-3`; lite's byte model zero-extends. This one IS already named in `PLAN.md:215-217` as "the deviation the unionc fixture pins". |
| 10 | :70 | `(u==1)&&(sizeof(unsigned)==8)` | `sizeof(unsigned)==4`. The `u==1` half is C-true; the conjunction fails on the slot-sized `sizeof` of the int family (`PLAN.md:132-135`, `PLAN.md:2255`). |
| 15 | :81 | `(side==42)&&(v.i==(long)ip)` | `side==42` is C-true; the union pun is not — `v.i` is the low 32 bits of the pointer (measured: `v.i=767321508`, `ip=140721075807652`), so the comparison fails. lite makes the int member the whole quad. |
| 29 | :113 | `sizeof("ab")==8` | C says 3; lite answers the pointer size (`PLAN.md:225-226`). |

All four correspond to deviations PLAN already documents. That is the sense in
which 40 is derivable rather than merely asserted — 40 = 39 checks + 1 closer
(`unionc.c:124`, `return tot+1;`), of which 4 are C-false by design.

**4. Five of the agreements in that run are ABI luck, not model agreement.**

Checks 11, 12, 17, 19 and 25 (e.g. `sizeof(v)==8`, `sizeof(struct Box)==16`,
`sizeof(cs)==24`) pass under C on this host only because C's layout happens to
land on lite's slot sizes: measured `sizeof(union Value)==8` (driven by the
8-byte `int *` member, not by "one slot") and `sizeof(struct {int w; union Value
val;})==16` (from padding). On an ILP32 host the same ritual would report them
as divergences. A per-check comparison of this fixture is therefore ABI-qualified
and should say so where it is recorded.

**5. What the rewritten-copy comparison does NOT prove about the fixture's
headline subject.**

- Check 1 (`sg+eg+rg+vg+ag==42`, "every qualifier form declares") passes under
  gcc only because the rewrite deleted the two spellings it was testing. The 1
  gcc prints for it is not evidence about `auto`/`register` at file scope — C
  forbids them, so no amount of marking tests lite's fold against C.
- Check 2 (`ifunc(5)==4`) passes under gcc only with `-fgnu89-inline`. So the
  fixture commits lite to GNU89 `inline` semantics — a bare `inline` definition
  is callable — while `-std=gnu99`, the standard every other fixture is measured
  against, says the definition does not exist. `## C subset limitations` does not
  list that choice; only the M34 milestone prose does (`PLAN.md:2238-2240`).

**6. The lite side could not be re-run at filing time.**

`CCRunFile` on both `T:/CC/TEST/unionc.c` and the rewritten copy failed before
reaching either file: the working tree's `T:/CC/CC_PARSE.HC` did not bootstrap on
either attempt — `ERROR: Missing expression at "`" T:/CC/CC_PARSE.HC,6059`, a
`/*`-comment continuation line reached outside a comment. That is a snapshot of a
tree mid-M34 (the file's size and mtime changed twice during this filing), not a
claim about the merged tree. The lite-side 40 is therefore transcribed from
`PLAN.md:2243-2246`, `PLAN.md:2265-2266` and `CC_TEST.HC:289`, not re-derived
here. Re-run step 3 against lite once
the tree bootstraps again; the gcc half is complete and needs no tree state
beyond the fixture.

## Directions (recorded, not implemented)

**(a) Split the fixture.** Put the gcc-buildable checks in a file that can be
marked and compared check by check, and leave the C-forbidden keyword shapes in a
lite-only file whose expected value is justified line by line. Cheap and honest:
the residue that is already C-true is 33 checks (the 35 of *Evidence 3*, minus
the two whose subject the rewrite removes; lite's side of that agreement follows
from its recorded 40, not from a re-run here). Costs: the split moves
`sg`/`eg`/`rg`/`vg`/`ag` and the `helper`/`sfunc`/`ifunc` trio, which check 1,
check 2 and check 39 all read, so the pair is no longer one translation unit and
check 39's "static global + qualified call" stops being the same shape; the new
lite-only file still needs its value justified by hand, because it is exactly the
file gcc cannot build; and the four C-false checks have to land on one side or
the other anyway, which is direction (b) work regardless. Also a new fixture is
a new `CCAssert` line and a new entry in the reload-stress list every milestone
re-runs.

**(b) Keep one file; record the derivation.** Add to `T:/CC/PLAN.md` — the
limitations bullet or the fixture's own header comment — the table in *Evidence
3*, i.e. that the fixture's 8/10/15/29 are the four C-false checks and which
documented deviation each one pins, that the 35 C-true marks were obtained
only after rewriting lines 12 and 14 and adding `-fgnu89-inline`, and that the
union-size agreement is LP64 luck. Costs: the file stays un-markable, so a future
FAIL is still unattributed — the derivation shortens the diagnosis instead of
replacing it; and PLAN.md is where this lives, and PLAN.md is already the file
most at risk of being read as intent rather than evidence. Adding the `inline`
semantics line to `## C subset limitations` is the part of (b) with the least
cost and the most independent value.

(a) and (b) are not alternatives for the four checks: whichever file layout wins,
someone has to write down 8/10/15/29 and why. The choice is only about whether
the remaining 33 checks get a mechanical per-check comparison.

## Reproduction traps

1. **Three rounds, not one.** `-Wall -c` shows only the `auto` error; the
   `register` error appears only after `auto` is fixed; the `inline` one is a
   LINK error and never appears with `-c`. Verified: with both storage lines
   present, gcc reports the `auto` one and no other error at all.
2. **`-fgnu89-inline` is required, and it changes the standard being measured.**
   Without the flag the build fails at link; with it, gcc's `inline` meaning is
   no longer the meaning of the `-std=gnu99` the rest of the suite is measured
   against (see *Evidence 5*).
3. **Do not trust `/tmp/opencode/m34uc.py` as the driver.** It neutralises only
   `auto int ag;` and does not pass `-fgnu89-inline`, so re-running it now dies
   at `FileNotFoundError: .../ctest/uc_gcc` — gcc refused to produce the binary.
   Its docstring ("a copy with that ONE line changed") is stale for the same
   reason. A working version, with the two rewrites and the flag, is
   `/tmp/opencode/verify/ucmark.py`; beside it `uc_base.c` (the rewritten
   source), `uc_mark.c` (the marked copy, 39 checks), `marks_gcc.txt` (39
   marks), `marks_ours.txt` (0 marks: that half hit *Evidence 6*) and `probe.c`
   (the per-check C reasons). `/tmp` is ephemeral, so the commands in *Evidence*
   are the durable record.
4. **The mark script's backslash rule must be about the last byte, not the
   whole line.** A `tot=tot+` line whose LAST character is a backslash would
   swallow the `printf` inserted below it; a backslash inside a string literal is
   harmless, and this fixture legitimately has `tot=tot+(strlen("a\tb")==3);`
   (check 30) plus other escapes. A script asserting "no backslash on a check
   line" refuses to mark this fixture at all.
5. **Prepending `int printf(const char *,...);` is enough for both engines** —
   the M34 `decls.c` marked copies for gcc and for lite were byte-identical, so
   one marked file can be fed to both; the `strcpy`/`strlen`/`atoi` implicit
   declarations gcc warns about are warnings only, and `-w` silences them.
6. **The lite half needs a bootstrapping tree** (see *Evidence 6*); the gcc half
   does not, so the two halves can be re-run independently.

## Not claimed

- "**Only** fixture without gcc evidence" comes from the M34 entry and the
  parent session's sweep; only parts of it are checkable, and here is what was
  checked. The cross-check habit is recorded at M21 (`PLAN.md:1218-1220`), and
  the milestone log from its start at `PLAN.md:505` through line 1217 contains
  no `gcc` at all — so for the M1-M20 fixtures, `unionc.c` included (added at
  M19, entry `PLAN.md:1059-1115`; "39 checks + closer" at 1107-1111), no gcc
  cross-check is RECORDED. That is weaker than "was never done", and those
  fixtures are small arithmetic programs whose expected values are
  hand-derivable anyway, which is why the gap has teeth only here. Anyone
  wanting the superlative as a fact should sweep the M1-M20 fixture set; this
  file does not assert it.
- Whether the four C-false checks are the ONLY model deviations the fixture pins
  is not claimed either — they are the four whose C verdict flips, which is not
  the same question (see *Evidence 4*: five more agree only by ABI luck).

## Repo notes

`tracking/` is not in this repo's `.gitignore` (`git check-ignore` says nothing),
so once this file is committed a deletion here would be recoverable from history
at the deletion commit's parent — unlike the convention the tracking skill
assumes elsewhere. Per that skill, a suggestion file is never deleted anyway:
whichever direction is taken, this file stays, renamed to `(c)` with status
`Implemented` or `Declined` and its reasoning kept.
