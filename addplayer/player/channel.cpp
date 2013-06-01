#include <cstdio>
#include <cmath>
#include <cstring>
#include <cstddef>
#include "config.h"
#include "channel.h"
#include "unit.h"
#include "units/toneblock.h"
#include "eks_math.h"


Channel::Channel() {
	this->pan = 1.0f;
	this->volume = 1.0f;
	this->chain.numberOfEffects = 0;
	this->instrument = NULL;

	for (int i=0;i<Channel::MAX_UNITS;i++) {
		units[i] = NULL;
	}

	this->buffer = new SAMPLE_TYPE[SYN_MAX_BUFFER_SIZE*2];
	memset(this->buffer, 0, SYN_MAX_BUFFER_SIZE*2*sizeof(float));

	for (int i=0;i<SYN_CHN_LFO_AMOUNT;i++) {
		LFO* lfop = &this->lfo[i];
		lfop->frequency = 1.0f;
		lfop->gain = 1.0f;
		lfop->wavefunc = generators::sine;
	}

	// just testin'
	units[0] = new CToneBlock();
}

Channel::~Channel() {
	for (int i=0;i<Channel::MAX_UNITS;i++) {
		if (units[i] == NULL) {
			continue;
		}

		delete units[i];
	}

	delete this->buffer;
}

/// i: buffer array index
void Channel::render(Voice* voice, int i, long t_samples) {
	Instrument * ins = voice->channel->instrument;
	double phase = voice->phase;
	double ofs = 0.0;
	double sample = 0.0;

	if (!voice->active) {
		goto render_end;
	}

	if (voice->channel != this) {
		#ifdef DEBUG_CHANNEL_SANITY_CHECKS
			fprintf(stderr, "Warning: trying to render foreign channel #%d voice in ::render!\n", voice->channel_id);
			goto render_end;
		#endif
	}

	if (this->instrument == NULL) {
		#ifdef DEBUG_INSTRUMENTS
			fprintf(stderr, "Warning: NULL instrument on active voice. Channel: %d\n", 
				voice->channel_id);
		#endif
		return;
	}

	for (int u=0;u<Channel::MAX_UNITS;u++) {
		if (this->units[u] == NULL) {
			continue;
		}

		// TODO add wet/dry knob
		sample += this->units[u]->render(phase, voice);
	}

	//Channel * chan = voice_list[v].channel;
	WaveformFunc_t wavefunc = ins->waveFunc;

	double f = NOTEFREQ(voice->pitch+3+ins->octave*12);

	double t = t_samples/(double)AUDIO_RATE;

	/*
	switch (ins->type) {
		case INS_OSC:
			sample = (float)wavefunc(phase * 2.0 * PI);
			break;
		case INS_FM_TWO_OP:
			sample = (float)ins->fmFunc(phase * 2.0 * PI, 1.0, 0.0);
			break;
		case INS_SAMPLER:
			sample = (float)ins->samplerFunc(
				t - voice->envstate.beginTime/(double)AUDIO_RATE * (f/440.0), 
				ins->sample->data, 
				ins->sample->length);
			break;
		default:
			#ifdef DEBUG_INSTRUMENT_SANITY_CHECKS
				fprintf(stderr, "Invalid instrument type: 0x%x\n", ins->type);
			#endif
			break;
	}*/
	
	voice->phase = fmod(voice->phase + ((f/(double)AUDIO_RATE)), 1.0);
		
	if (t_samples - voice->envstate.endTime > ins->env[0].release*AUDIO_RATE) {
				
		if (voice->envstate.released) {
			voice->active=false;
		}
	}

	//sample *= voice->state.vol;

	render_end:

	this->buffer[i*2] += sample;
	this->buffer[i*2+1] += sample;
}