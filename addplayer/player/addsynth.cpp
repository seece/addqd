/// Core synth implementation.
///
/// All phases are stored as a value in range [0, 1]. The actual
/// phase is determined when rendering, and the phase variable
/// is multiplied with 2*PI.

#define VC_EXTRALEAN
#define WIN32_LEAN_AND_MEAN

#include <cstdio>
#include <cmath>
#include <limits>
#include <cstdlib>
#include <Windows.h>
#include "../misc.h"
#include "eks_math.h"
#include "config.h"
#include "addsynth.h"
#include "effect.h"

#ifndef INTROLIB
	#include <cassert>
#endif

using namespace addqd;

static SynthState state;
static Channel * channel_list;

// an array of instrument pointers
static Instrument * instrument_list = NULL;
static int instrument_list_max_length = SYN_MAX_INSTRUMENTS;
static Voice voice_list[SYN_MAX_VOICES];

static SAMPLE_TYPE * temp_array;
static SAMPLE_TYPE sine_LUT[SYN_SINE_TABLE_SIZE];

/// Initializes a voice state to its default values.
static void init_voice_state(VoiceState* vstatep) {
	vstatep->pan = 0.0f;
	vstatep->vol = 1.0f;
}

static void init_voice(Voice * v) {
	v->active = false;
	v->channel = NULL;
	v->envstate.hold = false;
	v->envstate.beginTime = 0;
	v->envstate.volume = 1.0;
	v->envstate.target_volume = v->envstate.volume;
	v->pitch = 0;

	init_voice_state(&v->state);

	memset(v->state.params, 0, sizeof(Parameter)*SYN_MAX_PARAMETERS);
	memset(v->state.mod_signals.env, 0, sizeof(float)*SYN_CHN_ENV_AMOUNT);
	memset(v->state.mod_signals.lfo, 0, sizeof(float)*SYN_CHN_LFO_AMOUNT);
}

static void init_voices() {
	for (int i=0;i<SYN_MAX_VOICES;i++) {
		init_voice(&voice_list[i]);
	}
}

static void init_envelope(Envelope* envp) {
	envp->attack = 0.05f;
	envp->release = 0.1f;
	envp->decay = 0.2f;
	envp->hold = 1.0f;
}

/*
static void init_lfo(LFO* lfop) {
	lfop->frequency = 1.0f;
	lfop->gain = 1.0f;
	lfop->wavefunc = generators::sine;
}
*/

static void init_route(ModRoute* routep) {
	routep->amount=1.0;
	routep->enabled=false;
	routep->source = MOD_NONE;
	routep->target.device = MOD_DEVICE_NONE;
	routep->target.param_index = 0;
}

static void init_instrument_values(Instrument * ins) {
	ins->volume = 1.0f;
	ins->waveFunc = NULL;
	ins->octave = 0;

	for (int i=0;i<SYN_CHN_ENV_AMOUNT;i++) {
		init_envelope(&ins->env[i]);
	}

	/*
	// check the Channel constructor for this
	for (int i=0;i<SYN_CHN_LFO_AMOUNT;i++) {
		init_lfo(&ins->lfo[i]);
	}
	*/

	for (int i=0;i<SYN_CHN_MOD_AMOUNT;i++) {
		init_route(&ins->matrix.routes[i]);
	}
};

// returns a pointer to the instrument list pointer
Instrument** syn_get_instrument_list_pointer() {
	return &instrument_list;
}

void syn_set_instrument_list_pointer(Instrument * listpointer) {
	instrument_list = listpointer;
}

LFO* syn_get_channel_lfo(int channel_id, int lfo_id) {
	#ifdef DEBUG_CHANNEL_SANITY_CHECKS
		assert(channel_id < state.channels);
	#endif

	return &channel_list[channel_id].lfo[lfo_id];
}

Instrument syn_init_instrument(InstrumentType type) {
	Instrument ins;

	init_instrument_values(&ins);
	ins.type = type;

	return ins;
}

static void free_instrument(Instrument * ins) {

}

void syn_free_instrument(Instrument * ins) {
	free_instrument(ins);
}

/// Finds the next event start time from the given eventbuffer.
long find_next_event_start(EventBuffer * eventbuffer, long start) {
	// TODO do a binary search here?
	for (int i=0;i<eventbuffer->amount;i++) {
		if (eventbuffer->event_list[i].when >= start) {
			return eventbuffer->event_list[i].when;
		}
	}

	return -1;
}

