/* 
* This file contains datatypes used by the synthesizer.
*/

#ifndef ADDSYNTH_TYPES_H
#define ADDSYNTH_TYPES_H

//#include "../generators.h"
#include "config.h"
#include "effect.h"

enum InstrumentType {INS_OSC, INS_SAMPLER, INS_FM_TWO_OP};
enum ModulationSignal {MOD_NONE, MOD_ENV1, MOD_ENV2, MOD_LFO1, MOD_LFO2};
// Used in ModTarget
enum ModTargetDevice {MOD_DEVICE_NONE, MOD_DEVICE_LOCAL, MOD_DEVICE_EFFECT};

// These values are used in ModTarget.param_index when 
// ModTarget.device == MOD_DEVICE_LOCAL
enum ModParamLocal {
	PARAM_VOLUME = 0, 
	PARAM_PAN = 1
}; 

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
	WaveformFunc_t wavefunc;	// oscillator function
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

struct ModTarget {
	ModTargetDevice device;	// target device
	int param_index;		// identifies the parameter. See ModParamLocal
};

struct ModRoute {
	bool enabled;
	ModulationSignal source;
	ModTarget target;
	float amount;
};

struct ModMatrix {
	ModRoute routes[SYN_CHN_MOD_AMOUNT];
};

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

struct Channel {
	Instrument * instrument;
	float volume;
	float pan;					// channel pan, between [-1.0, 1.0]
	EffectChain chain;
	SAMPLE_TYPE * buffer;		// channel mixing buffer, see SYN_MAX_BUFFER_SIZE
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