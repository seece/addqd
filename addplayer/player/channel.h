#ifndef CHANNEL_H
#define CHANNEL_H

#include "config.h"
#include "effect.h"
#include "instrument.h"
#include "voice.h"
#include "unit.h"
#include "../voicelist.h"

class Channel {
	public:
	static const int MAX_UNITS = 3;

	Channel();
	~Channel();

	void renderVoice(Voice* voice, int i, long t_samples);
	void render(int i, long t_samples);
	void addPlayingVoice(Voice* voice);
	void removePlayingVoice(Voice* voice);
	void processModulation();
	int unitsLoaded();

	int index;	// the index is set in void syn_init function
	Instrument * instrument;	// TODO remove the Instrument struct altogether, use device configurations instead
	float volume;
	float pan;					// channel pan, between [-1.0, 1.0]
	EffectChain chain;
	SAMPLE_TYPE * buffer;		// channel mixing buffer, see SYN_MAX_BUFFER_SIZE

	LFO lfo[SYN_CHN_LFO_AMOUNT];
	Envelope env[SYN_CHN_ENV_AMOUNT];				
	
	CUnit* units[MAX_UNITS];	// we must use pointers since we are taking advantage of polymorphism

	LFO* getLFO(int index);
	Envelope* getEnvelope(int index);
	CUnit* getUnit(int index);

	private:
	CVoiceList voicelist;
};

#endif