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

#define CHECK_ERROR if (result != MMSYSERR_NOERROR) { fprintf(stderr, "Audio error at %s. Error code: 0x%X\n", __LINE__, result); }

SAMPLE_TYPE	lpSoundBuffer[SYN_BUFFERSIZE*2]; 
HWAVEOUT	hWaveOut;

WAVEFORMATEX WaveFMT =
{
#ifdef FLOAT_32BIT	
	WAVE_FORMAT_IEEE_FLOAT,
#else
	WAVE_FORMAT_PCM,
#endif		
	2, // channels
	SYN_RATE, // samples per sec
	SYN_RATE*sizeof(SAMPLE_TYPE)*2, // bytes per sec
	sizeof(SAMPLE_TYPE)*2, // block alignment;
	sizeof(SAMPLE_TYPE)*8, // bits per sample
	0 // extension not needed
};

WAVEHDR WaveHDR = 
{
	(LPSTR)lpSoundBuffer, 
	SYN_BUFFERSIZE*sizeof(SAMPLE_TYPE)*2,
	0, 
	0, 
	0, 
	0, 
	0, 
	0
};

struct AudioBank {
	WAVEHDR header;
	SAMPLE_TYPE samples[SYN_BUFFERSIZE*2];
};

struct AudioBank buffer[AUDIO_BANKS];

double tt = 0.0;

void render_sound(SAMPLE_TYPE * lpSoundBuffer) {
	for (int i=0;i<SYN_BUFFERSIZE;i++) {
		double t = tt + i/(double)SYN_RATE;
		lpSoundBuffer[i*2] = (sin(2.0*3.14*t*400.0) * 0.4) *( 1.0+sin(2.0*3.14*t*0.2)*0.5);
		lpSoundBuffer[i*2+1] = lpSoundBuffer[i*2];
	}

	tt += SYN_BUFFERSIZE/(double)SYN_RATE;
}


static void CALLBACK fillSoundBuffer(HWAVEOUT hWaveOut,
									 UINT uMsg,
									 DWORD dwInstance,
									 DWORD dwParam1,
									 DWORD dwParam2 ) {

	WAVEHDR* waveheader = (WAVEHDR*) dwParam1;

	switch(uMsg) {
		case WOM_DONE:
			MMRESULT result; 
			waveOutUnprepareHeader(hWaveOut, waveheader, sizeof(WaveHDR));

			render_sound((float *)waveheader->lpData);

			waveheader->dwFlags = 0;
			result = waveOutPrepareHeader(hWaveOut, waveheader, sizeof(WaveHDR));
			CHECK_ERROR;
			waveOutWrite(hWaveOut,waveheader,sizeof(WAVEHDR));
			CHECK_ERROR;

			fprintf(stdout, "WOM_DONE\n");
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

	render_sound(lpSoundBuffer);

	for (int i=0;i<AUDIO_BANKS;i++) {
		//buffer[i].header = WaveHDR;
	}

	MMRESULT result; 
	result = waveOutOpen(&hWaveOut, WAVE_MAPPER, &WaveFMT, (DWORD_PTR)fillSoundBuffer, (DWORD)&WaveHDR, CALLBACK_FUNCTION); 
	CHECK_ERROR;
	result = waveOutPrepareHeader(hWaveOut, &WaveHDR, sizeof(WaveHDR));
	CHECK_ERROR;
	result = waveOutWrite(hWaveOut, &WaveHDR, sizeof(WaveHDR));	
	CHECK_ERROR;

	fprintf(stdout, "Sound system ready.\n");
}

void free_sound(void) {
	waveOutClose(hWaveOut);
}

#endif