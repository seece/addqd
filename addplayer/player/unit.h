#ifndef UNIT_H
#define UNIT_H

#include "device.h"
#include "voice.h"

class CUnit : public CDevice {
	public:
		CUnit();
		virtual ~CUnit();
		/// The actual rendering function may ignore the phase information
		/// altogether, and just use the voice play time. This is the case
		/// with e.g. sampler generators.
		virtual float render(double phase, Voice* voice) = 0;
		virtual void panic() = 0;
		float param_values[CDevice::MAX_PARAMS];
	private:

};

#endif