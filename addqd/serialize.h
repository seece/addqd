#ifndef SERIALIZE_H
#define SERIALIZE_H

#include "event.h"

char * serialize_event_array(Event event_array[], int amount, long * filesize);
Event * unserialize_event_array(const char * eventdata, int amount);
#endif