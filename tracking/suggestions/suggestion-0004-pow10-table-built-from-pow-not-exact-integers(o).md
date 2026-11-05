# The kernel's power-of-ten table is built with `Pow(10.,i)`, which is why the host's float printing is not C's

## Status

Proposed

**Severity:** P1, for what the host prints today. This is a wrong *number*,
measured, not a tidiness preference, and it is the mechanism behind a documented
non-standard rather than one more edge case: `T:/KEND.HC:13` fills the kernel's
whole power-of-ten table with `pow10_I64[i+309]=Pow(10.,i)`, a transcendental;
`Pow10I64` (`T:/KMATH.HC:675-683`) only indexes that table and returns `F64`; the
`%f` arm of the format engine (`T:/STRPRINT.HC:555-599`, dispatched from
`case 'f':` at `T:/STRPRINT.HC:524`) scales the **value** by `Pow10I64(precision)`
before it walks any digits; and `Str2F64` (`T:/STRB.HC:527`) finishes every
non-integer decimal literal with `d=i*Pow10I64(-k)` (`T:/STRB.HC:574`) or
`d=i*Pow10I64(j-k)` for the exponent form (`T:/STRB.HC:592`), so decimal float
literals inherit the same table's error. Measured: `%.15f` of `1.5` prints
`1.499999999999998` where glibc prints `1.500000000000000`, `%.6f` of `1e10` prints
`9999999999.999998` where C prints `10000000000.000000`, and the route M37 chose for
lite — the whole conversion through this engine — differs from glibc in **22.74% of a
631,424-cell pricing** (143,569 cells; see *Corrections* for the run that produced that
percentage, for the 934,272-cell run it was travelling with by mistake, and for the
187-of-342-cell direct measurement). **This is filed as a
suggestion about the host, not about `T:/CC/`:** M37 shipped lite delegating
the whole `%f` conversion to the kernel's float printer with no part of the
decimal rendering, so lite's `%f` output IS the host's output and "differs from
glibc" is a documented non-standard of the host rather than a lite defect — the same
shape PLAN already takes for `getchar` ("C byte semantics on a console are
documented deviation, not emulation"). The suggestion is offered to whoever wants
the host itself to be C-compatible; nothing here asks for a change in lite, and lite
does not need one.

It is a suggestion and not a `BUG-NNNN(o).md` for a second reason as well: the
tracking directory's precedent for a host-side conversion defect is
`suggestion-0003`, which files `Str2F64` the same way and was accepted by the owner.
A reader who thinks an inherited-kernel defect that prints a wrong number deserves a
bug number should re-file this one as `BUG-0002(o)` — `tracking/bugs/` is empty and
numbers are never reused, and every measurement below transfers verbatim.

The cost, named so this is not a preference: today the kernel's decimal formatting
and decimal parsing are wrong in the last digit (or in the digit count, or in the
digits entirely) **by construction**. Four conversion paths in the kernel depend on
that one table — the format engine's `%f`, `%e` and `%g` arms and `Str2F64` — plus the
HolyC compiler's own float scanner, plus any `T:/` program that calls `Pow10I64`
through the extern at `KERNELA.HH:5179`; and any program that prints or parses a
decimal float — in HolyC, in a `T:/` C program compiled by lite, or in a DolDoc string
with `A="%6.3f"` — inherits it. `suggestion-0003` already carries the same finding one
layer down, in the parser.

## Finding

`T:/KEND.HC:13`, inside `SysGlblsInit()` (`T:/KEND.HC:2`):

```
pow10_I64=CAlloc(sizeof(F64)*(308+308+1));	// KEND.HC:11
for (i=-308;i<309;i++)				// KEND.HC:12
	pow10_I64[i+309]=Pow(10.,i);		// KEND.HC:13  <-- the finding
```

`pow10_I64` is a global `F64 *` (`T:/KGLBLS.HC:2`). `Pow10I64` computes nothing: it
clamps out-of-range indices (`KMATH.HC:677-680`) and returns `pow10_I64[i+309]`
(`:682`). Its own comment at `KMATH.HC:676` reads `//F64 int powers of ten.` — the
routine is named for integer powers and returns whatever the kernel's transcendental
`Pow` produces, and nothing in `AGENTS.md`, `README.MD` or `T:/Doc/` records which
of those values are exact. `T:/KERNELA.HH:5179` externs it with no doc comment and
`T:/KernelC.HC:506` re-exports it, so every `T:/` program can reach the same
numbers.

**Who consumes it** (all of `T:/`'s callers, grepped, `T:/CC/` excluded):

| caller | lines | what it scales |
|---|---|---|
| `StrPrintJoin`'s `%f` arm | `T:/STRPRINT.HC:564`, `:567`, `:572` (`case 'f':` at 524; the scaling-and-peel block is 555-599) | the whole value, by `Pow10I64(precision)`, before the digit walk |
| its `%e` arm | `T:/STRPRINT.HC:646`, `:668` | the value, and the tie-half `Pow10I64(-dec_len)/2` |
| its `%g` arm | `T:/STRPRINT.HC:719`, `:736`, `:755`, `:773` | the same two shapes |
| `Str2F64` (`T:/STRB.HC:527`) | `:574` `d=i*Pow10I64(-k)`, `:590` `d=i*Pow10I64(-j-k)`, `:592` `d=i*Pow10I64(j-k)` | every decimal literal that is not an integer, and every exponent form |
| the **HolyC compiler's own** float scanner | `T:/LEX.HC:579`, `:598`, `:600` (`cc->cur_f64=i*Pow10I64(...)`) | every float literal in every HolyC program compiled at run time |

So one line of kernel start-up is the scale factor for the kernel's three float
conversions, the kernel's decimal parser, the HolyC compiler's literal scanner, and
anything that calls `Pow10I64` directly.

### Consequence 1 — the host's own float printing

Because the `%f` arm scales the value and *then* rounds (`n=Log10(d)` at
`STRPRINT.HC:558`; `d*=Pow10I64(i)` at 567, `d*=Pow10I64(i-n)` at 564,
`d*=Pow10I64(-n)` at 572; `m=Round(d)` at 576; the digit peel after that, bounded by
`TMP_BUF_LEN-SLOP` at 579 and 593), an error in the
factor becomes an error in the printed digits — at any precision, including ones
where the value itself needs no rounding at all.

Measured directly against `StrPrintJoin`, values handed over as raw quads of bits so
the kernel's own decimal parser cannot move the comparison, oracle glibc's
correctly-rounded round-half-even answer for the same bits (*Reproducing*):
**187 of 342 (value, format) cells disagree with C**, and **91 of those are cells
whose precision is stated and at or below 15** (out of 228 such cells). Per format,
disagreeing/total across the 38 probed values:

```
%f 38/38   %.0f 10/38   %.1f 13/38   %.3f 14/38   %.6f 15/38
%.15f 25/38   %.16f 28/38   %10.3f 14/38   %.20f 30/38
```

| shape | value | the host prints | glibc prints |
|---|---|---|---|
| `%.15f` | 1.5 | `1.499999999999998` | `1.500000000000000` |
| `%.15f` | 1.0 | `0.999999999999998` | `1.000000000000000` |
| `%.15f` | 10.0 | `9.999999999999984` | `10.000000000000000` |
| `%.15f` | 100000.0 | `99999.999999999968000` | `100000.000000000000000` |
| `%.15f` | 1e9 | `999999999.999998880000000` | `1000000000.000000000000000` |
| `%.15f` | 2.5 | `2.499999999999996` | `2.500000000000000` |
| `%.15f` | 2/3 | `0.666666666666666` | `0.666666666666667` |
| `%.6f` | 1e10 | `9999999999.999998` | `10000000000.000000` |
| `%.3f` | 1e14 | `99999999999999.984` | `100000000000000.000` |
| `%.1f` | 1e16 | `9999999999999998.4` | `10000000000000000.0` |
| `%.6f` | 12345678901234567.0 | `12345678901234566.400000` | `12345678901234568.000000` |
| `%.0f` | 1e19 | `10000000000000001600` | `10000000000000000000` |
| `%.0f` | 6.02e23 | `602000000000000000000000.000000` | `601999999999999995805696.000000` |
| `%.20f` | 1e-6 | `0.00000099999999999999` | `0.00000100000000000000` |
| `%.6f` | 1e300 | 120 zeros, then `.000000` | the 301-digit exact expansion, `10000000000000000525047…` |

`1.5`, `1.0`, `10.0`, `100000.0`, `1e9` and `2.5` are exactly representable and their
C renderings need 1–3 significant digits, so there is no rounding decision to get
wrong at any precision ≤ 15: the digits are wrong because the factor is wrong. And
`%.6f` of 1e10 has precision 6, so no rule about precision alone can predict it. At
the largest magnitudes it is not the last digit at all — for 1e300, 1e308 and
DBL_MAX the probed rows render as a run of zeros, i.e. the value's digits are gone;
and 6.02e23 comes back spelled the way the *literal* reads rather than as the double
it becomes. Subnormals and small decimals (5e-324, 1e-308, 1e-10) render correctly
at precisions 0 through 15.

Two behaviours in the same arm are part of the same non-standard and are worth
recording because a reader who delegates to this engine inherits them: `inf` prints
as `i` and `-inf` as `-i` (the arm at `STRPRINT.HC:533-552` is entered by `d==inf`,
and C says `inf`/`-inf`); a NaN is not `nan` but the decimal number
`9223372036854775808`, and it is **scaled like a magnitude**, so `%.6f` of a NaN
gives `9223372036854.775808` and `%.15f` gives `9223.372036854775808`, which is the
cleanest demonstration available that the multiply happens before the digit walk;
and with no precision stated the arm uses **0** (`dec_len<0 → 0`, `:556-557`) where
C's default is 6, so `%f` of 1.5 prints `2`, disagreeing on all 38 probed values.
Under the shipped delegation lite cures that particular shape by writing `.6` into the
directive before handing it over: 23 of those 38 cells then come back C's answer and 15 do
not, and those 15 are the scale factor, not the default.
What the engine does do exactly right, and what a caller therefore keeps for free:
field width, `0`-padding and the half-even tie rule — `%10.3f` of 1.5 measured
`     1.500`, identical to C, and `%.0f` of 1.5 and of 2.5 both print `2`.

Priced over a value set large enough to be a rate: a host python script — `m37design.py`, run
from `/tmp/opencode/`, and **gone: `/tmp` is wiped on reboot and this pricing tool, unlike
`T:/CC/GATES/host.HC`, was never moved into the tree** — re-implemented this arm's arithmetic in
IEEE doubles, `F64 × Pow10I64(p)` with the
table entry modelled as `pow(10.0,p)`, the same transcendental family, then `Round`,
then peel, with the engine's 17-significant-digit truncation, and scored it against
glibc: **143,569 of its 631,424 cells disagree, 22.74%** (see *Corrections* for that run's
cell count, and for the 934k figure this number has been travelling with). The 22.74% is
therefore a recorded number and not a repo-reproducible one. What the tree DOES reproduce is the
direct measurement: `T:/CC/GATES/host.HC` run as gate 4 of `T:/CC/GATES/README.txt` prints the
342 kernel renderings that `T:/CC/GATES/rows.txt` holds, byte for byte, and scoring those rows
against glibc gives the 187-of-342 figure (`m37map.py` did that scoring and is likewise gone, so
the diff itself is a two-minute job against the committed table rather than a script you can run).

### Consequence 2 — every decimal floating literal

`Str2F64` accumulates the digits into an `I64` and then, for anything that is not an
integer literal, multiplies by the same table (`T:/STRB.HC:574`, `:590`, `:592`). So
a decimal float literal is "digits × an inexact factor". Measured bit patterns
(postfix-cast reinterpretation inside the kernel — see *Reproducing*):

| literal | what the kernel's parser produces | correctly rounded | error |
|---|---|---|---|
| `1e15` | `4831355200913801204` (…204) | `…216` | **12 ULP low** |
| `1e10` | — | — | 23 ULP low |
| `0.25` | `4598175219545276417` (…417) | `…416` | **1 ULP high** |
| `1.5`, `10.0`, `0.1` | — | — | exact |

The cause was pinned and not inferred: `Pow10I64(15)` measured on the host has the
**identical** wrong pattern `…204`, and `1e15` is parsed as `1 * Pow10I64(15)` — the
literal's error *is* the table's error. The `0.25` row is the same fact from the
other end: `0.25` is exactly representable, but it is computed as 25 × the table's
`0.01`, and `0.01` is not representable, so a value that needs no work at all lands
one ULP high. `1.5`, `10.0` and `0.1` land exactly because their digits need no
inexact scale.

Two calibrations on that table, so nobody reads it as "every decimal literal is
wrong". `suggestion-0003` measured a wider plain-decimal set (`1.5`, `2.5`, `.5`, `2.`,
`0.1`, `100`, `1000.0`, `123456.789`) and every one of those came back exact, so
"inherits the path" is not "always differs": the drift is the table entry's own error
plus the single rounding of the multiply, and it shows only when the two together
exceed half an ulp at the target magnitude. That also says what each row measures —
`1e10` parses as `1 * Pow10I64(10)` and a multiply by 1 is exact, so its 23-ULP gap IS
the table entry's error, while `0.25`'s factor is within a rounding of the true 0.01
and only the extra multiply pushes the product one ulp up. Both are caused by the table
being built with `Pow`; the split between "the entry is off" and "one extra rounding" is
an inference from these measurements plus the source, not a separate probe, and the
direct probes are `Pow10I64(15)`'s bits and the four literal patterns.

Practical effect: `x * 1e15` and `x * <the exact integer-derived 10^15>` give
different answers — measured, `1.5 * 1e15` is `1499999999999998` with the literal
factor and `1500000000000000` with an integer-derived one — so anything doing
decimal formatting or decimal scaling is off by one in the last digit some of the
time. And because the HolyC compiler's own scanner uses the same three expressions
(`T:/LEX.HC:579`, `:598`, `:600`), this is not a C-only concern: HolyC float literals
drift the same way, which is why `0.1+0.2==0.3` is TRUE in HolyC and FALSE in C
(`suggestion-0003` records that consequence).

### The 18-digit rule the host documents, with no error path

`T:/STRB.HC:529-534` documents, of `Str2F64`: "Does not allow more than 18-digits
before or after the decimal point because the numbers before and after the decimal
point are stored in 64-bits. Use exponentiated forms to avoid this."
`T:/STRPRINT.HC:210-217` says the same about float formatting. Neither has an error
path when the rule is broken: the accumulator is an `I64` and the overflow wraps.
Measured consequence: a 19-digit float literal is not merely imprecise, it is a
**different number**, and a comparison against one is a different truth value —
`a >= 9223372036854775808.0` answered **TRUE for `a = 0.0`**, because the literal on
the right is not 2^63. (The wrapped digit sum itself is `suggestion-0003`'s item 2;
this file records the rule being stated with no diagnostic, and the same table
sitting on both sides of it.)

## What lite does with this, and why it is not a lite defect

M37 **shipped** as lite delegating the whole `%f` conversion to the kernel's float printer.
`CCXFmt` (`T:/CC/CC_LIB.HC`, whose section comment reads "M37: printf's %f, delegated WHOLE")
takes a destination, the value's bit pattern and **the directive as TEXT**, and calls
`StrPrintJoin(dst, dir, 1, &v)`; the runtime `sprintf` engine's `cc_sprintf` and `CCPrintf`'s
float arm both build that text, writing `.6` in when the source stated no precision (precisely
because the host's engine defaults to 0) and scanning `*` without copying it. What lite does own
is the two NUMBERS inside that text: `CCIClap` rewrites a width run above **1000** down to 1000
and a precision run above **600** down to 600, keeping a leading `0` when a run begins with one
so the kernel's pad flag survives, because `StrPrintJoin` takes no buffer length and printf's
destination is `cc->fmtbuf[1024]`. So the caps are lite's and the rendering is the host's: a
`.600f` asks for 600 fraction digits and the engine prints what its own scratch holds — measured
at 250 characters — while a `%01500.3f` asks for a field lite has bounded at 1000. Lite owns no
decimal rendering: no
digit walk of its own, no scale factor of its own, no rounding rule of its own
beyond what the host's `Round()` already does, and no magnitude special case.
Consequences follow automatically, and they are the host's, recorded as such:
lite's `%.15f` of 1.5 prints `1.499999999999998`; lite's `%f` of `inf` prints `i`;
lite's `%.15f` of a NaN prints a scaled decimal number; lite's `%.15f` of 1e19 prints
`10000000000000001600`. That is the same posture PLAN already takes on `getchar` —
"C byte semantics on a console are documented deviation, not emulation" — and it is
what the delegation rule in `T:/CC/PLAN.md`'s `## Correctness policy` is for.

One consequence of the switch deserves naming, because it moved who hits the host's 18-digit
rule. That rule is documented twice, for the two directions of a decimal float: `Str2F64`'s
"Does not allow more than 18-digits before or after the decimal point … Use exponentiated forms
to avoid this" (`T:/STRB.HC:529-534`) and the format engine's "In float formatting, do not exceed
18-digits before or after the decimal point because the numbers before and after the decimal
point are stored in 64-bits" (`T:/STRPRINT.HC:210-215`, enforced by the
`k < TMP_BUF_LEN-SLOP` bounds at `:205-206` and the walks that use it). Under the abandoned
helper design the third instance — lite's own multiply by a power of ten — was lite's problem and
lite could refuse it. Under the shipped design there is no lite-side multiply, so the rule bites
in the two places where the host owns it: on the way **in**, on every decimal float literal
`Str2F64` parses (which is `suggestion-0003`'s finding, and lite's lexer re-spells its own
literals precisely to stay out of it), and on the way **out**, on whatever directive lite hands
over. The `CCIClap` cap is a buffer bound and not a repair: it decides how many digits to ASK
for, and the engine still prints at most what its 64-bit halves and its scratch can hold.

Two things this filing measured are worth keeping visible during the change, because
they are the reason the delegation is defensible and the reason the fix belongs on
the host side:

- The engine does the parts a C dtoa would otherwise have to be written for —
  integer→decimal walk over the signed quad, field width, `0`-padding, and
  round-half-to-even at a tie (`Round(1.5)=2`, `Round(2.5)=2`, `Round(3.5)=4`,
  `Round(-2.5)=-2`, measured) — and those are exactly the parts this file's own
  measurements show it gets *right*. What it gets wrong is the scale factor, which
  is what this file is about.
- The alternative was measured, and the delegation won on the rule rather than on the
  number. Earlier in the same session a fraction-only scaling design was built and
  measured — scale the fraction by a factor from exact integer quads, cap precision at
  15 (10^16 is past 2^53), and produce the digits of magnitudes ≥ 2^63 by doubling the
  significand's digits — and it scored 0.16% disagreement against glibc where the
  whole-conversion route scored 22.74%. That design has now been **abandoned**, so the
  exact-quad table, the cap and the doubling route are not lite's any more: they are the
  measurement that priced this decision, recorded so the next reader knows what was
  traded away. **What this filing predicted would have to happen next has happened, and the
  sentence that asked for it is now false and is reversed here:** `T:/CC/CC_LIB.HC`'s M37
  section comment reads "M37: printf's %f, delegated WHOLE"; `T:/CC/PLAN.md`'s M37 heading
  reads "delegated WHOLE: lite rebuilds the directive as text, hands it to the kernel's float
  printer, and emits what comes back. Every divergence from C that follows is a documented
  non-standard, not a gap to route around"; the `## Correctness policy` paragraph that used to
  say "the radix walk, the left padding and the rounding are the host's, and the scaling is the
  helper's" now states the WHOLE delegation and puts the by-parts design in the past tense with
  its 0.16% against the 22.74% it replaced; and `## C subset limitations` describes the shipped
  accept set rather than the priced one. The same-change rule this file invoked was satisfied by
  the change that shipped the code, so this bullet is a record of a correction that landed and
  not an open request.
- The other seam closed the same way. `T:/CC/TEST/ffmt.c` no longer pins C's spellings and no
  longer diffs a float line against gcc's stdout: it is regenerated to pin the **kernel's** 63
  renderings — its header: "Every rendering pinned below is what `T:/STRPRINT.HC`'s `case 'f':`
  writes for these bit patterns — measured in TempleOS, not derived from what C would print …
  Where the kernel and C disagree, the comment says so and the kernel wins here, because lite
  hands the kernel the directive and emits the answer" — and `T:/CC/CC_TEST.HC`'s `CCProbeFloat`
  pins the same bit patterns from the kernel side, with the directives lite actually sends
  (`.6f` for `%f`, `%+.6f` for `%+f`, `%01000.3f` for a source that wrote `%01500.3f`). Both
  come from `T:/CC/GATES/rows.txt`, which is committed, and `T:/CC/GATES/genffmt.c` regenerates
  either body from it — `./genffmt` for the fixture, `./genffmt probe` for `CCProbeFloat`, with a
  second argument overriding the table path. The two directions now assert one standard, and the
  divergence from C is documented rather than tested for.
- **The deviation, counted from the C side, and why two different rates are both right.**
  Compiling the sweep's own programs with gcc instead of lite puts **328 of its 684 `sprintf`
  cells** on the non-C side — 164 left-hand and 164 right-hand failures, i.e. **164 distinct
  (value, directive) cells out of 342, 48.0%** — while the direct probe scores **187 of the same
  342 cells, 54.7%**, against glibc. The 23-cell difference is exactly the bare-`%f` column, and
  it is this filing's own mechanism showing up in a cell count: the probe compares the engine's
  precision-0 rendering against glibc's precision-6 rendering, because the engine HAS no default
  precision, while the sweep compares like with like after lite writes `.6` into the directive.
  Of that column's 38 probe-bad cells, the 15 that stay bad in the sweep are the same 15 the
  probe measured bad at `%.6f`; every sweep-bad cell is probe-bad, and no cell that was good at
  the probe went bad in the sweep, so the containment is one-way. The point for this filing is
  that the deviation is the host's in both counts — compiling these programs with gcc measures
  the host and not lite, which is why `T:/CC/GATES/README.txt` calls gcc "the wrong oracle" here.

## Already decided?

Checked, and the split is clean:

- **Decided, and this file does not reopen it:** that lite delegates the whole `%f`
  conversion to the host's printer and does not own decimal rendering. Under that
  ruling the host's spelling is lite's spelling, by design, exactly as the host's
  keyboard-event `getchar` is lite's `getchar`. Nothing here proposes a lite-side
  workaround, and nothing here says the current code is wrong.
- **Decided elsewhere and not reopened:** `Str2F64`'s own two parse defects — the
  dropped exponent `+` and the unchecked digit accumulator — are
  `suggestion-0003`'s items 1 and 2.
- **NOT decided anywhere in the tree:** that the host's float table should be what
  its name says. `AGENTS.md`, `README.MD` and `T:/Doc/` say nothing about
  `pow10_I64`, `Pow10I64` or the exactness of a power of ten; `KMATH.HC:676` asserts
  the property this file measures as absent; and the only tree records of the
  inexactness are `T:/CC/`'s own — PLAN's `## Correctness policy` paragraph and its M37 entry,
  which cite this table as the reason the host's rendering is a documented non-standard rather
  than as the reason for a lite-side split, and `T:/CC/CC_LIB.HC`'s M37 comment, which now reads
  "delegated WHOLE". Those records were written for the by-parts design and rewritten in the same
  change that shipped the delegation, so the host has still never taken a position on its own
  table, and this file is the first record of it as a host question.

## Proposal

Build the table exactly, for the range that can be exact — which is the whole range
the engine's own 18-digit rules can ask for. The suggested fix is to fill
`pow10_I64` from exact integer quads instead of calling `Pow`, so that `Pow10I64`
returns a value produced by `d=i` assignment (`i` an integer) rather than by a
transcendental. Three verified facts shape it:

1. **10^k is exactly representable as a binary64 for every k ≤ 22, and not for
   k = 23** (checked against exact integer arithmetic: `float(10**k) == 10**k`).
   So an exact table is possible, and it covers everything `STRPRINT.HC`'s
   17-significant-digit truncation and `Str2F64`'s 18-digit rule can reach.
2. **A quad holds 10^k only to 10^18 signed (10^19 unsigned).** 10^19..10^22 fit the
   double but no integer type, and they do not need one: an IEEE multiply of the
   exact double 10.0 rounds once to a representable result, so the chain
   `d=1.0; for (k=1;k<=22;k++) {pow10_I64[k+309]=d; d*=10.0;}` is exact at every
   step — no constants, no `Pow`, no big-integer arithmetic. The integer-quad
   variant reaches the same 10^18/10^19 band and then needs the same multiply to get
   to 22. Above 10^22 keep `Pow`: those values cannot be exact at all.
3. **Negative exponents need the division, not the reciprocal.**
   `d=i*Pow10I64(-k)` should become `d=i/Pow10I64(k)`: the divisor is exact by step
   1 or 2, and the division rounds once, so the result is the correctly rounded value
   of the true quotient. That is the change that fixes the `0.25` row and the
   post-point literals, and it is three call sites in `T:/STRB.HC` (`:574`, `:590`,
   `:592`) plus the three in the HolyC scanner (`T:/LEX.HC:579`, `:598`, `:600`) if
   HolyC literals are to move with it.

Named costs, because a suggestion that cannot name them is a preference:

- It is not read-at-runtime work: `KEND.HC` is in `HCRT_TOS.HC`'s include set, so it
  needs the `Cmp("FULL_PACKAGE.HC",…)` rebuild and the `T/HCRT.BIN` → `./HCRT.BIN`
  copy (`AGENTS.md`), and the host recompile is not needed but the image is.
- It is a **behaviour change in a live kernel, and now also a change to lite's
  output**, because lite delegates this conversion: every number the kernel prints
  would gain a different last digit, every HolyC float literal would parse to
  different bits, and lite's `%f` would change with them. `0.1+0.2==0.3` (TRUE in
  HolyC today, FALSE in C, per `suggestion-0003`) would move toward C — the right
  direction, but still a change something under `T/` could depend on, including
  third-party content in `T/Community-Heiresis/` and any saved DolDoc with a rendered
  number in it.
