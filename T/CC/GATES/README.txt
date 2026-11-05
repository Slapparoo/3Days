T:/CC/GATES — the M37 float-printing gate battery
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

     grep -E '^A[0-9][0-9]\|' refuse.log | cut -d"|" -f2- | sort > a.txt
     grep -E '^B[0-9][0-9]\|' refuse.log | cut -d"|" -f2- | sort > b.txt
     grep -E '^K[0-9][0-9]\|' refuse.log | head -11 | cut -d"|" -f2- | sort > k.txt
     diff a.txt k.txt ; diff b.txt k.txt    # expect: no output, both

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

     sh T/CC/GATES/citeck.sh T/CC/PLAN.md T/CC/CC_TEST.HC tracking/suggestions/*.md

   Prints every `T:/file:NNN` reference in the arguments together with the lines
   it lands on, and flags MISSING TARGET and OUT OF RANGE.  It does not judge
   whether a citation supports the sentence that cites it — the printed lines
   are there for the author to read.  Run it after any change that moves code in
   a file another file quotes lines from: a PLAN that cites `CC_EMIT.HC:508` is
   wrong the moment someone adds a comment above line 508, and nothing but this
   audit notices.

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
visible in the next regeneration diff.

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
