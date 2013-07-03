#ifndef UNITVIEW_H
#define UNITVIEW_H

#include "../JuceLibraryCode/JuceHeader.h"
#include "logger.h"

#include "player/addsynth.h"
#include "KnobList.h"

// You shouldn't create instances of this class, use GenericUnitView instead. This
// is just an interface or something.
class UnitView : public Component
{
	public:
		static const int MAX_HEIGHT = 90;
		UnitView(Channel* targetChannel, CUnit* targetUnit);
		virtual ~UnitView();
		//virtual void paint (Graphics& g);

	private:
		Channel* channel;
		CUnit* unit;
		void fetchValues();
};

#endif