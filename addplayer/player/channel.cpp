#include <cstdio>
#include <cmath>
#include <cstring>
#include <cstddef>
#include "config.h"
#include "channel.h"
#include "envelope.h"
#include "unit.h"
#include "units/toneblock.h"
#include "eks_math.h"


Channel::Channel() {
	//this->index = index;	// the index is set in void syn_init function
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

	for (int i=0;i<SYN_CHN_ENV_AMOUNT;i++) {
		init_envelope(&env[i]);
	}

	// TODO read device configuration from a file/binary blob
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

void Channel::render(int i, long t_samples) {
	for (int u=0;u<SYN_MAX_VOICES;u++) {
		Voice* voice = this->voicelist.voices[u];

		if (voice == NULL) 
			continue;

		if (!voice->active)
			continue;

		renderVoice(voice, i, t_samples);
	}
}

/// i: audio buffer array index
void Channel::renderVoice(Voice* voice, int i, long t_samples) {
	Instrument * ins = voice->channel->instrument;
	double phase = voice->phase;
	double ofs = 0.0;
	double sample = 0.0;

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

	// TODO use table lookup for note pitch 
	double f = NOTEFREQ(voice->pitch+3+ins->octave*12);
	double t = t_samples/(double)AUDIO_RATE;

	voice->phase = fmod(voice->phase + ((f/(double)AUDIO_RATE)), 1.0);
		
	if (t_samples - voice->envstate.endTime > this->env[0].release*AUDIO_RATE) {
		if (voice->envstate.released) {
			this->removePlayingVoice(voice);
			voice->active = false;
			voice->channel = NULL;
		}
	}

	//sample *= voice->state.vol;

	render_end:

	this->buffer[i*2] += sample;
	this->buffer[i*2+1] += sample;
}

void Channel::addPlayingVoice(Voice* voice) {
	voice->channel = this;
	voice->channel_id = this->index;
	this->voicelist.addVoice(voice);
}

void Channel::removePlayingVoice(Voice* voice) {
	this->voicelist.removeVoice(voice);
}

void Channel::processModulation() {

}

int Channel::unitsLoaded() {
	int amount = 0;

	for (int u=0;u<Channel::MAX_UNITS;u++) {
		if (this->units[u] == NULL) {
			continue;
		}

		amount++;
	}

	return amount;
}

LFO* Channel::getLFO(int index) {
	#ifdef DEBUG_CHANNEL_SANITY_CHECKS
	if (index < 0 || index >= SYN_CHN_LFO_AMOUNT) {
		fprintf(stderr, "LFO index out of bounds: %d\n", index);
		return nullptr;
	}
	#endif

	return &this->lfo[index];
}

Envelope* Channel::getEnvelope(int index) {
	#ifdef DEBUG_CHANNEL_SANITY_CHECKS
	if (index < 0 || index >= SYN_CHN_ENV_AMOUNT) {
		fprintf(stderr, "ENV index out of bounds: %d\n", index);
		return nullptr;
	}
	#endif

	return &this->env[index];
}