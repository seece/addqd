#ifndef TONEBLOCKVIEW_H
#define TONEBLOCKVIEW_H

#include <map>

#include "../JuceLibraryCode/JuceHeader.h"
#include "logger.h"
#include "player/addsynth.h"
#include "player/units/toneblock.h"
#include "KnobList.h"
#include "UnitView.h"

class ToneBlockView :	public UnitView,
				public SliderListener
{
	public:
		ToneBlockView(Channel* targetChannel, CToneBlock* targetToneBlock);
		~ToneBlockView();
		virtual void paint (Graphics& g);
		void sliderValueChanged(Slider* slider);
	private:
		enum knobIndex {
			KNOB_VOL = 0, 
			KNOB_OCTAVE = 1,
			KNOB_FINE = 2,
			KNOB_A = 3,
			KNOB_B = 4};

		Channel* channel;
		CToneBlock* toneBlock;
		KnobList* knobs;
		void fetchValues();
		std::map<juce::String, float*> knobMap;
};

#endif