#ifndef CHANNEL_H
#define CHANNEL_H

#include "config.h"
#include "effect.h"
#include "instrument.h"

class Channel {
	public:
		Channel();
		~Channel();

		Instrument * instrument;
		float volume;
		float pan;					// channel pan, between [-1.0, 1.0]
		EffectChain chain;
		SAMPLE_TYPE * buffer;		// channel mixing buffer, see SYN_MAX_BUFFER_SIZE
};

#endif