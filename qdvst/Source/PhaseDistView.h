#ifndef PHASEDISTVIEW_H
#define PHASEDISTVIEW_H

#include "../JuceLibraryCode/JuceHeader.h"
#include "logger.h"
#include "player/addsynth.h"
#include "player/units/phasedist.h"
#include "KnobList.h"
#include "UnitView.h"
#include "WaveformSelector.h"

class PhaseDistView:	public UnitView,
				public SliderListener
				//public ComboBox::Listener
{
	public:
		PhaseDistView(Channel* targetChannel, CPhaseDist* targetPhaseDist);
		~PhaseDistView();
		virtual void paint (Graphics& g);
		void sliderValueChanged(Slider* slider);
		//void comboBoxChanged (ComboBox* box);

		enum knobIndex {
			KNOB_OCTAVE = 0,
			KNOB_FINE = 1,
			KNOB_A = 2,
			KNOB_B = 3};

	private:
		Channel* channel;
		CPhaseDist* phaseDist;

		KnobList* knobs;
};
#endif