- It fixes the host's factor and not the host's structure. After it, `m=Round(d)` on
  a scaled whole value still requires the scaled product to stay
  integral-representable below 2^53 for the peel to mean what it assumes, and that
  second limit is independent: the pricing harness measured a whole-value route still
  disagreeing with C on values whose scaled product passes 2^53 even when the factor
  is exact. So "build the table exactly" would remove the ULP-level drift (and with
  it, most of the 22.74%) without by itself making the engine's rendering C-exact at
  every precision and magnitude; `%.16f` and above, and the ≥ 19-digit magnitudes,
  are the engine's truncation rules rather than its table.
- Touching `Str2F64`'s exponent path changes literal values kernel-wide, so it
  deserves its own measurement pass first — the 409-literal truncation run
  `suggestion-0003` records is the obvious one, and lite's suite is the other one
  (it compiles and runs `T:/`'s C content, so a literal change shows up there).

## Corrections (to the two numbers this filing was handed)

The finding is intact; two figures in the handover this file was written from do not
survive checking against the harness output they came from. Per the tracking rule a
draft that turns out wrong is corrected in the report rather than dropped, and
neither correction changes the conclusion.

1. **The 22.74% is right; the denominator travelling with it is from another run.**
   The percentage is 143,569 cells out of **631,424** (39,464 values × 16
   precisions) — `m37design.py`, re-run for this filing, printed `A (delegate whole)
   disagrees with C in 143569 cells (22.74%)`. 934,272 is `m37price.py`'s cell count
   (58,392 values × 16 precisions), the run that priced the fraction-scaling design
   (0.1733% there; its `:97` printed the design-A figure as a hard-coded string, which
   is how the two got joined). **PLAN now carries the corrected pairing, so this half of
   the correction is applied and closed:** as of the pass that closed `suggestion-0002`,
   PLAN's `## Correctness policy` paragraph prints "143,569 of 631,424 cells, 22.74%", its
   M6c addendum pairs the percentage with the direct probe's 187-of-342 (and 91 of them at
   stated precisions ≤ 15), and the only 934,272 left in the file is in the pricing paragraph,
   where it is the abandoned model's own denominator. Order of magnitude and direction are
   unaffected. **Durability, stated as a limitation and not as a footnote:** both scripts were
   host python under `/tmp/opencode/`, the host `/tmp` is wiped on reboot, and neither was moved
   into the tree — so neither rate can be re-derived from the repo, and the `:97` hard-coding
   cited above as the mechanism of the slip is a fact about a file that no longer exists. What IS
   reproducible is the direct measurement: `T:/CC/GATES/host.HC` re-runs to `T:/CC/GATES/rows.txt`
   byte for byte, per gate 4 of `T:/CC/GATES/README.txt`.
