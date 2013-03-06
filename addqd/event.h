#ifndef EVENT_H
#define EVENT_H

struct Event {
	double when;		// event begin time in seconds
	char type;			// see the EVENT_ defines
	unsigned char channel;	// channel num
	char data[2];		// primary data
	char payload[4];	// secondary data
};


// The event stack
struct EventBuffer {
	Event * events;	// pointer to the event array
	int amount;		// how many events does the buffer currently hold
};

#endif