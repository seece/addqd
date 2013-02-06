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

#define CHECK_ERROR if (result != MMSYSERR_NOERROR) { fprintf(stderr, "Audio error! Error code: %d\n", result); }

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
static int currentBuffer = 0;	// the next buffer to write to

double tt = 0.0;

void render_sound(SAMPLE_TYPE * lpSoundBuffer) {
	double bonus = 0.0;

	if (GetAsyncKeyState(VK_SPACE)) {
		bonus = 100.0;
	}
	for (int i=0;i<SYN_BUFFERSIZE;i++) {
		double t = tt + i/(double)SYN_RATE;
		double f = 400.0 + bonus;
		lpSoundBuffer[i*2] = (sin(2.0*3.14*t*f) * 0.4) *( 1.0+sin(2.0*3.14*t*0.2)*0.5);
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
	WAVEHDR * header;

	switch(uMsg) {
		case WOM_DONE:
			MMRESULT result; 
			waveOutUnprepareHeader(hWaveOut, waveheader, sizeof(WaveHDR));

			currentBuffer = (currentBuffer + 1) % AUDIO_BANKS;

			render_sound(buffer[currentBuffer].samples);
			buffer[currentBuffer].header = WaveHDR;
			buffer[currentBuffer].header.lpData = (LPSTR)buffer[currentBuffer].samples;

			result = waveOutPrepareHeader(hWaveOut, (LPWAVEHDR)&(buffer[currentBuffer].header), sizeof(buffer[currentBuffer].header));
			CHECK_ERROR;
			waveOutWrite(hWaveOut,(LPWAVEHDR)&(buffer[currentBuffer].header),sizeof(WAVEHDR));
			CHECK_ERROR;

			/*
			render_sound((float *)waveheader->lpData);

			waveheader->dwFlags = 0;
			result = waveOutPrepareHeader(hWaveOut, waveheader, sizeof(WaveHDR));
			CHECK_ERROR;
			waveOutWrite(hWaveOut,waveheader,sizeof(WAVEHDR));
			CHECK_ERROR;
			*/



			fprintf(stdout, "WOM_DONE\n");
			fprintf(stdout, "wrote to buffer %d\n", currentBuffer);
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

	//render_sound(lpSoundBuffer);

	for (int i=0;i<AUDIO_BANKS;i++) {
		render_sound(buffer[i].samples);
		buffer[i].header = WaveHDR;
		buffer[i].header.lpData = (LPSTR)buffer[i].samples;
	}

	MMRESULT result; 
	result = waveOutOpen(&hWaveOut, WAVE_MAPPER, &WaveFMT, (DWORD_PTR)fillSoundBuffer, (DWORD)&WaveHDR, CALLBACK_FUNCTION); 
	CHECK_ERROR;

	currentBuffer = AUDIO_BANKS-1;	// this will wrap around to 0 on first WOM_DONE
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