2. **"`%.20f` of 1e18 prints `100000000000000000.0000000000000000`" does not
   reproduce.** The measured host row for 1e18 at `%.20f` is
   `1000000000000000000.00000000000000000000`, which is what C prints, and 1e18 at
   `%.15f` matches too. The quoted string is what the priced **model** produces for
   1e18 at precision 15: the model drops one integer digit into the fraction field,
   which the engine's own zero-padding bookkeeping does not. So for magnitudes at and
   above 1e17 the priced model over-predicts failure and the direct probe is the
   authority — while the probe's own large-magnitude failures are real and larger in
   kind (`%.0f` of 1e19 → `10000000000000001600`, `%.6f` of 1e300 → a run of zeros).
   This file quotes the probe for what the host does and the model only for the rate,
   and says which is which at each use.

## Not claimed

- That the host's `%f` is wrong **only** where this file measured it. The 38 values
  are a chosen discriminating set (powers of ten, half-way ties, the special values,
  subnormal 5e-324, 1e300, 1e308, DBL_MAX), not a sweep, and the 22.74% is a modelled
  rate rather than a second measurement. What IS claimed is that it is wrong at
  precisions and magnitudes that a "keep the precision at or below 15" rule calls
  safe, on values that need no rounding decision at all.
- That fixing the table makes the engine's rendering C-exact, or makes `Str2F64`
  correctly rounded. The 2^53 integral-representability limit, the engine's own
  digit-count truncation rules, the digit accumulator's overflow (`suggestion-0003`
  item 2) and the dropped exponent `+` (item 1) are separate mechanisms.
