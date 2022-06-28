# Welcome to 3Days,a HolyC compiler
Welcome to 3Days,a HolyC Compiler for FreeBSD and Windows,(a linux build will be out shortly).

This contains a port of the TempleOS compiler from the original sources of TempleOS,along with other things like the DolDoc engine and Gr stuff.
To get started,you will need `tcsh`,`SDL2`, and `gcc` to build the stuff,which you can do by typing `tcsh make.tcsh` on linux and `tcsh makeBSD.tcsh` on FreeBSD.
There is a wonky `makeWIN.tcsh`,but you will need tcsh for windows. 

# Development
This compiler has had a long history,it started out being written in C/yacc. I wasnt pleased with it so i rewrote it in HolyC. After I had a working HolyC compiler,I ported the TempleOS compiler to run under FreeBSD,which led to an easy port of the DolDoc engine
All depencies (except for SDL on unix) are included with this repo so building should be a problem. 

To build the compiler binary `HCRT.BIN`,type `Cmp("FULL_PACKAGE.HC",,"HCRT.BIN");` in 3Days in the root directory of `T:`
There is a booting process though, `KERNELA.HH` contains the functions that aren't linked to assembler symbols,but `KERNELB.HH` contains functions linked to assembler symbols. They are loaded in `FULL_PACKAGE.HC`
 

# HolyC Caveats
## 1: Lvalues can be typecasted
```c
U8 *x=NULL;
x(I64*)++;
x==8; //I64 is 8 bytes wide
```
## 2: Inheritance
```c
class abc {
    I64 a,b,c;
};
class abcdef:abc {
    I64 d,e,f;
};
abcdef six={'a','b','c','d','e','f'};
```
## 3: Weird for statements
Dont put declarations in for statements
```c
for(I64 dont_do=0;;dont_do++) //Invalid code
    ...;
```
Instead do this
```c
I64 x=0;
for(x=0;x!=10;x++)
    "%d\n",x;
```

# Contributing
I could use help in the areas of porting more TempleOS stuff to 3Days,there is a lot of things marked as `RESTORE` in the source,restoring them would be epic.
Good things would include 
- Autocomplete
- A Holy Bible API
- Games 

# Screenshot
![Screenshot](hello.png)

# Credits
 - https://github.com/rxi/map
 - https://github.com/rxi/vec
 - C_Unescaper(me nrootconauto@gmail.com)
 - https://github.com/argtable/argtable3
 - https://github.com/futurist/CommandLineToArgvA
