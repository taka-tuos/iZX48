#include "iZX48.h"
#include "libgpu.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define FPS				50.0f
#define MHZ				3.5f

extern char psg_buffer[22050];
extern int cpu_beep;

int cpu_sndptr = 0;
int cpu_wait = 0;

byte *z80_memory;

void quit_program(int code)
{
	SDL_Quit();

	exit(code);
}

void handle_keydown(SDL_keysym* keysym)
{
	switch(keysym->sym) {
	case SDLK_ESCAPE:
		quit_program(0);
		break;
	default:
		break;
	}

}

void process_events(void)
{
	SDL_Event event;
	
	while(SDL_PollEvent(&event)) {
		switch(event.type) {
		case SDL_KEYDOWN:
			handle_keydown(&event.key.keysym);
			break;
		case SDL_QUIT:
			quit_program(0);
			break;
		}

	}

}

unsigned int sec_tstates = 0;
int psg_sync = 0;
int z80_tstates = 0;

void z80_execute(Z80_STATE *z80)
{
	int elim;
	unsigned int tstates;
	
	if(!psg_sync) SDL_PauseAudio(0);
	psg_sync = 1;
	
	tstates = z80_tstates;
	
	cpu_wait = 0;
	
	for(elim = 0; (int)((unsigned long)z80_tstates - tstates) < MHZ*1000.0f*1000.0f/FPS;elim++) {
		int ptbk = (int)((unsigned long)z80_tstates - sec_tstates) / 158;
		int bpbk = cpu_beep;
		
		ptbk = ptbk < 22050 ? ptbk : 22049;
		
		z80_tstates += Z80Emulate(z80,1,NULL);
		
		z80_tstates += cpu_wait;
		cpu_wait = 0;
		
		int lm = (int)((unsigned long)z80_tstates - sec_tstates);
		cpu_sndptr = lm / 158;
		cpu_sndptr = cpu_sndptr < 22050 ? cpu_sndptr : 22049;
		if(cpu_sndptr-ptbk > 0) memset(psg_buffer+ptbk,bpbk,cpu_sndptr-ptbk);
		psg_buffer[cpu_sndptr] = cpu_beep;
	}
	
	Z80Interrupt(z80, 0, NULL);
}

void adjust_fps(void) {
	static unsigned long maetime=0;
	static int frame=0;
	long sleeptime;
	if(!maetime) SDL_GetTicks();
	frame++;
	sleeptime=(frame<FPS)?
		(maetime+(long)((float)frame*(1000.0f/FPS))-SDL_GetTicks()):
		(maetime+1000-SDL_GetTicks());
	if(sleeptime>0)SDL_Delay(sleeptime);
	if(frame>=FPS) {
		frame=0;
		maetime=SDL_GetTicks();
	}
}

void z80_vmreset(Z80_STATE *z80)
{
	memset(z80,0,sizeof(Z80_STATE));
	
	Z80Reset(z80);

	sec_tstates = 0;
	z80_tstates = 0;
	
	psg_sync = 0;
}

SDL_Surface *sdl_screen;

int main(int argc, char* argv[])
{
	if(SDL_Init(SDL_INIT_VIDEO|SDL_INIT_AUDIO) < 0) {
		fprintf(stderr, "SDL initialization failed: %s\n",
			 SDL_GetError());
		quit_program(1);
	}
	
	sdl_screen = SDL_SetVideoMode(256,192,32,SDL_SWSURFACE);
	
	init_psg();

	libgpu_init();

	z80_memory = (byte *)malloc(0x10000);
	memset(z80_memory,0xff,0x10000);

	Z80_STATE z80;
	
	memset(&z80,0,sizeof(Z80_STATE));
	
	Z80Reset(&z80);
	
	z80_vmreset(&z80);
	
	FILE *fp = fopen("48e.rom","rb");
	
	fread(z80_memory, 1, 16384, fp);
	
	fclose(fp);
	
	int s_frame = 0;
	int frame = 0;
	char fps_string[64] = "0.00 fps";
	
	int timer_base = SDL_GetTicks();
	
	while(1) {
		if((s_frame % (int)(FPS)) == 0) sec_tstates = z80_tstates;
		
		z80_execute(&z80);
		adjust_fps();
		vdp_grefresh(sdl_screen,s_frame);
		libgpu_puts(sdl_screen,0,0,0xffffff,fps_string);
		SDL_UpdateRect(sdl_screen,0,0,0,0);
		memset(sdl_screen->pixels,0,sdl_screen->pitch*8);
		s_frame++;
		frame++;
		
		if(SDL_GetTicks() - timer_base >= 1000) {
			sprintf(fps_string,"%.2f fps",(float)frame/((float)(SDL_GetTicks() - timer_base)/1000.0f));
			timer_base = SDL_GetTicks();
			frame = 0;
		}
		
		process_events();
	}

	return 0;
}
