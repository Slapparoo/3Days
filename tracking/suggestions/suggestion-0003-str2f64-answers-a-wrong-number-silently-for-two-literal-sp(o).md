# Str2F64 answers a wrong number, silently, for two literal spellings

## Status

Proposed

**Severity:** P1 for what it returns. Both defects produce a *value* — not a
refusal, not a diagnostic — that is far from what the text says, and the second
one reports that it read the whole input. They are cheap for the kernel to fix and
`T:/CC/CC_LEX.HC` already works around both, so the cost today is duplicated
knowledge in a compiler that is supposed to be an adapter layer.

## Finding

`extern F64 Str2F64(U8 *src,U8 **_end_ptr=NULL);` (`T:/KERNELA.HH:4786`) is the
kernel's only decimal->binary entry point. Measured on it directly, and again
through compiled C in `T:/CC/`:

**1. It stops at an exponent's `+` and returns the mantissa.**

| text | `Str2F64` returns | end pointer says | C says |
|---|---|---|---|
| `1e+2` | `1` | stopped after 2 chars | 100 |
| `1E+2` | `1` | stopped after 2 chars | 100 |
| `1e+0002` | `1` | stopped after 2 chars | 100 |
| `1.5e+2` | `1.5` | stopped after 3 chars | 150 |
| `1e2` | `99.999999999999972` | consumed all 3 | 100 |
| `1e02` | `99.999999999999972` | consumed all 4 | 100 |

So the exponent digits after a `+` are not mis-rounded, they are **ignored**, and
`1e+2` is off by two orders of magnitude. The same text without the `+` parses. The
`+` carries nothing a decimal exponent needs, which is why a caller can paper over
it — but every caller has to know to.

**2. The mantissa digits are summed into a signed 64-bit integer with no check,
and the end pointer still reports success.**

| text | `Str2F64` returns | C says | end pointer |
|---|---|---|---|
| `123456789012345678` | `1.2345678901234568e+17` | same | consumed all |
| `1234567890123456789` | `1.2345678901234568e+18` | same | consumed all |
| `12345678901234567890` | `-6.1010651724749834e+18` | `1.2345678901234567e+19` | consumed all |
| `9999999999999999999` | `-8.4467440737095516e+18` | `1e+19` | consumed all |
| `9223372036854775807` | `9.2233720368547758e+18` | same | consumed all |
| `9223372036854775808` | `-9.2233720368547758e+18` | `9.2233720368547758e+18` | consumed all |
| `0.12345678901234567890` | `-0.061010651724750349` | `0.12345678901234568` | consumed all |
| `1.23456789012345678901e0` | `-0.056704195036212296` | `1.2345678901234567` | consumed all |

The boundary is `0x7FFFFFFFFFFFFFFF` exactly, and the returned value is the digit sum
reduced modulo 2^64 and read as a signed integer: `12345678901234567890` minus 2^64
is `-6101065172474983726`, whose double is the `-6.1010651724749834e18` in the table,
and `9223372036854775808` minus 2^64 is `-9223372036854775808`, which is what came
back for that one too; the same holds for `9999999999999999999`,
`12345678901234561111` and `123456789012345611111`. So the answer is not merely wrong
but wrong by a known multiple of 2^64: the defect is a missing overflow check, not an
approximate conversion. `0x7FFFFFFFFFFFFFFF` itself is exact; one more than it is
negative. The digit sum therefore needs the test the host's own
integer-string converters make and this one does not, and because the parse *did*
consume the text, a caller that trusts the end pointer has no way to see it.

