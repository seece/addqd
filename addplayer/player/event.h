#ifndef EVENT_H_
#define EVENT_H_

#define ADQ_EVENT_NONE		0
#define ADQ_EVENT_NOTE_ON	1
#define ADQ_EVENT_NOTE_OFF	2
#define ADQ_EVENT_END_ALL	3
#define ADQ_EVENT_VOLUME	4

namespace addqd {
	struct Event {
		double when;		// event start time in seconds
		char type;			// see the ADQ_EVENT_ defines
		unsigned char channel;	// channel num
		char data[2];		// primary data
		char payload[4];	// secondary data
	};

	// The event stack
	struct EventBuffer {
		Event * event_list;	// pointer to the event array
		int amount;		// how many events does the buffer currently hold
		int max_events;	// the size of the stack
	};

}

addqd::Event create_volume_event(double when, int channel, int volume);
addqd::Event create_note_event(double when, int channel, int pitch, bool state, unsigned char volume=200);
addqd::Event create_end_all_event(double when, int channel);
#endif