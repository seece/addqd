
#include "synth.h"

// song specific initialization
static void init_loaded_song() {

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