#!/bin/tcsh
source filelistWIN.tcsh
set PATH = /mingw64/bin:$PATH
set CC = "/mingw64/bin/gcc"
set PKG_CONF = "/mingw64/bin/pkg-config" 
set CFlags = "-DTARGET_WIN32 -Ofast -g3 -lm -fno-omit-frame-pointer -static `$PKG_CONF --cflags portaudio-2.0` -w"
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
$CC $Objs ext/wineditline-2.206/lib64/libedit_static.a  -mwindows -lm -lshlwapi -ldbghelp `$PKG_CONF --libs --static portaudio-2.0` -lShcore -Ofast -o 3d_loader.exe
