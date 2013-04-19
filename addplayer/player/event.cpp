
#include <cstdio>
#include <cstring>
#include <limits.h>
#include "event.h"
#include "config.h"

#ifdef DEBUG_EVENT_SANITY_CHECKS
#define CHECK_EVENT_CHANNEL if (channel < 0 || channel > 255) { \
		fprintf(stderr, "Invalid channel num %d in %s\n", channel, __FUNCTION__); \
	} 

#else
// on release mode we don't want additional checks
#define CHECK_EVENT_CHANNEL 
#endif

Event create_volume_event(double when, int channel, int volume) {
	CHECK_EVENT_CHANNEL;

	Event e;
	memset(&e, 0xFE, sizeof(e));	// for debug purposes
	e.channel = unsigned char (channel);
	e.type = ADQ_EVENT_VOLUME;
	
	e.data[0] =  static_cast<char>(volume);
	//printf("vol: %d == %d\n", volume, e.data[0]);
	e.when = when;

	return e;
}

// creates a note on/note off event
// outcome depends on the flag
Event create_note_event(double when, int channel, int pitch, bool state, unsigned char volume) {
	
	CHECK_EVENT_CHANNEL;
	
	#ifdef DEBUG_EVENT_SANITY_CHECKS
	if (pitch < SHRT_MIN || pitch > SHRT_MAX) {
		fprintf(stderr, "Invalid pitch value %d in %s\n", pitch, __FUNCTION__);	
	}
	#endif

	short spitch = short (pitch);

	Event e;
	memset(&e, 0xFE, sizeof(e));

	e.channel = unsigned char (channel);
	e.data[0] = spitch >> 8;
	e.data[1] = spitch & 0x00FF;

	e.payload[0] = static_cast<char>(volume);

	if (state) {
		e.type = ADQ_EVENT_NOTE_ON;
	} else {
		e.type = ADQ_EVENT_NOTE_OFF;
	}
	e.when = when;

	return e;
}

Event create_end_all_event(double when, int channel) {
	CHECK_EVENT_CHANNEL;

	Event e;
	memset(&e, 0xFE, sizeof(e));
	e.channel = unsigned char (channel);
	e.type = ADQ_EVENT_END_ALL;
	e.when = when;

	return e;
}
