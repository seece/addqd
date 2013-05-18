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
#include "config.h"
#include "addsynth.h"
#include "effect.h"

// The size of the internal mixer buffer, the buffer size
// requested by the host shouldn't exceed this value.
// The size is in audio frames (stereo samples).
// SYN_AUDIO_SANITY_CHECKS toggles the error checks for this value.
#define SYN_MAX_BUFFER_SIZE 44100

using namespace addqd;

static SynthState state;
static Channel * channel_list;

// an array of instrument pointers
static Instrument * instrument_list = NULL;
static int instrument_list_max_length = SYN_MAX_INSTRUMENTS;
static Voice voice_list[SYN_MAX_VOICES];

static SAMPLE_TYPE * temp_array;
static SAMPLE_TYPE sine_LUT[SYN_SINE_TABLE_SIZE];

static void init_voice(Voice * v) {
	v->active = false;
	v->channel = NULL;
	v->envstate.hold = false;
	v->envstate.beginTime = 0;
	v->envstate.volume = 1.0;
	v->envstate.target_volume = v->envstate.volume;
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

	channel->chain.numberOfEffects = 0;
	channel->instrument = NULL;

	for (int i=0;i<SYN_MAX_EFFECTS;i++) {
		init_effect(&channel->chain.effects[i]);
	}

	for (int i=0;i<SYN_CHN_LFO_AMOUNT;i++) {
		channel->lfostate[i].phase = 0.0;
	}	

	channel->buffer = new SAMPLE_TYPE[SYN_MAX_BUFFER_SIZE*2];
	memset(channel->buffer, 0, SYN_MAX_BUFFER_SIZE*2*sizeof(float));
}

static void free_channel(Channel * channel) {
	delete channel->buffer;
	channel->buffer = NULL;

	for (int i=0;i<SYN_MAX_EFFECTS;i++) {
		free_effect(&channel->chain.effects[i]);
	}
}

static void init_envelope(Envelope* envp) {
	envp->attack = 0.05f;
	envp->release = 0.1f;
	envp->decay = 0.2f;
	envp->hold = 1.0f;
}

static void init_lfo(LFO* lfop) {
	lfop->frequency = 1.0f;
	lfop->gain = 1.0f;
	lfop->wavefunc = generators::sine;
}

static void init_route(ModRoute* routep) {
	routep->amount=1.0;
	routep->enabled=false;
	routep->source = MOD_NONE;
	routep->target.device = MOD_DEVICE_NONE;
	routep->target.param_index = 0;
}

