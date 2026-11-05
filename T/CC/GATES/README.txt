T:/CC/GATES — the gate battery (M37 %f delegation, and the kernel-rebuild probes)
=================================================

Everything that PROVES a %f claim lives here, in the tree, and not in the host's
`/tmp`: `/tmp` is wiped on reboot, and a gate that lives there evaporates along
with the evidence it produced.  Each file below is either a program that decides
something, the table a program's expectations were taken from, or the generator
that rebuilds a program from that table.  No file here is a transcript: run the
gate and it prints its own verdict.

This directory is NOT compiled into HCRT.BIN (nothing under T:/CC/ is — the
kernel image includes T:/HCRT_TOS.HC's 64 files).  It IS copied into a staged
release by T:/ReleasePackage.HC, which packs the whole T: tree except
`Offical`, `Greek`, `SmallTalk` and `TempleOS.ISO`; add "CC/GATES" to that
`ReleasePackage(...)` call if end-users should not carry these files.

THE GATES
---------

1. THE IN-TREE SUITE  (the gate every change must pass)

     cd <repo root>
     timeout 1500 stdbuf -oL ./3d_loader -t T -c T:/CC/CC_TEST.HC </dev/null

   Expect: 89 `PASS` lines, no `FAIL`, no `FAULT`, no `CC ERROR:`, and
   `CC TESTS: 0 failed`.  The float lines are `TOT ffmt 63 63` (the fixture's
   own count), `PASS objfloat = 63` (the same fixture compiled, saved as an
   object, reloaded and run — the case that proves the %f site's CC_B_CTRL
   buffer address and its CC_B_LOCALS directive string both rebase, and that the
   helper's in-place rewrite of that string survives save/load), and
   `PASS float kernel = 58 cells` (CC_TEST.HC's `CCProbeFloat`, which calls the
   kernel's `StrPrintJoin` directly on the same bit patterns with the directive
   lite SENDS).  The exit code is always 0; grep the output.

2. THE DELEGATION SWEEP  (does every %f sink answer like the kernel?)

     ./3d_loader -t T -c T:/CC/GATES/gsd.HC </dev/null

   `gsd.HC` runs `gs0.c` .. `gs9.c` through lite in one session.  Each program
   is 38 bit patterns x 9 directives split into 4 patterns: every cell does
   `sprintf` and compares the bytes against the kernel's own rendering, then
   `printf`s the same value with the same directive and prints the expected text
   in the same line.  Expect 10 `RUN ... rc=0` lines, `TOT ... 0 fails` in each,
   684 self-checked cells with no `FAIL` line, and 342 `P ` lines.  Compare the
   printf sink against the kernel table:

     ./genm37 expect T:/CC/GATES/gs | sort > want.txt
     grep '^P ' sweep.log | awk -F'|' '{print $1"|"$4"|"}' | sort > got.txt
     diff want.txt got.txt          # expect: no output

   rc is the program's own failure count, so `rc=0` IS the verdict.

3. REFUSAL AND ACCEPTANCE  (what lite must reject, and what it must render
   exactly as the host does)

     ./3d_loader -t T -c T:/CC/GATES/rfdriver.HC </dev/null

   `r01.c` .. `r15.c` each carry ONE directive that must be a COMPILE ERROR
   (`%e %g %lf %hd %5d %010d %10.3d %.2d %x % c %n %.2e %ld %u %5s`): expect 15
   `Rnn rc=-1` lines and one `CC ERROR: printf: unsupported format (only %d %s
   %c %% and %f)` per case.  A wrong ANSWER is not possible in these programs —
   the directive is the whole format string, so the compile either fails or it
   does not.  `acc.c` is the other half: 11 flag/width/precision directives
   (`%20.15f %010.4f %-10.4f %#10.2f % 10.2f %+10.2f %.600f %999.999f %01000.3f
   %3.1f %-3.1f`) printed through `sprintf` (A-lines) and `printf` (B-lines),
   plus the three ARITY cells that pin what the runtime engine does with a flag,
   a width or a `*` in front of a conversion it lowers itself.  The driver then
   renders those same directives through the kernel with 1.5's bit pattern
   (K01-K11) and with 10's (K12).  Expect `ACC rc=0` and:

     grep -E '^A[0-9][0-9]\|' refuse.log | cut -d"|" -f2 | sort > a.txt
     grep -E '^B[0-9][0-9]\|' refuse.log | cut -d"|" -f2 | sort > b.txt
     grep -E '^K[0-9][0-9]\|' refuse.log | head -11 | cut -d"|" -f2 | sort > k.txt
     diff a.txt k.txt ; diff b.txt k.txt    # expect: no output, both

   `-f2`, not `-f2-`: `acc.c` prints `A01|<rendering>|<n>|`, one field more than
   the `K01|<rendering>|` lines, so `-f2-` would compare the rendering plus a
   stray `|<count>` against the oracle and report 11 differences that are only
   the count.  (This was measured on the rebuilt kernel of 2026-11-05.)

   The K-lines are the oracle, produced in the same session as the A- and B-
   lines, so this gate needs no external C compiler and no external expectation.

4. THE KERNEL MEASUREMENT  (where the pinned renderings come from)

     ./3d_loader -t T -c T:/CC/GATES/host.HC </dev/null > rows.txt

   `host.HC` calls `StrPrintJoin(buf, fmt, 1, &q)` for 38 bit patterns x 9
   directives and prints `K <pattern> <directive> <bits> <count> [<rendering>]`.
   Strip the boot-noise header — everything before the first `K` line, 50 lines
   on this build (`Using ./HCRT.BIN...`, `REMOVE:1024,768`, the `WARN: Fun
   header...` spam) — and the 342 `K` rows that remain ARE `rows.txt`, byte for
   byte; verified on this tree.  `rows.txt` is the expectation source for the
   fixture, the sweep and CCProbeFloat; regenerate those three rather than
   editing a rendering by hand.

5. WHAT THE ENGINE ITSELF ACCEPTS  (the pass-through safety probe)

     ./3d_loader -t T -c T:/CC/GATES/hostcap.HC </dev/null

   18 bit patterns x 15 directive spellings = 270 rows, each
   `K <value-row> <directive-row> <StrLen> [<text>]`, and the run must print
   `DONE 18 values 15 directives`.  The two row numbers index the `bits[]` and
   `fmts[]` tables at the top of the file; the length comes from the engine's own
   output, so no ceiling claim depends on how a host tool counts.  Fewer than 270
   rows means something died: the interesting rows are `%+f`/`% f`/`%#f` printing
   the rest of the directive instead of a number, `%-10.3f` printing
   right-justified text, `inf` as `i`, the max engine-reported length (248), and
   the huge-magnitude rows.  The 15 were chosen by measuring what the other
   spellings do from the ONE argv slot a lite call can offer: `%e` and `%g` work
   but spell 1.5 as `1.50000000e0` (C: `1.500000e+00`) and `%E`/`%G` print
   nothing, which is why they are refused rather than delegated; `%s` FAULTS,
   because it dereferences the slot as a pointer and the slot holds a bit
   pattern — that is what made a 28-directive version of this probe stop after 48
   rows, and no try/catch catches it (a HolyC `throw` IS catchable; this is not a
   throw); and a two-slot call is not even expressible in this HolyC — see
   hostcapvararg.HC, which records the parse error.  Buffer is 512 bytes and
   `StrPrintJoin` gets no length, so this probe sees only renderings under 512.

6. CITATION AUDIT  (host shell tool; no loader needed)

     sh T/CC/GATES/citeck.sh T/CC/PLAN.md T/CC/CC_TEST.HC T/CC/CC.HH
         T/CC/CC_LIB.HC T/CC/CC_PARSE.HC T/CC/CC_EMIT.HC T/CC/INC/*.h
         T/CC/TEST/*.c T/CC/GATES/*.txt T/CC/GATES/*.HC T/CC/GATES/genm38.c
         tracking/suggestions/*.md

   Prints every `T:/file:NNN` reference in the arguments together with the lines
   it lands on, and flags MISSING TARGET and OUT OF RANGE.  It does not judge
   whether a citation supports the sentence that cites it — the printed lines
   are there for the author to read.  Run it after any change that moves code in
   a file another file quotes lines from: a PLAN that cites `CC_EMIT.HC:508` is
   wrong the moment someone adds a comment above line 508, and nothing but this
   audit notices.  Give it the files that HOLD citations, not just the plan:
with no arguments it now refuses, because a run that scanned nothing
reported nothing and read as a clean audit on 2026-11-05 while 61 fresh
citations went unchecked.

7. REBUILDING THE KERNEL IMAGE  (only needed when a file HCRT_TOS.HC includes
   changes; nothing under T:/CC/ is in the image)

     ./3d_loader -t T -c T:/CC/GATES/rebuild.HC           # writes T:/HCRT.NB.*
     cp T/HCRT.NB.BIN HCRT.BIN && cp T/HCRT.NB.BIN T/HCRT.BIN
     cp T/HCRT.NB.DBG.Z T/HCRT.DBG.Z

   `rebuild.HC` runs the same call the GUI does, `Cmp("FULL_PACKAGE.HC",
   "HCRT.NB.DBG.Z","HCRT.NB.BIN")`, but to a SCRATCH name and only after
   `Cd("T:/")`.  Both details matter.  A -c run starts in the HOST cwd mounted
   as Z: (`main.c:167-176`), so a relative name is not `T:/` - and a failed
   `Cmp` does not merely fail, it overwrites its `out_name` with an empty
   module: 48 bytes here, after which every boot dies with a bare SIGSEGV and
   no message, because `tos_aot.c:250` dereferences an unchecked symbol lookup
   before `SetupDebugger()`.  Expect `Errs:0 ... Code:105445 Size:110E70` and a
   `T:/HCRT.NB.BIN` of the same order as the tracked `HCRT.BIN` (1115856 bytes
   before the 2026-11-05 rebuild, 1117808 after).  The image and its `.DBG.Z`
   are a pair - `FULL_PACKAGE.HC`'s boot reads one - so move them together.
   Then re-run gates 1-5 against the new image: gate 4's 342 `K` rows must come
   out byte-identical, because they are the kernel's own renderings.

8. WHY THE REBUILD USED TO DIE  (host shell tool; ~60 s, writes nothing)

     sh T/CC/GATES/abprobe.sh

   Compiles one construct six ways against the current kernel and prints which
   spellings the compiler accepts.  See the header of `abprobe.sh` for the rule
   it measures and for the five source files it took to find: the kernel's
   header spells its Greek letters as TEXT (`#define \E3 pi`, `F64 \E8`), and a
   raw 0xE3 standing alone as a token is rejected, which is what made
   `T:/AMATH.HC` - and only that file, of 155 - stop the whole build with
   `Code:0 Size:0`.  Run this before concluding the compiler is broken.

9. THE MATH BUILTINS  (M38: sqrt/pow/sin/... delegated to the kernel's own
   entries, and pinned against what the kernel prints for them)

      cd <repo root>
      gcc -O1 -Wall -Wextra -o T/CC/GATES/genm38 T/CC/GATES/genm38.c
      sh T/CC/GATES/mthgate.sh

    `genm38.c` reads `mthrows.txt` — 34 rows, `K <nn> <kernel expression>
    [<rendering>]` — and writes the two fixtures, both self-checking in
    ffmt.c's shape: a cell that matches adds 1 to `tot`, so `rc` IS the pass
    count and a clean run returns 34.

      T:/CC/TEST/mth.c    sprintf's `%.17f`, compared byte for byte against
                          the pinned text; `Ms<nn>|<got>|<want>|` rows.
      T:/CC/TEST/mthp.c   printf's own `%f` in the same row (`Mp<nn>|...`), so
                          the newer sink is visible in the log, while the cell
                          is counted by a sprintf of the same call against the
                          same text — exactly how gs0.c scores a printf cell.

    The wrapper regenerates both from the table, runs them in ONE session
    through `mthgate.HC` (`RUN ... rc=34` twice), requires `TOT mth 34 34` and
    `TOT mthp 34 34`, then compares the got and want columns of every `M[sp]`
    row and prints any row where they differ.  Its last line is `VERDICT:
    PASS` or `VERDICT: FAIL`, and it exits nonzero on the failure.  CC_REPO
    picks a different checkout, as for the generators above; the compiled
    `genm38` is host scratch like `genm37` and is not committed.

    The generator refuses to write ANYTHING unless the table holds exactly 34
    `K` rows numbered 01 to 34 with no gap and no duplicate, each with a
    non-empty rendering that fits a fixture buffer, and unless row `<nn>`'s
    kernel expression is the entry the fixture's cell calls.  That check is
    the point of the tool living here rather than in `/tmp`: a fixture that
    quietly lost cells still looks like a self-checking program while `rc`
    drops, and an expectation that drifted from the expression it describes
    reports a PASS for the wrong call.  Both are refusals now.

    WHERE THE PINNED TEXT COMES FROM, and how to check it again.  The header
    of `mthrows.txt` records the md5 of the `HCRT.BIN` and of the `3d_loader`
    it was measured with — that line IS its provenance, so a rendering is
    never hand-edited and never taken from a C library: it is RE-MEASURED, and
    if a re-measurement moves a rendering, the header's md5s move with it
    (rebuild the image first, README.txt gate 7).  `mthmeas.HC` writes the 34
    kernel calls out one per cell, renders each through `StrPrintJoin` at
    `%.17f` — a bare `%f` is precision 0 here — and prints `K <nn>
    [<rendering>]` and nothing else:

      ./3d_loader -t T -c T:/CC/GATES/mthmeas.HC </dev/null > meas.log
      grep '^K ' meas.log | awk '{print $2, $NF}' > got.txt
      awk '/^K /{print $2, $NF}' mthrows.txt > want.txt
      diff want.txt got.txt    # expect: no output, 34 rows on each side

    Why the kernel and not libm: M38 hands the kernel's entry the argument and
    prints what comes back, so the kernel's text IS the answer.  Where it
    differs from C — `Round` is nearest-even, `Pow(-2.0,0.5)` returns a real
    and not a NaN, and note that `Sqrt(4.0)` PRINTS `1.99999999999999520` even
    though its quad IS exactly 2.0 (mathulp.HC below) — the difference is
    the finding, and a glibc diff would hide it.  Same reason as the `%f`
    gates below: compiling these fixtures with gcc measures the host, not lite.

  Two probes ride along, because a rendering is not a value and the fixture does
  not enter the hard domains:

    ./3d_loader -t T -c T:/CC/GATES/mathulp.HC </dev/null
    ./3d_loader -t T -c T:/CC/GATES/edgemth.HC </dev/null

  `mathulp.HC` separates the two effects.  Its R lines render BIT PATTERNS of
  doubles whose exact decimals are known (1.0, 2.0, 2.5, 3.0, 1024) and the
  kernel prints them several ulps low -- 1.0 comes out `0.99999999999999760` --
  so no `%f` can ever settle an ulp question.  Its U lines settle it at the quad,
  printing `kernel answer - correctly-rounded answer` as an integer (the
  reference column, with each real value to 60 digits, is `mathref.txt`), and
  24 of 30 of them come out 0.  `edgemth.c` with `edgemth.HC` covers what the
  fixture never reaches: NaN, the infinities, signed zero, overflow and
  underflow -- 15 cells, each printed by lite and by the kernel's own call in the
  same session, all 15 agreeing: `sqrt(-1)` gives `-92.23372036854775808`, an
  infinity gives `i` or `-i`, `sqrt(-0.0)` keeps the sign and `fabs(-0.0)` does
  not.  Neither file pins a golden: they are probes, and what they establish is
  recorded in T:/CC/PLAN.md.

GENERATORS  (host tools; gcc/clang, NOT part of the compiled tree)
------------------------------------------------------------------

  genm37.c    ./genm37 sweep  T:/CC/GATES/gs   -> gs0.c .. gs9.c
              ./genm37 driver T:/CC/GATES/gs 10 -> gsd.HC
              ./genm37 expect T:/CC/GATES/gs    -> the 342 golden printf
              lines;  reads rows.txt beside the base path.
  genffmt.c   ./genffmt                        -> the ffmt.c fixture body
              ./genffmt probe                  -> CC_TEST.HC's CCProbeFloat
              body;  a second argument overrides the table path (default
              T:/CC/GATES/rows.txt).
  mkrefuse.c  ./mkrefuse                       -> r01.c..r15.c, acc.c,
              rfdriver.HC
  splice_probe.sh  sh T/CC/GATES/splice_probe.sh <probe-file>
              -> replaces the CCProbeFloat block inside T:/CC/CC_TEST.HC,
              writing through /tmp because in-place edits on the mounted
              DrvFs can fail their rename and DELETE the target.

Each writes into the directory named by its `T:/CC/GATES/...` argument:
`T:/CC/GATES/x` means `<repo root>/T/CC/GATES/x`.  Set `CC_REPO` to point at a
different checkout (a worktree, for instance).  A generated file is stamped with
the T:/ path it was generated as, so a hand edit inside a generated program is
visible in the next regeneration diff.  The compiled generators themselves
(`genm37`, `genffmt`, `mkrefuse`) are host scratch and are NOT committed; build
one with `cc -O1 -o genm37 genm37.c` in this directory.  Only their `.c` sources,
the tables, and the programs that decide something are in the tree - which is
why `rows.txt` is the one file here whose lines exceed 83 characters: it is a
table of renderings, one rendering per line, not a program.

LINE LENGTH
-----------

Every file in this directory is within the tree's 83-character discipline,
including the generated sweep programs: an expectation is loaded in pieces
of at most 68 characters so a `strcpy(w,"");` line stays legal, and the
`ck()` signature is short enough to fit.  T:/CC/TEST/ffmt.c does the same
because LITE refuses a generated-C literal longer than CC_TOK_MAX (128).
Nothing here needs an exception, and the kernel's own ceilings (a digit
walk stopping at 248 characters, a measured rendering of 250) are well
inside what a piecewise-built expectation can hold.

WHY THESE AND NOT A gcc DIFF
----------------------------

lite hands the kernel's format engine the directive and prints what comes back,
so the kernel's rendering IS the answer; the 63 pinned cells in
T:/CC/TEST/ffmt.c and the 342 in rows.txt are the kernel's text, not glibc's.
Where the two disagree — `%.15f` of 1.5 is `1.499999999999998` here and
`1.500000000000000` in glibc — the divergence is a documented non-standard (see
T:/CC/PLAN.md and tracking/suggestions/), and 328 of this sweep's 684 sprintf
cells are on the non-C side of that line.  Compiling the same programs with gcc
therefore measures the HOST, not lite: it is a useful number, and it is the
wrong oracle.  Nothing here diffs a float line against gcc's stdout.
