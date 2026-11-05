#!/bin/sh
# Citation audit (host tool).  Pulls every `T:/path:NNN` reference out of
# the given files and prints the lines it points at, so the author can see
# whether each citation lands on what the prose claims.  It prints the target,
# it does not judge it: read the printed lines.  Run from the repo root, because
# the TempleOS drive prefix `T:/` is the directory `T/` on this host.
#
#   sh T/CC/GATES/citeck.sh T/CC/PLAN.md T/CC/CC_TEST.HC
#
# The scratch file below lives under /tmp because it is throwaway; nothing a
# gate needs in order to survive a host reboot lives there.
SCRATCH=${TMPDIR:-/tmp}/opencode/citeck.list
# No arguments means no files were scanned, and a scan that scanned nothing
# reports no problems.  That is not a pass, so it refuses: on 2026-11-05 a bare
# `citeck.sh` exited 0 having checked none of the 61 citations the M38 change
# had just added, and read as a clean audit.
if [ $# -eq 0 ]; then
  echo "usage: sh T/CC/GATES/citeck.sh FILE [FILE...]"
  echo "no arguments means nothing was checked, which is not a pass: refusing"
  exit 1
fi
for f in "$@"; do
  [ -f "$f" ] || continue
  if [ ! -d T/CC ]; then
    echo "run me from the repo root (no T/CC here): citations print as MISSING"
    exit 1
  fi
  grep -oE 'T:/[A-Za-z0-9_/.-]*\.(HC|HH|MD|C|c|h):[0-9]+(-[0-9]+)?' "$f" \
    | sort -u > "$SCRATCH"
  while read -r ref; do
    ln="${ref##*:}"			# the 137 of T:/CC/CC.HH:137
    tgt="T${ref#T:}"			# T:/CC/CC.HH:137 -> T/CC/CC.HH:137
    tgt="${tgt%:*}"			# -> T/CC/CC.HH
    a=$(printf '%s' "$ln" | cut -d- -f1)
    b=$(printf '%s' "$ln" | cut -d- -f2)
    [ -z "$b" ] && b=$a
    if [ ! -f "$tgt" ]; then
      echo "MISSING TARGET $ref (in $f)"
      continue
    fi
    end=$(wc -l < "$tgt")
    if [ "$a" -gt "$end" ]; then
      echo "OUT OF RANGE $ref (in $f) -- $tgt has $end lines"
      continue
    fi
    printf '### %s  (cited by %s)\n' "$ref" "$f"
    awk -v a="$a" -v b="$b" 'NR>=a && NR<=b {printf "    %d: %s\n", NR, $0}' "$tgt"
  done < "$SCRATCH"
done
