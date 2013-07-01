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
	this->selected_osc = generators::OSC_SINE;
	osc_functions[generators::OSC_SINE] = generators::sine;
	osc_functions[generators::OSC_SQUARE] = generators::square; 
	osc_functions[generators::OSC_SAW] = generators::sawsin; // TODO get a proper saw wave

	param_values[CToneBlock::PARAM_VOLUME] = 1.0f;
	param_values[CToneBlock::PARAM_OCTAVE] = 3.0f;
	param_values[CToneBlock::PARAM_FINE] = 0.0f;
	param_values[CToneBlock::PARAM_MOD_A] = 1.0f;
	param_values[CToneBlock::PARAM_MOD_B] = 1.0f;
}

CToneBlock::~CToneBlock() {

}

float CToneBlock::render(double phase, Voice* voicep) {
	#ifdef DEBUG_UNITS
		assert(static_cast<int>(selected_osc) < NUM_OSCILLATOR_TYPES);
	#endif

	int octave = (int)param_values[PARAM_OCTAVE];
	float fine = param_values[CToneBlock::PARAM_FINE];

	double f = NOTEFREQ(voicep->pitch+3+octave*12 + (double)fine);
	float sample = static_cast<float>(osc_functions[selected_osc](phase * 2.0 * PI));
	sample*=param_values[PARAM_VOLUME];
	sample*=voicep->state.vol;

	return sample;
}

void CToneBlock::panic() {

}

int CToneBlock::getParamNum() {
	return CToneBlock::PARAMETER_AMOUNT;
}

addqd::UnitType CToneBlock::getType() {
	return addqd::UnitType::UNIT_TONEBLOCK;
}