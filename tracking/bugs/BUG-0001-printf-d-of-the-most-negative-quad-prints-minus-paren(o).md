# `%d` of the most negative 64-bit value prints `-(` instead of `-9223372036854775808`

## Status

Open

**Severity:** P1 — wrong result today, in the `printf` subset the compiler
advertises, and it is reachable without anyone hunting for an edge: `%d` is the
only signed-decimal spelling lite accepts (`%ld` is refused at compile time, so
there is no workaround, *Evidence* 11), and the value `0x8000000000000000` is
what ordinary work lands on — `1 << 63` measures as exactly that value both
folded and computed at run time, `-v` of it measures as itself, and the
`0x8000000000000000` literal and `-9223372036854775807-1` give the same bits
(*Evidence* 6). It is P1 and not P2 because the damage is not only cosmetic
text: the same root cause makes `sprintf` return **2** where C returns **20**, so
a caller that sizes anything from the return value gets a wrong number too. Not
P0: nothing about it is unsafe — the count is understated, never overstated, so
this value cannot overflow the caller's buffer, and no lifetime or pointer
invariant is involved.

Wrong in both observable channels, measured (see *Evidence*):

| channel | today | correct |
|---|---|---|
| `printf("%d\|",v)` with `v == -9223372036854775807-1` | `-(\|` | `-9223372036854775808\|` |
| `sprintf(b,"%d",v)` | `b` = `-(` + NUL, returns 2 | `-9223372036854775808`, returns 20 |

The correct column is not this filing's opinion: the host's own formatter,
reached from HolyC in the same tree, prints `-9223372036854775808` for the same
source expression, and gcc prints it for the same 64-bit value under a
type-matched conversion.

## Finding

Decimal digit generation for `%d` lives in the pure-C runtime CC prepends to
every module — `CC_PRELUDE`, `T:/CC/CC_EMIT.HC:9`, prepended inside `CCCompile`
at `T:/CC/CC.HC:101-106`, and pre-reserved in the symbol table (the bullet that
names `CCItoa` is `T:/CC/PLAN.md:525-532`, `CCItoa` itself at 531). The function
is `CCItoa` at **`T:/CC/CC_EMIT.HC:24-35`**, and the defective lines are:

```
24: "int CCItoa(int cc_v)\n"
28: "    putchar(45);\n"          <- the sign, emitted first
29: "    cc_v=-cc_v;\n"           <- then the value is negated
31: "  if (cc_v>=10)\n"           <- recursion guard, reads the NEGATED value
32: "    CCItoa(cc_v/10);\n"
33: "  putchar(48+cc_v-(cc_v/10*10));\n"   <- last digit, same
```

`%d` is lowered at compile time straight into a call to it: `CCPrintf`
(`T:/CC/CC_PARSE.HC:3073`) looks the symbol up at `CC_PARSE.HC:3080` and emits
`CCCall(cc,hi)` for the `d` arm at `CC_PARSE.HC:3172-3173`. The conversion-letter
test at `CC_PARSE.HC:3162` accepts only `d`, `s`, `c` and `%`, so `%ld` is refused
(`CC ERROR: printf: unsupported format`, `CC_PARSE.HC:3183`) — `%d` is the only
signed-decimal spelling a lite program has.

The mechanism, read off the source and confirmed by measurement rather than
inferred from the symptom. lite's `int` is one 64-bit quad — `unsigned`/`long`/
`signed`/`short` "fold to the single 64-bit int — no width distinction"
(`T:/CC/PLAN.md:166-167`) — so `cc_v = 0x8000000000000000` is a legal `int` and
is negative:

1. `cc_v<0` at :26 is true → `putchar(45)` writes `-`, then `cc_v=-cc_v` at :29
   maps `0x8000000000000000` to itself: two's complement negation of the most
   negative quad overflows back to the same negative value. Measured directly:
   lite gives `m = -(9223372036854775807+1)` the same bit pattern the literal
   `0x8000000000000000` gives (*Evidence* 6 — `@V` and `@P6` both print `-(`,
   and `1<<63` prints `-(` three ways).
2. The guard at :31 then tests the *still-negative* value: `cc_v>=10` is false,
   so the recursion never runs and exactly **one** digit is emitted. That is why
   the answer is two characters long, not twenty.
3. :33 computes the digit from that negative: lite's `/` truncates toward zero
   (`T:/CC/CC_PARSE.HC:1017-1023` is the shared `case '/'` / `case '%'` arm and
   emits `CQO` at 1022 then `IDIV RCX` at 1023 — signed, always), measured
   `-9223372036854775808/10 == -922337203685477580` and
   `% 10 == -8` (*Evidence* 10). So `cc_v-(cc_v/10*10)` is `-8`,
   `48 + (-8)` is `40`, and byte 40 is `(`. Measured byte: `b1=40`.

A floor-dividing engine would have printed `-2` for the same input
(`-9223372036854775808 - (-922337203685477581*10) == 2`), so the observed `(`
also pins lite's truncation direction. This is the classic `abs(INT_MIN)` edge,
arriving through `cc_v=-cc_v` rather than through a call to `abs`.

**Same root cause, two more sites, one of them already wrong too.** The
`sprintf` engine has its own sign handling and its own digit walkers in the same
string: the `%d` arm negates at **`T:/CC/CC_EMIT.HC:474-480`** (`*cc_b=45` then
`cc_v=-cc_v`) and hands the result to `cc_dig` at :481 — whose own contract, per
its comment at :386 ("write unsigned `cc_v` MSB-first"), is that the *caller*
supplies a magnitude, and whose guard at :389 (`cc_v>=10`) collapses exactly the
way :31 does. Measured: `sprintf(b,"%d",min)` yields `-(` and returns 2. The
`%x` arm negates at 488-491 before `cc_hexd` (400-421, guard at 403), and
`sprintf(b,"%x",min)` measures as **one** character, `0` (*Evidence* 7). So this
is not one function with one bad line: it is the "negate, then walk" shape
duplicated three times in `CC_PRELUDE`, and the digit count is the tell in every
case.

