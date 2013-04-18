
#include <cassert>
#include "serialize.h"
#include "util.h"

bool test_serialization(void) {
	double thresold = 0.001;
	long fsize=-1;	
	int eamount=-1;
	char * eventdata;
	Event * res;

	Event orig[5];
	orig[0].when = 5.125;
	orig[0].type = ADQ_EVENT_NOTE_ON;
	orig[0].channel = 0;
	orig[0].data[0] = 42;
	orig[0].data[1] = 254;
	*(int *)(orig[0].payload) = 2<<30;	// test to fit a 32bit int to the payload field

	orig[1].when = 8.0;
	orig[1].type = ADQ_EVENT_NOTE_OFF;
	orig[1].channel = 1;
	orig[1].data[0] = 32;
	orig[1].data[1] = 150;
	*(int *)(orig[1].payload) = 2<<29;	
	
	eventdata = serialize_event_array(orig, 2, &fsize);
	//dprint(fsize);

	res = deserialize_event_array(eventdata, &eamount);

	assert(eamount == 2);

	assert(res[0].channel	==	orig[0].channel);
	assert(res[0].type		==	orig[0].type);
	printf("%lf\n", orig[0].when);
	assert(res[0].when		<=	orig[0].when+thresold
		&& res[0].when		>=	orig[0].when-thresold);
	assert(res[0].data[0]	==	orig[0].data[0]);
	assert(res[0].data[1]	==	orig[0].data[1]);
	assert(res[0].data[1]	==	orig[0].data[1]);
	assert(*(int *)(res[0].payload)	== 2<<30);

	assert(res[1].channel	==	orig[1].channel);
	assert(res[1].type		==	orig[1].type);
	assert(res[1].when		<=	orig[1].when+thresold
		&& res[1].when		>=	orig[1].when-thresold);
	assert(res[1].data[0]	==	orig[1].data[0]);
	assert(res[1].data[1]	==	orig[1].data[1]);
	assert(res[1].data[1]	==	orig[1].data[1]);
	assert(*(int *)(res[1].payload)	== 2<<29);


	
	return true;
}