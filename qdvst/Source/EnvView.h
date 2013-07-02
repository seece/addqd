#ifndef ENVVIEW_H
#define ENVVIEW_H

#include "KnobList.h"
#include "../JuceLibraryCode/JuceHeader.h"
#include "player/addsynth.h"

class EnvView : public Component,
				public SliderListener,
				public ComboBox::Listener
{
	public:
	EnvView(int index, Channel* channel);
	~EnvView();

	void paint (Graphics& g);
	void sliderValueChanged(Slider* slider);
	void comboBoxChanged (ComboBox* box);

	private:
	int index;
	Channel* channel;
	KnobList* knobs;
	Label* nameLabel;

	enum knobIndex {KNOB_ATT = 0, KNOB_DEC = 1, KNOB_HOLD = 2, KNOB_SUS = 3, KNOB_REL = 4};

	void fetchValues();

};

#endif