- That `%e` and `%g` are wrong in these specific shapes. Their arms call `Pow10I64`
  at `T:/STRPRINT.HC:646`, `:668`, `:719`, `:736`, `:755`, `:773`, which is why they
  are in the caller inventory; only `%f` was probed. `suggestion-0002` already
  records that the kernel's `%e`/`%g` are not C's for reasons unrelated to this table
  (default precision 8 rather than 6, exponent spelled `e0`/`e-1` rather than
  `e+00`/`e-01`, `%g` behaving like `%e`, `%E` and `%G` printing nothing) — which is
  why those conversions are refused by printf rather than delegated; the refusals are measured
  (`T:/CC/GATES/r01.c`, `r02.c` and `r12.c` among the 15 that `rfdriver.HC` runs), and `%E`/`%G`
  follow from the same letter test in the code.
- That `Atoa`, `F2Str` or the DolDoc `%f` formatter share this table: they were not
  probed. The grepped caller list is for `Pow10I64` only and excludes `T:/CC/`.
- That lite's current `%f` output is wrong. As shipped it is the host's output, which is the
  point of the delegation; the cells this file lists as
  disagreements with C are, for lite, the host's documented spellings.
- That the fix is wanted. It is offered: the host is inherited code, the fix changes
  output kernel-wide, and there is a real argument for leaving the kernel's float
  printing exactly as it is and letting every consumer document the deviation —
  including lite, which now does. Weigh it against the cost table above.

