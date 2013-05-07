#include <cstring>
#include <cassert>
#include "player/sound.h"
#include "player/event.h"
#include "util.h"

using namespace addqd;


char * serialize_event_array(Event event_array[], int amount, long * filesize) {
	
	long eventsize = sizeof(long) + sizeof(char)*5;
	// header + eventdata
	long totalsize = 4 + eventsize * amount;
	long pos = 0;
	char * data = new char[totalsize];
	*filesize = totalsize;

	memcpy(data+pos, &amount, sizeof(amount)); pos+=4;

	for (int i=0;i<amount;i++) {
		Event e = event_array[i];
		memcpy(data+pos, &e.when, sizeof(long)); pos+=sizeof(long);
		memcpy(data+pos, &e.type, sizeof(char)); pos+=sizeof(char);
		memcpy(data+pos, &e.channel, sizeof(unsigned char)); pos+=sizeof(unsigned char);
		memcpy(data+pos, &e.note, sizeof(char)); pos+=sizeof(char);
		memcpy(data+pos, &e.payload, 2*sizeof(char)); pos+=2*sizeof(char);
	}

	dprint(pos);
	dprint(totalsize);
	assert(pos == totalsize);

	return data;
}


// Records note events from the given playfunc for a while.
void record_events(	PollEventCallback_t playfunc, 
					long record_sample_num, 
					EventBuffer * eventbuffer) {
	assert(eventbuffer != NULL);

	long samples = 0;
	int temp_size = 1024;
	long blocksize = 4096;	// Block length in frames, used when requesting samples from the 
							// rendering function.
	EventBuffer tempbuffer;
	EventBuffer * bigbuf = eventbuffer;

	bigbuf->amount = 0;
	bigbuf->max_events = 1024;
	bigbuf->event_list = (Event *) malloc(sizeof(Event) * bigbuf->max_events);
	
	tempbuffer.event_list = (Event *) malloc(sizeof(Event) * temp_size);
	tempbuffer.max_events = temp_size;
	tempbuffer.amount = 0;

	while (samples < record_sample_num) {
		playfunc(&tempbuffer, blocksize);

		if (bigbuf->max_events <= (bigbuf->amount + tempbuffer.amount)) {
			// resize the buffer dynamically
			bigbuf->max_events *= 2;
			bigbuf->event_list = (Event *) realloc(bigbuf->event_list, sizeof(Event) * bigbuf->max_events);
			assert(bigbuf->event_list != NULL);
		}

		// copy the recorded events to the big event array
		for (int i=0;i<tempbuffer.amount;i++) {
			bigbuf->event_list[bigbuf->amount + i] = tempbuffer.event_list[i];
		}

		bigbuf->amount += tempbuffer.amount;

		tempbuffer.amount = 0;
		samples += blocksize;
	}

	free(tempbuffer.event_list);
}


void event_to_string(const Event& e, char * output, int max_length) {
	sprintf_s(output, max_length, "%lf\tCHN: %d\t0x%x 0x%x", e.when, e.channel, e.note, e.velocity);
}