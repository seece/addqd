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
		buffers[i].header.dwFlags |= WHDR_DONE;	// reset the buffers, so they are rendered into on the first run
	}

	currentBuffer = 0;
}

// If necessary, renders a new block of audio and "swaps" the buffers
void poll_sound(SynthRender_t synthRender) {
	MMRESULT result;
	int renderStart, renderTime;

	if (buffers[currentBuffer].header.dwFlags & WHDR_DONE) {
		// On the first round only WHDR_DONE flag is set
		if (buffers[currentBuffer].header.dwFlags & WHDR_PREPARED) {
			SAFE_WAVEOUT_ACTION(
				waveOutUnprepareHeader(hWaveOut, &buffers[currentBuffer].header, sizeof(WAVEHDR));
				);
		}

		renderStart = GetTickCount();
		synthRender(buffers[currentBuffer].data, AUDIO_BUFFERSIZE);
		renderTime = GetTickCount()-renderStart;

		SAFE_WAVEOUT_ACTION(waveOutPrepareHeader(hWaveOut, &buffers[currentBuffer].header, sizeof(WAVEHDR)));
		SAFE_WAVEOUT_ACTION(waveOutWrite		(hWaveOut, &buffers[currentBuffer].header, sizeof(WAVEHDR)));
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