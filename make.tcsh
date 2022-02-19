#!/bin/tcsh
source filelist.tcsh
set CFlags = "-Os -g3 `sdl2-config --cflags --libs` -lm -fno-omit-frame-pointer"

if ! -e 3d_loader then
  foreach f ( $CFiles )
    gcc $CFlags -c $f -o $f.o
  end
else
  foreach f ( $CFiles )
    set find = `find -wholename $f -newer 3d_loader `
    if($#find) gcc $CFlags -c $f -o $f.o
  end
endif

if ! -e 3d_loader then
  foreach f ( $AsmFiles )
    yasm -f elf64 $f -o $f.o
  end
else
  foreach f ( $AsmFiles )
    set find = `find -wholename $f -newer 3d_loader `
    if($#find) yasm -f elf64 $f -o $f.o
  end
endif

set Objs = ()
foreach f ( $CFiles )
  set Objs = ( $Objs "$f.o" )
end
foreach f ( $AsmFiles )
  set Objs = ( $Objs "$f.o" )
end
gcc $Objs `sdl2-config --libs` -lm -o 3d_loader