static void syn_process_event(Event * e) {
	#ifdef DEBUG_EVENT_SANITY_CHECKS
	if (e->channel < 0 || e->channel >= state.channels) {
		fprintf(stderr, "Invalid event channel %d.", e->channel);
		return;
	}
	#endif

	#ifdef SYN_DEBUG_EVENT
	//printf("CHN: %2d\t", e->channel);
	print_event(*e);
	#endif
	// used in note events
	int pitch = e->note;
	Voice * voice = NULL;
	unsigned char vol;
	double volume;

	switch (e->type) {
		case ADQ_EVENT_NONE:
			#ifdef SYN_DEBUG_EVENT
			printf("EVENT_NONE at %ld.\n", e->when);
			#endif
			break;
		case ADQ_EVENT_NOTE_ON:
			#ifdef SYN_DEBUG_EVENT
			printf("EVENT_NOTE_ON (%d) at %ld.\n", pitch, e->when);
			#endif

			voice = syn_play_note(e->channel, pitch, e->when);
			if (voice != NULL) {
				vol = e->velocity;
				voice->envstate.target_volume = vol/255.0;
			}

			break;
		case ADQ_EVENT_NOTE_OFF:
			#ifdef SYN_DEBUG_EVENT
			printf("EVENT_NOTE_OFF at %ld.\n", e->when);
			#endif

			syn_end_note(e->channel, pitch);
			break;
		case ADQ_EVENT_END_ALL:
			#ifdef SYN_DEBUG_EVENT
			printf("EVENT_END_ALL at %ld.\n", e->when);
			#endif

			syn_end_all_notes(e->channel);
			break;
		case ADQ_EVENT_VOLUME:
			vol = e->velocity;
			volume = vol/255.0;
			#ifdef SYN_DEBUG_EVENT
			printf("EVENT_VOLUME %d (%lf) at %%ld.\n", vol, volume, e->when);
			#endif

			set_channel_volume(e->channel, volume);
			break;
		default:
			#ifdef SYN_DEBUG_EVENT
			fprintf(stderr, "Invalid event type %d on channel %d.\n", e->type, e->channel);
			#endif

			return;
	}

	#ifdef SYN_DEBUG_EVENT
	//printf("\n");
	#endif
}


static void process_channel_modulation(Channel* channelp) {
	/*
	#ifdef DEBUG_CHANNEL_SANITY_CHECKS
		if (channelp == NULL) {
			fprintf(stderr, "Warning: trying to process NULL channel modulation!\n");
			return;
		}

		if (channelp->instrument == NULL) {
			fprintf(stderr, "Warning: instrument is NULL in channel modulation!\n");
			return;
		}
	#endif

	if (channelp->instrument == NULL) {
		return;
	}
	*/
}

/// Calculates voice ADSR-like envelope and stores the results in
/// voice->state.mod_signals array.
static void process_voice_envelope(Voice* voicep, double t) {
	Instrument * ins = voicep->channel->instrument;
	
	// envstate.beginTime and .endTime are in samples 
	// but all other envstate values are in seconds (double)
	// so we have to convert the values first

	double voicetime = t - voicep->envstate.beginTime/(double)AUDIO_RATE;
	double endtime = voicep->envstate.endTime/(double)AUDIO_RATE;

	// Calculate envelopes
	for (int i=0;i<SYN_CHN_ENV_AMOUNT;i++) {
		float envelope_amp = saturate(((voicetime+0.00001f))/ins->env[i].attack);

		if (voicep->envstate.released) {
			envelope_amp *= saturate(1.0f-(t - endtime)/ins->env[i].release);
		}

		voicep->state.mod_signals.env[i] = envelope_amp;
	}
}

static void process_voice_lfo(Voice* voicep, double t) {
	for (int i=0;i<SYN_CHN_LFO_AMOUNT;i++) {
		LFO* lfop = &voicep->channel->lfo[i];

		if (lfop->wavefunc == NULL) {
			continue;
		}

		float f = lfop->frequency;
		float signal = lfop->wavefunc(2.0*PI*t*f) * lfop->gain;
		voicep->state.mod_signals.lfo[i] = signal;
	}
}

