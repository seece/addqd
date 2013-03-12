
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
	v->envstate.volume = 1.0;
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

	memset(channel->buffer, 0, AUDIO_BUFFERSIZE*2);
}

static void init_instrument(Instrument * ins) {
	ins->volume = 1.0f;
	ins->waveFunc = sin;
	ins->octave=0;
	ins->env.attack=0.1f;
	ins->env.release=0.1f;
}

static void free_instrument(Instrument * ins) {

}

void syn_free_instrument(Instrument * ins) {
	free_instrument(ins);
}

static SAMPLE_TYPE temp_array[AUDIO_BUFFERSIZE*2];
static SAMPLE_TYPE sine_LUT[SYN_SINE_TABLE_SIZE];

// by Alexander Kritov
double sawsin(double x)
{
   double t = fmod(x/(2*PI),(double)1.0);
   if (t>0.5)
	   return -sin(x);
   if (t<=0.5)
	   return (double)2.0*t-1.0;
}

static double find_next_event_start(EventBuffer * eventbuffer, double start) {
	// TODO do a binary search here?
	for (int i=0;i<eventbuffer->amount;i++) {
		if (eventbuffer->event_list[i].when >= start) {
			return eventbuffer->event_list[i].when;
		}
	}

	return -1.0;
}

static void syn_process_event(Event * e) {
	#ifdef DEBUG_EVENT_SANITY_CHECKS
	if (e->channel < 0 || e->channel >= state.channels) {
		fprintf(stderr, "Invalid event channel %d.", e->channel);
		return;
	}
	#endif

	#ifdef DEBUG_EVENT
	printf("CHN: %2d\t", e->channel);
	#endif
	// used in note events
	int pitch = e->data[0] << 8 | e->data[1];	
	Voice * voice = NULL;
	unsigned char vol;
	double volume;

	switch (e->type) {
		case EVENT_NONE:
			#ifdef DEBUG_EVENT
			printf("EVENT_NONE at %lf.", e->when);
			#endif
			break;
		case EVENT_NOTE_ON:
			#ifdef DEBUG_EVENT
			printf("EVENT_NOTE_ON (%d) at %lf.", pitch,e->when);
			#endif

			voice = syn_play_note(e->channel, pitch);
			if (voice != NULL) {
				vol = ((unsigned char)(e->payload[0]));
				voice->envstate.volume = vol/255.0;
			}

			break;
		case EVENT_NOTE_OFF:
			#ifdef DEBUG_EVENT
			printf("EVENT_NOTE_OFF at %lf.", e->when);
			#endif

			syn_end_note(e->channel, pitch);
			break;
		case EVENT_END_ALL:
			#ifdef DEBUG_EVENT
			printf("EVENT_END_ALL at %lf.", e->when);
			#endif

			syn_end_all_notes(e->channel);
			break;
		case EVENT_VOLUME:
			vol = e->data[0];
			volume = vol/255.0;
			#ifdef DEBUG_EVENT
			printf("EVENT_VOLUME %d (%lf) at %lf.", vol, volume, e->when);
			#endif

			set_channel_volume(e->channel, volume);
			break;
		default:
			#ifdef DEBUG_EVENT
			fprintf(stderr, "Invalid event type %d on channel %d.\n", e->type, e->channel);
			#endif

			return;
	}

	#ifdef DEBUG_EVENT
	printf("\n");
	#endif
}

