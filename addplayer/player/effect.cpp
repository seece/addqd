#include "../misc.h"
#include "config.h"
#include "effect.h"

#define SYN_EFFECT_NONE 0
#define SYN_EFFECT_TEST 1

char * effectName[] = {"no effect", "test effect"};


void init_effect(Effect * effect) {
	effect->name = effectName[SYN_EFFECT_NONE];
	effect->numParams = 0;
}

void free_effect(Effect * effect) {

}
