#ifndef WAVEFORMSELECTOR_H
#define WAVEFORMSELECTOR_H

#include "../JuceLibraryCode/JuceHeader.h"
#include "player/addsynth.h"

class WaveformSelector : public ComboBox
{
	public:
	WaveformSelector(juce::String componentName = juce::String::empty);
	generators::osc_type handleChange(ComboBox* box);

	private:
};

#endif