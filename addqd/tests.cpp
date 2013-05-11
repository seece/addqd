
#include <cassert>
#include "serialize.h"
#include "player/player.h"
#include "util.h"

using namespace addqd;

bool test_serialization(void) {
	long fsize=-1;	
	int eamount=-1;
	char * eventdata;
	Event * res;

	Event orig[5];
	orig[0].when = 5125;
	orig[0].type = ADQ_EVENT_NOTE_ON;
	orig[0].channel = 0;
	orig[0].note = 42;
	*(short *)(orig[0].payload) = 2<<10;	// test fitting a 16bit int to the payload field

	orig[1].when = 8000;
	orig[1].type = ADQ_EVENT_NOTE_OFF;
	orig[1].channel = 1;
	orig[1].note = -2;
	*(short *)(orig[1].payload) = 2<<9;	
	
	eventdata = serialize_event_array(orig, 2, &fsize);
	//dprint(fsize);

	res = deserialize_event_array(eventdata, &eamount);

	assert(eamount == 2);

	assert(res[0].channel	==	orig[0].channel);
	assert(res[0].type		==	orig[0].type);
	printf("%lf\n", orig[0].when);
	assert(res[0].when		==	orig[0].when);
	assert(res[0].note		==	orig[0].note);
	assert(*(short *)(res[0].payload)	== 2<<10);

	assert(res[1].channel	==	orig[1].channel);
	assert(res[1].type		==	orig[1].type);
	assert(res[1].when		==	orig[1].when);
	assert(res[1].note		==	orig[1].note);
	assert(*(short *)(res[1].payload)	== 2<<9);

	return true;
}