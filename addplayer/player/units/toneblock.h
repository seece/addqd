#ifndef TONEBLOCK_H
#define TONEBLOCK_H
#include "../unit.h"
#include "../addsynth_types.h"


class CToneBlock : CUnit {
	public:
		CToneBlock();
		~CToneBlock();
		virtual float render(double phase);
		virtual void panic();
	private:
		static const int NUM_OSCILLATOR_TYPES = 3;
		static enum {OSC_SIN = 0, OSC_SQUARE, OSC_SAW} osc_type;
		static WaveformFunc_t osc_functions[NUM_OSCILLATOR_TYPES];

};

#endif