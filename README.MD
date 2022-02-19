# Welcome to 3D,a HolyC compiler
This compiler contains a bounds checker and a debugger,but in order to use these you will need to enable them at the command line by using "-d" for the debugger and "-b" for the bounds checker(which will also enable the debugger). It currently comes with a runtime with compression support,though the data structures used here are type-aligned so they are not compatible with native TempleOS archives. Press `TAB` to autocomplete. There is currently no assembler support(I would rather port to other architectures than hassle with assembler)

This compiler is currently mostly a compiler,but it needs a better runtime. Take a peak at the runtime to get a good idea of what your working with. If something isnt in there,try adding it from TempleOS yourself and maybe share your port.

There is a `Debian_pkg.sh` and `FreeBSD_pkg.sh` for building packages for those platforms. You can use `3Days.nsi` with NSIS to build an installer for windows.

# Debugger(BSAT Before Space and Time)
Start the compiler with `-d`(or `-b` for bounds checking) and call `Debugger;`

# Building
You will need yasm,gcc tcsh and sdl2. Other than that all dependencies are included in `ext/`  A modified [Oregon trail port from TinkerOS ](https://github.com/tinkeros/OT1975) is provided in the source code. The compiler is a bootstrapping Compiler so it needs to compiler itself. To do this,a `HCRT/HCRT.BIN` is provided,but building it can be done in a [PRooted enviroment](https://proot-me.github.io) to isolate it from the rest of the system which doesnt need root access,to build it this way, do `tcsh builder.tcsh`. This script will download an Alpine Linux and PRoot for you and will build a new `HCRT/HCRT.BIN` which will contain the HolyC Compiler,runtime,and even a HolyC editor.

# Editor
This comes with a barebones editor,you can run it using `Ed("Filename.HC")` Press `alt+f` to access the file menu and `alt+c` to access the code menu(Which gives a fuzzy symbol select and compile check). If you are using windows terminal you may want to change the cursor shape to a block because the background is white. It operates on a "documet stack" and you pop the stack by pressing `ctrl+q`,so if you want to quit pop all of the stack items.
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
## First 64bits only
TempleOS only takes the first 64bit of any type passed to a function,so dont pass big classes as arguments
```
class {I64 first,last;} ten={10,0xffff};
"%d\n",ten;
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
 - There is no lexical scoping.
 - Like TempleOS,pointers can be nested 3 levels.
  ```
  #if IsWindows
  #include "C:\\File.HC"
  #else
  #include "/File.HC"
  #endif
  ```
 - Don't put declarations in for statements as per TempleOS
 ```
 //Dont
 for(I64 x=10;x>=0;x--) "%d\n",x;
 //Do
 I64 x;
 for(x=10;x>=0;x--) "%d\n",x;
 ```
 - Explicit typecasts are bit-for-bit. Look up IEEE754 if you use `x(F64)`,(otherwise dont typecast between F64/I64 or use `ToF64()`/`ToI64()`)
 ```
 pi==(0x400921FB54442D18(F64));
 I64 int_pi=ToI64(pi);
 ```
# Assembler
This compiler includes an assembler,see `asmtest.hc` for usage. All functions are placed in the first 32 bits of memory,so you should be able to jump to them if you import them. **However the case isn't the same for global variables,load their pointer into a register before addressing them**(Most memory operations use 32bit offsets,see `MOFFSxx` in `HCRT/Opcodes.DD` if you really want to.).
```
U0 Hi() {
 asm {
	 IMPORT Hello;
	 CALL Hello;
 }
}
```
If you suspect the assembler is acting up,open in gdb and type `disas GetVariable("Func_with_asm_in_it").func.funcptr,+100` for a dissambly. I would love to fix any problems in the assembler. Also,make sure you follow the ABI for your OS when using assembler code. And **DONT JUMP INTO ASM BLOCKS FROM HolyC Code**. All registers are unloaded prior to entering an asm block and before any label that an asm block jumps to.


# Internals
This compiler uses a modified myjit which runs under windows(new feature),linux(and breifly tested on FreeBSD). The main part of the Compiler is in compile.c. The compiler uses a value stack for temporary values(if they are on the stack on the same time they interfere). The grammar is in `HolyC.y`. The lexer uses a rope data structure provided by a patched `ext/mropes`.  I plan on making a game with this compiler so I will add to the runtime as needed. If you want to add functions,look at `runtime.c`,but a functional text editor is provided in `HolyEd/EDITOR.HC`.

# Contributing
You can contribute by adding to the runtime(see `runtime.c` and `HCRT/`).  Nice things would include:

 - A fltk binding
 - A DolDoc engine on top of said binding
 - A Holy Bible API and such
 - Make a `Help` function.

# Future!!!
I am writing a HolyC parser in HolyC to complement the HCRT/LEXER.HC(which the compiler uses). So bugfixes in the current parser would be  nice but it a new parser will be rolled out soon.


# Internals
Im glad you asked ;) . The compiler used to be written in C but now it is bootstrapping. I ditched makefiles to allow for a `filelist.tcsh` to tell `builder.tcsh` where the files are so it can put them in a chroot and build it that way (using `tcsh make.tcsh`).

The lexer is mostly unmodified from TempleOS in `HCRT/LEXER.HC`. The real epic sauce of the Compiler is in `PARSER.HC` which provides a debugger,compiler and REPL. The parser is a recursive descent parser. `ParseStmt` and it's friends have a argument named `instant_eval` which tell the compiler to not look for a next symbol when parsing(Which is useful for the REPL).

Error checking is mostly done in `AssignTypeToNode`. But other error checks can be found in the parser. `ParserFail` will immediately fail and flush the input,but `ParserFail2` will fail and display a diagnostic where the `AST` passed points to. Be sure to `CatchIf('Parser','Lexer')` in try blocks to recover from a bad parse(Not everything should be fatal). If you are really unlucky `CompileAST` will throw an exception,which (if it does),please create an issue. I will try to fix it.

`CompileAST` is the bread and butter of the Compiler. A set of bindings for `myjit` in `ext/myjit` are provided for the Compiler. This library has been modified with bug fixes,relocations and bodacious debugging capacities (see `jit_debugger_get_reg_ptr`  and `jit_debugger_get_vreg_ptr_from_parent` more on them later). `__CompileAST` is mostly a gaint switch statement with most of the AST types. `CompileAST` is the frontend to `__CompileAST`. It has a buddy named `CompileAstWithBreakpoint` which does the debugger magic. `Assign` does some lifting for the compiler too.

And the debugger(and bounds checker) are here too. `DebuggerRoutine` is aliased to `Debugger`,but you will step out of it for epicness( as `Debugger` is a function ,TODO make `Debugger` secretly trigger the next breakpoint ). The breakpoints are stored in `Compiler.breakpoints` which contain  `CBreakpoints` which are a specialization of `CHash`. The reason there are multiple breakpoints in `CBreakpoints.bps` is that a single line may have multiple breakpoints at one source location. `jit_FunctionParents` and `GetFunctionByPtr` are your friend when getting a backtrace. And  `jit_ParentFramePtrs` is usefull for getting the base pointers of the callers for the debugger(use `jit_debugger_get_reg_ptr` get a spilled registers location for the current function,and `jit_debugger_get_vreg_ptr_from_parent` when you have the parent's base pointer).


# Credits
 - https://github.com/martincohen/x64
 - https://github.com/ruimaciel/mropes
 - http://myjit.sourceforge.net/index.htm
 - https://github.com/rxi/map
 - https://github.com/rxi/vec
 - C_Unescaper(me nrootconauto@gmail.com)
 - https://github.com/argtable/argtable3
 - https://pdcurses.org/
 - https://sourceforge.net/projects/mingweditline/
