#ifndef EVENT_H_
#define EVENT_H_

#define EVENT_NONE		0
#define EVENT_NOTE_ON	1
#define EVENT_NOTE_OFF	2
#define EVENT_END_ALL	3

struct Event {
	double when;		// event begin time in seconds
	char type;			// see the EVENT_ defines
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

Event create_note_event(double when, int channel, int pitch, bool state);
Event create_end_all_event(double when, int channel);
#endif