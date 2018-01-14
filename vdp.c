#include "iZX48.h"

extern byte *z80_memory;

int rgb_table[] = {
	0x000000,
	0x0000d7,
	0x00d700,
	0x00d7d7,
	0xd70000,
	0xd700d7,
	0xd7d700,
	0xd7d7d7,
	0x000000,
	0x0000ff,
	0x00ff00,
	0x00ffff,
	0xff0000,
	0xff00ff,
	0xffff00,
	0xffffff,
};

void vdp_grefresh(SDL_Surface *sdl_screen, int frame)
{
	unsigned int *fb = ((unsigned int *)sdl_screen->pixels);
	for(int y = 0; y < 192; y++) {
		for(int x = 0; x < 32; x++) {
			int dataByteHigh=0x40|((y/8)&0x18)|(y%8), dataByteLow=(((y/8)&0x7)<<5)|x;
			int attrByteHigh=0x58|((y/8)>>3), attrByteLow=dataByteLow;
			
			unsigned char data = z80_memory[(dataByteHigh<<8)|dataByteLow];
			
			int attr = z80_memory[(attrByteHigh<<8)|attrByteLow];
			
			for(int i = 0; i < 8; i++) {
				int bit = ((data >> (7-(i & 7))) & 1);
				int fg = attr & 7;
				int bg = (attr >> 3) & 7;
				int c = (bit ? rgb_table[fg] : rgb_table[bg]) * (((frame / 15) & 1) | ((attr >> 7) ^ 1));
				int ptr = y * (sdl_screen->pitch/4) + (x*8+i);
				
				fb[ptr+0] = c;
			}
		}
	}
}