**3. For context, not as a defect claim: the rounding is not correct either.**
Errors in ULPs of the value a correctly-rounded parser returns — the spacing of
doubles there, which is why they are whole numbers: `1e2` -2, `1e9` -12, `1e10` -23,
`1e18` -22, `3.e5` -11, `1e-5` +12, `0.15` +1, `3.141592653589793` +9, `1e308` -160,
`1e300` -457, `5e-324` -1 (it underflows that one to zero), while `1e-400` -> 0 and
`1e400` -> inf match C. Plain decimal spellings came back exact across
the same set (`1.5`, `2.5`, `.5`, `2.`, `0.1`, `100`, `1000.0`, `123456.789`).
Kernel float literals drift the same way (the kernel's own `0.3` is one ULP high,
so `0.1+0.2==0.3` is TRUE in HolyC and FALSE in C), so this third item is the
kernel's existing behaviour being what it is; items 1 and 2 are different in kind,
because they are not rounding. **[Cause named since filing, for the drift in this
item: the exponent is applied by multiplying by `Pow10I64`, and `T:/KEND.HC:13`
fills that table with `Pow(10.,i)` — `T:/STRB.HC:574`, `:590` and `:592` are the
three sites — so most of this table's ULP counts are the table's error rather than a
rounding choice (`1e10` parses as `1 * Pow10I64(10)` and a multiply by 1 is exact, so
this file's −23 ULP row for that literal is the table entry's own error — a derivation
from this file's measurement plus the source, not a separate probe of `Pow10I64(10)`;
M37 did probe `Pow10I64(15)` itself and it has exactly the pattern its literal
produces), and the same table is what the kernel's `%f` scales by. Filed as
`tracking/suggestions/suggestion-0004-pow10-table-built-from-pow-not-exact-integers(o).md`,
whose proposal item 1 is this file's proposal item 3 (an exact power-of-ten table)
and which lists the routine's other consumers — the format engine's `%f`/`%e`/`%g`
arms and the HolyC compiler's own literal scanner at `T:/LEX.HC:579`, `:598`,
`:600`, which is why the drift is not C-only. One reading of the table above needs a
caveat: these ULP counts are the HOST's parse of each spelling, measured through `Str2F64`
itself. A lite-compiled program does not see most of them — `CCNumFloat` re-spells a literal
whose positive exponent fits (`T:/CC/CC_LEX.HC:381-397` turns `1e10` into the digit string
`10000000000` and `1e15` into `1000000000000000`), and the host sums a digit string exactly, so
those two literals are the exact double in a `T:/CC/` program and drifted in a HolyC probe.
Items 1 and 2 of this file are
unaffected by that cause and remain unfixed in the host.]**

## Why it matters to `T:/CC/`

`CCNumFloat` delegates the whole conversion — lite owns no decimal->binary
algorithm — and a delegation that can be handed a wrong number without saying so is
a guess. So the lexer now:

