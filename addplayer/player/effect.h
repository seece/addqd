#ifndef EFFECT_H
#define EFFECT_H

enum ParameterType {TYPE_FLOAT, TYPE_BOOL, TYPE_STEP12};

struct Parameter {
	ParameterType type;
	union {
		int ivalue;
		float fvalue;
		int bvalue;
	} value;
	char * name;
};

struct Effect {
	char * name;
	int numParams;
	Parameter params[SYN_MAX_PARAMETERS];
};

struct EffectChain {
	int numberOfEffects;
	Effect effects[SYN_MAX_EFFECTS];
};

void init_effect(Effect * effect);
void free_effect(Effect * effect);

#endif