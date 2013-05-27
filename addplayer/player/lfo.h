#ifndef LFO_H
#define LFO_H

#include "generators.h"

struct LFO {
	float frequency;
	float gain;
	WaveformFunc_t wavefunc;	// oscillator function
};

#endif