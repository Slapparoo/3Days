#!/bin/tcsh
set gFindLoc = `which find`
set binary = 3d_loader
set cc = gcc
if ! $#gFindLoc then
	echo "Install findutils for your BSD,im or if your up to it,port this file to use the BSD find."
	exit
endif
source filelist.tcsh
set CFlags = "-Os -g3 -lm -fno-omit-frame-pointer -lpthread"

if ! -e $binary then
  foreach f ( $CFiles )
     $cc $CFlags -c $f -o $f.o || rm $f.o
  end
else
  foreach f ( $CFiles )
    set find = `$gFindLoc -wholename    $f -newer $binary `
    if($#find)  $cc $CFlags -c $f -o $f.o || rm $f.o 
  end
endif

if ! -e $binary then
  foreach f ( $AsmFiles )
    yasm -f elf64 $f -o $f.o || rm $f.o
  end
else
  foreach f ( $AsmFiles )
    set find = `$gFindLoc -wholename $f -newer $binary `
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
$cc $Objs -lm -lpthread -lX11 -o $binary
