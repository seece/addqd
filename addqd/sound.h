/*
* Include this only from main.cpp
*/

#ifndef SOUND_H
#define SOUND_H

#include <climits>
#include <windows.h>
#include <windowsx.h>
#include <mmsystem.h>
#include <mmreg.h>

#include "config.h"
#include "addsynth.h"

#define CHECK_WAVEOUT_ERROR if (result != MMSYSERR_NOERROR) { fprintf(stderr, "Audio error at %d! Error code: %d\n", __LINE__ , result); }
#define SAFE_WAVEOUT_ACTION(sta) result = sta; CHECK_WAVEOUT_ERROR

HWAVEOUT	hWaveOut;
static SAMPLE_TYPE lpBuffer[AUDIO_BUFFERSIZE*2];

WAVEFORMATEX WaveFMT =
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

WAVEHDR WaveHDR = 
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

int renderpos = 0;	// rendering position in lpBuffer in stereo samples

void init_sound(void) {
	MMRESULT result;

	WaveHDR.lpData = (LPSTR)lpBuffer;
	WaveHDR.dwFlags |= WHDR_BEGINLOOP | WHDR_ENDLOOP;
	WaveHDR.dwLoops = INT_MAX;

	syn_render_block(lpBuffer, AUDIO_BUFFERSIZE);

	SAFE_WAVEOUT_ACTION(waveOutOpen(&hWaveOut, WAVE_MAPPER, &WaveFMT, 0, 0, CALLBACK_NULL));
	SAFE_WAVEOUT_ACTION(waveOutPrepareHeader(	hWaveOut, &WaveHDR, sizeof(WaveHDR)));
	SAFE_WAVEOUT_ACTION(waveOutWrite		(	hWaveOut, &WaveHDR, sizeof(WaveHDR)));
}

void poll_sound(void) {
	MMTIME time;
	MMRESULT result;

	int renderlength = 441;	// in stereo samples
	//int rendersize = renderlength*2;	// in real samples
	int lookahead = 4;

	time.wType = TIME_SAMPLES;
	SAFE_WAVEOUT_ACTION(waveOutGetPosition(hWaveOut, &time, sizeof(time)));
	//fprintf(stdout, "samples: %d\n", time.u.sample);

	while ((time.u.sample + lookahead*renderlength)*2 > renderpos) {
		// render block at lpBuffer[renderpos] with the length renderlength
		
		int startpos = renderpos % AUDIO_BUFFERSIZE;

		printf("\r");
		for (int i=0;i<AUDIO_BUFFERSIZE/renderlength;i++) {
			if (startpos/renderlength >= i) {
				printf("=");
			} else {
				printf(".");
			}
		}
		printf("\n");
		//if (startpos/rendersize == 0) {
		fprintf(stdout, "%d\t %d\t%d\t%d\n", (startpos/renderlength), startpos, renderlength, (time.u.sample));
		//}
		
		// the synth wants the length as stereo samples
		syn_render_block((lpBuffer + startpos*2), renderlength);
		renderpos += renderlength;
	}
}

void free_sound(void) {
	MMRESULT result;
	SAFE_WAVEOUT_ACTION(waveOutClose(hWaveOut));
}

#endif