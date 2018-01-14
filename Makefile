TARGET		= iZX48
OBJS_TARGET	= iZX48.o io80.o z80emu.o vdp.o psg.o libgpu.o

CFLAGS = -O0 -g `sdl-config --cflags` 
LDFLAGS = 
LIBS = `sdl-config --libs`

include Makefile.in
