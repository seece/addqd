#ifndef UNITVIEW_H
#define UNITVIEW_H

#include "../JuceLibraryCode/JuceHeader.h"
#include "logger.h"

#include "player/addsynth.h"
#include "KnobList.h"

class UnitView : public Component
{
	public:
		static const int MAX_HEIGHT = 90;
		UnitView(Channel* targetChannel, CUnit* targetUnit);
		~UnitView();
		void paint (Graphics& g);

	private:
		Channel* channel;
		CUnit* unit;
};

#endif