
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
	v->envstate.beginTime = 0;
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

	memset(channel->buffer, 0, AUDIO_BUFFERSIZE*2);
}

static void init_instrument(Instrument * ins) {
	ins->volume = 1.0f;
	ins->waveFunc = sinf;
	//ins->spectra.interpolation = NULL;
	//ins->spectra.keyframe_amount = 0;
	//ins->spectra.spectrum = NULL;
}

static void free_instrument(Instrument * ins) {
	//if (ins->spectra.interpolation != NULL) {
	//	delete ins->spectra.interpolation;
	//}
	//
	//if (ins->spectra.spectrum != NULL) {
	//	delete ins->spectra.spectrum;
	//}
}

void syn_free_instrument(Instrument * ins) {
	free_instrument(ins);
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
	float bonus = 0.0;
	float sample = 0.0;
	float t;
	float envelope_amp;
	float f;

	if (length > AUDIO_BUFFERSIZE*2) {
		fprintf(stderr, "Warning: Requesting too big buffersize: %d\n ", length);
	}

	int active = 0;
	
	for (int v=0;v<SYN_MAX_VOICES;v++) {
		if (!voice_list[v].active) {
			continue;
		}

		active++;

		if (voice_list[v].channel->instrument == NULL) {
			continue;
		}

		Voice * voice = &voice_list[v];
		Instrument * ins = voice->channel->instrument;
		WaveformFunc_t wavefunc = voice_list[v].channel->instrument->waveFunc;

		f = NOTEFREQ(voice_list[v].pitch+3);
		float rate = (float)AUDIO_RATE;

		for (int i=0;i<length;i++) {
			t = state.time + i/rate;
			#ifdef SYN_UBERSAMPLE
				//sample = 0.5f + wavefunc(2.0*PI*t*f)*0.5f;
				sample = 0.5f + wavefunc(fmod(2.0*PI*t*f, 2.0*PI))*0.5f;
			#else
				sample = 0.5f + wavefunc(2.0*PI*t*f)*0.5f;
			#endif
			
			//sample = 0.5f + sinf(2.0*PI*t*f)*0.25f + sinf(2.0*PI*t*f+0.0f)*0.25;
			envelope_amp = saturate(((t-voice->envstate.beginTime)+0.001f)/ins->env.attack);

			if (voice->envstate.released) {
				envelope_amp *= saturate(1.0f-(t-voice->envstate.endTime)/ins->env.release);
			}

			if (t-voice->envstate.endTime > ins->env.release) {
				
				if (voice->envstate.released) {
					voice->active=false;
					
				}
			}

		
			sample *= 0.3f * envelope_amp;

			

			voice_list[v].channel->buffer[i*2] += sample;
			voice_list[v].channel->buffer[i*2+1] += sample;
		}
	}	

	memset(buf, 0, length*sizeof(SAMPLE_TYPE)*2);

	for (int c=0;c<state.channels;c++) {
		if (channel_list[c].volume == 0.0) {
			continue;
		}

		double volume = channel_list[c].volume;

		for (int i=0;i<length*2;i++) {
			// TODO panning (user proper power distribution)
			buf[i] += channel_list[c].buffer[i] * volume;
		}

		memset(&channel_list[c].buffer, 0, AUDIO_BUFFERSIZE*2*sizeof(SAMPLE_TYPE));
	}

	state.time += length/(double)AUDIO_RATE;

	printf("voices active: %d\n", active);
}


EnvState constructEnvstate() {
	EnvState s;
	s.hold = true;
	s.beginTime = state.time;
	s.released = false;
	return s;
}

void syn_play_note(int channel, int pitch) {
	int activeVoices = 0;

	// skip if this voice is already playing at this pitch
	/*
	for (int v=0;v<SYN_MAX_VOICES;v++) {
		Voice * voice = &voice_list[v];
		if (voice->active && voice->channel_id == channel) {
			if (!voice->envstate.released && voice->pitch == pitch) {
				return;
			}
		}
	}
	*/

	for (int v=0;v<SYN_MAX_VOICES;v++) {
		Voice * voice = &voice_list[v];

		if (voice->active) {
			//if (voice->channel_id != channel) {
				continue;
			//}
		}

		voice->active = true;
		voice->channel = &channel_list[channel];
		voice->channel_id = channel;
		voice->envstate = constructEnvstate();
		voice->pitch = pitch;

		printf("new voice %d!\n", v);
		return;
	}

	fprintf(stderr, "Warning: voice buffer overflow!\n");
}

// stops a playing voice
void syn_end_note(int channel, int pitch) {
	for (int v=0;v<SYN_MAX_VOICES;v++) {
		if (!voice_list[v].active) {
			continue;
		}

		if (voice_list[v].channel_id != channel) {
			continue;
		}

		if (voice_list[v].pitch != pitch) {
			continue;
		}

		if (voice_list[v].envstate.released) {
			continue;
		}

		printf("%d ends here\n", channel);

		//voice_list[v].active = false;
		voice_list[v].envstate.released = true;
		voice_list[v].envstate.endTime = state.time;
	}
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