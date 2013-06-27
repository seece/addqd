#ifndef VOICE_H
#define VOICE_H

#include "envelope.h"
#include "modmatrix.h"
#include "effect.h"

class Channel;

/// VoiceState is not updated on each processed sample, but on a regular
/// interval (SYN_ENVELOPE_JITTER) instead. The new calculated values are 
/// always saved in the respective VoiceState of the voice, and looked up
/// from there (by Units) during the actual rendering.
struct VoiceState {
	float vol;	// volume after modulation
	float pan;	// panning after modulation
	Parameter params[SYN_MAX_PARAMETERS];	// effect chain parameter values
	ModSource mod_signals;		// mod source signals
};

struct Voice {
	int pitch;
	int index;
	EnvState envstate;
	VoiceState state;
	bool active;
	double phase;
	Channel * channel;	// used for quick access
	int channel_id;		// used to detect active notes on each channel 
};


#endif