## Reproducing

Host measurements start from the 342-cell probe, whose driver is now committed:
`T:/CC/GATES/host.HC` formats 38 values as raw quads of bits × 9 formats — `%f`,
`%.0f`, `%.1f`, `%.3f`, `%.6f`, `%.15f`, `%.16f`, `%10.3f`, `%.20f` — each into a buffer by
`StrPrintJoin` and prints it bracketed, and gate 4 of `T:/CC/GATES/README.txt` gives the command
(`timeout 300 stdbuf -oL ./3d_loader -t T -c T:/CC/GATES/host.HC` **from the
repo root**); the rows it prints are `T:/CC/GATES/rows.txt`, byte for byte, verified on this
tree. Two steps of the original chain were host tools under `/tmp/opencode/` and are **gone with
`/tmp`**: the generator that emitted the driver (`m37fprobe.py`) and the scorer (`m37map.py`)
that turned the rows into the exactness grid. The counts and per-cell tables in *Consequence 1*
(187/342, 91/228, the per-format lines, the value rows, and the `%10.3f`-of-1.5
agreement) were re-derived for this filing by scoring `rows.txt` against `'%.*f'` in
python, which is glibc's correctly-rounded answer for the same bits; because `rows.txt` is now
committed, anyone with a C library can re-derive them, and the filing did not need the scorer to
survive for the numbers to be checkable. **No loader run was made for this filing** — a gate
battery was using `3d_loader` — so every host number here is either a re-scoring of that saved
output or a citation of one.

