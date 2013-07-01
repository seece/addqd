#ifndef KNOBLIST_H
#define KNOBLIST_H

#include <vector>
#include "../JuceLibraryCode/JuceHeader.h"
#include "player/addsynth.h"

class KnobList : public Component
{
	public:
	KnobList(int knobAmount, int width, int height);
	virtual ~KnobList();
	void addKnob(int index, Slider* knob);

	private:
	int knobAmount;
	std::vector<juce::Slider*> knobs;
};

#endif