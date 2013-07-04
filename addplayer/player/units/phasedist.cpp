#include "phasedist.h"

CPhaseDist::CPhaseDist() {
	param_values[PARAM_OCTAVE] = 0.0f;
	param_values[PARAM_FINE] = 0.0f;
	param_values[PARAM_A] = 0.0f;
	param_values[PARAM_B] = 0.0f;
}

CPhaseDist::~CPhaseDist() {

}

float CPhaseDist::render(double phase, Voice* voicep) {
	return generators::resonant_fm(phase, param_values[PARAM_A], param_values[PARAM_B]);
}

void CPhaseDist::panic() {

}

int CPhaseDist::getParamNum() {
	return 2;
}

addqd::UnitType CPhaseDist::getType() {
	return addqd::UnitType::UNIT_PHASEDIST;
}