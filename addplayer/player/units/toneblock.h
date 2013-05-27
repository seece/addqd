#ifndef TONEBLOCK_H
#define TONEBLOCK_H
#include "../unit.h"

class CToneBlock : CUnit {
	public:
		CToneBlock();
		~CToneBlock();
		virtual float render(double phase);
		virtual void panic();
	private:
};

#endif