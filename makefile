HEADERS := HolyC.tab.h 3d.h
CC := gcc
CFLAGS :=  -g3 -Os -DUSEGC -Wformat=0 -Wreturn-type -DUSEGC  `sdl2-config --cflags --libs`
all: 3d_tests 3d
	echo "Done"
HCRT := HCRT/ASM.HC HCRT/COMPRESS.HC HCRT/FS.HC HCRT/Opcodes.DD HCRT/BITS.HC HCRT/FIFO.HC HCRT/HASH.HC HCRT/HCRT.HC HCRT/PRINT.HC HCRT/CHAR.HC HCRT/FILE.HC HCRT/MATH.HC HCRT/QSORT.HC HCRT/LEXER.HC
OBJS := linenoise.o aot.cyborg.o tags.cyborg.o rl.cyborg.o vec.cyborg.o rt.cyborg.o jitlib-core.cyborg.o map.cyborg.o compile.cyborg.o lexer.cyborg.o HolyC.cyborg.o unesc.cyborg.o tosprn.cyborg.o gc.cyborg.o debugger.cyborg.o rope.cyborg.o at3.cyborg.o exceptLin.cyborg.o asm.cyborg.o
3d: $(OBJS) HCRT/HCRT.BIN main.cyborg.o
	gcc $(OBJS) main.cyborg.o -lncurses -ltinfo -lpthread -lSDL2 -lm -o $@
HCRT/HCRT.BIN: 3d.bootstrap $(HCRT)
	./3d.bootstrap --noruntime -c HCRT/HCRT.BIN HCRT/HCRT.HC --binheader HCRT/HCRT.HH
linenoise.o: ext/linenoise/linenoise.c ext/linenoise/linenoise.h
	$(CC) $(CFLAGS) -c ext/linenoise/linenoise.c -o linenoise.o
package: 3d HCRT/HCRT.BIN
	tcsh Debian_pkg.sh
main.o: main.c
	$(CC) $(CFLAGS) -c  main.c -o $@
3d.bootstrap: aot.o linenoise.o tags.o rl.o vec.o rt.o jitlib-core.o map.o compile.o lexer.o HolyC.o unesc.o tosprn.o gc.o debugger.o rope.o main.o at3.o exceptLin.o asm.o
	gcc $(CFLAGS) $^ -lm -lncurses -ltinfo -lpthread -lSDL2 -o $@
3d_tests: aot.o tags.o rl.o linenoise.o vec.o rt.o jitlib-core.o map.o compile.o lexer.o HolyC.o unesc.o tests.c tosprn.o gc.o debugger.o rope.o exceptLin.o asm.o
	gcc $(CFLAGS) $^ -lm -lncurses -ltinfo -lpthread -lSDL2 -o $@
HolyC.o: HolyC.tab.c $(HEADERS)
	$(CC) $(CFLAGS) -g -c HolyC.tab.c -o $@
HolyC.tab.c HolyC.tab.h: HolyC.y
	bison -Wother -v -d -b HolyC HolyC.y
vec.o: ext/vec/src/vec.c $(HEADERS)
	$(CC) $(CFLAGS) -c ext/vec/src/vec.c  -o $@
map.o: ext/map/src/map.c $(HEADERS)
	$(CC) $(CFLAGS) -c ext/map/src/map.c -o $@
compile.o: compile.c 3d.h $(HEADERS)
	$(CC) $(CFLAGS) -c compile.c -o $@
lexer.o: lexer.c 3d.h $(HEADERS)
	$(CC) $(CFLAGS) -c lexer.c -o $@
jitlib-core.o: ext/myjit/myjit/jitlib-core.c
	$(CC) $(CFLAGS) -c ext/myjit/myjit/jitlib-core.c -o $@
tosprn.o: TOSPrint.c $(HEADERS)
	$(CC) $(CFLAGS) -c TOSPrint.c -o $@
unesc.o: ext/C_Unescaper/escaper.c  $(HEADERS)
	$(CC) $(CFLAGS) -c ext/C_Unescaper/escaper.c -o unesc.o
rope.o: ext/mropes/src/rope.c  $(HEADERS)
	$(CC) $(CFLAGS) -c ext/mropes/src/rope.c -o $@
at3.o: ext/argtable3/argtable3.c  $(HEADERS)
	$(CC) $(CFLAGS) -c ext/argtable3/argtable3.c -o $@
rl.o: rl.c  $(HEADERS)
	$(CC) $(CFLAGS) -c rl.c -o $@
rt.o: runtime.c  $(HEADERS)
	$(CC) $(CFLAGS) -c runtime.c -o $@