// writes stereo samples to the given array
// buf			sample buffer
// length		buffer length in stereo samples
// eventbuffer	list of note events to apply on this buffer
// Compatible with SynthRender_t found from sound.h
void syn_render_block(SAMPLE_TYPE * buf, int length, EventBuffer * eventbuffer) {
	float bonus = 0.0;
	float sample = 0.0;
	double t;
	double phase;
	double envelope_amp;
	double f;
	double next_event_time = 0.0;

	if (length > AUDIO_BUFFERSIZE*2) {
		fprintf(stderr, "Warning: Requesting too big buffersize: %d\n ", length);
	}

	#ifdef DEBUG_EVENT_SANITY_CHECKS
		double buffer_end_time = state.time+length/(float)AUDIO_RATE;
		for (int u=0;u<eventbuffer->amount;u++) {
			if (eventbuffer->event_list[u].when > buffer_end_time) {
				fprintf(stderr, "Warning: trying to render future note events (diff:%lf)!\n", 
					eventbuffer->event_list[u].when - buffer_end_time);
			}
		}
	#endif

	int active = 0;

	float rate = (float)AUDIO_RATE;
	double addition = (double)1.0/(double)rate;
	int current_event = 0;

	for (int i=0;i<length;i++) {
		t = state.time + (double)i/rate;

		while(eventbuffer->event_list[current_event].when <= t) {
			if (current_event >= eventbuffer->amount) {
				break;
			}	

			syn_process_event(&eventbuffer->event_list[current_event]);

			current_event++;
		}
		

		for (int v=0;v<SYN_MAX_VOICES;v++) {
			if (!voice_list[v].active) {
				continue;
			}

			if (voice_list[v].channel->instrument == NULL) {
				#ifdef DEBUG_INSTRUMENTS
					fprintf(stderr, "Warning: NULL instrument on active voice %d\n", 
						voice_list[v].channel_id);
				#endif
				continue;
			}

			Voice * voice = &voice_list[v];
			Instrument * ins = voice->channel->instrument;
			WaveformFunc_t wavefunc = voice_list[v].channel->instrument->waveFunc;

			f = NOTEFREQ(voice_list[v].pitch+3+ins->octave*12);
			envelope_amp = saturate(((t-voice->envstate.beginTime)+0.00001f)/ins->env.attack);

			if (voice->envstate.released) {
				envelope_amp *= saturate(1.0f-(t-voice->envstate.endTime)/ins->env.release);
			}

			phase = voice->phase;
			double ofs = 0.0;
			sample = float(wavefunc(phase * 2.0 * PI));
			voice->phase = fmod(voice->phase + ((f/(double)AUDIO_RATE)), 1.0);
		
			if (t-voice->envstate.endTime > ins->env.release) {
				
				if (voice->envstate.released) {
					voice->active=false;
				}
			}

			sample *= ins->volume * float(envelope_amp) * float(voice->envstate.volume);

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
			buf[i] += channel_list[c].buffer[i] * float(volume);
		}

		memset(&channel_list[c].buffer, 0, AUDIO_BUFFERSIZE*2*sizeof(SAMPLE_TYPE));
	}

	state.time += length/(double)AUDIO_RATE;
	state.samples = state.samples + length;

	if (eventbuffer->amount > current_event) {
		#ifdef DEBUG_EVENT_SANITY_CHECKS
		printf("Warning: eventbuffer not empty: %d/%d\n", current_event, eventbuffer->amount);
		#endif
		eventbuffer->amount -= current_event;	// try to recover gracefully and render
												// these on then next round
	} else {
		eventbuffer->amount=0;	
	}
}


EnvState constructEnvstate() {
	EnvState s;
	s.hold = true;
	s.beginTime = state.time;
	s.released = false;
	return s;
}

// returns a pointer to the voice where the note was assigned to
// if no suitable voices are found, returns NULL
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
		voice->envstate = constructEnvstate();
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
static void set_channel_volume(int channel, double volume) {
	for (int i=0;i<SYN_MAX_VOICES;i++) {
		Voice * v = &voice_list[i];
		
		if (!v->active) {
			continue;
		}

		if (v->channel_id == channel) {
			v->envstate.volume = volume;
		}
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

	printf("Ins %d attached to channel %d.\n", instrument_slot, channel);
}

void print_instrument_pointers(void) {
	for (int i=0;i<SYN_MAX_INSTRUMENTS;i++) {
		printf("%d:\t%X\n", i, instrument_list[i]);
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
	state.samples = 0;

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