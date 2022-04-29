#!/bin/tcsh
set CFiles= ( \
 "main.c"  \
 "ext/vec/src/vec.c" \
 "ext/map/src/map.c" \
 "TOSPrint.c" \
 "ext/C_Unescaper/escaper.c" \
 "ext/argtable3/argtable3.c" \
 "rl.c" \
 "runtime.c" \
 "poopalloc.c" \
 "aot.c" \
)
set AsmFiles = ( \
  "exceptWin.yasm" \
  "debuggerAsm.yasm" \
  "FFI_WIN64.yasm" \
  "TOSJMP.yasm" \
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
  "3d.h" \
  "poopalloc.h" \
  "alloc.h" \
  "rl.h" \
)

set MiscFiles = ( \
  "make.tcsh" \
  "filelist.tcsh" \
  "HCRT/HCRT_TOS.BIN" \
)
