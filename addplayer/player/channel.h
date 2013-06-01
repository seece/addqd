#ifndef CHANNEL_H
#define CHANNEL_H

#include "config.h"
#include "effect.h"
#include "instrument.h"
#include "voice.h"
#include "unit.h"

class Channel {
	public:
		static const int MAX_UNITS = 3;

		Channel();
		~Channel();

		Instrument * instrument;	// TODO remove the Instrument struct altogether
		float volume;
		float pan;					// channel pan, between [-1.0, 1.0]
		EffectChain chain;
		SAMPLE_TYPE * buffer;		// channel mixing buffer, see SYN_MAX_BUFFER_SIZE
		LFO lfo[SYN_CHN_LFO_AMOUNT];

		void render(Voice* voice, int i, long t_samples);
		
		CUnit* units[MAX_UNITS];
};

#endif