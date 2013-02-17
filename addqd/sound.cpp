#include <cstdio>
#include <windows.h>
#include <mmsystem.h>
#include <mmreg.h>
#include "sound.h"

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
	MMRESULT result;
	int renderStart, renderTime;

	if ((buffers[currentBuffer].header.dwFlags & WHDR_DONE) != 0) {
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
		double ratio = (((double)AUDIO_BUFFERSIZE/(double)AUDIO_RATE))/(double)(renderTime/1000.0);
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