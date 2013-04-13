/*
*	Each channel can have multiple voices assigned to them.
*	Each voice has a pitch, envelope etc. and after the voice
*	is rendered, it's added to the channel audio bus.
*
*	After this, all DSP effects for this channel are processed,
*	and then routed to the main bus.
*
*	A voice can change during the buffer rendering.
*/

#ifndef ADDSYNTH_H
#define ADDSYNTH_H
#include "config.h"
#include "event.h"
#include "generators.h"

enum ParameterType {TYPE_FLOAT, TYPE_BOOL, TYPE_STEP12};
enum InstrumentType {INS_OSC, INS_SAMPLER};

// IL Harmor has 516 partials
#define SYN_PARTIAL_AMOUNT 20
#define SYN_MAX_EFFECTS 10
#define SYN_MAX_PARAMETERS 8
#define SYN_MAX_INSTRUMENTS 8
#define SYN_PARTIAL_HIGH_CUT 17500.0
#define SYN_SINE_TABLE_SIZE 2205
#define SYN_MAX_VOICES 64

// used with Channel.target_volume
#define SYN_VOLUME_LERP_THRESOLD (0.01f)

#define SYN_UBERSAMPLE 

#define INTERPOLATION_NONE 0
#define INTERPOLATION_LINEAR 1

#define NOTEMAGIC 1.059460646483
//#define PI 3.14159265
#define TAU (2*PI)
#define NOTEFREQ(pitch) pow(NOTEMAGIC, pitch) * 440.0

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

struct Sample {
	float * data;
	int length;
};

struct Instrument {
	InstrumentType type;
	//Spectra spectra;
	Envelope env;
	//char * name;
	float volume;
	int octave;
	WaveformFunc_t waveFunc;
	SamplerFunc_t samplerFunc;
	Sample * sample;
};

struct EnvState {
	bool hold;	// if a key is held down
	double beginTime;	// the moment (in seconds) when the key was repressed
	double endTime;
	bool released;
	double volume;	// volume set by note-on command
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
	float pan;
	EffectChain chain;
	SAMPLE_TYPE buffer[AUDIO_BUFFERSIZE*2];
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
	double time;
	int samples;
	int channels;
};

void create_spectrum(Spectrum * spectrum);
void syn_load_instrument(int slot, Instrument * instrument);
void syn_attach_instrument(int channel, int instrument_slot);
void syn_free_instrument(Instrument * instrument);
void syn_render_block(SAMPLE_TYPE * buf, int length, EventBuffer * eventbuffer);
void syn_init(int channels);
Instrument syn_create_instrument(InstrumentType type);
Voice * syn_play_note(int channel, int pitch);	
static void set_channel_volume(int channel, double volume);
void syn_end_note(int channel, int pitch);	
void syn_end_all_notes(int channel);
void syn_free();
Channel * syn_get_channel(int num);

#endif