#include "iZX48.h"

extern byte *z80_memory;

int cpu_beep = 0;

int klist[5*8] = {
	SDLK_LSHIFT, SDLK_z, SDLK_x, SDLK_c, SDLK_v,
	SDLK_a, SDLK_s, SDLK_d, SDLK_f, SDLK_g,
	SDLK_q, SDLK_w, SDLK_e, SDLK_r, SDLK_t,
	SDLK_1, SDLK_2, SDLK_3, SDLK_4, SDLK_5,
	SDLK_0, SDLK_9, SDLK_8, SDLK_7, SDLK_6,
	SDLK_p, SDLK_o, SDLK_i, SDLK_u, SDLK_y,
	SDLK_RETURN, SDLK_l, SDLK_k, SDLK_j, SDLK_h,
	SDLK_SPACE, SDLK_LCTRL, SDLK_m, SDLK_n, SDLK_b,
};

byte io80_readm(int param, ushort address)
{
	return z80_memory[address];
}

void io80_writem(int param, ushort address, byte data)
{
	if(address >= 0x4000) z80_memory[address] = data;
}

byte io80_readp(int param, ushort address)
{
	int ret = 0xff;
	
	if(address == 0xfefe || address == 0xfdfe || address == 0xfbfe || address == 0xf7fe || address == 0xeffe || address == 0xdffe || address == 0xbffe || address == 0x7ffe)
	{
		Uint8 *keystate = SDL_GetKeyState(NULL);
		
		int tbl;
		
		switch(address)
		{
		case 0xfefe:
			tbl=0;
			break;
		case 0xfdfe:
			tbl=1;
			break;
		case 0xfbfe:
			tbl=2;
			break;
		case 0xf7fe:
			tbl=3;
			break;
		case 0xeffe:
			tbl=4;
			break;
		case 0xdffe:
			tbl=5;
			break;
		case 0xbffe:
			tbl=6;
			break;
		case 0x7ffe:
			tbl=7;
			break;
		}
		
		for(int i = 0; i < 5; i++) {
			if(keystate[klist[tbl*5+i]]) ret &= ~(1 << i);
		}
		
		//printf("READ %04x RET %02x\n",address,ret);
	}
	
	return ret;
}

void io80_writep(int param, ushort address, byte data)
{
	printf("WRITE %04x(%02x)\n",address,data);
	
	if((address & 1) == 0) cpu_beep = (data & 16) ? 1 : 0;
}
