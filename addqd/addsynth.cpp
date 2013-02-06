
#include <cstdio>
#include <cmath>
#include <limits>
#include <Windows.h>
#include "util.h"
#include "config.h"
#include "addsynth.h"

static SynthState state;
static Channel * channelp;

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

// writes stereo samples to the given array
void syn_render_block(SAMPLE_TYPE * buf, int length) {
	double bonus = 0.0;

	for (int i=0;i<length;i++) {
		for (int c=0;c<state.channels;c++) {
			if (channelp[c].instrument == NULL) {
				continue;
			}

		}

		double t = state.time + i/(double)AUDIO_RATE;
		double f = 400.0 + bonus;
		buf[i*2] = (sin(2.0*3.14*t*f) * 0.4) *( 1.0+sin(2.0*3.14*t*0.2)*0.5);
		buf[i*2+1] = buf[i*2];
	}

	state.time += length/(double)AUDIO_RATE;
}

// loads an instrument to the given channel
void syn_load_instrument(int channel, Instrument * instrument) {
	if (channel >= state.channels) {
		fprintf(stderr, "Invalid channel number %d in %s!\n", channel, __FUNCDNAME__);
		return;
	}

	channelp->instrument = instrument;
}

void syn_free_instrument(Instrument * instrument) {
	delete instrument->spectra.interpolation;
	delete instrument->spectra.spectrum;
}

void create_spectrum(Spectrum * spectrum) {
	ZeroMemory(spectrum->bands, SYN_PARTIAL_AMOUNT);
}

void syn_init(int channels) {
	channelp = new Channel[channels];

	for (int i=0;i<channels;i++) {
		init_channel(&channelp[i]);
	}

	state.channels = channels;
	state.time = 0.0;

	return;
}

void syn_free(void) {
	delete channelp;
}