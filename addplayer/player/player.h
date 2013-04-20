#ifndef PLAYER_H
#define PLAYER_H

namespace player {
	void init();
	bool load_song(EventBuffer * song);
	void update(EventBuffer * buffer, long samplecount);
}

#endif