#include "envelope.h"

void init_envelope(Envelope* env) {
	env->attack = 0.1f;
	env->decay= 0.1f;
	env->hold= 0.5f;
	env->release= 0.1f;
	env->sustain= 0.6f;
}

EnvState init_envstate(long sampleTime) {
	EnvState s;
	s.hold = true;
	s.beginTime = sampleTime;
	s.released = false;
	return s;
}