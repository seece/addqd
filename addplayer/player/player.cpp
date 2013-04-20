#include <cstdio>
#include <cstring>	// for NULL
#include <cassert>
#include "config.h"
#include "event.h"
#include "player.h"

static long player_samples = 0;
static int last_event = 0;
static EventBuffer * loaded_song = NULL;

void player::init() {
	last_event = 0;
	player_samples = 0;
}

// returns true on error
bool player::load_song(EventBuffer * song) {
	loaded_song = song;
	return false;
}

void player::update(EventBuffer * buffer, long samplecount) {
	if (loaded_song == NULL) {
		#ifdef DEBUG_PLAYER_SANITY_CHECKS
			fprintf(stderr, "Song not loaded, cannot play!\n");
		#endif

		return;
	}

	double time = (double)player_samples/(double)AUDIO_RATE;
	double endtime = (double)(player_samples+samplecount)/(double)AUDIO_RATE;

	for (int i=last_event;i<loaded_song->amount;i++) {
		Event * e = &loaded_song->event_list[i];

		if (e->when >= time && e->when < endtime) {
			buffer->event_list[buffer->amount] = *e;
			buffer->amount++;

			#ifndef INTROLIB
			assert(buffer->amount < buffer->max_events);
			#endif
		}

		if (e->when > endtime) {
			last_event = i;
			break;
		}
	}

	player_samples += samplecount;
}