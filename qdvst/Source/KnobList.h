#ifndef KNOBLIST_H
#define KNOBLIST_H

#include <map>
#include <vector>
#include "../JuceLibraryCode/JuceHeader.h"
#include "player/addsynth.h"

class KnobList : public Component
{
	public:
	KnobList(int knobAmount, int width, int height);
	virtual ~KnobList();
	void addKnob(int index, Slider* knob, String label, float* targetValue = nullptr);
	Slider* get(int index);
	bool updateSlider(Slider* slider);

	static Slider* createSlider(float low, float high, float step, SliderListener* listener);

	std::vector<juce::Slider*> knobs;

	private:
	int knobAmount;

	std::vector<juce::Label*> labels;
	std::map<juce::String, float*> knobMap;
};

#endif