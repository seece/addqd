#include <cstdio>
#include <cmath>
#include <cstring>
#include <cstddef>
#include "config.h"
#include "channel.h"
#include "unit.h"
#include "eks_math.h"

#define NOTEMAGIC 1.059460646483
//#define PI 3.14159265
#define TAU (2*PI)
#define NOTEFREQ(pitch) (pow(NOTEMAGIC, pitch) * 440.0)

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

double Channel::render(Voice* voice, long t_samples) {
	Instrument * ins = voice->channel->instrument;

	if (!voice->active) {
		return 0.0;
	}

	if (this->instrument == NULL) {
		#ifdef DEBUG_INSTRUMENTS
			fprintf(stderr, "Warning: NULL instrument on active voice. Channel: %d\n", 
				voice->channel_id);
		#endif
		return 0.0;
	}


	//Channel * chan = voice_list[v].channel;
	WaveformFunc_t wavefunc = ins->waveFunc;

	double f = NOTEFREQ(voice->pitch+3+ins->octave*12);

	double phase = voice->phase;
	double ofs = 0.0;
	double sample = 0.0;
	double t = t_samples/(double)AUDIO_RATE;

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
	}
	
	voice->phase = fmod(voice->phase + ((f/(double)AUDIO_RATE)), 1.0);
		
	if (t_samples - voice->envstate.endTime > ins->env[0].release*AUDIO_RATE) {
				
		if (voice->envstate.released) {
			voice->active=false;
		}
	}

	sample *= ins->volume * voice->state.vol;

	return sample;
}