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

struct Buffer {
	WAVEHDR header;
	SAMPLE_TYPE data[AUDIO_BUFFERSIZE*2];
};

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

int currentBuffer;
static Buffer buffers[AUDIO_BUFFERS];
int renderpos = 0;	// rendering position in lpBuffer in stereo samples

void init_sound(void) {
	MMRESULT result;

	SAFE_WAVEOUT_ACTION(waveOutOpen(&hWaveOut, WAVE_MAPPER, &WaveFMT, 0, 0, CALLBACK_NULL));

	for (int i=0;i<AUDIO_BUFFERS;i++) {
		buffers[i].header=WaveHDR;
		buffers[i].header.lpData = (LPSTR)buffers[i].data;
		syn_render_block(buffers[i].data, AUDIO_BUFFERSIZE);
		
		SAFE_WAVEOUT_ACTION(waveOutPrepareHeader(	hWaveOut, &buffers[i].header, sizeof(buffers[i].header)));
		SAFE_WAVEOUT_ACTION(waveOutWrite		(	hWaveOut, &buffers[i].header, sizeof(buffers[i].header)));
	}

	currentBuffer = 0;
}

void poll_sound(void) {
	MMTIME time;
	MMRESULT result;
	int renderStart, renderTime;

	if (buffers[currentBuffer].header.dwFlags & WHDR_DONE != 0) {
		SAFE_WAVEOUT_ACTION(
			waveOutUnprepareHeader(hWaveOut, &buffers[currentBuffer].header, sizeof(WAVEHDR));
			);

		renderStart = GetTickCount();
		syn_render_block(buffers[currentBuffer].data, AUDIO_BUFFERSIZE);
		renderTime = GetTickCount()-renderStart;

		SAFE_WAVEOUT_ACTION(waveOutPrepareHeader(	hWaveOut, &buffers[currentBuffer].header, sizeof(WAVEHDR)));
		SAFE_WAVEOUT_ACTION(waveOutWrite		(	hWaveOut, &buffers[currentBuffer].header, sizeof(WAVEHDR)));
		currentBuffer=(currentBuffer+1) % AUDIO_BUFFERS;

		#ifdef DEBUG_PRINT_SPEED
		float ratio = (((double)AUDIO_BUFFERSIZE/(double)AUDIO_RATE))/(double)(renderTime/1000.0);
		fprintf(stdout, "took %d ms\t %f x realtime \n", renderTime, ratio);
		#endif
	}
}

void free_sound(void) {
	MMRESULT result;
	// free all buffers
	/*
	for (int i=0;i<AUDIO_BUFFERS;i++) {
		if (buffers[currentBuffer].header.dwFlags | WHDR_PREPARED) {
			SAFE_WAVEOUT_ACTION(waveOutUnprepareHeader(hWaveOut, &buffers[i].header, sizeof(WAVEHDR)););
		}
	}
	*/

	SAFE_WAVEOUT_ACTION(waveOutClose(hWaveOut));
}

#endif