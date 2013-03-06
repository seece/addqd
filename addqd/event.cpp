
#include <cstdio>
#include <limits.h>
#include "event.h"

Event create_note_event(double when, int channel, int pitch) {
	if (channel < 0 || channel > 255) {
		fprintf(stderr, "Invalid channel num %d in %s", channel, __FUNCTION__);	
	}

	if (pitch < SHRT_MIN || pitch > SHRT_MAX) {
		fprintf(stderr, "Invalid pitch value %d in %s", pitch, __FUNCTION__);	
	}

	short spitch = short (pitch);

	Event e;
	e.channel = unsigned char (channel);
	e.data[0] = spitch >> 8;
	e.data[1] = spitch & 0x00FF;
	e.type = EVENT_NOTE_ON;
	e.when = when;

	return e;
}
