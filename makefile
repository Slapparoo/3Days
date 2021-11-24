HEADERS := HolyC.tab.h 3d.h
CC := gcc
CFLAGS :=  -g3 -O0 -Wformat=0 -Wreturn-type -DUSEGC `pkg-config --libs --cflags readline` #-fsanitize=address
all: 3d_tests 3d
	echo "Done"
HCRT := HCRT/ASM.HC HCRT/COMPRESS.HC HCRT/FS.HC HCRT/Opcodes.DD HCRT/BITS.HC HCRT/FIFO.HC HCRT/HASH.HC HCRT/HCRT.HC HCRT/PRINT.HC HCRT/CHAR.HC HCRT/FILE.HC HCRT/MATH.HC HCRT/QSORT.HC
HCRT/HCRT.BIN: 3d $(HCRT)
	./3d -c HCRT/HCRT.BIN HCRT/HCRT.HC
package: 3d HCRT/HCRT.BIN
	tcsh Debian_pkg.sh
main.o: main.c
	$(CC) $(CFLAGS) -c  main.c -o $@
3d: aot.o tags.o rl.o vec.o rt.o jitlib-core.o map.o compile.o lexer.o HolyC.o unesc.o tosprn.o gc.o debugger.o rope.o main.o at3.o exceptLin.o asm.o
	gcc $(CFLAGS) $^ -lm -lncurses -ltinfo -lpthread -lreadline -lSDL2 -o $@
3d_tests: aot.o tags.o rl.o vec.o rt.o jitlib-core.o map.o compile.o lexer.o HolyC.o unesc.o tests.c tosprn.o gc.o debugger.o rope.o exceptLin.o asm.o
	gcc $(CFLAGS) $^ -lm -lncurses -ltinfo -lpthread -lreadline -lSDL2 -o $@
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
	$(CC) $(CFLAGS) -c gc.c -o gc.o
asm.o: asm.c $(HEADERS)
	$(CC) $(CFLAGS) -c asm.c -o asm.o
debugger.o: debugger.c  $(HEADERS)
	$(CC) $(CFLAGS) -c debugger.c -o $@
clean:
	rm *.o HolyC.tab.c HolyC.tab.h epm 3d 3d_tests || cd ext/epm-5.0.0 &&  make clean
exceptLin.o: exceptLin.yasm
	yasm -f elf64 exceptLin.yasm -o $@
tags.o: tags.c  $(HEADERS)
	$(CC) $(CFLAGS) -c tags.c -o $@
aot.o: aot.c $(HEADERS)
	$(CC) $(CFLAGS) -c aot.c -o aot.o
sexy:
	astyle *.c *.h
