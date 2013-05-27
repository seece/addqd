#ifndef UNIT_H
#define UNIT_H

#include "device.h"

class CUnit : CDevice {
	public:
		CUnit();
		~CUnit();
		virtual float render(double phase) = 0;
		virtual void panic() = 0;
	private:

};

#endif