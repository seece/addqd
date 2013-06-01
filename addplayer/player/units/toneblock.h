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

	private:
		static const int PARAM_VOLUME = 0;
		
		

};

#endif