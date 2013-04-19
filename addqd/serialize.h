#ifndef SERIALIZE_H
#define SERIALIZE_H

#include "player/event.h"
#include "player/sound.h"
#include "modplayer.h"

char * serialize_event_array(Event event_array[], int amount, long * filesize);
Event * deserialize_event_array(const char * eventdata, int * amountp);
void record_events(PollEventCallback_t playfunc, long record_sample_num, EventBuffer * eventbuffer);
void event_to_string(Event& e, char * output, int max_length);

#endif