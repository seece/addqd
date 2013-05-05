#ifndef PLAYER_H
#define PLAYER_H

addqd::Event * deserialize_event_array(const char * eventdata, int * amountp);

namespace player {
	void init();
	bool load_song(addqd::EventBuffer * song);
	void update(addqd::EventBuffer * buffer, long samplecount);
}

#endif