- The literal-vs-integer-quad bit comparisons (`Pow10I64(15)` itself, `1e15` …204 against
  …216, `0.25` …417 against …416, and `1.5 * 1e15` against `1.5 *
  (F64)1000000000000000`) came from `m37bits.HC` and `m37scale.HC`, host scratch under
  `/tmp/opencode/`: **gone, and not moved into the tree**, so the bit patterns they printed are
  recorded in *Consequence 2* without their driver.
- `m37design.py` priced the whole-value route (pure python, no loader); `m37price.py`,
  `m37round.py`, `m37why.py` and `m37zone.py` priced the fraction-scaling design and its
  rounding rule on the same sweep shape, and `m37refuse.py` was the per-case accept/refuse
  witness for the format spellings. All were host tools under `/tmp/opencode/` and none was
  moved into the tree, **so no priced rate in this file is re-derivable from the repo.** The
  accept/refuse witness does have an in-tree successor: `T:/CC/GATES/mkrefuse.c` regenerates
  `r01.c`..`r15.c`, `acc.c` and `rfdriver.HC`, and gate 3 of the README runs them.
- The in-tree fixture is `T:/CC/TEST/ffmt.c`, and it is no longer the caveat this bullet used to
  attach: it is regenerated to pin the **kernel's** renderings, so its expectations match what
  the delegation asserts. What survives of the caveat is its first half — the header's rule that
  no decimal float literal is a test value, because the literals drift for exactly the reason
  this file files — and that rule is `suggestion-0003`'s finding, not this file's.
