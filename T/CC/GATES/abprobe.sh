#!/bin/sh
# abprobe.sh - the token rule that blocks `Cmp("FULL_PACKAGE.HC",...)`
#
# WHY THIS EXISTS: on 2026-11-05 the kernel rebuild died with
#     ERROR: Invalid lval at "<E3>" T:/AMATH.HC,19   (Errs:2 Code:0 Size:0)
# i.e. the compiler emitted NOTHING, and finding out why took ~15 loader runs.
# This reproduces the rule in ~60 s and writes nothing it does not clean up.
# Run it before "fixing" the compiler or the sources.
#
# THE RULE, measured against ./HCRT.BIN (the rebuild of 2026-11-05 gives the
# same verdicts as the 2022 image it replaced):
#   raw 0xE3 standing ALONE as a token .................. REJECTED
#   raw 0xE3 as a parameter DEFAULT VALUE .............. REJECTED
#   raw 0xE3 INSIDE a longer identifier (\xE3_x shape) .. ACCEPTED
#   KERNELA's own text spelling (\E3, \E8, \E9, \EA) .... ACCEPTED
#   plain ASCII (e, pi) ................................ ACCEPTED
# Adding `#define \E9 e` does NOT make a lone raw 0xE3 compile, so this is a
# token/lexer rule and not a missing macro.
#
# THREE CONTAMINATION TRAPS this script discovered the hard way, each of which
# produced a confident WRONG answer before it was understood:
#  1. Each case needs its OWN function name.  Cases sharing a name interfere:
#     a case that compiles clean alone, and clean even immediately after two
#     FAILED parses, reported `Not in fun. Can't return a val` when the case
#     before it had failed under the SAME name.  A failed parse leaves the
#     symbol defined, so "redefine it next" is not "define it fresh".
#  2. A probe line must be valid HolyC.  A first version returned a value from
#     a U0 function and the fault was blamed on the byte being tested.
#  3. `.gitattributes` says `* text=auto`, so a git BLOB is LF while the
#     working-tree file is CRLF.  Split git-sourced bytes on LF, never CRLF:
#     splitting on CRLF found zero matching lines and the script died with an
#     IndexError having run no compiler at all.
# The verdict marker goes BEFORE each Cmp, because the compiler prints its own
# `Errs:` summary DURING the compile, so an after-marker sits above the wrong
# case's numbers.  Cmp's return value is no help: T:/CMAIN.HC:480 says "Returns
# err_cnt", but a call from JIT'd HolyC hands back a pointer in RAX (three
# different builds all printed addresses) and nothing in the tree uses it.
#
# KERNELA.HH - the header the kernel's whole surface is declared in - spells
# every one of these characters as TEXT and holds no raw high byte at all:
# `#define \E3 pi` :360, `F64 \E8` :4582, `F64 \EA` :4583, `F64 \E9` :4584,
# `F64 \E9_x` :5421.  The DEFINITIONS spelled them as raw bytes: T:/AMATH.HC
# (12), T:/DOLDOC/DocGr.HC (6), T:/GR/GrMath.HC (9), T:/GR/GrSpritePlot.HC (6),
# T:/GR/SpriteEd.HC (3) - the complete set out of the 155 files reachable from
# FULL_PACKAGE.HC.
#
# WHAT THE SHIPPED IMAGE STORES (read straight out of ./HCRT.BIN):
#   extern I64 *Mat4x4RotX(I64 *m,F64 <E8>);   raw byte in a prototype
#   F64 <E9>_z\tformat ...                    raw byte in a class field name
#   #define <E3> pi                           raw byte in a macro name
# so the object's spelling is the raw byte while the source's is the text
# escape: the compiler converts one to the other, and which source spellings it
# accepts is what this script measures.  That is also why patching definitions
# and nothing else was enough: KERNELA.HH:4340 already spells the very
# prototype this file tests as `Wrap(F64 e,F64 base=-pi)`.
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
# AMATH comes from git, so this keeps measuring the RAW-BYTE form after the tree
# was patched.  HEAD carries it until the fix lands; afterwards the pre-fix
# commit does.  A bad rev makes `git show` print nothing, so a rewritten history
# falls through to the error below rather than reporting a false result.
def blob(rev):
  b = subprocess.run(['git', '-C', r, 'show', '%s:T/AMATH.HC' % rev],
                     stdout=subprocess.PIPE).stdout
  return b.replace(b'\r\n', b'\n')          # a blob is LF, see header note 3
