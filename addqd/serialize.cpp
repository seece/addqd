#include <cstring>
#include <cassert>
#include "event.h"
#include "util.h"

int read_memdata(char * dest, const char * src, long * pos, size_t size) {
	memcpy((dest + *pos), src, size); 
	*pos+=size;
}

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
		memcpy(data+pos, &e.when, sizeof(&e.when)); pos+=sizeof(double);
		memcpy(data+pos, &e.type, sizeof(&e.type)); pos+=sizeof(char);
		memcpy(data+pos, &e.channel, sizeof(&e.channel)); pos+=sizeof(unsigned char);
		memcpy(data+pos, &e.data, 2*sizeof(char)); pos+=2*sizeof(char);
		memcpy(data+pos, &e.payload, 4*sizeof(char)); pos+=4*sizeof(char);
	}

	dprint(pos);
	dprint(totalsize);
	assert(pos == totalsize);

	return data;
}

Event * unserialize_event_array(const char * eventdata, int * amountp) {
	long pos = 0;
	int amount = -1;
	read_memdata((char *)&amount, eventdata, &pos, sizeof(int));
	Event * event_array = new Event[amount];
	*amountp = amount;

	return event_array;
}