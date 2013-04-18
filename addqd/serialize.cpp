#include <cstring>
#include <cassert>
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