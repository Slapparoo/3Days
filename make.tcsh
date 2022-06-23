#!/bin/tcsh
set gFindLoc = `which find`
set binary = 3d_loader
set cc = gcc
source filelist.tcsh
set CFlags = "-Ofast -g3 `pkg-config --cflags portaudio-2.0`  -lm -fno-omit-frame-pointer -lpthread"
enter:
if ! -e $binary then
  foreach f ( $CFiles )
     $cc $CFlags -c $f -o $f.o || rm $f.o
  end
else
  set find = `$gFindLoc . -wholename  make.tcsh  -newer $binary `
  if( ! $#find ) rm $binary
  if( ! $#find ) goto enter
  foreach f ( $CFiles )
    set find = `$gFindLoc . -wholename    $f -newer $binary `
    if($#find)  $cc $CFlags -c $f -o $f.o || rm $f.o 
  end
endif

if ! -e $binary then
  foreach f ( $AsmFiles )
    yasm -f elf64 $f -o $f.o || rm $f.o
  end
else
  foreach f ( $AsmFiles )
    set find = `$gFindLoc . -wholename $f -newer $binary `
    if($#find) yasm -f elf64 $f -o $f.o || rm $f.o
  end
endif

set Objs = ()
foreach f ( $CFiles )
  set Objs = ( $Objs "$f.o" )
end
foreach f ( $AsmFiles )
  set Objs = ( $Objs "$f.o" )
end
$cc $Objs `pkg-config --libs portaudio-2.0` -lm -lpthread -lX11 -o $binary
