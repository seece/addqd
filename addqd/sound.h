/*
* Include this only from main.
*/

#ifndef SOUND_H
#define SOUND_H

#include <windows.h>
#include <windowsx.h>
#include <mmsystem.h>
#include <mmreg.h>

#include "config.h"

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

void  InitSound()
{
	/*
#ifdef USE_SOUND_THREAD
	CreateThread(0, 0, (LPTHREAD_START_ROUTINE)RenderSound, lpSoundBuffer, 0, 0);
#else
	//RenderSound(lpSoundBuffer);
#endif
	*/
	MMRESULT openResult = 
		waveOutOpen	( &hWaveOut, WAVE_MAPPER, &WaveFMT, NULL, 0, CALLBACK_NULL );

	if (openResult != MMSYSERR_NOERROR) {
		fprintf(stderr, "Couldn't open audio device. Error code: 0x%X\n", openResult);
	}

	waveOutPrepareHeader( hWaveOut, &WaveHDR, sizeof(WaveHDR) );
	waveOutWrite		( hWaveOut, &WaveHDR, sizeof(WaveHDR) );	

	fprintf(stdout, "Sound system ready.\n");
}


#endif