gc.o: gc.c gc.h  $(HEADERS)
	$(CC) -O0 -g -c gc.c -o gc.o
asm.o: asm.c $(HEADERS)
	$(CC) $(CFLAGS) -c asm.c -o asm.o
debugger.o: debugger.c  $(HEADERS)
	$(CC) $(CFLAGS) -c debugger.c -o $@
clean:
	rm *.o HolyC.tab.c HolyC.tab.h 3d.bootstrap 3d 3d_tests HCRT/HCRT.BIN
exceptLin.o: exceptLin.yasm
	yasm -f elf64 exceptLin.yasm -o $@
tags.o: tags.c  $(HEADERS)
	$(CC) $(CFLAGS) -c tags.c -o $@
aot.o: aot.c $(HEADERS)
	$(CC) $(CFLAGS) -c aot.c -o aot.o
sexy:
	astyle *.c *.h


#Cyborg section
main.cyborg.o: main.c
	$(CC) $(CFLAGS) -DBOOTSTRAPED -c  main.c -o $@
HolyC.cyborg.o: HolyC.tab.c $(HEADERS)
	$(CC) $(CFLAGS) -DBOOTSTRAPED -g -c HolyC.tab.c -o $@
HolyC.tab.c HolyC.tab.h: HolyC.y
	bison -v -d -b HolyC HolyC.y
vec.cyborg.o: ext/vec/src/vec.c $(HEADERS)
	$(CC) $(CFLAGS) -DBOOTSTRAPED -c ext/vec/src/vec.c  -o $@
map.cyborg.o: ext/map/src/map.c $(HEADERS)
	$(CC) $(CFLAGS) -DBOOTSTRAPED  -c ext/map/src/map.c -o $@
compile.cyborg.o: compile.c 3d.h $(HEADERS)
	$(CC) $(CFLAGS) -DBOOTSTRAPED  -c compile.c -o $@
lexer.cyborg.o: lexer.c 3d.h $(HEADERS)
	$(CC) $(CFLAGS) -DBOOTSTRAPED  -c lexer.c -o $@
jitlib-core.cyborg.o: ext/myjit/myjit/jitlib-core.c
	$(CC) $(CFLAGS) -DBOOTSTRAPED  -c ext/myjit/myjit/jitlib-core.c -o $@
tosprn.cyborg.o: TOSPrint.c $(HEADERS)
	$(CC) $(CFLAGS) -DBOOTSTRAPED  -c TOSPrint.c -o $@
unesc.cyborg.o: ext/C_Unescaper/escaper.c  $(HEADERS)
	$(CC) $(CFLAGS) -DBOOTSTRAPED  -c ext/C_Unescaper/escaper.c -o unesc.cyborg.o
rope.cyborg.o: ext/mropes/src/rope.c  $(HEADERS)
	$(CC) $(CFLAGS) -DBOOTSTRAPED  -c ext/mropes/src/rope.c -o $@
at3.cyborg.o: ext/argtable3/argtable3.c  $(HEADERS)
	$(CC) $(CFLAGS) -DBOOTSTRAPED  -c ext/argtable3/argtable3.c -o $@
rl.cyborg.o: rl.c  $(HEADERS)
	$(CC) $(CFLAGS) -DBOOTSTRAPED  -c rl.c -o $@
rt.cyborg.o: runtime.c  $(HEADERS)
	$(CC) $(CFLAGS) -DBOOTSTRAPED  -c runtime.c -o $@
gc.cyborg.o: gc.c gc.h  $(HEADERS)
	$(CC) -O0 -g3 -DBOOTSTRAPED  -c gc.c -o gc.cyborg.o
debugger.cyborg.o: debugger.c  $(HEADERS)
	$(CC) $(CFLAGS) -DBOOTSTRAPED  -c debugger.c -o $@
exceptLin.cyborg.o: exceptLin.yasm
	yasm -f elf64 exceptLin.yasm -o $@
tags.cyborg.o: tags.c  $(HEADERS)
	$(CC) $(CFLAGS) -DBOOTSTRAPED  -c tags.c -o $@
asm.cyborg.o: asm.c $(HEADERS)
	$(CC) $(CFLAGS) -DBOOTSTRAPED  -c asm.c -o asm.cyborg.o
aot.cyborg.o: aot.c $(HEADERS)
	$(CC) $(CFLAGS) -DBOOTSTRAPED  -c aot.c -o aot.cyborg.o
main.cyborg.o: main.c $(HEADERS)
	$(CC) $(CFLAGS) -DBOOTSTRAPED  -c  main.c -o $@
