#include "../config.h"

#ifdef DEBUG_UNITS
	#include <cassert>
	#include <cstdio>
#endif

#include "toneblock.h"
#include "../addsynth_types.h"
#include "../generators.h"


CToneBlock::CToneBlock() {
	this->osc_type = OSC_SIN;
	osc_functions[this->OSC_SIN] = generators::sine;
	osc_functions[this->OSC_SQUARE] = generators::square; 
	osc_functions[this->OSC_SAW] = generators::sawsin; // TODO get a proper saw wave
}

CToneBlock::~CToneBlock() {

}

float CToneBlock::render(double phase) {
	#ifdef DEBUG_UNITS
		assert(static_cast<int>(osc_type) < NUM_OSCILLATOR_TYPES);
	#endif

	return static_cast<float>(osc_functions[osc_type](phase));
}

void CToneBlock::panic() {

};