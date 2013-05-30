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
#include "voice.h"
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


struct SynthState {
	double time;		// play time in seconds
	long time_ms;		// play time in milliseconds
	long samples;		// play time in samples
	int channels;		// instrument channel amount
	long env_counter;	// envelope processor timeout counter in samples
};

#endif