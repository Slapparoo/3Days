CC=gcc
CFLAGS=$(shell pkg-config --cflags portaudio-2.0) -Ofast -g3 -lm -m64 -fno-omit-frame-pointer -DUSE_NETWORKING 
CFILES=ext/ln/linenoise.c sound.c main.c ext/dyad/src/dyad.c ext/vec/src/vec.c ext/map/src/map.c TOSPrint.c ext/C_Unescaper/escaper.c ext/argtable3/argtable3.c runtime.c poopalloc.c tos_aot.c window.c multic.c vfs.c
AFILES=FFI_SYSV.yasm
COBJS=$(foreach o,$(CFILES),$(o).o)
AOBJS=$(foreach o,$(AFILES),$(o).o) 
OBJS=$(COBJS) $(AOBJS)
LIBS= -lm -lX11 -lpthread $(shell pkg-config --libs portaudio-2.0)
all: 3d_loader 
	echo "done"
3d_loader: $(OBJS)
	$(CC) $(OBJS) $(LIBS) -o 3d_loader
%.c.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@
%.yasm.o: %.yasm
	yasm -f elf64 -o $@ $<
clean:
	rm $(OBJS)