static void process_voice_modulation(Voice* voicep, double t) {
	Channel* channel = voicep->channel;
	Instrument* ins = voicep->channel->instrument;
	ModMatrix* matrix = &voicep->channel->instrument->matrix;

	process_voice_envelope(voicep, t);
	process_voice_lfo(voicep, t);

	// Process modulation routing
	for (int i=0;i<SYN_CHN_MOD_AMOUNT;i++) {
		float signal = 0.0;
		ModRoute* route = &matrix->routes[i];
		
		if (!matrix->routes[i].enabled) {
			continue;
		}

		if (matrix->routes[i].source == MOD_DEVICE_NONE) {
			continue;
		}

		switch (matrix->routes[i].source) {
			case MOD_ENV1:
				signal = voicep->state.mod_signals.env[0];
				break;
			case MOD_ENV2:
				signal = voicep->state.mod_signals.env[1];
				break;
			case MOD_LFO1:
				signal = voicep->state.mod_signals.lfo[0];
				break;
			case MOD_LFO2:
				signal = voicep->state.mod_signals.lfo[1];
				break;
			default:
			#ifdef DEBUG_MODULATION_SANITY_CHECKS
				fprintf(stderr, "Invalid mod source %d on voice pointer %p!\n",
					matrix->routes[i].source,
					voicep);
			#endif
			continue;
		}

		if (route->target.device == MOD_DEVICE_EFFECT) {
			#ifdef DEBUG_MODULATION_SANITY_CHECKS
				fprintf(stderr, "Invalid target device %d, skipping...\n", 
					matrix->routes[i].target.device);
			#endif
			continue;
		}

		signal *= matrix->routes[i].amount;

		if (route->target.device == MOD_DEVICE_LOCAL) {
			switch (matrix->routes[i].target.param_index) {
				case PARAM_VOLUME:
				voicep->state.vol = signal;
				break;

				case PARAM_PAN:
				voicep->state.pan = signal;
				break;

				default:
				#ifdef DEBUG_MODULATION_SANITY_CHECKS
					fprintf(stderr, "Invalid mod target param_index %d on voice pointer %p!\n",
						matrix->routes[i].target.param_index,
						voicep);
				#endif
				continue;
			}

		} else if (route->target.device == MOD_DEVICE_UNIT) {
			int device_index = route->target.device_index;

			#ifdef DEBUG_MODULATION_SANITY_CHECKS
				assert(voicep->channel->units[device_index] != NULL);
				assert(voicep->channel->units[device_index]->getParamNum() > route->target.param_index);
			#endif

			CUnit* unit = voicep->channel->units[device_index];
			unit->param_values[route->target.param_index] = signal;
		}
		
	}
}

// writes stereo samples to the given array
// buf			sample buffer
// length		buffer length in frames
// eventbuffer	list of note events to apply on this buffer
// Compatible with SynthRender_t found from sound.h
void syn_render_block(SAMPLE_TYPE * buf, int length, EventBuffer * eventbuffer) {
	float bonus = 0.0;
	float sample = 0.0;
	double t;
	long t_samples;
	//double phase;
	//double f;
	double next_event_time = 0.0;

	#ifdef DEBUG_INSTRUMENT_SANITY_CHECKS
	if (instrument_list == NULL) {
		fprintf(stderr, "Warning: instrument_list is NULL!\n");
	}
	#endif

	#ifdef DEBUG_AUDIO_SANITY_CHECKS
	if (length > SYN_MAX_BUFFER_SIZE) {
		fprintf(stderr, "Warning: Requested buffer size too big: %d\n ", length);
	}
	#endif

	#ifdef DEBUG_EVENT_SANITY_CHECKS
		long buffer_end_time = (long)(state.samples+length);

		for (int u=0;u<eventbuffer->amount;u++) {
			if (eventbuffer->event_list[u].when > buffer_end_time) {
				fprintf(stderr, "Warning: trying to render future note events (diff:%lf)!\n", 
					eventbuffer->event_list[u].when - buffer_end_time);
			}
		}
	#endif

	int active = 0;

	float rate = (float)AUDIO_RATE;
	int current_event = 0;
	
	for (int i=0;i<length;i++) {
		bool env_counter_hit = false;

		t = state.time + (double)i/rate;	// time in seconds
		t_samples = state.samples + i;
		state.env_counter--;

		if (state.env_counter <= 0) {
			env_counter_hit = true;
			state.env_counter = SYN_ENVELOPE_JITTER;
		}

		//t_ms = long((state.samples + i)/44.1);

		while(eventbuffer->event_list[current_event].when <= t_samples) {
			if (current_event >= eventbuffer->amount) {
				break;
			}	
		
			syn_process_event(&eventbuffer->event_list[current_event]);
			current_event++;
		}

		if (env_counter_hit) {
			for (int c=0;c<state.channels;c++) {
				process_channel_modulation(&channel_list[c]);
			}
		}

		for (int v=0;v<SYN_MAX_VOICES;v++) {
			Voice * voice = &voice_list[v];
			
			// voice volume change smoothing
			// TODO implement actual interpolation
			float vol = (float)voice->envstate.volume;
			float target = (float)voice->envstate.target_volume;
			voice->envstate.volume = voice->envstate.target_volume;

			if (voice->channel == NULL) {
				continue;
			}

			Instrument * ins = voice->channel->instrument;

			if (env_counter_hit) {
				process_voice_modulation(&voice_list[v], t);
			}

			voice->channel->render(voice, i, t_samples);
			
			//sample *= ins->volume * float(envelope_amp) * float(voice->envstate.volume);
		}
	}	

	memset(buf, 0, length*sizeof(SAMPLE_TYPE)*2);

	for (int c=0;c<state.channels;c++) {
		if (channel_list[c].volume <= 0.0001) {
			continue;
		}

		double volume = channel_list[c].volume;

		for (int i=0;i<length*2;i++) {
			// TODO panning (use proper power distribution)
			buf[i] += channel_list[c].buffer[i] * float(volume);
		}

		memset(channel_list[c].buffer, 0, SYN_MAX_BUFFER_SIZE*2*sizeof(SAMPLE_TYPE));
	}

	state.samples = state.samples + length;
	state.time = state.samples/(double)AUDIO_RATE;
	state.time_ms = (long)(state.samples/44.1);

	if (eventbuffer->amount > current_event) {
		#ifdef DEBUG_EVENT_SANITY_CHECKS
		printf("Warning: eventbuffer not empty: %d/%d\n", current_event, eventbuffer->amount);
		#endif
		eventbuffer->amount -= current_event;	// try to recover gracefully and render
												// these on the next round
	} else {
		eventbuffer->amount=0;	
	}
}


