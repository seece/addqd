#include "../config.h"

#ifdef DEBUG_UNITS
	#include <cassert>
	#include <cstdio>
#endif

#include <cmath>
#include "../eks_math.h"
#include "toneblock.h"
#include "../addsynth_types.h"
#include "../generators.h"

generators::osc_type CToneBlock::selected_osc;
WaveformFunc_t CToneBlock::osc_functions[NUM_OSCILLATOR_TYPES];

CToneBlock::CToneBlock() {
	this->selected_osc = generators::OSC_SIN;
	osc_functions[generators::OSC_SIN] = generators::sine;
	osc_functions[generators::OSC_SQUARE] = generators::square; 
	osc_functions[generators::OSC_SAW] = generators::sawsin; // TODO get a proper saw wave

	param_values[PARAM_VOLUME] = 1.0f;
}

CToneBlock::~CToneBlock() {

}

float CToneBlock::render(double phase, Voice* voicep) {
	#ifdef DEBUG_UNITS
		assert(static_cast<int>(selected_osc) < NUM_OSCILLATOR_TYPES);
	#endif

	int octave = 1;

	double f = NOTEFREQ(voicep->pitch+3+octave*12);

	return static_cast<float>(osc_functions[selected_osc](phase)) * param_values[PARAM_VOLUME];
}

void CToneBlock::panic() {

};

int CToneBlock::getParamNum() {
	return 1;
}