#include <cstring>
#include <cassert>
#include "player/sound.h"
#include "player/event.h"
#include "util.h"


char * serialize_event_array(Event event_array[], int amount, long * filesize) {
	
	long eventsize = sizeof(double) + sizeof(char)*8;
	// header + eventdata
	long totalsize = 4 + eventsize * amount;
	long pos = 0;
	char * data = new char[totalsize];
	*filesize = totalsize;

	memcpy(data+pos, &amount, sizeof(amount)); pos+=4;

	for (int i=0;i<amount;i++) {
		Event e = event_array[i];
		memcpy(data+pos, &e.when, sizeof(double)); pos+=sizeof(double);
		memcpy(data+pos, &e.type, sizeof(char)); pos+=sizeof(char);
		memcpy(data+pos, &e.channel, sizeof(unsigned char)); pos+=sizeof(unsigned char);
		memcpy(data+pos, &e.data, 2*sizeof(char)); pos+=2*sizeof(char);
		memcpy(data+pos, &e.payload, 4*sizeof(char)); pos+=4*sizeof(char);
	}

	dprint(pos);
	dprint(totalsize);
	assert(pos == totalsize);

	return data;
}

Event * deserialize_event_array(const char * eventdata, int * amountp) {
	long pos = 0;
	int amount = -1;
	memcpy(&amount, eventdata, sizeof(int)); pos+=sizeof(int);
	Event * event_array = new Event[amount];
	*amountp = amount;

	for (int i=0;i<amount;i++) {
		Event e;
		memcpy(&e.when,		eventdata + pos, sizeof(double));	pos+=	sizeof(double);
		memcpy(&e.type,		eventdata + pos, sizeof(char));		pos+=	sizeof(char);
		memcpy(&e.channel,	eventdata + pos, sizeof(char));		pos+=	sizeof(unsigned char);
		memcpy(e.data,		eventdata + pos, 2*sizeof(char));	pos+=	2*sizeof(char);
		memcpy(e.payload,	eventdata + pos, 4*sizeof(char));	pos+=	4*sizeof(unsigned char);

		event_array[i] = e;	
	}

	return event_array;
}

// Records note events from the given playfunc for a while.
void record_events(	PollEventCallback_t playfunc, 
					long record_sample_num, 
					EventBuffer * eventbuffer) {
	assert(eventbuffer != NULL);

	long samples = 0;
	int temp_size = 1024;
	long blocksize = 2048;	// block length in samples
	int max_events = 1024;
	int event_num = 0;

	EventBuffer bigbuffer = {0,0,0};

	bigbuffer.event_list = (Event *) malloc(sizeof(Event) * max_events);

	EventBuffer tempbuffer;
	tempbuffer.event_list = (Event *) malloc(sizeof(Event) * temp_size);
	tempbuffer.max_events = temp_size;
	tempbuffer.amount = 0;

	while (samples < record_sample_num) {
		playfunc(&tempbuffer, blocksize);

		if (max_events <= (event_num + tempbuffer.amount)) {
			// resize the buffer dynamically
			max_events *= 2;
			bigbuffer.event_list = (Event *) realloc(tempbuffer.event_list, sizeof(Event) * max_events);
			assert(bigbuffer.event_list != NULL);
			//tempbuffer.event_list = (Event *) realloc(sizeof(Event) * temp_size);
		}

		// copy the recorded events to the big event array
		for (int i=0;i<tempbuffer.amount;i++) {
			bigbuffer.event_list[event_num + i] = tempbuffer.event_list[i];
		}

		event_num += tempbuffer.amount;

		tempbuffer.amount = 0;
		samples += blocksize;
	}

	free(tempbuffer.event_list);

	*eventbuffer = bigbuffer;
}


void event_to_string(Event& e, char * output, int max_length) {
	sprintf(output, "%lf\t0x%x 0x%x 0x%x", e.when, e.channel, e.data[0], e.data[1]);
}