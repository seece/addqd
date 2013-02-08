#ifndef ADDSYNTH_H
#define ADDSYNTH_H
#include "config.h"

enum ParameterType {TYPE_FLOAT, TYPE_BOOL, TYPE_STEP12};

// IL Harmor has 516 partials
#define SYN_PARTIAL_AMOUNT 50
#define SYN_MAX_EFFECTS 10
#define SYN_MAX_PARAMETERS 8
#define SYN_MAX_INSTRUMENTS 8
#define SYN_PARTIAL_HIGH_CUT 17500.0
#define SYN_SINE_TABLE_SIZE 2205

#define EVENT_NONE		0
#define EVENT_NOTE_ON	1
#define EVENT_NOTE_OFF	2

#define INTERPOLATION_NONE 0
#define INTERPOLATION_LINEAR 1

#define NOTEMAGIC 1.059460646483
#define PI 3.14159265
#define TAU (2*PI)
#define NOTEFREQ(pitch) pow(NOTEMAGIC, pitch) * 440.0


struct Event {
	char type;
	unsigned char channel;
	char data[2];
	char payload[4];
};

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
	char attack;
	char decay;
	char hold;
	char sustain;
	char release;
};

struct Instrument {
	Spectra spectra;
	Envelope env;
	//char * name;
	float volume;
};

struct EnvState {
	bool hold;	// if a key is held down
	float lastPress;
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
	int pitch;	
	EnvState envstate;
	EffectChain chain;
};

struct SynthState {
	double time;
	int channels;
};

void create_spectrum(Spectrum * spectrum);
void syn_load_instrument(int slot, Instrument * instrument);
void syn_attach_instrument(int channel, int instrument_slot);
void syn_free_instrument(Instrument * instrument);
void syn_render_block(SAMPLE_TYPE * buf, int length);
void syn_init(int channels);
void syn_free();

#endif