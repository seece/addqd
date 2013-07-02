
#include "synth.h"
#include "../player/addsynth.h"

using namespace addqd;

Instrument insarr[SYN_MAX_INSTRUMENTS];	// the size of this must match the variable 
											// instrument_list_max_length in addsynth.cpp

// song specific initialization
static void init_loaded_song() {
	Instrument * noise = &insarr[0];
	Instrument * tri = &insarr[1];
	Instrument * square = &insarr[2];
	Instrument * square2 = &insarr[3];

	*noise = syn_init_instrument(INS_OSC);
	noise->volume=0.3f;
	noise->waveFunc = *generators::noise;
	noise->octave = -3;

	*tri = syn_init_instrument(INS_OSC);
	tri->volume=0.5f;
	tri->octave=-2;
	tri->waveFunc = *generators::triangle;

	*square = syn_init_instrument(INS_OSC);
	square->volume=0.3f;
	square->octave = -1;
	square->waveFunc = *generators::sinsquare;

	*square2 = syn_init_instrument(INS_OSC);
	square2->volume=0.3f;
	square2->octave = -1;
	square2->waveFunc = *generators::square;

	Instrument** listpointer = syn_get_instrument_list_pointer();
	*listpointer = (Instrument *)&insarr;

	syn_attach_instrument(0, 1);
	syn_attach_instrument(1, 0);
	syn_attach_instrument(2, 2);
	syn_attach_instrument(3, 3);
	syn_attach_instrument(4, 1);
	syn_attach_instrument(5, 0);
	syn_attach_instrument(6, 0);
	syn_attach_instrument(7, 0);
}

// loads a song from binary dump
EventBuffer load_serialized_song(char * eventdata) {
	int event_amount = -1;
	Event * events = deserialize_event_array((char *)eventdata, &event_amount);
	EventBuffer buf;
	buf.amount = event_amount;
	buf.event_list = events;
	buf.max_events = event_amount;

	init_loaded_song();

	return buf;
}