#ifndef PHASEDIST_H
#define PHASEDIST_H

#include "../unit.h"
#include "../addsynth_types.h"
#include "../generators.h"


class CPhaseDist : public CUnit {
	public:
		CPhaseDist();
		~CPhaseDist();
		virtual float render(double phase, Voice* voicep);
		virtual void panic();
		int getParamNum();
		addqd::UnitType getType();

		static const int PARAMETER_AMOUNT = 4;

		static enum {
			PARAM_OCTAVE = 0,
			PARAM_FINE = 1,
			PARAM_MOD_A = 2,
			PARAM_MOD_B = 3
			};
	private:

};

#endif