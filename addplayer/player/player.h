#ifndef PLAYER_H
#define PLAYER_H

Event * deserialize_event_array(const char * eventdata, int * amountp);

namespace player {
	void init();
	bool load_song(EventBuffer * song);
	void update(EventBuffer * buffer, long samplecount);
}

#endif