#ifndef INSTRUMENT_H
#define INSTRUMENT_H

#include "config.h"
#include "lfo.h"
#include "envelope.h"
#include "modmatrix.h"

enum InstrumentType {INS_OSC, INS_SAMPLER, INS_FM_TWO_OP};

/// An instrument descriptor. Doesn't hold any instrument state.
struct Instrument {
	InstrumentType type;
	//Spectra spectra;
	Envelope env[SYN_CHN_ENV_AMOUNT];				
	LFO lfo[SYN_CHN_LFO_AMOUNT];
	//char * name;
	float volume;				// volume in [0.0, 1.0]
	int octave;					// octave, 0 = C4
	WaveformFunc_t waveFunc;	// oscillator function
	SamplerFunc_t samplerFunc;	// sampler function
	OscFunc_2op_t fmFunc;		// any two-op generator function
	Sample * sample;			// Sample pointer for samplerFunc
	ModMatrix matrix;			// modulation matrix

};

#endif