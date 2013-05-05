#ifndef INTERFACE_SYNTH_H
#define INTERFACE_SYNTH_H

// Do not include this with the regular synth headers (addsynth.h etc.)
// since it's meant to be used as the minimal standalone player interface.

#include "../player/config.h"
#include "../player/sound.h"
#include "../player/event.h" // TODO replace this with some standalone definitions
										// or wrap the loader in a load_binary_song function

#define MAGIC

void syn_init(int channels);

void init_sound(void);
void poll_sound(SynthRender_t synthRender, PollEventCallback_t updatePlayer);
void free_sound(void);

addqd::Event * deserialize_event_array(const char * eventdata, int * amountp);
void syn_render_block(SAMPLE_TYPE * buf, int length, addqd::EventBuffer * eventbuffer);

namespace player {
	void init();
	bool load_song(addqd::EventBuffer * song);
	void update(addqd::EventBuffer * buffer, long samplecount);
}

#endif