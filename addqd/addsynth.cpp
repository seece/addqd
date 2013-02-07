
#include <cstdio>
#include <cmath>
#include <limits>
#include <Windows.h>
#include "util.h"
#include "config.h"
#include "addsynth.h"

static SynthState state;
static Channel * channel_list;
// an array of instrument pointers
static Instrument * instrument_list[SYN_MAX_INSTRUMENTS];

#define EFFECT_NONE 0
#define EFFECT_TEST 1
char * effectName[] = {"no effect", "test effect"};

static void init_effect(Effect * effect) {
	effect->name = effectName[EFFECT_NONE];
	effect->numParams = 0;
}

static void init_channel(Channel * channel) {
	channel->pan = 1.0f;
	channel->volume = 1.0f;
	channel->pitch = 0;
	channel->envstate.hold = false;
	channel->envstate.lastPress = MINUS_INFINITY;

	channel->chain.numberOfEffects = 0;
	channel->instrument = NULL;

	for (int i=0;i<SYN_MAX_EFFECTS;i++) {
		init_effect(&channel->chain.effects[i]);
	}
}

static void init_instrument(Instrument * ins) {
	ins->volume = 1.0f;
	ins->spectra.interpolation = NULL;
	ins->spectra.keyframe_amount = 0;
	ins->spectra.spectrum = NULL;
}

static void free_instrument(Instrument * ins) {
	if (ins->spectra.interpolation != NULL) {
		delete ins->spectra.interpolation;
	}

	if (ins->spectra.spectrum != NULL) {
		delete ins->spectra.spectrum;
	}
}

// writes stereo samples to the given array
void syn_render_block(SAMPLE_TYPE * buf, int length) {
	double bonus = 0.0;

	for (int i=0;i<length;i++) {
		for (int c=0;c<state.channels;c++) {
			if (channel_list[c].instrument == NULL) {
				continue;
			}

			double vol = channel_list[c].instrument->volume;
			double t = state.time + i/(double)AUDIO_RATE;
			double f = 400.0 + bonus;
			buf[i*2] = (sin(2.0*3.14*t*f) * 0.4) *( 1.0+sin(2.0*3.14*t*0.2)*0.5) * vol;
			buf[i*2+1] = buf[i*2];
		}	
	}

	state.time += length/(double)AUDIO_RATE;
}

// loads an instrument to the given slot
void syn_load_instrument(int slot, Instrument * instrument) {
	if (slot >= SYN_MAX_INSTRUMENTS) {
		fprintf(stderr, "Invalid slot number %d in %s!\n", slot, __FUNCDNAME__);
		return;
	}

	instrument_list[slot] = instrument;
	fprintf(stdout, "Instrument pointer 0x%X loaded to slot %d\n", instrument, slot);
}

void syn_attach_instrument(int channel, int instrument_slot) {
	if (channel >= state.channels) {
		fprintf(stderr, "Invalid channel number %d in %s!\n", channel, __FUNCDNAME__);
		return;
	} else if (instrument_slot >= SYN_MAX_INSTRUMENTS) {
		fprintf(stderr, "Invalid instrument slot %d in %s!\n", instrument_slot, __FUNCDNAME__);
		return;
	}

	channel_list[channel].instrument = instrument_list[instrument_slot];

	fprintf(stdout, "Ins %d attached to channel %d.\n", instrument_slot, channel);
}

void print_instrument_pointers(void) {
	for (int i=0;i<SYN_MAX_INSTRUMENTS;i++) {
		fprintf(stdout, "%d:\t%X\n", i, instrument_list[i]);
	}
}

void syn_free_instrument(Instrument * instrument) {
	delete instrument->spectra.interpolation;
	delete instrument->spectra.spectrum;
}

void create_spectrum(Spectrum * spectrum) {
	ZeroMemory(spectrum->bands, SYN_PARTIAL_AMOUNT);
}

void syn_init(int channels) {
	channel_list = new Channel[channels];

	for (int i=0;i<channels;i++) {
		init_channel(&channel_list[i]);
	}

	for (int i=0;i<SYN_MAX_INSTRUMENTS;i++) {
		instrument_list[i] = NULL;
		//init_instrument(&instrument_list[i]);
	}

	state.channels = channels;
	state.time = 0.0;

	return;
}

void syn_free(void) {
	// TODO free all channels too?
	delete channel_list;

	for (int i=0;i<SYN_MAX_INSTRUMENTS;i++) {
		//free_instrument(instrument_list[i]);
	}

	//delete instrument_list;
}