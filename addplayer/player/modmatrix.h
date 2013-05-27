#ifndef MODMATRIX_H
#define MODMATRIX_H

#include "config.h"

enum ModulationSignal {MOD_NONE, MOD_ENV1, MOD_ENV2, MOD_LFO1, MOD_LFO2};
// Used in ModTarget
enum ModTargetDevice {MOD_DEVICE_NONE, MOD_DEVICE_LOCAL, MOD_DEVICE_EFFECT};

// These values are used in ModTarget.param_index when 
// ModTarget.device == MOD_DEVICE_LOCAL
enum ModParamLocal {
	PARAM_VOLUME = 0, 
	PARAM_PAN = 1
}; 

/// Channel modulation source container. All generated mod
/// signals are saved here on regular interval.
struct ModSource {
	float env[SYN_CHN_ENV_AMOUNT];
	float lfo[SYN_CHN_LFO_AMOUNT];
};

struct ModTarget {
	ModTargetDevice device;	// target device
	int param_index;		// identifies the parameter. See ModParamLocal
};

struct ModRoute {
	bool enabled;
	ModulationSignal source;
	ModTarget target;
	float amount;
};


struct ModMatrix {
	ModRoute routes[SYN_CHN_MOD_AMOUNT];
};

#endif