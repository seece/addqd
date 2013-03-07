
#include <cstdio>
#include <cstring>
#include <limits.h>
#include "event.h"

#define CHECK_EVENT_CHANNEL if (channel < 0 || channel > 255) { \
		fprintf(stderr, "Invalid channel num %d in %s", channel, __FUNCTION__); \
	} \

Event create_volume_event(double when, int channel, unsigned char volume) {
	CHECK_EVENT_CHANNEL;

	Event e;
	memset(&e, 0xFE, sizeof(e));	// for debug purposes
	e.channel = unsigned char (channel);
	e.type = EVENT_VOLUME;
	e.data[0] =  static_cast<char>(volume);
	e.when = when;

	return e;
}

// creates a note on/note off event
// outcome depends on the flag
Event create_note_event(double when, int channel, int pitch, bool state, unsigned char volume) {
	CHECK_EVENT_CHANNEL;

	if (pitch < SHRT_MIN || pitch > SHRT_MAX) {
		fprintf(stderr, "Invalid pitch value %d in %s", pitch, __FUNCTION__);	
	}

	short spitch = short (pitch);

	Event e;
	memset(&e, 0xFE, sizeof(e));

	e.channel = unsigned char (channel);
	e.data[0] = spitch >> 8;
	e.data[1] = spitch & 0x00FF;

	e.payload[0] = static_cast<char>(volume);

	if (state) {
		e.type = EVENT_NOTE_ON;
	} else {
		e.type = EVENT_NOTE_OFF;
	}
	e.when = when;

	return e;
}

Event create_end_all_event(double when, int channel) {
	CHECK_EVENT_CHANNEL;

	Event e;
	memset(&e, 0xFE, sizeof(e));
	e.channel = unsigned char (channel);
	e.type = EVENT_END_ALL;
	e.when = when;

	return e;
}

