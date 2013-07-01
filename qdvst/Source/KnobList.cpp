#include <vector>
#include "../JuceLibraryCode/JuceHeader.h"
#include "player/addsynth.h"
#include "KnobList.h"

KnobList::KnobList(int knobAmount, int width, int height)
{
	this->knobAmount = knobAmount;
	this->setSize(width, height);
	this->knobs = std::vector<juce::Slider*>(knobAmount);
};

KnobList::~KnobList()
{
	
}

void KnobList::addKnob(int index, Slider* knob)
{
	int size = 28;

	knobs[index] = knob;
	knob->setBounds(index*size, 0, size, size);
	knob->setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
	// TODO add listener
	addAndMakeVisible(knob);
}