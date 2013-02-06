
#include <cmath>
#include "util.h"
#include "config.h"
#include "addsynth.h"

static double tt = 0.0;
Channel * channelp;

#define EFFECT_NONE 0
#define EFFECT_TEST 1
char * effectName[] = {"no effect", "test effect"};


static void init_effect(Effect * effect) {
	effect->name = effectName[EFFECT_NONE];
	effect->numParams = 0;
	
}

static void init_channel(Channel * channel) {
	for (int i=0;i<SYN_MAX_EFFECTS;i++) {
		init_effect(&channel->chain.effects[i]);
	}
	
}

// writes stereo samples to the given array
void syn_render_block(SAMPLE_TYPE * buf, int length) {
	double bonus = 0.0;

	for (int i=0;i<length;i++) {
		double t = tt + i/(double)AUDIO_RATE;
		double f = 400.0 + bonus;
		buf[i*2] = (sin(2.0*3.14*t*f) * 0.4) *( 1.0+sin(2.0*3.14*t*0.2)*0.5);
		buf[i*2+1] = buf[i*2];
	}

	tt += length/(double)AUDIO_RATE;
}

void syn_init(int channels) {
	channelp = new Channel[channels];

	for (int i=0;i<channels;i++) {
		init_channel(&channelp[i]);
	}

	return;
}

void syn_free(void) {
	delete channelp;
}