**What is not broken, so nobody chases it.** The decimal literal parser and the
digit walk itself are fine. The failure set is bounded by step 1 (values whose
negation is not positive), and the neighbours measure clean: `-9223372036854775807`
(19 digits), `9223372036854775807`, `-922337203685477580`, `-1000000000000000000`
and `-2147483648` all print correctly (*Evidence* 5), as does a value that only
exists after wrapping — `9999999999999999999` prints `-8446744073709551617`,
which is bit-for-bit what gcc holds for the same literal (*Evidence* 9). Nothing
about parsing, sign-ness of literals, `/`, `%` or the recursion depth is involved:
change only the value by `1` and the answer becomes correct.

## Correction to the host witness as handed over

The handover recorded this command as the host comparison:

```
gcc -std=gnu99 -w -o minq_gcc minq.c && ./minq_gcc      # claimed: @-9223372036854775808|
```

Re-run at filing time, it prints **`@0|`**, not `@-9223372036854775808|`. The
reason is on the host side and not a second defect: in that source
`-9223372036854775807-1` has type `long` (LP64, 64-bit), while `%d` converts an
`int` (32-bit), so gcc reads the low 32 bits of the quad — which for
`0x8000000000000000` are zero. `gcc -std=gnu99 -Wall -c minq.c` says exactly
that: `format '%d' expects argument of type 'int', but argument 2 has type
'long int'`, with the note `|-> %ld`. With the conversion letter matched to the
argument's type, gcc prints the string this report claims: `@-9223372036854775808|`
(*Evidence* 3).

The claim being filed survives the correction; only its witness changes. On this
host `%d` cannot print a 64-bit signed decimal at all, because the host's `int`
is 32-bit, so "gcc's `%d` of this expression" is the wrong comparison to make.
The two sound witnesses are (i) the host's formatter reached from HolyC in this
tree, which prints the 20-char string for the identical source expression
(*Evidence* 4), and (ii) gcc with a 64-bit conversion (`%ld`/`%lld`) on the same
value (*Evidence* 3). Both are recorded; the `%d`-spelled host line is recorded
as wrong output from a mismatched conversion, not as evidence.

## The cost today

- **User-visible wrong text and a wrong length**, from a construct a lite program
  can write today: `printf("%d|",v);` where `v` is the most negative quad, and
  `sprintf(b,"%d",v);` returns 2 instead of 20. A `%d` is the normal way to print
  a number here, and `v` arrives from arithmetic and bit operations, not from
  spelling `INT64_MIN`.
- **The suite is green with this live**, so a regression in this exact shape has
  nothing holding it: `./3d_loader -t T -c T/CC/CC_TEST.HC` measured 85 PASS /
  0 FAIL at filing time, including `PASS printf = 0`, `PASS pfmt2 = 7` and
  `PASS sprintf = 12` (*Evidence* 8). The one fixture comment that claims to
  cover it — `/* 9 widest %d */` at `T:/CC/TEST/sprintf.c:24` — pins
  `-2147483648`, which is `int32`'s edge and a **non-edge** for lite's quad int;
  the line passes for the wrong reason. Per the tracking rule that a comment
  which *hides* a bug rides along with the fix rather than becoming its own
  filing, that comment is corrected as part of the change that closes this.
- **Three copies of the digit walk, so three chances to get it wrong again.**
  `CCItoa` (console `%d`), `cc_dig` + its `%d` caller (`sprintf`), `cc_hexd` +
  its `%x` caller — and M35's `%f` is queued to add a fourth in the same string
  (`T:/CC/PLAN.md:783-791`), written by the same hand and under the same
  "negate first" habit, with no unsigned type and no unsigned divide available
  to help it (*Evidence* 10).
- **The bug is invisible from the printed output alone.** `-(` is two characters
  where twenty belong; nothing downstream warns, and lite has no warning channel
  (`T:/CC/PLAN.md:58-64` treats a compile-time refusal as the tool).

## Already decided?

Partly, and this filing does not reopen the open part.

