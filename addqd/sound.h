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

static void CALLBACK fillSoundBuffer(HWAVEOUT hWaveOut,
									 UINT uMsg,
									 DWORD dwInstance,
									 DWORD dwParam1,
									 DWORD dwParam2 ) {

	WAVEHDR* waveheader = (WAVEHDR*) dwParam1;

	double ratio;
	int renderStart, renderTime;
	int q;
	switch(uMsg) {
		case WOM_DONE:
			MMRESULT result; 
			waveOutUnprepareHeader(hWaveOut, waveheader, sizeof(WaveHDR));
			
			renderStart = GetTickCount();
			syn_render_block((float *)waveheader->lpData, AUDIO_BUFFERSIZE);
			renderTime = GetTickCount()-renderStart;

			result = waveOutPrepareHeader(hWaveOut, waveheader, sizeof(WAVEHDR));
			CHECK_ERROR;
			waveOutWrite(hWaveOut,waveheader,sizeof(WAVEHDR));
			CHECK_ERROR;
			
			q=0;
			// check other banks too
			for (int i=0;i<AUDIO_BANKS;i++) {
				if (buffer[i].header.dwFlags & WHDR_INQUEUE) {
					q++;
				}
			}
			printf("%d buffers in queue\t\n", q);
			Sleep(10);
			ratio = (((double)AUDIO_BUFFERSIZE/(double)AUDIO_RATE))/(double)(renderTime/1000.0);
			
			fprintf(stdout, "WOM_DONE\n");
			fprintf(stdout, "wrote to buffer %X\t took: %d ms %f\n", waveheader->lpData, renderTime, ratio);
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