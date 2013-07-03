#ifndef RACKVIEW_H
#define RACKVIEW_H

#include <vector>
#include "../JuceLibraryCode/JuceHeader.h"
#include "logger.h"

#include "player/addsynth.h"
#include "KnobList.h"
#include "UnitView.h"

class RackView : public Component 
{
	public:
		RackView(Channel* targetChannel);
		~RackView();
		void paint (Graphics& g);

	private:
		Channel* channel;
		std::vector<UnitView* > units;

		void fetchValues();
		void clearUnits();
};
#endif