raw = None
for rev in ('HEAD', '793cc19^'):
  b = blob(rev)
  if b'\xe9' in b and b'\xe3' in b:
    raw, where = b, rev
    break
if raw is None:
  sys.exit('no T/AMATH.HC with the raw pi/tau bytes in HEAD or 793cc19^: '
           'the history this probe reads has been rewritten')
sig = [l for l in raw.split(b'\n') if l.startswith(b'public F64 Wrap')][0]
i9, i3 = sig.index(b'\xe9'), sig.index(b'\xe3')

def src(name, s):
  """one function, named `name`, with a body that is known to compile"""
  return (s.replace(b'Wrap', name) +
          b'\n{\n  F64 res;\n  res = 1.0;\n  return res;\n}\n')

B = lambda x: x.encode('latin1')
cases = [
  ('A0RAW', src(B('QW0'), sig),
                'as shipped: raw E9 name + raw E3 default value'),
  ('A1VAL', src(B('QW1'), sig[:i3] + B('\\E3') + sig[i3 + 1:]),
                'value as text \\E3, name still raw E9'),
  ('A2NAM', src(B('QW2'), sig[:i9] + B('\\E9') + sig[i9 + 1:]),
                'name as text \\E9, value still raw E3'),
  ('A3TXT', src(B('QW3'),
                sig.replace(b'\xe9', B('\\E9')).replace(b'\xe3', B('\\E3'))),
                'both as KERNELA text escapes'),
  ('A4ASC', src(B('QW4'),
                re.sub(rb'\xe9', B('e'), re.sub(rb'\xe3', B('pi'), sig))),
                'ASCII e / pi = the KERNELA.HH:4340 spelling'),
  # A5 and A6 are ONE body spelled two ways - the pi byte ALONE as an operand
  # versus the same byte INSIDE an identifier, the shape KERNELA.HH:5421 uses
  # for its `\E9_x` field.  Different function names, on purpose: see header.
  ('A5LONE', b'public F64 QZ5(F64 x)\n{\n  F64 res;\n  res = x + \xe3;\n'
             b'  return res;\n}\n',
                'lone raw E3 as an operand'),
  ('A6INID', b'public F64 QZ6(F64 \xe3_in)\n{\n  F64 res;\n  res = \xe3_in;\n'
             b'  return res;\n}\n',
                'raw E3 inside an identifier'),
]
for tag, s, _ in cases:
  open(r + '/T/CC/GATES/ab%s.HC' % tag, 'wb').write(head + s)

run = [b'Cd("T:/");\n']
for tag, _, _ in cases:
  run.append(B('"AB %s\\n";\n' % tag))                     # marker BEFORE cmp
  run.append(B('Cmp("T:/CC/GATES/ab%s.HC", "T:/CC/GATES/ab%s.DBG.Z");\n'
               % (tag, tag[:2])))
run.append(b'Exit;\n')
open(r + '/T/CC/GATES/abrun.HC', 'wb').write(b''.join(run))
print('AMATH read from %s: parameter byte %02X, default-value byte %02X'
      % (where, sig[i9], sig[i3]))
open('/tmp/opencode/abprobe.expected', 'w').write(
  '\n'.join('%-7s %s' % (t, d) for t, _, d in cases) + '\n')
PY

timeout 900 stdbuf -oL "$L" -t T -c "T:/CC/GATES/abrun.HC" </dev/null 2>/dev/null \
  > /tmp/opencode/abprobe.log

cat /tmp/opencode/abprobe.expected
echo '--- verdicts: each CASE line is followed by ITS OWN compiler summary ---'
echo '--- a "-> PASS" line means that case compiled (Errs:0, Code and Size) ---'
SELMATCH='^AB |^ERROR: Invalid lval|^ERROR: Expecting|^Errs'
grep -aE "$SELMATCH" /tmp/opencode/abprobe.log |
  sed -e 's/^AB /CASE /' -e 's/^Errs:0.*/  -> PASS/' \
      -e 's/^Errs:[1-9].*/  -> FAIL: compiler emitted nothing/' \
      -e 's/\(.\{79\}\).*/\1/'
rm -f "$R"/T/CC/GATES/ab*.HC "$R"/T/CC/GATES/ab*.DBG.Z "$R"/T/CC/GATES/ab*.BIN.Z
