#ifndef GENERICUNITVIEW_H
#define GENERICUNITVIEW_H

#include "../JuceLibraryCode/JuceHeader.h"
#include "logger.h"
#include "player/addsynth.h"
#include "player/units/toneblock.h"
#include "KnobList.h"
#include "UnitView.h"

class GenericUnitView :	public UnitView
{
	public:
		GenericUnitView(Channel* targetChannel, CUnit* targetUnit);
		~GenericUnitView();
		virtual void paint (Graphics& g);
	private:
		CUnit* unit;
};

#endif