EnvState init_envstate() {
	EnvState s;
	s.hold = true;
	s.beginTime = state.samples;
	s.released = false;
	return s;
}

// returns a pointer to the voice where the note was assigned to
// if no suitable voice is found, returns NULL
Voice * syn_play_note(int channel, int pitch, long t_samples) {
	int activeVoices = 0;

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
		voice->envstate = init_envstate();
		voice->pitch = pitch;

		init_voice_state(&voice->state);
		process_voice_modulation(voice, t_samples/(double)AUDIO_RATE); 

		#ifdef DEBUG_VOICE
		printf("new voice %d on chn %d!\n", v, voice->channel_id);
		#endif
		return voice;
	}

	#ifdef DEBUG_VOICE
	fprintf(stderr, "Warning: voice buffer overflow!\n");
	#endif
	return NULL;
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

		//voice_list[v].active = false;
		voice_list[v].envstate.released = true;
		voice_list[v].envstate.endTime = state.samples;
	}
}

void syn_end_all_notes(int channel) {
	for (int v=0;v<SYN_MAX_VOICES;v++) {
		if (!voice_list[v].active) {
			continue;
		}

		if (voice_list[v].envstate.released) {
			continue;
		}

		if (voice_list[v].channel_id == channel) {
			voice_list[v].envstate.released = true;
			voice_list[v].envstate.endTime = state.samples;
		}
	}
}

// finds all voices on the given channel and sets their volume
static void set_channel_volume(int channel, double volume) {
	for (int i=0;i<SYN_MAX_VOICES;i++) {
		Voice * v = &voice_list[i];
		
		if (!v->active) {
			continue;
		}

		if (v->channel_id == channel) {
			v->envstate.target_volume = volume;
		}
	}
}

void syn_attach_instrument(int channel, int instrument_slot) {
	if (channel >= state.channels) {
		fprintf(stderr, "Invalid channel number %d in %s!\n", channel, __FUNCDNAME__);
		return;
	} else if (instrument_slot >= SYN_MAX_INSTRUMENTS) {
		fprintf(stderr, "Invalid instrument slot %d in %s!\n", instrument_slot, __FUNCDNAME__);
		return;
	}

	channel_list[channel].instrument = &instrument_list[instrument_slot];

	printf("Ins %d attached to channel %d.\n", instrument_slot, channel);
}

void print_instrument_pointers(void) {
	for (int i=0;i<SYN_MAX_INSTRUMENTS;i++) {
		printf("%d:\t%X\n", i, &instrument_list[i]);
	}
}

void create_spectrum(Spectrum * spectrum) {
	ZeroMemory(spectrum->bands, SYN_PARTIAL_AMOUNT);
}

/// Get a channel pointer.
/// num: the channel index
Channel * syn_get_channel(int num) {
	#ifdef DEBUG_CHANNEL_SANITY_CHECKS
		if (num >= state.channels || num < 0) {
			fprintf(stderr, "%s: Invalid channel num %d!\n", __FUNCTIONW__, num);
			return NULL;
		}
	#endif

	return &channel_list[num];
}


void syn_init(int channels) {
	channel_list = new Channel[channels];

	for (int i=0;i<channels;i++) {
		//channel_list[i] = Channel();
	}

	init_voices();

	state.channels = channels;
	state.time = 0.0;
	state.time_ms = 0L;
	state.samples = 0L;
	state.env_counter = 0L;

	temp_array = new SAMPLE_TYPE[SYN_MAX_BUFFER_SIZE*2];

	return;
}


void syn_free(void) {
	delete[] channel_list;

	for (int i=0;i<SYN_MAX_INSTRUMENTS;i++) {
		//free_instrument(instrument_list[i]);
	}

	// the instrument list is allocated in outer scope, so we don't worry about that

	delete temp_array;
}