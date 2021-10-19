# Welcome to 3D,a HolyC compiler
This compiler contains a bounds checker and a debugger,but in order to use these you will need to enable them at the command line by using "-d" for the debugger and "-b" for the bounds checker(which will also enable the debugger). It currently comes with a runtime with compression support,though the data structures used here are type-aligned so they are not compatible with native TempleOS archives. Press `TAB` to autocomplete. There is currently no assembler support(I would rather port to other architectures than hassle with assembler)

# Debugger(BSAT Before Space and Time)
Start the compiler with `-d`(or `-b` for bounds checking) and call `Debugger;`

# Building
You will need yasm,gnu bison,gcc and make. Other than that all dependencies are included in `ext/`  A modified [Oregon trail port from TinkerOS ](https://github.com/tinkeros/OT1975) is provided in the source code. Do `make installer` to make an installer for linux via `ext/epm`. This will require fltk for a graphical installer. TODO make a NSIS installer for windows when the compiler gets more builtins.

# Editor
This comes with a barebones editor,you can run it using `#include "HolyEd/EDITOR.HC"` Press `alt+f` to access the file menu and `alt+c` to access the code menu(Which gives a fuzzy symbol select and compile check). If you are using windows terminal you may want to change the cursor shape to a block because the background is white
![Editor screenshot](ed_screenshot.png)

# Features from TempleOS
## No variable length arrays
Array Dimensions are computed at compile time.
```
I64 dim=10;
I64 main() {
  I64 dim=100;
  I64 arr[dim]; //dim==10,we take the global;
  "DIM is %d\n",sizeof(arr)/sizeof(*arr);
}
main;
```
## Inheritance
You can use C++ Style inheritance like in TempleOS(which isn't explained well in the official docs)
```
class ABC {
  I64 a,b,c;
};
class ABC{I64 a,b,c;};
class ABCDEF:ABC {I64 d,e,f;};
ABCDEF a={1,2,3,4,5,6};
"%d,%d\n",a.a,a.d;
```
## exe
This compiler is JIT so you can #exe(among other things)
```
I64 x=#exe{StreamPrint("10");};
"x:%d\n",x;
```

# Pitfalls/(and some "features")
 - This compiler will set all bytes to 0 when `MAlloc`'ed
 - Garbage collection is disabled when running HolyC code.
 - (Finer) syntax checking is done only when compiling a statement or function.
 - `#include` uses the file path scheme of the OS,so you should
  ```
  #if IsWindows
  #include "C:\\File.HC"
  #else
  #include "/File.HC"
  #endif
  ```

# Internals
This compiler uses a modified myjit which runs under windows(new feature),linux(and breifly tested on FreeBSD). The main part of the Compiler is in compile.c. The compiler uses a value stack for temporary values(if they are on the stack on the same time they interfere). The grammar is in `HolyC.y`. The lexer uses a rope data structure provided by a patched `ext/mropes`.  I plan on making a game with this compiler so I will add to the runtime as needed. If you want to add functions,look at `runtime.c`,but a functional text editor is provided in `HolyEd/EDITOR.HC`.

# Contributing
You can contribute by adding to the runtime(see `runtime.c` and `HCRT/`).  Nice things would include:

 - A fltk binding
 - A DolDoc engine on top of said binding
 - A Holy Bible API and such
 - An assembler(I am working on it,i consider this to be a secondary feature as i want the compiler to be architecture independent`See HolyEd/ASM.HC`)
 - Make this friendly to compile on FreeBSD(use yacc instead of bison and favor FreeBSD's gmake)
