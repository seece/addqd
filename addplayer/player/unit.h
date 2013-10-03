#ifndef UNIT_H
#define UNIT_H

#include "device.h"

class CUnit : CDevice {
	public:
		CUnit();
		~CUnit();
		/// Renders a single sample with the given phase. Units can track time internally.
		virtual float render(double phase) = 0;
		/// Each Unit should have a function that kills all audio output
		virtual void panic() = 0;
		float param_values[CDevice::MAX_PARAMS];
	private:

};

#endif
