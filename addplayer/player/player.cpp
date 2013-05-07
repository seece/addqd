#include <cstdio>
#include <cstring>	// for NULL
#include <cassert>
#include "config.h"
#include "event.h"
#include "player.h"

using namespace addqd;

static long player_samples = 0;
static int last_event = 0;
static EventBuffer * loaded_song = NULL;

Event * deserialize_event_array(const char * eventdata, int * amountp) {
	long pos = 0;
	int amount = -1;
	memcpy(&amount, eventdata, sizeof(int)); pos+=sizeof(int);
	Event * event_array = new Event[amount];
	*amountp = amount;

	for (int i=0;i<amount;i++) {
		Event e;
		memcpy(&e.when,		eventdata + pos, sizeof(long));		pos+=	sizeof(long);
		memcpy(&e.type,		eventdata + pos, sizeof(char));		pos+=	sizeof(char);
		memcpy(&e.channel,	eventdata + pos, sizeof(char));		pos+=	sizeof(unsigned char);
		memcpy(&e.note,		eventdata + pos, 1*sizeof(char));	pos+=	1*sizeof(char);
		memcpy(e.payload,	eventdata + pos, 2*sizeof(char));	pos+=	2*sizeof(unsigned char);

		event_array[i] = e;	
	}

	return event_array;
}

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
	#ifdef DEBUG_PLAYER_SANITY_CHECKS
	if (loaded_song == NULL) {
		fprintf(stderr, "Song not loaded, cannot play!\n");
		return;
	}
	#endif 

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