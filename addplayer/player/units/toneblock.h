#ifndef TONEBLOCK_H
#define TONEBLOCK_H
#include "../unit.h"
#include "../addsynth_types.h"
#include "../generators.h"


class CToneBlock : public CUnit {
	public:
		CToneBlock();
		~CToneBlock();
		virtual float render(double phase, Voice* voicep);
		virtual void panic();
		int getParamNum();
		static const int NUM_OSCILLATOR_TYPES = 3;
		static generators::osc_type selected_osc;
		static WaveformFunc_t osc_functions[NUM_OSCILLATOR_TYPES];
		addqd::UnitType getType();

		static enum {
			PARAM_VOLUME = 0, 
			PARAM_OCTAVE = 1,
			PARAM_FINE = 2,
			PARAM_MOD_A = 3,	// operator multipliers for FM
			PARAM_MOD_B = 4
			};
	private:
		
		static const int PARAMETER_AMOUNT = 5;
		double voicePhases[SYN_MAX_VOICES]; // holds the current phase of each voice
};

#endif