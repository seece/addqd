#include <cstdio>
#include <windows.h>
#include <mmsystem.h>
#include <mmreg.h>
#include "sound.h"
#include "event.h"

// does this really belong to the sound subsystem?
static Event event_array[AUDIO_MAX_EVENTS];
static EventBuffer event_buffer;

void init_sound(void) {
	MMRESULT result;

	SAFE_WAVEOUT_ACTION(waveOutOpen(&hWaveOut, WAVE_MAPPER, &WaveFMT, 0, 0, CALLBACK_NULL));

	for (int i=0;i<AUDIO_BUFFERS;i++) {
		buffers[i].header=WaveHDR;
		buffers[i].header.lpData = (LPSTR)buffers[i].data;
		buffers[i].header.dwFlags |= WHDR_DONE;	// reset the buffers, so they are rendered into on the first run
	}

	currentBuffer = 0;

	// setup events
	event_buffer.amount = 0;
	event_buffer.events = event_array;
}


// If necessary, renders a new block of audio and "swaps" the buffers
void poll_sound(SynthRender_t synthRender, PollEventCallback_t updatePlayer) {
	MMRESULT result;
	int renderStart, renderTime;

	if (buffers[currentBuffer].header.dwFlags & WHDR_DONE) {
		// On the first round only WHDR_DONE flag is set
		if (buffers[currentBuffer].header.dwFlags & WHDR_PREPARED) {
			SAFE_WAVEOUT_ACTION(
				waveOutUnprepareHeader(hWaveOut, &buffers[currentBuffer].header, sizeof(WAVEHDR));
				);
		}

		// render a new set of note events
		// TODO and then pass the events to the rendering routine 
		updatePlayer(&event_buffer, (long)AUDIO_BUFFERSIZE);
		
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