#ifndef SERIALIZE_H
#define SERIALIZE_H

#include "player/event.h"
#include "player/sound.h"
#include "modplayer.h"

char * serialize_event_array(addqd::Event event_array[], int amount, long * filesize);

// now implemented in addplayer/player/player.cpp
//Event * deserialize_event_array(const char * eventdata, int * amountp);

void record_events(PollEventCallback_t playfunc, long record_sample_num, addqd::EventBuffer * eventbuffer);
void event_to_string(const addqd::Event& e, char * output, int max_length);

#endif