- `/tmp` is not durable, and the tree now knows it: the gate battery lives at `T:/CC/GATES/`,
  with `README.txt` documenting how to run each gate and how to regenerate each program. What
  that move does NOT cover is the python pricing layer described above, and no record here should
  imply that those numbers came with it.

## Repo notes

Filed on `bugfix/build-isssues` in the primary checkout `/mnt/d/Source/3Days` (HEAD
`6b1b04c`, M36; M37 implemented in the working tree and uncommitted, and being
rewritten under the delegation ruling as this file is written). Named
`suggestion-0004-pow10-table-built-from-pow-not-exact-integers(o).md`, status
`Proposed`: the host-side question is open and undecided, so the marker is `(o)`.
`tracking/` is **not** in this repo's `.gitignore` (`git check-ignore tracking/`
prints nothing, rc=1), so once committed a deletion here would be recoverable from
history at the deletion commit's parent — but per the lifecycle a suggestion file is
never deleted regardless, and this one is the only record of a host-side cause that
two other reports point at.

Cross-links, both directions: `tracking/suggestions/suggestion-0002-...printf-number-conversions-
never-mapped-onto-kernel-printing(c).md` (the delegation mapping probe, now closed as
`Implemented` with `%f` shipped whole; its recorded "state the precision and keep it at or below
15" rule is falsified by this table, and its float-shape table, wider sweep and three recorded
answers are where several of these cells were first seen), and
`tracking/suggestions/suggestion-0003-...` (`Str2F64`: its items 1 and 2 are the
parser's own defects, its item 3 is the drift this file explains, and its proposal
item 3 — a table of exact powers of ten — is this file's proposal). Tree side, cited
by name because PLAN's line numbers move as M37 is rewritten: `T:/CC/PLAN.md`'s
`## Correctness policy` delegation paragraph and its M36/M37 milestone entries,
`T:/CC/CC_LIB.HC`'s M37 comment, `T:/CC/TEST/ffmt.c`'s header and the gate battery at
`T:/CC/GATES/` — each of which records this table, and each of which was rewritten in the
M37 change to describe the delegation as shipped rather than the fraction-scaling design that
earlier pass had priced (this filing once said the opposite; that sentence is corrected in
*What lite does with this*).