- **Decided: the rule this defect violates.** `T:/CC/PLAN.md`'s `## Correctness
  policy` section (heading at 44) now carries "**Delegation is the default,
  because this is an adapter layer, not a standalone C implementation.**" at
  **`PLAN.md:86-109`**, with non-delegation allowed only in two argued cases.
  That paragraph was cited by `suggestion-0002` as a decision relayed but not yet
  in the file; it **is** now in the file (in the uncommitted working-tree edit of
  `PLAN.md` that another session has in flight — `git status` shows `M T/CC/PLAN.md`,
  +37 lines, at handover time and unchanged through this filing). Under that rule
  the prelude's private decimal walk is the "second implementation hidden inside"
  the adapter, and this filing is a measured instance of it producing a wrong
  answer where the host produces the right one.
- **Decided, and already recorded as this defect:** `PLAN.md:777-791` (the M6c
  delegation note plus its bracketed addendum) states that `suggestion-0002`
  "records a live DEFECT it found (lite's `%d` of the most negative quad), filed
  separately as a bug". This file is that filing. `suggestion-0002` left it
  unfiled on purpose, scoped itself to the mapping question, and asked that the
  two be linked: see the first bullet of its *Not claimed, and one thing
  deliberately not filed* section
  (`tracking/suggestions/suggestion-0002-printf-number-conversions-never-mapped-onto-kernel-printing(o).md`,
  beginning at line 377; that bullet is longer now than when the file was
  staged, because this filing appended the cross-link to it), and row 5 of its
  integer table (same file, line 172), which records `-(` beside the kernel's
  correct string. Cross-linked both ways by this filing (see *Repo notes*).
- **Decided for its own scope, not this one:** M6c declined routing conversions
  through the host formatter (`PLAN.md:777-782`: "deliberately not done here (C's
  format-flag semantics would have to survive the mapping)"). Nothing here says
  M6c should have done it in that milestone; it is the reason the prelude
  implementation exists and is wrong.
- **Not decided:** which of the directions below to take; whether the durable fix
  is a shared magnitude-safe walker for all three sites or three local peels;
  what `%x` of the most negative quad should print at all, since lite's
  documented rule there is "prints the magnitude of a signed value"
  (`PLAN.md:544`) and this value has no positive magnitude in the model; and
  whether the `sprintf` fixture gains a 13th check or replaces check 9's
  32-bit case (which changes the asserted total at `T:/CC/CC_TEST.HC:266`).

## Evidence (all of it re-run at filing time; every line below is a run, not a recollection)

Host: gcc 13.3.0 (Ubuntu 13.3.0-6ubuntu2~24.04.1) on x86-64 Linux, LP64 — the
same ABI the C host uses. Engine runs are `./3d_loader -t T -c <driver.HC>` from
the repo root `/mnt/d/Source/3Days`, wrapped in `timeout` + `stdbuf -oL`; boot
noise (`WARN: Fun header args mismatch …`, ALSA/JACK lines, `FL:T:/KERNELA.HH,5955`)
is filtered out of the quotes below. Sources live in `/tmp/opencode` and are read
by the engine through `Z:` (`-c` mounts the host root at `Z:`). `T:/CC/PLAN.md`
is being edited by another session, so every `PLAN.md:<lines>` citation here is
as of filing time; the bullet and milestone **names** are the durable handles.

**1. Ours, console channel.** `/tmp/opencode/minq.c`:

```c
int printf(const char *,...);
int main(){ printf("@%d|\n",-9223372036854775807-1); return 7; }
```

Driver `/tmp/opencode/minq_drv.HC`:

```c
#include "T:/CC/CC.HC"
"rc=%d\n",CCRunFile("Z:/tmp/opencode/minq.c");
Exit;
```

```
$ cd /mnt/d/Source/3Days
$ timeout 300 stdbuf -oL ./3d_loader -t T -c /tmp/opencode/minq_drv.HC | grep -E '^@|rc='
@-(|
rc=7
```

**2. The host line as handed over — it prints `@0|`.**

```
$ cd /tmp/opencode && gcc -std=gnu99 -w -o minq_gcc minq.c && ./minq_gcc ; echo "exit=$?"
@0|
exit=7
$ gcc -std=gnu99 -Wall -c minq.c -o /dev/null
warning: format ‘%d’ expects argument of type ‘int’, but argument 2 has type ‘long int’ [-Wformat=]
```

**3. Host, conversion letter matched to the value's width.** `/tmp/opencode/g_ld.c`
(`%ld`), `/tmp/opencode/g_ll.c` (`%lld`, `LL` suffix, `<stdio.h>`),
`/tmp/opencode/g_both.c` (`printf("@%d|%ld|%x|\n",v,v,v)` with `long v`):

```
$ gcc -std=gnu99 -w -o g_ld_gcc g_ld.c && ./g_ld_gcc ; echo "exit=$?"
@-9223372036854775808|
exit=7
$ gcc -std=gnu99 -w -o g_ll_gcc g_ll.c && ./g_ll_gcc
@-9223372036854775808|
$ gcc -std=gnu99 -w -o g_both_gcc g_both.c && ./g_both_gcc      # one value, three letters
@0|-9223372036854775808|0|
```

`@0|-9223372036854775808|0|` is the whole of *Correction* in one line: the same
quad prints as nothing under `%d`, correctly under `%ld`, and its low 32 bits
under `%x`.

**4. The kernel's own formatter, in this tree, on the identical expression.**
`/tmp/opencode/kminq3.HC` formats with `MStrPrint` and dumps the result **as
bytes**, printing `StrLen` beside it so a dump line cannot be trusted on its own
(see *Reproduction traps* 10) — padding and NULs are invisible in console text
anyway (`suggestion-0002` trap 5):

```c
s=MStrPrint("@%d|",-9223372036854775807-1);
"len=%d\n",StrLen(s);
"hex:";  i=0;  while (i<StrLen(s)) { "%02X ",s[i]; i++; }  "\n";
"dec:";  i=0;  while (i<StrLen(s)) { "%d ",s[i];  i++; }   "\n";
"text:[%s]\n",s;
```

```
$ timeout 300 stdbuf -oL ./3d_loader -t T -c /tmp/opencode/kminq3.HC | grep -E 'len=|hex:|dec:|text:'
len=22
hex:40 2D 39 32 32 33 33 37 32 30 33 36 38 35 34 37 37 35 38 30 38 7C
dec:64 45 57 50 50 51 51 55 50 48 51 54 56 53 52 55 55 53 56 48 56 124
text:[@-9223372036854775808|]
```

22 bytes, `40 2D` = `@-`, 19 digit bytes, `7C` = `|`; the decimal dump is the
same 22 values and the text line agrees with both. The engine behind that string
is `StrPrintJoin`, `T:/STRPRINT.HC:208` (extern'd at `T:/KERNELA.HH:5355`) — the
routine M6c's note meant when it said "kernel `Prnt`"; `Prnt` does not exist
under that name in this tree (see `suggestion-0002`, *Not claimed*). So: same
expression, same 64-bit value, host formatter correct, prelude formatter wrong.

**5. The failure is exactly one value, not a class of wide-value problems.**
`/tmp/opencode/liteA.c` prints eight values through `%d`; `/tmp/opencode/gA.c`
prints the same eight through `%ld` with every argument cast to `long`, so the
host's 32-bit `int` never truncates the comparison.

| # | value | gcc `%ld` (measured) | lite `%d` (measured) |
|---|---|---|---|
| A | `-9223372036854775807-1` (most negative) | `-9223372036854775808` | `-(|` ✗ |
| B | `-9223372036854775807` | `-9223372036854775807` | ✓ |
| C | `9223372036854775807` | `9223372036854775807` | ✓ |
| D | `-922337203685477580` | `-922337203685477580` | ✓ |
| E | `-1000000000000000000` | `-1000000000000000000` | ✓ |
| F | `-10` | `-10` | ✓ |
| G | `10` | `10` | ✓ |
| H | `-2147483648` | `-2147483648` | ✓ |

```
$ timeout 300 stdbuf -oL ./3d_loader -t T -c /tmp/opencode/liteA_drv.HC | grep '^@'
@A=-(|
@B=-9223372036854775807|
@C=9223372036854775807|
@D=-922337203685477580|
@E=-1000000000000000000|
@F=-10|
@G=10|
@H=-2147483648|
```

B is A plus one: nineteen digits, same sign, same recursion depth — correct. D
and E are 18- and 19-digit negatives — correct. So the digit walk, the recursion
and the parser are exonerated by the runs beside them, and only the negation at
`CC_EMIT.HC:29` (and its twins at 479 and 490) is implicated.

**6. It is not constant folding.** `/tmp/opencode/liteC.c` computes the value at
run time in a variable (`v=-9223372036854775807; v=v-1;`) and prints it:

```
$ timeout 300 stdbuf -oL ./3d_loader -t T -c /tmp/opencode/liteC_drv.HC | grep '^@'
@V=-(|
@P1=9223372036854775807|
@P2=-8446744073709551617|
@P3=4294967296|
@P4=1234567890123|
@P5=9223372036854775807|
@P6=-(|
@c9_32bit=11|
@c9_quad=2|
```

`@V` is the runtime case, so no constant folding is involved. Three more
spellings of the same bit pattern, `/tmp/opencode/liteG.c` beside
`/tmp/opencode/gG.c`:

```
$ timeout 300 stdbuf -oL ./3d_loader -t T -c /tmp/opencode/liteG_drv.HC | grep '^@'
@shl63_rt=-(|
@shl63_lit=-(|
@neg_of_min=-(|
$ gcc -std=gnu99 -w -o gG_gcc gG.c && ./gG_gcc      # long v=1; v=v<<63; %ld
@shl63=-9223372036854775808|@neg=-9223372036854775808|
```

`v<<63` is this value both computed at run time and folded as `1<<63`, and
`-v` of it keeps the bit pattern on **both** engines (gcc prints the 20-char
string for `-LONG_MIN` too) — the arithmetic wrap is not the defect, the print
of the wrapped value is. `@lit` in the separate `/tmp/opencode/liteB.c` run
(`printf("@%d|\n",-9223372036854775808);`, the spelling a user reaches for
first) is also `-(`, and gcc on the same spelling prints
`-9223372036854775808`. Four independent routes to `0x8000000000000000` in the
argument slot all print `-(`.

**7. The `sprintf` channel, and the `%x` sibling.** `/tmp/opencode/minqs.c`
(ours) and `/tmp/opencode/gspr.c` (gcc, `%lld`), both printing the return count
and the first three bytes as decimal:

```
$ timeout 300 stdbuf -oL ./3d_loader -t T -c /tmp/opencode/minqs_drv.HC | grep '^@'
@n=2|
@b0=45 b1=40 b2=0|
$ gcc -std=gnu99 -w -o gspr_gcc gspr.c && ./gspr_gcc
@n=20|b0=45 b1=57 b2=50|
```

Ours: `-` (45), `(` (40), NUL. gcc: `-` (45), `9` (57), `2` (50). The count
divergence — 2 against 20 — is the part a caller can act on and the part no
fixture looks at. `/tmp/opencode/liteE.c` adds the hex arm:

```
@x_min=1[0]|      sprintf(b,"%x",min) -> "0", count 1
@x_neg1=1[1]|     sprintf(b,"%x",-1)  -> "1", count 1   (documented: magnitude, PLAN.md:544)
@d_min=2[-(]|
@d_max=19[9223372036854775807]|
```

`%x` of `-1` printing `1` is a documented deviation and not this bug; `%x` of
the most negative quad printing a single `0` is this bug's second face, because
`cc_v=-cc_v` at `CC_EMIT.HC:490` leaves the bit pattern unchanged and the
`cc_v>=16` guard at :403 then stops after one digit.

What the two other implementations print for that same bit pattern —
`/tmp/opencode/khex.HC` and `/tmp/opencode/khex2.HC` (kernel, `MStrPrint`;
`StrLen` printed beside each string so the width is not eyeballed) and
`/tmp/opencode/ghex.c` (gcc, `v = (unsigned long long)(1LL << 63)`):

```
khex len=18 text:@8000000000000000|      kernel %x of the min quad, 16 digits
khex_neg1 len=18 text:@ffffffffffffffff| kernel %x of -1
kernel u text:@9223372036854775808|      kernel %u of the min quad
@llx=8000000000000000|lu=9223372036854775808|
```

So the host's formatter gives sixteen hex digits and the unsigned decimal for
this value, and so does gcc with type-matched letters. Only lite's own walk
produces a single character.

**8. The suite is green with the defect live.**

```
$ timeout 600 stdbuf -oL ./3d_loader -t T -c T/CC/CC_TEST.HC > /tmp/opencode/suite_out.txt 2>&1
$ grep -c '^PASS' /tmp/opencode/suite_out.txt
85
$ grep -c 'FAIL' /tmp/opencode/suite_out.txt
0
$ tail -1 /tmp/opencode/suite_out.txt
CC TESTS: 0 failed
$ grep -n 'printf\|pfmt2\|sprintf' /tmp/opencode/suite_out.txt | grep PASS
159:PASS printf = 0
161:PASS pfmt2 = 7
397:PASS sprintf = 12
```

`CCAssert` (`T:/CC/CC_TEST.HC:9-19`) compares **`CCRunFile`'s return value**
against `want` and prints only a number, so the expected values pinned at
`CC_TEST.HC:250` (`printf` → 0) and `:251` (`pfmt2` → 7) are exit codes: neither
observes a single printed byte. `T:/CC/TEST/printf.c` and
`T:/CC/TEST/pfmt2.c` (6 and 5 lines) print `-7`, `1000`, `0`, `42` — all
magnitudes far below the edge. The `sprintf` fixture is the one place formatted
**text** is checked, because `sprintf`'s return value is the written length and
its checks compare it plus `strcmp` of the buffer (`T:/CC/TEST/sprintf.c:13-30`,
`/*  2 length */` at :14, check 9 at :24) — and check 9's label claims the widest
case is covered while pinning the 32-bit edge. See *Directions (f)*.

**9. The parser is not involved: bit-for-bit agreement on the literals.**
Comparing `@P1…@P6` above with gcc's reading of the same literals
(`/tmp/opencode/gparse.c`, printing both the natural type and the signed
reinterpretation):

| literal | gcc (measured) | lite `%d` (measured) | reading |
|---|---|---|---|
| `9223372036854775807` | `9223372036854775807` | same | ✓ |
| `9999999999999999999` | `9999999999999999999` unsigned; `(long long)` → `-8446744073709551617` | `-8446744073709551617` | ✓ same 64 bits; lite has one signed quad (`PLAN.md:166-167`) |
| `4294967296` | `4294967296` | same | ✓ |
| `1234567890123` | `1234567890123` | same | ✓ |
| `0x7fffffffffffffff` | `9223372036854775807` | same | ✓ |
| `0x8000000000000000` | `9223372036854775808` unsigned; `(long long)` → `-9223372036854775808` | `-(` | bits agree; **the walk** is wrong |

So the decimal/hex literal parser keeps the whole 64-bit value, the wrap at
2^63 is the documented single-quad model rather than a defect, and the `-(` is
produced after the value is already correct.

**10. `unsigned` is no help today: lite has no unsigned divide.** This is what
bounds the fix directions. `/tmp/opencode/liteD.c` (`unsigned long u; u=…`):

```
@u_div10=-922337203685477580|      u=0x8000000000000000;  u/10  -> signed answer
@u_gt0=0|                          u>0 is false  -> still compared signed
@u_mod10=-8|                       u%10 is -8     -> still the signed remainder
@m_div10=-922337203685477580|      m=-9223372036854775807-1; m/10
@m_mod10=-8|                       m%10           -> the -8 in 48+(-8)=40='('
@md_div10=922337203685477580|      -(m/10) is representable  -> the peel works
```

`unsigned` folds (`PLAN.md:166-167`) and every `/` in generated code is the
`CQO` + `IDIV RCX` pair (`T:/CC/CC_PARSE.HC:1017-1023`, the shared `/`-and-`%`
arm; `CC_PARSE.HC:2885` notes
"the only other RDX user is the `CQO`/`IDIV` pair behind `%`"). The last line is
the useful one: `-(m/10)` = `922337203685477580` **is** representable, so a walk
that peels the low digit before negating has everything it needs inside the
subset lite compiles today.

**11. No `%ld` escape hatch.** `/tmp/opencode/liteF.c` (`printf("@ld=%ld|\n",5);`):

```
CC ERROR: printf: unsupported format [tk=3 kw=0 num=59 str='@ld=%ld|
CC ERROR: expected '}' [tk=1 kw=2 num=59 str='return'
rc=-1
```

gcc prints `@ld=5|`. The refusal is `CC_PARSE.HC:3183` (letter test at 3162) and
it kills the module, so the second line is cascade, not a second finding.

## Directions (recorded, not implemented)

**(a) Peel the last digit before negating.** Restructure `CCItoa`
(`CC_EMIT.HC:24-35`) so the value that gets negated is always `cc_v/10`, never
`cc_v`. For a negative `cc_v`: emit `45`, keep `d = -(cc_v%10)` (measured `8` for
the edge), recurse on `-(cc_v/10)` **only when that quotient is non-zero**
(measured representable at the edge: `922337203685477580`; skipping the guard
would make `-5` print `-05`, because the positive path emits a digit for `0`),
then emit `48+d`. `*Evidence 10` is the existence proof that every piece is
expressible in the prelude subset. Costs: two extra locals and one more
recursion level's worth of bookkeeping in a recursive function that runs on the
module's private stack, and the negative path becomes structurally different
from the positive path, which is exactly the kind of split that has bitten this
file before. Worked: `-10` → `-`, `d=0`, quotient `1` → recurse prints `1`, then
`0` → `-10`; `-5` → `-`, `d=5`, quotient `0`, no recursion → `-5`; the edge →
`-`, `d=8`, quotient `922337203685477580` → its 18 digits, then `8`. **It fixes
none of the other two sites by itself** — `cc_dig`'s `%d` caller (`474-481`) and
`cc_hexd`'s `%x` caller (`488-492`) need the same treatment or a shared helper.

**(b) A shared magnitude-safe walker, i.e. fix the shape once.** One function
that takes a signed quad and yields digits without ever negating the input, used
by `CCItoa`, by the `sprintf` `%d` arm and by the `%x` arm. Costs: `CCItoa`'s
`putchar`-to-`cc->out` form and `cc_dig`'s write-to-buffer-and-return-count form
are different (the latter is what makes `sprintf`'s count correct, and the count
is a documented API at `PLAN.md:541-550`), so "one walker" means either
`CCItoa` goes through a buffer (one 20-byte scratch write per conversion, plus
the copy to `cc->out`) or the helper is written twice with one shared digit
rule. A shared rule is what stops the fourth implementation (M35's `%f`) from
re-introducing the bug, which is the reason to prefer (b) over (a) if the change
is going to be made at all.

**(c) glibc's actual answer, and why it is not available yet.** libc formats
this value by working from the **unsigned** magnitude (`-(unsigned long)v`),
which is exact at `INT_MIN`. Measured here: lite has no unsigned reading to
borrow (`u/10`, `u>0` and `u%10` are all signed even for an `unsigned long`,
*Evidence* 10), so the spelling libc uses cannot be written in `CC_PRELUDE`
today. It becomes available if either generated code gains an unsigned path
(`DIV` with `RDX` zeroed instead of `CQO`-sign-extended, beside the `IDIV` pair
at `CC_PARSE.HC:1017-1023`), or the conversion is delegated (direction (d)) —
worth recording so a fixer who reaches
for "just use unsigned" does not spend a session discovering that `unsigned` is
a no-op keyword here.

**(d) Delete the cause: delegate `%d` to the host's formatter.**
`StrPrintJoin`'s `d` arm (`T:/STRPRINT.HC:208`, conversion arm at 433) prints
this value correctly (*Evidence* 4), so routing lite's `%d` there removes the
defect by removing the implementation. That is what the new rule at
`PLAN.md:86-109` now asks for by default, and it is the same direction
`suggestion-0002` carries — see its *Directions (b)* and rows 1-12 of its
integer table; the mapping answer for the shape this bug needs is already in
that file (its row 5 is literally `-9223372036854775808` on the kernel side
against lite's `-(`). Costs are recorded there and are real: ~14 bytes and an
RDI-scratch touch per conversion, two of four `cc->ext` slots per call, and a
bounded destination problem, because `StrPrintJoin(dst,…)` takes no capacity
argument. The honest framing for the fixer: **(a)/(b) are the fix if the
prelude stays; (d) is the fix if the conversion goes to the host, and then this
file is deleted along with `CCItoa`.** Whichever is chosen, this defect is an
argument in (d)'s column: the second implementation is not merely redundant, it
disagrees with both the host and C on a value the host gets right.

**(e) Whatever is chosen, decide `%x`'s most-negative case at the same time.**
Measured `sprintf(b,"%x",min)` prints a single `0`; the documented rule
(`PLAN.md:544`, `%x` prints the magnitude of a signed value) has no answer for a
value with no positive magnitude. Options: the **unsigned reading**, which is
what both other implementations already produce for this very bit pattern —
`8000000000000000` measured from the kernel's `%x` and from gcc's `%llx`, and
`9223372036854775808` measured from the kernel's `%u` and gcc's `%llu`
(*Evidence* 7); a refusal at the call; or documenting `0`. The unsigned reading
is not a free choice either: it changes what `%x` does for negatives
**everywhere** (today `sprintf(b,"%x",-1)` prints `1` while the kernel prints
sixteen `f`s for the same pattern), so it is a change to the documented
deviation at `PLAN.md:544` and has to land in `## C subset limitations` in the
same change, not only in the code. Direction (d) settles the question by
default — a delegated `%x` inherits the kernel's reading measured above — which
is another reason to answer (e) at the same time as (a)-(d) rather than after.
This filing does not pick; it records that leaving it undecided means the fix
"makes `%d` right and leaves `%x` silently weird" in the same file.

**(f) Pinning it in the suite.** The check that is mechanically visible today
belongs in **`T:/CC/TEST/sprintf.c`**, beside check 9, because that fixture is
the only one in the printf family whose assertion observes formatted content:
`sprintf`'s return value **is** the written length, so

```c
tot=tot+(sprintf(b,"%d",-9223372036854775807-1)==20);   /* quad edge: 1 sign + 19 digits */
```

fails today (2 against 20, *Evidence* 7) and cannot be satisfied by an
approximately-right walk. A `strcmp` against the exact 20-character string is
the stronger form and costs nothing extra in that fixture's idiom, which already
pairs a count with a content comparison — its checks 4, 7, 8 and 10 are
`(n==k) && (strcmp(...)==0)`, and its opening trio splits the same information
into content (check 1, :13), length (check 2, :14) and terminator (check 3, :15).
Add it and `T:/CC/CC_TEST.HC:266` (`CCAssert(12,"T:/CC/TEST/sprintf.c","sprintf")`)
becomes `13` — or replace check 9's 32-bit case instead, keeping 12, which is
the cheaper edit but loses a check and re-labels `/* 9 widest %d */` as what it
actually is. Either way **correct that comment** (`T:/CC/TEST/sprintf.c:24`): it
is what let a 32-bit edge read as full coverage while lite's `int` is a quad.

Adding the same value to the two fixtures named as the `printf` tests —
`T:/CC/TEST/printf.c` (`CCAssert(0,…)` at `CC_TEST.HC:250`) and
`T:/CC/TEST/pfmt2.c` (`CCAssert(7,…)` at `:251`, the negative-value one, printed
text `-7 1000 0`) — is worth doing for readability of intent, but on its own it
pins nothing: `CCAssert` compares `CCRunFile`'s exit code, and neither file's
return value depends on the printed digits. If the console path is to be pinned
by value rather than by eyeballed output, the fixture has to feed something to
`return` — e.g. `return tot;` with `tot` derived from a `sprintf` length or a
`strcmp` — which is the pattern `sprintf.c` already uses and `pfmt2.c` does not.
Recorded so the next reader does not add a `printf` line to `pfmt2.c`, see the
suite stay green, and conclude the case is covered.

**(g) What has to be true before this file goes away.** The `%d` case at
*Evidence* 1 printing `-9223372036854775808`, the `sprintf` case at *Evidence* 7
returning 20 and matching the 20-character string, a fixture check that fails
against the pre-fix build (i.e. added before or with the fix, not after), a full
`suite_out.txt`-style run at 0 failed, and the record updated in the same change:
the M6c delegation note and `PLAN.md:783-791`'s "filed separately as a bug" line,
plus `## C subset limitations` (`PLAN.md:112`) if the answer for `%x` (direction
(e)) becomes a documented deviation. Then this file is deleted per the lifecycle,
and `suggestion-0002` stays whatever happens to it — a suggestion is never
deleted.

## Reproduction traps

1. **The host `%d` line prints `@0|` and looks like a third wrong answer.** The
   value is a `long` on the host and `%d` converts an `int`, so gcc truncates to
   32 bits; `-Wall` names the mismatch. Compare with `%ld`/`%lld`, and cast small
   literals to `long` so every row is the same width on both sides. (The
   mirror-image trap: `printf("@%ld|\n",-10)` on the host reads 64 bits out of a
   32-bit `int` argument and measured `4294967286`.)
2. **Do not compare exit codes across engines.** The loader hands back the low 8
   bits, so `return (int)9999999999999999999;` "disagrees" with gcc for
   truncation reasons only. The claims in *Evidence* 9 are all made with printed
   text and `sprintf` counts.
3. **lite cannot run a `.c` file directly**: `-c` hands its argument to the HolyC
   compiler (`ERROR: Undefined identifier at "main"` if you try). The two-line
   HolyC driver wrapping `CCRunFile("Z:/tmp/opencode/<x>.c")` is required, and
   `Z:` is the host root as mounted by `-c`. Reads through `Z:` are fine;
   `FileWrite("Z:/…")` faults (`AGENTS.md`).
4. **Run from `/mnt/d/Source/3Days`.** `./HCRT.BIN` is resolved from the process
   cwd; a run started in `/tmp/opencode` boots a different image and produces no
   script output.
5. **No `HCRT.BIN` rebuild is needed for any of this**, contrary to what a reader
   of `AGENTS.md`'s bootstrap section will assume: the CC compiler is not in the
   boot image (`grep` for `CC/` over `T/HCRT_TOS.HC` and `T/FULL_PACKAGE.HC`
   finds no `#include` of any `T:/CC/` file; `T:/CC/CC.HC` is pulled in at run
   time by the driver's own `#include`, and it splices `CC_LIB.HC` at
   `CC.HC:16` and `CC_EMIT.HC` at `CC.HC:9`). Editing `CC_EMIT.HC`, `CC_TEST.HC`
   or `TEST/*.c` takes effect on the next run. This also qualifies
   `suggestion-0002` trap 2, which says a change in `CC_LIB.HC` needs the image
   rebuilt.
6. **`WARN: Fun header args mismatch 'CCQuadAbs'` in the boot noise is not a
   lead.** `CCQuadAbs` is lite's own relocation recorder
   (`T:/CC/CC_EMIT.HC:620`, `CCQuadAbs(cc,kind,addr)`), not a kernel quad-`abs`
   helper — it is not declared in `T:/KERNELA.HH` or `T:/KERNELB.HH` at all. The
   mismatch line is the ordinary boot spam `AGENTS.md` says not to chase.
7. **Measure bytes, not console text.** Both channels end at the kernel's
   `"%s"` print, so the pre-fix `-(` and a post-fix 20-char answer are both just
   text on a blue screen; the discriminating facts in this file are byte values
   (`b1=40` against `b1=57`) and counts (`2` against `20`). Printing a bare `$`
   sends the kernel off to parse `T:/Tmp.DD.Z` (`suggestion-0002` trap 5); `@`
   and `|` delimiters avoid that. `T/Tmp.DD.Z` was already untracked in this
   checkout (`?? T/Tmp.DD.Z`) and was **not** rewritten by these runs (mtime
   still `Jul 17 22:46`) because nothing printed markup.
8. **A bad format letter cascades.** One `%ld` in a fixture produces
   `printf: unsupported format` **and** `expected '}'`, with `rc=-1`, so a probe
   that trips on it looks like two errors and a parse failure. Put risky shapes
   last, or in their own file (`liteF.c` exists only to demonstrate this).
9. **Always `timeout` and `stdbuf -oL`.** rc is 0 even when the HolyC side throws
   or the module fails to compile, and stdout is block-buffered, so a killed run
   loses the very line you were measuring (`AGENTS.md`).
10. **Print the length beside any byte dump.** The first kernel-side run of this
    filing (`/tmp/opencode/kminq.HC`) rendered a 22-byte string as a 21-token
    `"%02X "` line — one `33` is missing from the rendered dump, and nothing in
    the line says so. The re-run in *Evidence* 4 prints `StrLen` first and both a
    hex and a decimal dump, and all three agree; that is the version quoted
    above. A dump with no length beside it is a claim about a line, not about a
    string.

## Not claimed

- **"Exactly one bit pattern is affected" is an argument from the code plus
  neighbours, not an exhaustive sweep.** The set of `cc_v` for which `cc_v=-cc_v`
  fails to produce a value `>=10` is `{0x8000000000000000}` under truncating
  two's-complement quad arithmetic, and rows B-E and H of *Evidence* 5 bracket it
  on both sides at 1, 18 and 19 digits. Every 20-digit negative was not run.
- **Only the `%d` and `%x` arms of both engines were exercised.** `%u`, `%i`,
  `%o`, `%+d`, widths, precisions and `%f` were not probed from lite here; the
  host-side flag mapping is `suggestion-0002`'s subject and its tables are
  measured there, including `%-8d` and `%-8x`, which fail on the **kernel** side
  (its rows 13-14) and are not this defect.
- **The `sprintf` buffer-content claim is bounded to the first three bytes**
  (`b0`, `b1`, `b2`) plus the return count, as measured. The remaining bytes of
  the `-(` result were not dumped; count 2 and `b2=0` together fix the string.
- **No claim about which direction is right, and no code was changed.** (a)-(g)
  are recorded for the fixer; nothing in `T:/` was touched by this filing.
- **`suggestion-0002`'s file paths were not corrected here.** It cites the host
  formatter as `T:/CC/STRPRINT.HC:208` (its line 33); the file in this tree is
  **`T:/STRPRINT.HC`** — `T:/CC/STRPRINT.HC` does not exist. Its `CC_EMIT.HC`
  cites are within one line of the ones used here (`CCItoa` 24-36 against 24-35,
  `cc_dig` 386-398 against 386-399), and its `PLAN.md` line numbers are as of
  its own filing time and are now about 26 lines lower than the ones in this
  file, because the delegation paragraph was added to `## Correctness policy` in
  between.
- **Whether `PLAN.md`'s delegation paragraph and the "filed separately as a
  bug" line survive to the commit is not this filing's claim.** `T:/CC/PLAN.md`
  is uncommitted (`M`) and owned by another session; the citations above read the
  working tree at 22:37 and its diff was still 37 insertions at 22:55. There is
  no `M35` milestone entry in the file yet — the only `M35` text is the forward
  reference at `PLAN.md:784` ("M35's `%f` is the first place it comes due") — so
  nothing here depends on one. The durable handles are the section headings and
  the milestone names, not the line numbers.

## Repo notes

- Filed on `bugfix/build-isssues` in the primary checkout
  `/mnt/d/Source/3Days` — `git rev-parse --show-toplevel` prints that path, and
  no separate worktree was named for this item, so the primary checkout **is**
  the working tree for it. Branch confirmed via `git branch --show-current`.
- `tracking/bugs/` did not exist in this checkout; this filing created it. It
  holds exactly one file, so `ls tracking/bugs/ | grep -o 'BUG-[0-9]*' | sort
  -t- -k2 -n | tail -1` reports `BUG-0001` and the next free id is `BUG-0002`.
  Numbers are never reused, so when the fix lands this file is **deleted**, not
  renamed — per the lifecycle a fixed bug goes away while the linked suggestion
  is never deleted.
- **Recoverability of a deletion here.** `tracking/` is **not** gitignored in
  this repo (`git check-ignore -v tracking/ tracking/bugs` prints nothing, rc=1),
  which contradicts the assumption the tracking skill carries elsewhere, so once
  this file is committed a deletion is recoverable from history at the deletion
  commit's parent. But at handover time this file and `suggestion-0002` are
  **staged, not committed** — the only tracking file anywhere in history is
  `suggestion-0001`, added at `68e4f34` ("CC: M34 one file-scope name is one
  object, and what extern means"), and a `--diff-filter=A` log over `tracking/`
  lists that one file and nothing else. So: **if this file is deleted before it
  is committed it is gone outright**, with no commit to read it back from; once
  the user's commit contains it, a later deletion is recoverable both from that
  commit and from the deletion commit's parent.
- Change set, staged and not committed (`git add` by explicit path, no commit):
  1. **added** `tracking/bugs/BUG-0001-printf-d-of-the-most-negative-quad-prints-minus-paren(o).md`
     (this file);
  2. **changed** `tracking/suggestions/suggestion-0002-printf-number-conversions-never-mapped-onto-kernel-printing(o).md`
     — the bullet in *Not claimed, and one thing deliberately not filed* that
     recorded this defect as left unfiled now names this file, records the
     `%d`-vs-`long` host-witness correction, and notes that the bug's direction
     (d) is that suggestion's direction (b); the rest of the bullet and all of
     its reasoning are unchanged. The staged copy was re-added by path so the
     index matches the worktree.
- Nothing outside `tracking/` was written. `T/CC/PLAN.md`'s pre-existing
  uncommitted edit, `T/Tmp.DD.Z` and every `T:/CC/` source are untouched by this
  filing (the probes only read them; `/tmp/opencode/*.c` and `/tmp/opencode/*.HC`
  drivers and `suite_out.txt` are scratch and outside the repo).
