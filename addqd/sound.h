/*
* Include this only from main.
*/

#ifndef SOUND_H
#define SOUND_H

#include <windows.h>
#include <windowsx.h>
#include <mmsystem.h>
#include <mmreg.h>

#include <cmath>

#include "config.h"
#include "addsynth.h"

#define CHECK_ERROR if (result != MMSYSERR_NOERROR) { fprintf(stderr, "Audio error! Error code: %d\n", result); }

HWAVEOUT	hWaveOut;

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
	NULL,	// this will be filled in later in the callback
	AUDIO_BUFFERSIZE*sizeof(SAMPLE_TYPE)*2,
	0, 
	0, 
	0, 
	0, 
	0, 
	0
};

struct AudioBank {
	WAVEHDR header;
	SAMPLE_TYPE samples[AUDIO_BUFFERSIZE*2];
};

struct AudioBank buffer[AUDIO_BANKS];
static int currentBuffer = 0;	// the next buffer to write to

static void CALLBACK fillSoundBuffer(HWAVEOUT hWaveOut,
									 UINT uMsg,
									 DWORD dwInstance,
									 DWORD dwParam1,
									 DWORD dwParam2 ) {

	WAVEHDR* waveheader = (WAVEHDR*) dwParam1;
	WAVEHDR * header;
	double ratio;
	int renderStart, renderTime;

	switch(uMsg) {
		case WOM_DONE:
			MMRESULT result; 
			waveOutUnprepareHeader(hWaveOut, waveheader, sizeof(WaveHDR));

			currentBuffer = (currentBuffer + 1) % AUDIO_BANKS;

			renderStart = GetTickCount();
			syn_render_block(buffer[currentBuffer].samples, AUDIO_BUFFERSIZE);
			renderTime = GetTickCount()-renderStart;

			buffer[currentBuffer].header = WaveHDR;
			buffer[currentBuffer].header.lpData = (LPSTR)buffer[currentBuffer].samples;

			result = waveOutPrepareHeader(hWaveOut, (LPWAVEHDR)&(buffer[currentBuffer].header), sizeof(buffer[currentBuffer].header));
			CHECK_ERROR;
			waveOutWrite(hWaveOut,(LPWAVEHDR)&(buffer[currentBuffer].header),sizeof(WAVEHDR));
			CHECK_ERROR;

			//ratio = (((double)AUDIO_BUFFERSIZE/(double)AUDIO_RATE))/(double)(renderTime/1000.0);

			fprintf(stdout, "WOM_DONE\n");
			fprintf(stdout, "wrote to buffer %d\t took: %d ms\n", currentBuffer, renderTime);
			break;
		case WOM_OPEN:
			fprintf(stdout, "WOM_OPEN\n");

			break;
		case WOM_CLOSE:
			fprintf(stdout, "WOM_CLOSE\n");
			break;
		default:
			fprintf(stderr, "Illegal audio callback.\n");
			break;
	}
}


void init_sound(void) {
	/*
#ifdef USE_SOUND_THREAD
	CreateThread(0, 0, (LPTHREAD_START_ROUTINE)RenderSound, lpSoundBuffer, 0, 0);
#else
	
#endif
	*/

	for (int i=0;i<AUDIO_BANKS;i++) {
		// TODO fill the initial blocks with silence
		syn_render_block(buffer[i].samples, AUDIO_BUFFERSIZE);
		buffer[i].header = WaveHDR;
		buffer[i].header.lpData = (LPSTR)buffer[i].samples;
	}

	MMRESULT result; 
	result = waveOutOpen(&hWaveOut, WAVE_MAPPER, &WaveFMT, (DWORD_PTR)fillSoundBuffer, (DWORD)&WaveHDR, CALLBACK_FUNCTION); 
	CHECK_ERROR;

	currentBuffer = AUDIO_BANKS-1;	
	for (int i=0;i<AUDIO_BANKS;i++) {
		result = waveOutPrepareHeader(hWaveOut, &(buffer[i].header), sizeof(WAVEHDR));
		CHECK_ERROR;
		result = waveOutWrite(hWaveOut, &(buffer[i].header), sizeof(WAVEHDR));	
		CHECK_ERROR;
	}
	
	fprintf(stdout, "Sound system ready.\n");
}

void free_sound(void) {
	waveOutClose(hWaveOut);
}

#endif