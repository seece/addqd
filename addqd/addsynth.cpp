
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
// TODO get this from the outside
static Instrument * instrument_list[SYN_MAX_INSTRUMENTS];
static Voice voice_list[SYN_MAX_VOICES];

#define EFFECT_NONE 0
#define EFFECT_TEST 1
char * effectName[] = {"no effect", "test effect"};

static void init_effect(Effect * effect) {
	effect->name = effectName[EFFECT_NONE];
	effect->numParams = 0;
}

static void init_voice(Voice * v) {
	v->active = false;
	v->channel = NULL;
	v->envstate.hold = false;
	v->envstate.lastPress = 0;
	v->pitch = 0;
}

static void init_voices() {
	for (int i=0;i<SYN_MAX_VOICES;i++) {
		init_voice(&voice_list[i]);
	}
}

static void init_channel(Channel * channel) {
	channel->pan = 1.0f;
	channel->volume = 1.0f;
	//channel->pitch = 0;
	//channel->envstate.hold = false;
	//channel->envstate.lastPress = MINUS_INFINITY;

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

static SAMPLE_TYPE temp_array[AUDIO_BUFFERSIZE*2];
static SAMPLE_TYPE sine_LUT[SYN_SINE_TABLE_SIZE];

float fast_sine(double phase) {
	int p = (fmod(phase, (double)PI*2.0)/(PI*2.0)) * SYN_SINE_TABLE_SIZE;
	return sine_LUT[p];
}

void init_sine_table(void) {
	double snr = 0.0;
	double rms_lut = 0.0;
	double rms = 0.0;
	double diff = 0.0;

	for (int i=0;i<SYN_SINE_TABLE_SIZE;i++) {
		double p = i/(double)SYN_SINE_TABLE_SIZE;
		sine_LUT[i] = sin(p*2.0*PI);
	}

	// check the LUT
	for (int i=0;i<SYN_SINE_TABLE_SIZE;i++) {
		double p = i/(double)SYN_SINE_TABLE_SIZE;
		double r = sin(p*2.0*PI);
		double r2 = fast_sine(p*2.0*PI); 
		rms += r * r;
		rms_lut += r2 * r2;

		if (i % 100 == 0) {
			printf("(%f %f)\n", r, r2);
		}

		if (diff < (r-r2)) {
			diff = r-r2;
		}
	}

	rms = sqrt(rms/(double)SYN_SINE_TABLE_SIZE);
	rms_lut = sqrt(rms_lut/(double)SYN_SINE_TABLE_SIZE);

	snr = pow(rms / rms_lut, 2.0);
	printf("Sine table SNR: %f\n", snr);
	printf("Sine table largest diff: %f\n", diff);


}

#define sine2(p) sine_LUT[(int)(fmod(p, (double)TAU)/(TAU) * SYN_SINE_TABLE_SIZE)]

// writes stereo samples to the given array
// buf		sample buffer
// length	buffer length in stereo samples
// Compatible with SynthRender_t found from sound.h
void syn_render_block(SAMPLE_TYPE * buf, int length) {
	double bonus = 0.0;
	
	for (int v=0;v<SYN_MAX_VOICES;v++) {
		if (!voice_list[v].active) {
			continue;
		}

		if (voice_list[v].channel->instrument == NULL) {
			continue;
		}

		//temp_array[]
			
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

	init_voices();

	for (int i=0;i<channels;i++) {
		init_channel(&channel_list[i]);
	}

	for (int i=0;i<SYN_MAX_INSTRUMENTS;i++) {
		instrument_list[i] = NULL;
		//init_instrument(&instrument_list[i]);
	}

	state.channels = channels;
	state.time = 0.0;

	init_sine_table();

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