set AlpineURL = "https://dl-cdn.alpinelinux.org/alpine/v3.15/releases/x86_64/alpine-minirootfs-3.15.0-x86_64.tar.gz"
set ProotURL = "https://proot.gitlab.io/proot/bin/proot"

echo "Welcome to the 3Days builder!"
echo "This script will build 3Days in a (PRooted) chroot,but first you will need an mini filesystem to use."
echo "If you have a (alpine linux) rootfs you want,type in its location,other press enter for download one from $AlpineURL ."
set Provided = $<
switch ( $Provided )
	case "":
	if -e $AlpineURL:t then
		echo "Already exists"
		set AlpineURL = $AlpineURL:t
		breaksw
	endif
	wget $AlpineURL
	set AlpineURL = $AlpineURL:t
	breaksw
	default:
	if -e $Provided
	else
		echo "$Provided doesnt exist."
		exit
	endif
	set AlpineURL = $Provided
endsw

echo "We will download proot,a userspace chroot utility to stay away from the root."
if -e $ProotURL:t then
	echo "Already exists"
else
	wget $ProotURL
	chmod +x $ProotURL:t
endif

if -e chroot then
	echo "Chroot already exists"
else
	mkdir chroot
	cd chroot
	tar xvf ../$AlpineURL
	cd ..
endif


source  filelist.tcsh
set SrcPath = "./chroot/3Days"
mkdir -p $SrcPath
foreach f ( $CFiles $AsmFiles $HolyCFiles $MiscFiles $HeaderFiles )
	set h = $f:h
	if $h != $f then
		mkdir -p "$SrcPath/$h"
	endif
	cp  $f "$SrcPath/$f"
end

./proot -R chroot/ apk add gcc sdl2-dev tcsh yasm musl-dev
./proot -r chroot/ -w /3Days tcsh "make.tcsh"
./proot -r chroot -w /3Days ./3d_loader --noruntime MAKE_HCRT.HC
cp chroot/3Days/REPL.VBIN ./HCRT/HCRT.BIN
source make.tcsh

echo "Recompiling HCRT/HCRT.BIN for non-chroot's SDL2"
./3d_loader --noruntime FULL_RUNTIME.HC

echo "Congradulations,you compiled a runtime and loader."
