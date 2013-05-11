/*
*	Each channel can have multiple voices assigned to them.
*	Each voice has a pitch, envelope etc. and after the voice
*	is rendered, it's added to the channel audio bus.
*
*	After this, all DSP effects for this channel are processed,
*	and then routed to the main bus.
*
*	A voice can change during the buffer rendering.
*/

#ifndef ADDSYNTH_H
#define ADDSYNTH_H
#include "config.h"
#include "event.h"
#include "generators.h"
#include "addsynth_types.h"

#define NOTEMAGIC 1.059460646483
//#define PI 3.14159265
#define TAU (2*PI)
#define NOTEFREQ(pitch) (pow(NOTEMAGIC, pitch) * 440.0)


void create_spectrum(Spectrum * spectrum);
Instrument** syn_get_instrument_list_pointer();
void syn_set_instrument_list_pointer(Instrument * listpointer);
void syn_attach_instrument(int channel, int instrument_slot);
void syn_free_instrument(Instrument * instrument);
void syn_render_block(SAMPLE_TYPE * buf, int length, addqd::EventBuffer * eventbuffer);
void syn_init(int channels);
Instrument syn_init_instrument(InstrumentType type);
Voice * syn_play_note(int channel, int pitch);	
static void set_channel_volume(int channel, double volume);
void syn_end_note(int channel, int pitch);	
void syn_end_all_notes(int channel);
void syn_free();
Channel * syn_get_channel(int num);

#endif