#include <SDL.h>
#include <dirent.h>

#include "z80emu.h"

typedef unsigned char byte;
typedef unsigned short ushort;

byte io80_readm(int param, ushort address);
void io80_writem(int param, ushort address, byte data);
byte io80_readp(int param, ushort address);
void io80_writep(int param, ushort address, byte data);

void vdp_grefresh(SDL_Surface *sdl_screen, int frame);

void init_psg(void);
void reset_psg(void);
