#!/bin/sh
# mthgate.sh - the M38 math-builtins gate (host wrapper; POSIX sh).
#
# What it decides, in this order:
#   1. genm38.c still compiles without a warning, and both fixtures are
#      REGENERATED from mthrows.txt before anything is measured, so a hand
#      edit inside a fixture cannot be what gets gated: the generator either
#      overwrites it or refuses to write at all (see its header).
#   2. both fixtures run through lite in ONE session (mthgate.HC) and each
#      returns its own pass count: `rc=34` twice, plus a `TOT <name> 34 34`
#      line.  rc is the number of cells that PASSED -- the T:/CC/TEST/ffmt.c
#      convention -- so a short count IS the failure and rc=0 means nothing
#      checked out.  The loader's exit code is always 0 (AGENTS.md), so
#      grepping the log is the only verdict there is.
#   3. the printf sink's own rendering equals the pinned kernel text: the Mp
#      rows carry `Mp<nn>|<got>|<want>|` and every row whose two columns
#      differ is printed by name.  The Ms rows are checked the same way, so
#      the sprintf sink is proved in the LOG and not only inside the program,
#      and `want` on both is what genm38.c read out of mthrows.txt.
#
# The oracle is the kernel, not a C library: `want` is what TempleOS's own
# float printer wrote for the same call (mthrows.txt, re-measurable with
# mthmeas.HC -- README.txt gate 9).  Nothing here compiles these fixtures with
# gcc and nothing here diffs a float against glibc: that would measure the
# host and not lite, which is the wrong oracle for a delegated conversion.
#
# Usage:  sh T/CC/GATES/mthgate.sh
#         CC_REPO=/path/to/tree sh T/CC/GATES/mthgate.sh
# Needs 3d_loader and HCRT.BIN in that tree, as abprobe.sh does, and gcc.
set -u
S0=$(dirname "$0")
R=${CC_REPO:-$(cd "$S0/../../.." && pwd)}
G="$R/T/CC/GATES"
L="$R/3d_loader"
S=${TMPDIR:-/tmp}/opencode
LOG="$S/mthgate.log"
DIF="$S/mthgate.diff"
bad() { echo "FAIL: $*"; echo "VERDICT: FAIL"; exit 1; }
[ -d "$G" ] || bad "no $G -- point CC_REPO at the checkout to gate"
[ -f "$G/mthrows.txt" ] || bad "no $G/mthrows.txt: no oracle to gate against"
[ -x "$L" ] || bad "no $L (make it, or copy it in)"
[ -f "$R/HCRT.BIN" ] || bad "no $R/HCRT.BIN -- the boot reads it from the cwd"
mkdir -p "$S" || bad "cannot create $S"
cd "$R" || bad "cannot cd to $R"

# 1. build the generator when the source is newer, then regenerate the fixtures
if [ ! -x "$G/genm38" ] || [ "$G/genm38.c" -nt "$G/genm38" ]; then
  gcc -O1 -Wall -Wextra -o "$G/genm38" "$G/genm38.c" \
    || bad "genm38.c does not compile clean"
fi
CC_REPO="$R" "$G/genm38" || bad "genm38 refused to write the fixtures"

# 2. both sinks through lite in one session.  A -c run Cd()s the task into the
# process cwd with the host root mounted at Z:, which is why mthgate.HC gives
# CCRunFile T:/ paths and this script does not rely on where it started.
timeout 1200 stdbuf -oL "$L" -t T -c "T:/CC/GATES/mthgate.HC" </dev/null \
  > "$LOG" 2>&1
rc=$?
[ "$rc" = 0 ] || bad "the -c run ended with $rc (124 is a timeout; a fault" \
  "left the debugger waiting for input, and the tail of $LOG says where)"
if grep -aE '^(CC )?ERROR:|ENTERED FAULT' "$LOG" > "$DIF"; then
  cat "$DIF"
  bad "lite reported an error or the session faulted -- see $LOG"
fi
rc_s=$(sed -n 's|^RUN T:/CC/TEST/mth\.c rc=||p' "$LOG" | tail -1)
rc_p=$(sed -n 's|^RUN T:/CC/TEST/mthp\.c rc=||p' "$LOG" | tail -1)
[ "$rc_s" = 34 ] || bad "mth.c gave rc=$rc_s, want 34 (its own cell count)"
[ "$rc_p" = 34 ] || bad "mthp.c gave rc=$rc_p, want 34"
grep -a -q -x 'TOT mth 34 34' "$LOG" || bad "no 'TOT mth 34 34' line in $LOG"
grep -a -q -x 'TOT mthp 34 34' "$LOG" || bad "no 'TOT mthp 34 34' line in $LOG"

# 3. the rendering in the log against the pinned text sitting in the same row
awk -F'|' '/^M[sp][0-9][0-9]\|/ {
  if ($2 != $3) print "DIFFERS " $1 " got [" $2 "] want [" $3 "]"
}' "$LOG" > "$DIF"
n=$(grep -a -c -E '^M[sp][0-9][0-9]\|' "$LOG")
[ "$n" = 68 ] || bad "saw $n Ms/Mp rows, want 34 of each (a row that never" \
  " printed is a cell that never ran)"
if [ -s "$DIF" ]; then
  cat "$DIF"
  bad "lite's rendering is not what the kernel printed for the same call"
fi
grep -aE '^RUN |^TOT ' "$LOG"
echo "rows checked: $n (Ms 34 + Mp 34), rows differing: 0"
echo "log: $LOG"
echo "VERDICT: PASS"
