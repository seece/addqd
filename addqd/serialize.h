#ifndef SERIALIZE_H
#define SERIALIZE_H

#include "player/event.h"

char * serialize_event_array(Event event_array[], int amount, long * filesize);
Event * deserialize_event_array(const char * eventdata, int * amountp);
#endif