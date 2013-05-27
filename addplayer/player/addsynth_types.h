/* 
* This file contains datatypes used by the synthesizer.
*/

#ifndef ADDSYNTH_TYPES_H
#define ADDSYNTH_TYPES_H

//#include "../generators.h"
#include "config.h"
#include "effect.h"
#include "envelope.h"
#include "lfo.h"
#include "modmatrix.h"
#include "sample.h"
#include "generators.h"
#include "instrument.h"
#include "channel.h"

// used with Channel.target_volume
#define SYN_VOLUME_LERP_THRESOLD (0.01f)

#define SYN_UBERSAMPLE 

#define INTERPOLATION_NONE 0
#define INTERPOLATION_LINEAR 1

struct Spectrum {
	char bands[SYN_PARTIAL_AMOUNT];
};

// a collection of Spectrums
struct Spectra {
	int keyframe_amount;
	char * interpolation;	// interpolation mode of each keyframe
	Spectrum * spectrum;	// spectrum of each keyframe
};

struct EnvState {
	bool hold;				// if a key is held down
	long beginTime;			// the moment in samples when the key was repressed
	long endTime;	
	bool released;
	double volume;			// volume set by note-on command
	double target_volume;	// if target_volume differs from volume, volume will be interpolated smoothly to it
};


/// VoiceState is not updated on each processed sample, but on a regular
/// interval (SYN_ENVELOPE_JITTER) instead. The new calculated values are 
/// always saved in the respective VoiceState of the voice, and looked up
/// from there during the actual rendering.
struct VoiceState {
	float vol;	// volume after modulation
	float pan;	// panning after modulation
	Parameter params[SYN_MAX_PARAMETERS];	// effect chain parameter values
	ModSource mod_signals;		// mod source signals
};

struct Voice {
	int pitch;
	EnvState envstate;
	VoiceState state;
	bool active;
	double phase;
	Channel * channel;	// used for quick access
	int channel_id;		// used to detect playing notes on each channel 
};

struct SynthState {
	double time;		// play time in seconds
	long time_ms;		// play time in milliseconds
	long samples;		// play time in samples
	int channels;		// instrument channel amount
	long env_counter;	// envelope processor timeout counter in samples
};

#endif