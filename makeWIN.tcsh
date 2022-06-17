#!/bin/tcsh
source filelistWIN.tcsh
set CC = "x86_64-w64-mingw32-gcc"
set CFlags = "-DTARGET_WIN32 -I./SDL2-mingw64/include -I./SDL2-mingw64/include/SDL2 -Os -g3 -lm -fno-omit-frame-pointer"
rm 3d_loader.exe
if ! -e 3d_loader.exe then
  foreach f ( $CFiles )
    $CC $CFlags -c $f -o $f.obj || rm $f.obj
  end
else
  foreach f ( $CFiles )
    set find = `find . -wholename "$f" -newer 3d_loader.exe `
    compile:
    if($#find) $CC $CFlags -c $f -o $f.obj || rm $f.obj
  end
endif

if ! -e 3d_loader.exe then
  foreach f ( $AsmFiles )
    yasm -f win64 $f -o $f.obj || rm $f.obj
  end
else
  foreach f ( $AsmFiles )
    set find = `find . -wholename "$f" -newer 3d_loader.exe `
    if($#find) yasm -f win64 $f -o $f.obj || rm $f.obj
  end
endif

set Objs = ()
foreach f ( $CFiles )
  set Objs = ( $Objs "$f.obj" )
end
foreach f ( $AsmFiles )
  set Objs = ( $Objs "$f.obj" )
end
$CC $Objs ext/wineditline-2.206/lib64/libedit_static.a SDL2.dll -lm -lshlwapi -ldbghelp -o 3d_loader.exe
