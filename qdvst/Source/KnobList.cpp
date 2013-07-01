#include <vector>
#include "logger.h"
#include "../JuceLibraryCode/JuceHeader.h"
#include "player/addsynth.h"
#include "KnobList.h"

KnobList::KnobList(int knobAmount, int width, int height)
{
	this->knobAmount = knobAmount;
	this->setSize(width, height);
	this->knobs = std::vector<juce::Slider*>(knobAmount);
	this->labels = std::vector<juce::Label*>(knobAmount);
};

KnobList::~KnobList()
{
	for (int i=0;i<knobAmount;i++) {
		delete knobs[i];
		delete labels[i];
	}
}

void KnobList::addKnob(int index, Slider* knob, juce::String labelText)
{
	int size = 24;

	labels[index] = new juce::Label();
	labels[index]->setText(labelText, juce::NotificationType::dontSendNotification);
	labels[index]->setFont(juce::Font(12));
	
	int knob_w = 24 + 5;
	int knob_h = 40;
	int row_len = this->getWidth() / knob_w;
	int knob_x = (index % row_len)*size;
	int knob_y = (index / row_len) * knob_h;

	knobs[index] = knob;
	knob->setBounds(knob_x, knob_y, size, size);
	labels[index]->setBounds(knob_x, knob_y + size, 75, 16);
	knob->setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);

	// TODO add listener
	addAndMakeVisible(knob);
	addAndMakeVisible(labels[index]);
}

Slider* KnobList::get(int index)
{
	if (index < 0 || index >= knobAmount) {
		logger::error("invalid getKnob index: %d", index);
		return 0;
	}

	return knobs[index];
}


Slider* KnobList::createSlider(float low, float high, float step)
{
	Slider* slider = new juce::Slider();
	slider->setRange(low, high, step);
	return slider;
}