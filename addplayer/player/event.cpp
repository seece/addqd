
#include <cstdio>
#include <cstring>
#include <limits.h>
#include "event.h"
#include "config.h"

#define EVENT_EMPTY_BYTE (0xAB)

#ifdef DEBUG_EVENT_SANITY_CHECKS
#define CHECK_EVENT_CHANNEL if (channel < 0 || channel > 255) { \
		fprintf(stderr, "Invalid channel num %d in %s\n", channel, __FUNCTION__); \
	} 
#else
// on release mode we don't want additional checks
#define CHECK_EVENT_CHANNEL 
#endif

addqd::Event create_volume_event(double when, int channel, int volume) {
	CHECK_EVENT_CHANNEL;

	addqd::Event e;
	memset(&e, EVENT_EMPTY_BYTE, sizeof(e));	// for debug purposes
	e.channel = unsigned char (channel);
	e.type = ADQ_EVENT_VOLUME;
	
	e.data[0] =  static_cast<char>(volume);
	//printf("vol: %d == %d\n", volume, e.data[0]);
	e.when = when;

	return e;
}

// creates a note on/note off event
// outcome depends on the flag
addqd::Event create_note_event(double when, int channel, int pitch, bool state, unsigned char volume) {
	
	CHECK_EVENT_CHANNEL;
	
	#ifdef DEBUG_EVENT_SANITY_CHECKS
	if (pitch < SHRT_MIN || pitch > SHRT_MAX) {
		fprintf(stderr, "Invalid pitch value %d in %s\n", pitch, __FUNCTION__);	
	}
	#endif

	short spitch = short (pitch);

	addqd::Event e;
	memset(&e, EVENT_EMPTY_BYTE, sizeof(e));

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

addqd::Event create_end_all_event(double when, int channel) {
	CHECK_EVENT_CHANNEL;

	addqd::Event e;
	memset(&e, EVENT_EMPTY_BYTE, sizeof(e));
	e.channel = unsigned char (channel);
	e.type = ADQ_EVENT_END_ALL;
	e.when = when;

	return e;
}

#ifndef INTROLIB
	void print_event(addqd::Event& e) {
		printf("Event: %d %lf\t%#x %#x \n", e.channel, e.when, e.data[0], e.data[1]);
	}
#endif