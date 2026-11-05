#!/bin/sh
# Splice a generated HolyC body over the M37 float probe inside CC_TEST.HC.
# The probe is GENERATED (by T:/CC/GATES/genffmt.c `probe` mode) from
# T:/CC/GATES/rows.txt, so it is replaced whole rather than hand-edited: a
# rendering that drifted in the kernel shows up as a regenerated body, not as a
# comment someone updated.
#
#   sh T/CC/GATES/splice_probe.sh <generated-probe-file>
#
# Writes via /tmp and `cp` because in-place editors on the mounted DrvFs can
# fail their rename and DELETE the target (this bit us twice on this drive).
set -e
SRC=${1:-/dev/stdin}
ROOT=${CC_REPO:-/mnt/d/Source/3Days}
TGT=$ROOT/T/CC/CC_TEST.HC
TMP=${TMPDIR:-/tmp}/opencode/splice.HC
mkdir -p "${TMPDIR:-/tmp}/opencode"

START=$(grep -n '^/\* M37: the KERNEL side of the ffmt table\.' "$TGT" | cut -d: -f1)
END=$(grep -n '^I64 CCProbeFloat()' "$TGT" | cut -d: -f1)
END=$(awk 'NR>=n {if ($0 == "}") {print NR; exit}}' n="$END" "$TGT")
[ -n "$START" ] && [ -n "$END" ] || { echo "probe block not found in $TGT"; exit 1; }

{
  head -n $((START - 1)) "$TGT"
  cat "$SRC"
  tail -n +"$((END + 1))" "$TGT"
} > "$TMP"

cp "$TMP" "$TGT"
echo "replaced lines $START-$END with $(wc -l < "$SRC") lines; file now $(wc -l < "$TGT") lines"
