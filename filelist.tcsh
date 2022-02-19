#!/bin/tcsh
set CFiles= ( \
 "main.c"  \
 "ext/vec/src/vec.c" \
 "ext/map/src/map.c" \
 "ext/myjit/myjit/jitlib-core.c" \
 "TOSPrint.c" \
 "ext/C_Unescaper/escaper.c" \
 "ext/argtable3/argtable3.c" \
 "rl.c" \
 "runtime.c" \
 "poopalloc.c" \
 "aot.c" \
 "ext/linenoise/linenoise.c" \
)
set AsmFiles = ( \
  "exceptLin.yasm" \
  "debuggerAsm.yasm" \
)

set HolyCFiles = ( \
  "SIGNAL.HC" \
  "PARSER.HC" \
  "PARSER.HH" \
  "LEXER.HH" \
  "HCRT/ASM.HC" \
  "HCRT/CHAR.HC" \
  "HCRT/FILE.HC" \
  "HCRT/HASH.HC" \
  "HCRT/HCRT.HH" \
  "HCRT/MATH.HC" \
  "HCRT/Opcodes.STR.HC" \
  "HCRT/QSORT.HC" \
  "HCRT/BITS.HC" \
  "HCRT/COMPRESS.HC" \
  "HCRT/FIFO.HC" \
  "HCRT/FS.HC" \
  "HCRT/HCRT.HC" \
  "HCRT/LEXER.HC" \
  "HCRT/PRINT.HC" \
  "MAKE_HCRT.HC" \
  "VECTOR.HC" \
  "HolyEd/EDITOR.HC" \
  "HolyEd/ROPE.HC" \
  "HolyEd/KEYS2.HC" \
  "HolyEd/FONT.HC" \
)

set HeaderFiles = ( \
  `find ext/ -name "*.h"` \
  "3d.h" \
  "poopalloc.h" \
  # IMPORTANT,these files are used like headers so they are put in HeaderFiles \
  "ext/myjit/myjit/llrb.c" \
  "ext/myjit/myjit/jitlib-debug.c" \
  "ext/myjit/myjit/code-check.c" \
  "ext/myjit/myjit/x86-common-stuff.c" \
)

set MiscFiles = ( \
  "make.tcsh" \
  "filelist.tcsh" \
  "HCRT/HCRT.BIN" \
)