- drops an exponent `+` from the text it hands over (`T:/CC/CC_LEX.HC`,
  `CCNumFloat`'s copy loop and its normalized path), because `1e+2` and `1e2` are
  the same C value;
- sums the significant digits itself, with the overflow test (`CCDigOvf`), and does
  not hand over a digit string the host would wrap: the digits past what fits are
  dropped and the exponent absorbs them, so `12345678901234567890` goes as
  `1.234567890123456789e19`. The truncation is measured lossless — 409 such literals
  (a host python script, `m35trunc.py`, built 400 random 20-to-26-digit integers plus a fractional
  tail, leading zeros after the point, and values that round up to the next power of
  ten; **the script ran under `/tmp/opencode/` and is gone with the host `/tmp`, so this
  409-literal claim is recorded and not re-runnable from the tree**) landed on the same double as
  the written spelling every time, against the
  host's own answer being off by 2^64. The boundary the test reproduces was verified
  against `9223372036854775807` (exact) and `9223372036854775808` (wrapped);
- compares `Str2F64`'s own end pointer with the text handed over, and reports rather
  than trusting when it stops short. That is the only way a float literal is refused
  at all now (`the kernel's decimal conversion did not read this literal whole`):
  every other spelling defect is repaired by re-spelling instead;
- folds a POSITIVE exponent into the digits when the result fits, because the
  accumulate-then-convert path is exact and the multiply-by-a-power-of-ten path is
  not (`1e2` -> `100`, `1e10` -> `10000000000`, `100e-2` -> `1`).

Items 1 and 2 are why that last point exists at all: before it, `1e10 == 10000000000`
was false in lite and true in C for a literal with an exact double value.

## Proposal

In `Str2F64` (and whatever `Atoa`-family routine shares its scanner):

1. accept `+` after `e`/`E` the way `-` is accepted today;
2. make the digit accumulator saturate or set the end pointer short when it passes
   `0x7FFFFFFFFFFFFFFF`, so a caller can tell that the digits were too many —
   either answer is fine, silence is not;
3. if the exponent is applied by repeated multiplication, a table of exact powers
   of ten up to 1e19 (and division for negative exponents) would remove most of
   the ULP drift in item 3 for free, since the multiply-then-round sequence is what
   produces it.

None of these is a new mechanism; 1 and 2 are the ordinary shape of a decimal
scanner, and 3 is a table.

## Not claimed

- That lite is right and the kernel is wrong about the *rounding* of any particular
  literal: item 3 is recorded as a divergence lite accepts, and `T:/CC/PLAN.md`
  M35 lists the numbers rather than arguing they are defects.
- That the workarounds in `CCNumFloat` are complete: a spelling the host reads
  partly is caught by the end-pointer check, but a host that reads a text wholly
  and still wraps its accumulator in some path not probed here would still pass.
  The digit test covers the mantissa; it does not attempt to predict the exponent
  path's accuracy.
- That `Atoa`, `F2Str` or the DolDoc `%f` formatter share this scanner or these
  defects: only `Str2F64` was probed. `%f`'s own mapping onto C's conversions is a
  separate record, now closed: `tracking/suggestions/suggestion-0002-printf-number-conversions-
  never-mapped-onto-kernel-printing(c).md`, `Implemented` — `%f` shipped as whole-value delegation,
  so the mapping question this pointer used to leave open is answered there, flag by flag, with
  the acceptance and refusal batteries in `T:/CC/GATES/` as its evidence.
- That the fix belongs in the kernel rather than in every caller: the proposal
  assumes the kernel is the cheaper place, because there is one kernel and several
  callers, and today `T:/CC/` carries the whole workaround.

## Reproducing

The measurements were host scratch under `/tmp/opencode/` and are **gone with it**:
`m35end.HC` (host-side: the value, the end pointer and the text length per line) and
`m35cliff2.HC` (the `0x7FFFFFFFFFFFFFFF` boundary) are the HolyC probes, and `m35lex.py` is what
ran the same spellings through compiled C against `gcc -std=gnu99`, one program per process,
separating cases by whether the host's parser had measured exact for that spelling. None of them
was moved into `T:/CC/GATES/`, which carries the M37 float-printing battery and not this parser
probe, so the tables in *Finding* are a record and not a gate.

What IS checkable from the tree by reading it, and is the part to check before re-running
anything: `T:/STRB.HC:580` accepts only `'-'` after an `e`/`E`, so a `'+'` is not consumed as a
sign and falls to the digit loop at `:585-587`, which takes the exponent as 0 and computes
`d=i*Pow10I64(j-k)` at `:592` — item 1's `1e+2` answering 1.0; `:567-571` accumulates the digits
into `i` (`i=i*10+ch-'0'`) with no saturation test and no short end pointer — item 2; and
`T:/STRB.HC:574`, `:590`, `:592` apply the exponent through `Pow10I64` — item 3's mechanism, whose
cause `suggestion-0004` files. lite's workarounds are `CCDigOvf` (`T:/CC/CC_LEX.HC:164`), the
`wrote`/`plain` tests at `:366-397` and the end-pointer refusal at `:468-476`
(`the kernel's decimal conversion did not read this literal whole`). Re-running the host side
takes one HolyC file of the reader's own; the numbers above are what this filing's file printed.
