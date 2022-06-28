#!/bin/tcsh
set CFiles = ( \
 "./sound.c" \
 "./main.c"      \
 "./ext/vec/src/vec.c" \
 "./ext/map/src/map.c" \
 "./TOSPrint.c" \
 "./ext/C_Unescaper/escaper.c" \
 "./ext/argtable3/argtable3.c" \
 "./runtime.c" \
 "./poopalloc.c" \
 "./tos_aot.c" \
 "./WIN_window.c" \
 "./multic.c" \
 "./vfs.c" \
 "./CommandLineToArgvA.c" \
)
set AsmFiles = ( \
    "FFI_WIN64.yasm" \
    "swapctxWIN.yasm" \
)
