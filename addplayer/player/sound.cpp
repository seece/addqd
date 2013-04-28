#include <cstdio>
#include <windows.h>
#include <mmsystem.h>
//#include <mmreg.h>
#include "sound.h"
#include "event.h"

#define CHECK_WAVEOUT_ERROR if (result != MMSYSERR_NOERROR) { fprintf(stderr, "Audio error at %d! Error code: %d\n", __LINE__ , result); }
#define SAFE_WAVEOUT_ACTION(sta) result = sta; CHECK_WAVEOUT_ERROR

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

	// setup events
	event_buffer.amount = 0;
	event_buffer.event_list = event_array;
	event_buffer.max_events = AUDIO_MAX_EVENTS;
}


// If necessary, renders a new block of audio and "swaps" the buffers
void poll_sound(SynthRender_t synthRender, PollEventCallback_t updatePlayer) {
	MMRESULT result;
	int renderStart, renderTime;

	for (int i=0;i<AUDIO_BUFFERS;i++) {
		DWORD flags = buffers[i].header.dwFlags;

		if (flags & WHDR_INQUEUE) {
			continue;
		}

		if (!(flags & WHDR_DONE)) {
			continue;
		}

		SAFE_WAVEOUT_ACTION(
			waveOutUnprepareHeader(hWaveOut, &buffers[i].header, sizeof(WAVEHDR));
			);

		// fetch a new set of note events
		updatePlayer(&event_buffer, (long)AUDIO_BUFFERSIZE);
		
		renderStart = GetTickCount();
		synthRender(buffers[i].data, AUDIO_BUFFERSIZE, &event_buffer);
		renderTime = GetTickCount()-renderStart;

		SAFE_WAVEOUT_ACTION(waveOutPrepareHeader(hWaveOut, &buffers[i].header, sizeof(WAVEHDR)));
		SAFE_WAVEOUT_ACTION(waveOutWrite		(hWaveOut, &buffers[i].header, sizeof(WAVEHDR)));

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