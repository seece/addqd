#ifndef ADDSYNTH_H
#include "config.h"

#define ADDSYNTH_H

enum EventType {NONE, NOTE_ON, NOTE_OFF};

struct Event {
	char type;
	unsigned char channel;
	char data[2];
	char payload[4];
};

void syn_render_block(SAMPLE_TYPE * buf, int length);

#endif