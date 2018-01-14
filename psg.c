#include "iZX48.h"
#include <SDL.h>

SDL_AudioSpec Desired;
SDL_AudioSpec Obtained;

char psg_buffer[22050];

extern int cpu_sndptr;

#define FREQ	22050

unsigned long psg_step = 0;

void callback_psg(void *unused, Uint8 *stream, int len)
{
	int i;
	int chs;
	Sint16 *frames = (Sint16 *) stream;
	int framesize = len / 2;
	for (i = 0; i < framesize; i++, psg_step++) {
		frames[i] = psg_buffer[(psg_step%FREQ)] ? 8192 : -8192;
	}
}

void reset_psg(void)
{
	SDL_PauseAudio(1);
	psg_step = 0;
	memset(psg_buffer,0,FREQ);
}

void init_psg(void)
{
	reset_psg();

	Desired.freq= FREQ; /* Sampling rate: 22050Hz */
	Desired.format= AUDIO_S16; /* 16-bit signed audio */
	Desired.channels= 1; /* Mono */
	Desired.samples= FREQ/50; /* Buffer size: 2205000B = 10 sec. */
	Desired.callback = callback_psg;
	Desired.userdata = NULL;

	SDL_OpenAudio(&Desired, &Obtained);
}
