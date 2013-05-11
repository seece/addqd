#ifndef EVENT_H_
#define EVENT_H_

#define ADQ_EVENT_NONE		0
#define ADQ_EVENT_NOTE_ON	1
#define ADQ_EVENT_NOTE_OFF	2
#define ADQ_EVENT_END_ALL	3
#define ADQ_EVENT_VOLUME	4

namespace addqd {
	struct Event {
		long when;		// event start time in samples
		char type;			// see the ADQ_EVENT_ defines
		unsigned char channel;	// channel num
		char note;
		unsigned char velocity;
		char payload[2];	// secondary data
	};

	// The event stack
	struct EventBuffer {
		Event * event_list;	// pointer to the event array
		int amount;		// how many events does the buffer currently hold
		int max_events;	// the size of the stack
	};

}

addqd::Event create_volume_event(long when, int channel, int volume);
addqd::Event create_note_event(long when, int channel, int pitch, bool state, unsigned char volume=200);
addqd::Event create_end_all_event(long when, int channel);

#ifndef INTROLIB
void print_event(addqd::Event& e);
#endif
#endif