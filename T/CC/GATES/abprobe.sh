#!/bin/sh
# abprobe.sh - the token rule that blocks `Cmp("FULL_PACKAGE.HC",...)`
#
# WHY THIS EXISTS: on 2026-11-05 the kernel rebuild died with
#     ERROR: Invalid lval at "<E3>" T:/AMATH.HC,19   (Errs:2 Code:0 Size:0)
# i.e. the compiler emitted NOTHING, and finding out why took ~15 loader runs.
# This reproduces the rule in ~60 s and writes nothing it does not clean up.
# Run it before "fixing" the compiler or the sources.
#
# THE RULE, measured against the committed ./HCRT.BIN (md5 5215fd5a...):
#   raw 0xE3 standing ALONE as a token .......... REJECTED
#   raw 0xE3 / 0xE8 / 0xE9 / 0xEA inside a longer identifier ... ACCEPTED
#   KERNELA's own text spelling (\E3, \E8, \E9, \EA) ............ ACCEPTED
#   plain ASCII (e, pi) ........................................ ACCEPTED
#   a parameter DEFAULT VALUE holding a lone raw 0xE3 ........... REJECTED
#   the same value as text \E3 ................................. ACCEPTED
# Adding `#define \E9 e` does NOT make a lone raw 0xE3 compile, so this is a
# token/lexer rule and not a missing macro.
#
# KERNELA.HH - the header the kernel's whole surface is declared in - spells
# every one of these characters as TEXT and contains no raw high byte at all:
# `#define \E3 pi` at :360, `F64 \E8` at :4582, `F64 \EA` at :4583,
# `F64 \E9` at :4584, `F64 \E9_x` at :5421.  It was the DEFINITIONS that spelled
# the same tokens as raw bytes: T:/AMATH.HC (12), T:/DOLDOC/DocGr.HC (6),
# T:/GR/GrMath.HC (9), T:/GR/GrSpritePlot.HC (6), T:/GR/SpriteEd.HC (3).  That
# is the whole set - I walked all 155 files reachable from FULL_PACKAGE.HC.
#
# WHAT THE SHIPPED IMAGE STORES (read straight out of ./HCRT.BIN):
#   extern I64 *Mat4x4RotX(I64 *m,F64 <E8>);     raw byte in the object
#   F64 <E9>_z\tformat ...                       raw byte in a class field name
#   #define <E3> pi                              raw byte in a macro name
# so the object's spelling is the raw byte while the source's is the text
# escape: the compiler converts one to the other, and which source spellings it
# accepts is what this script measures.  That is also why patching a definition
# to ASCII `pi`/`e` - and nothing else - is enough: KERNELA.HH:4340 already
# spells that very prototype `Wrap(F64 e,F64 base=-pi)`.
#
# Usage:  sh T/CC/GATES/abprobe.sh
#         CC_REPO=/path/to/tree sh T/CC/GATES/abprobe.sh
# Needs 3d_loader in that tree (it is gitignored: `make` it, or copy it in).
set -u
R=${CC_REPO:-/mnt/d/Source/3Days}
L="$R/3d_loader"
[ -x "$L" ] || { echo "no $L in $R"; exit 1; }
[ -f "$R/HCRT.BIN" ] || { echo "no $R/HCRT.BIN"; exit 1; }
cd "$R" || exit 1

python3 - "$R" <<'PY'
import re, subprocess, sys
r = sys.argv[1]
lines = open(r + '/T/KERNELA.HH', 'rb').read().split(b'\n')
head = b'\n'.join(lines[:366]) + b'\n'
# AMATH comes from git, so this script keeps working once the tree is patched
am = subprocess.run(['git', '-C', r, 'show', 'HEAD:T/AMATH.HC'],
                    stdout=subprocess.PIPE).stdout
sig = [l for l in am.split(b'\r\n') if l.startswith(b'public F64 Wrap')][0]
sig = sig.replace(b'Wrap', b'QU')
body = b'\r{\r  F64 res;\r  res = 1.0;\r  return res;\r}\r\n'
i9, i3 = sig.index(b'\xe9'), sig.index(b'\xe3')
cases = [
  ('A0RAW', sig,                                     'as shipped: raw E9 name + '
   'raw E3 default value'),
  ('A1VAL', sig[:i3] + b'\\E3' + sig[i3 + 1:],       'value as text \\E3, name '
   'still raw E9'),
  ('A2NAM', sig[:i9] + b'\\E9' + sig[i9 + 1:],       'name as text \\E9, value '
   'still raw E3'),
  ('A3TXT', sig.replace(b'\xe9', b'\\E9').replace(b'\xe3', b'\\E3'),
                                                  'both as KERNELA text escapes'),
  ('A4ASC', re.sub(rb'\xe9', b'e', re.sub(rb'\xe3', b'pi', sig)),
                                                  'ASCII e / pi = KERNELA:4340'),
  ('A5LONE', b'public F64 QZ(F64 x)\n{\n  F64 res;\n  res = x + \xe3;\n'
             b'  return res;\n}\n',                 'lone raw E3 as an operand'),
  ('A6INID', b'public F64 QZ(F64 x)\n{\n  F64 \xe3_in;\n  \xe3_in = x;\n'
             b'  return \xe3_in;\n}\n',             'raw E3 inside an identifier'),
]
for tag, s, _ in cases:
  open(r + '/T/CC/GATES/ab%s.HC' % tag, 'wb').write(head + s + body)
run = [b'Cd("T:/");\n']
for tag, _, _ in cases:
  run.append(('Cmp("T:/CC/GATES/ab%s.HC", "T:/CC/GATES/ab%s.DBG.Z");\n'
              % (tag, tag[:2])).encode('latin1'))
  run.append(('AB %s\n' % tag).encode('latin1'))
run.append(b'Exit;\n')
open(r + '/T/CC/GATES/abrun.HC', 'wb').write(b''.join(run))
print('shipped AMATH:19 parameter byte %02X, default-value byte %02X'
      % (sig[i9], sig[i3]))
open('/tmp/opencode/abprobe.expected', 'w').write(
  '\n'.join('%-7s %s' % (t, d) for t, _, d in cases) + '\n')
PY

timeout 900 stdbuf -oL "$L" -t T -c "T:/CC/GATES/abrun.HC" </dev/null 2>/dev/null \
  > /tmp/opencode/abprobe.log

cat /tmp/opencode/abprobe.expected
echo "--- verdicts, in the same order (PASS = Errs:0 Code:<n> Size:<n>) ---"
SELMATCH='^AB |^ERROR: Invalid lval|^ERROR: Expecting|^Errs'
grep -aE "$SELMATCH" /tmp/opencode/abprobe.log |
  sed -e 's/^AB /CASE /' -e 's/^Errs:0.*/  -> PASS/' \
      -e 's/^Errs:[1-9].*/  -> FAIL: compiler emitted nothing/' \
      -e 's/\(.\{79\}\).*/\1/'
rm -f "$R"/T/CC/GATES/ab*.HC "$R"/T/CC/GATES/ab*.DBG.Z "$R"/T/CC/GATES/ab*.BIN.Z