static void syn_init_instrument(Instrument * ins) {
	ins->volume = 1.0f;
	ins->waveFunc = NULL;
	ins->octave = 0;

	for (int i=0;i<SYN_CHN_ENV_AMOUNT;i++) {
		init_envelope(&ins->env[i]);
	}

	for (int i=0;i<SYN_CHN_LFO_AMOUNT;i++) {
		init_lfo(&ins->lfo[i]);
	}

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

Instrument syn_init_instrument(InstrumentType type) {
	Instrument ins;
	ins.type = type;
	ins.waveFunc = NULL;
	ins.volume = 1.0f;
	ins.octave = 0;

	for (int i=0;i<SYN_CHN_ENV_AMOUNT;i++) {
		init_envelope(&ins.env[i]);
	}

	for (int i=0;i<SYN_CHN_ENV_AMOUNT;i++) {
		init_lfo(&ins.lfo[i]);
	}

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

			voice = syn_play_note(e->channel, pitch);
			if (voice != NULL) {
				vol = e->velocity;
				//voice->envstate.volume = vol/255.0;
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

	ModMatrix* matrix = &channelp->instrument->matrix;

	// Process LFOs

	for (int i=0;i<SYN_CHN_LFO_AMOUNT;i++) {
		#ifdef DEBUG_CHANNEL_SANITY_CHECKS
			if (channelp->instrument->lfo[i].wavefunc == NULL) {
				fprintf(stderr, "Warning: LFO wavefunc is NULL!\n");
				return;
			}
		#endif

		double phase = channelp->lfostate->phase;
		double f = channelp->instrument->lfo[i].frequency;
		double gain = channelp->instrument->lfo[i].gain;

		channelp->lfostate[i].phase = fmod(phase + ((f/(double)AUDIO_RATE)), 1.0);
		channelp->mod_signals.lfo[i] = gain * channelp->instrument->lfo[i].wavefunc(phase * 2.0 * PI); 
	}

	// Process modulation routing
	
	for (int i=0;i<SYN_CHN_MOD_AMOUNT;i++) {
		float signal = 0.0;
		
		if (!matrix->routes[i].enabled) {
			continue;
		}

		if (matrix->routes[i].source == MOD_DEVICE_NONE) {
			continue;
		}

		switch (matrix->routes[i].source) {
			case MOD_ENV1:
				signal = channelp->mod_signals.env[0];
				break;
			case MOD_ENV2:
				signal = channelp->mod_signals.env[1];
				break;
			default:
			#ifdef DEBUG_MODULATION_SANITY_CHECKS
				fprintf(stderr, "Invalid mod source %d on channel pointer %p!\n",
					matrix->routes[i].source,
					channelp);
			#endif
			continue;
		}

		if (matrix->routes[i].target.device != MOD_DEVICE_LOCAL) {
			// Currently effect parameter modulation is unsupported.
			#ifdef DEBUG_MODULATION_SANITY_CHECKS
				fprintf(stderr, "Invalid target device %d, skipping\n", 
					matrix->routes[i].target.device);
			#endif
			continue;
		}

		switch (matrix->routes[i].target.param_index) {
			case PARAM_VOLUME:
			break;

			case PARAM_PAN:
			break;

			default:
			#ifdef DEBUG_MODULATION_SANITY_CHECKS
				fprintf(stderr, "Invalid mod target param_index %d on channel pointer %p!\n",
					matrix->routes[i].target.param_index,
					channelp);
			#endif
			continue;
		}
		
	}
}

/// Calculates voice envelopes and stores them in the channel signal array.
static void process_voice_envelope(Voice* voicep, double t) {
	Instrument * ins = voicep->channel->instrument;
	double voicetime = t - voicep->envstate.beginTime;

	for (int i=0;i<SYN_CHN_ENV_AMOUNT;i++) {
		float envelope_amp = saturate(((voicetime+0.00001f))/ins->env[i].attack);

		if (voicep->envstate.released) {
			envelope_amp *= saturate(1.0f-(t-voicep->envstate.endTime)/ins->env[i].release);
		}

		voicep->channel->mod_signals.env[i] = envelope_amp;
	}
}

static void process_voice_modulation(Voice* voicep) {
	Instrument* ins = voicep->channel->instrument;
	ModMatrix* matrix = &voicep->channel->instrument->matrix;
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
	double phase;
	double f;
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
			if (abs(target - vol) > SYN_VOLUME_LERP_THRESOLD) {
				//voice->envstate.volume += (target - vol) * 0.4;
				voice->envstate.volume = voice->envstate.target_volume;
				//voice->envstate.volume += 0.005f *  
					//sgn(voice->envstate.target_volume - voice->envstate.volume);
			}

			if (voice->channel == NULL) {
				continue;
			}

			Instrument * ins = voice->channel->instrument;
			process_voice_envelope(&voice_list[v], t);

			if (env_counter_hit) {
				//process_voice_modulation(&voice_list[v]);
			}

			if (!voice_list[v].active) {
				continue;
			}

			if (voice_list[v].channel->instrument == NULL) {
				#ifdef DEBUG_INSTRUMENTS
					fprintf(stderr, "Warning: NULL instrument on active voice. Channel: %d\n", 
						voice_list[v].channel_id);
				#endif
				continue;
			}
	
			//Channel * chan = voice_list[v].channel;
			WaveformFunc_t wavefunc = ins->waveFunc;

			f = NOTEFREQ(voice_list[v].pitch+3+ins->octave*12);

			phase = voice->phase;
			double ofs = 0.0;

			switch (ins->type) {
				case INS_OSC:
					sample = (float)wavefunc(phase * 2.0 * PI);
					break;
				case INS_FM_TWO_OP:
					sample = (float)ins->fmFunc(phase * 2.0 * PI, 1.0, 0.0);
					break;
				case INS_SAMPLER:
					sample = (float)ins->samplerFunc(
						t - voice->envstate.beginTime * (f/440.0), 
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
		
			if (t-voice->envstate.endTime > ins->env[0].release) {
				
				if (voice->envstate.released) {
					voice->active=false;
				}
			}

			sample *= ins->volume;
			//sample *= ins->volume * float(envelope_amp) * float(voice->envstate.volume);

			voice_list[v].channel->buffer[i*2] += sample;
			voice_list[v].channel->buffer[i*2+1] += sample;
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
	s.beginTime = state.time;
	s.released = false;
	return s;
}

// returns a pointer to the voice where the note was assigned to
// if no suitable voice is found, returns NULL
Voice * syn_play_note(int channel, int pitch) {
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
		voice_list[v].envstate.endTime = state.time;
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
			voice_list[v].envstate.endTime = state.time;
		}
	}
}

// finds all voices on the given channel and sets their volume
// uses Channel.target_volume for smooth interpolation
static void set_channel_volume(int channel, double volume) {
	for (int i=0;i<SYN_MAX_VOICES;i++) {
		Voice * v = &voice_list[i];
		
		if (!v->active) {
			continue;
		}

		if (v->channel_id == channel) {
			//v->envstate.volume = volume;
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

	init_voices();

	for (int i=0;i<channels;i++) {
		init_channel(&channel_list[i]);
	}

	for (int i=0;i<SYN_MAX_INSTRUMENTS;i++) {
		//instrument_list[i] = NULL;
		//init_instrument(&instrument_list[i]);
	}

	state.channels = channels;
	state.time = 0.0;
	state.time_ms = 0L;
	state.samples = 0L;
	state.env_counter = 0L;

	temp_array = new SAMPLE_TYPE[SYN_MAX_BUFFER_SIZE*2];

	return;
}


void syn_free(void) {
	
	for (int c=0;c<state.channels;c++) {
		free_channel(&channel_list[c]);
	}

	delete channel_list;

	for (int i=0;i<SYN_MAX_INSTRUMENTS;i++) {
		//free_instrument(instrument_list[i]);
	}

	// the instrument list is allocated in outer scope, so we don't worry about that

	delete temp_array;
}