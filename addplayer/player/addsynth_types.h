/* 
* This file contains all datatypes used by the synthesizer.
*/

#ifndef ADDSYNTH_TYPES_H
#define ADDSYNTH_TYPES_H

//#include "../generators.h"
#include "config.h"

enum ParameterType {TYPE_FLOAT, TYPE_BOOL, TYPE_STEP12};
enum InstrumentType {INS_OSC, INS_SAMPLER, INS_FM_TWO_OP};

// IL Harmor has 516 partials
#define SYN_PARTIAL_AMOUNT 20
#define SYN_MAX_EFFECTS 10
#define SYN_MAX_PARAMETERS 8
#define SYN_MAX_INSTRUMENTS 8
#define SYN_PARTIAL_HIGH_CUT 17500.0
#define SYN_SINE_TABLE_SIZE 2205
#define SYN_MAX_VOICES 32
#define SYN_CHN_ENV_AMOUNT 2
#define SYN_CHN_LFO_AMOUNT 2

// used with Channel.target_volume
#define SYN_VOLUME_LERP_THRESOLD (0.01f)

#define SYN_UBERSAMPLE 

#define INTERPOLATION_NONE 0
#define INTERPOLATION_LINEAR 1

// a pointer to a function that generates a waveform when given the phase p
typedef double (*WaveformFunc_t)(double p);
//typedef double (*FMFunc_1op_t)(double p, double a);
//typedef double (*FMFunc_2op_t)(double p, double a, double b);
typedef double (*OscFunc_2op_t)(double p, double a, double b);
typedef double (*SamplerFunc_t)(double time, float * samplearray, int arraysize);

struct Spectrum {
	char bands[SYN_PARTIAL_AMOUNT];
};

// a collection of Spectrums
struct Spectra {
	int keyframe_amount;
	char * interpolation;	// interpolation mode of each keyframe
	Spectrum * spectrum;	// spectrum of each keyframe
};

struct Envelope {
	float attack;	// attack time in seconds
	float decay;
	float hold;
	float sustain;
	float release;
};

struct LFO {
	float frequency;
	float gain;
};

struct Sample {
	float * data;	// pointer to sample data
	int length;		// amount of samples in data
};

/// Channel modulation source container. All generated mod
/// signals are saved here on regular interval.
struct ModSource {
	float env[SYN_CHN_ENV_AMOUNT];
	float lfo[SYN_CHN_LFO_AMOUNT];
};

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

};

struct EnvState {
	bool hold;				// if a key is held down
	long beginTime;			// the moment in samples when the key was repressed
	long endTime;	
	bool released;
	double volume;			// volume set by note-on command
	double target_volume;	// if target_volume differs from volume, volume will be interpolated smoothly to it
};

struct Parameter {
	ParameterType type;
	union {
		int ivalue;
		float fvalue;
		int bvalue;
	} value;
	char * name;
};

struct Effect {
	char * name;
	int numParams;
	Parameter params[SYN_MAX_PARAMETERS];
};

struct EffectChain {
	int numberOfEffects;
	Effect effects[SYN_MAX_EFFECTS];
};

struct Channel {
	Instrument * instrument;
	float volume;
	float pan;					// channel pan, between [-1.0, 1.0]
	EffectChain chain;
	SAMPLE_TYPE * buffer;		// channel mixing buffer, see SYN_MAX_BUFFER_SIZE
	ModSource mod;				// mod source signals
};

struct Voice {
	int pitch;
	EnvState envstate;
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
};

#endif