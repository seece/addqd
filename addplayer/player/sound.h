/*
* The audio back end. 
*/

#ifndef SOUND_H
#define SOUND_H

#include <climits>
#include <windows.h>
#include <windowsx.h>
#include <mmsystem.h>
#include <mmreg.h>

#include "config.h"
#include "event.h"

#define AUDIO_MAX_EVENTS 16384


static HWAVEOUT	hWaveOut;

static WAVEFORMATEX WaveFMT =
{
	#ifdef FLOAT_32BIT	
		WAVE_FORMAT_IEEE_FLOAT,
	#else
		WAVE_FORMAT_PCM,
	#endif		
	2, // channels
	AUDIO_RATE, // samples per sec
	AUDIO_RATE*sizeof(SAMPLE_TYPE)*2, // bytes per sec
	sizeof(SAMPLE_TYPE)*2, // block alignment;
	sizeof(SAMPLE_TYPE)*8, // bits per sample
	0 // extension not needed
};

static WAVEHDR WaveHDR = 
{
	NULL,	// this will be filled in later 
	AUDIO_BUFFERSIZE*sizeof(SAMPLE_TYPE)*2,
	0, 
	0, 
	0, 
	0, 
	0, 
	0
};

struct Buffer {
	WAVEHDR header;
	SAMPLE_TYPE data[AUDIO_BUFFERSIZE*2];
};

static Buffer buffers[AUDIO_BUFFERS];
static int renderpos = 0;	// rendering position in lpBuffer in stereo samples

typedef void (*SynthRender_t)(SAMPLE_TYPE *, int, addqd::EventBuffer *);
// void update_player(PTSong * song, int start_time, int end_time)
typedef void (*PollEventCallback_t)(addqd::EventBuffer * buffer, long samplecount);

void init_sound(void);
void poll_sound(SynthRender_t synthRender, PollEventCallback_t updatePlayer);
void free_sound(void);

#endif