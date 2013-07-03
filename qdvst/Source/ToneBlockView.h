#ifndef TONEBLOCKVIEW_H
#define TONEBLOCKVIEW_H

#include "../JuceLibraryCode/JuceHeader.h"
#include "logger.h"
#include "player/addsynth.h"
#include "player/units/toneblock.h"
#include "KnobList.h"
#include "UnitView.h"

class ToneBlockView :	public UnitView
{
	public:
		ToneBlockView(Channel* targetChannel, CToneBlock* targetToneBlock);
		~ToneBlockView();
		virtual void paint (Graphics& g);
	private:
		Channel* channel;
		CToneBlock* toneBlock;
		void fetchValues();
};

#endif