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
because they are not rounding.

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
  (`/tmp/opencode/m35trunc.py`: 400 random 20-to-26-digit integers plus a fractional
  tail, leading zeros after the point, and values that round up to the next power of
  ten) landed on the same double as the written spelling every time, against the
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
  separate record (`tracking/suggestions/suggestion-0002-...`).
- That the fix belongs in the kernel rather than in every caller: the proposal
  assumes the kernel is the cheaper place, because there is one kernel and several
  callers, and today `T:/CC/` carries the whole workaround.

## Reproducing

`/tmp/opencode/m35end.HC` (host-side: the value, the end pointer, and the text
length per line) and `/tmp/opencode/m35cliff2.HC` (the `0x7FFFFFFFFFFFFFFF`
boundary) are the measurements; `/tmp/opencode/m35lex.py` runs the same spellings
through compiled C against `gcc -std=gnu99`, one program per process, and separates
cases by whether the host's parser was measured exact for that spelling.
