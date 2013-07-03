
#include "../JuceLibraryCode/JuceHeader.h"
#include "WaveformSelector.h"
#include "logger.h"

WaveformSelector::WaveformSelector(juce::String componentName) : ComboBox(componentName)
{

	setBounds(75, 2, 95, 16);
	addItem("Sine", generators::osc_type::OSC_SINE + 1);
	addItem("Saw", generators::osc_type::OSC_SAW + 1);
	addItem("Square", generators::osc_type::OSC_SQUARE + 1);
	setSelectedId(generators::osc_type::OSC_SINE + 1);
}


// returns the selected waveform enum
generators::osc_type WaveformSelector::handleChange(ComboBox* box)
{
	if (box != this) {
		logger::error("trying to update different combobox in waveformselector!");
		return generators::osc_type::OSC_SINE; // TODO add an OSC_NONE type!
	}

	int newId = box->getSelectedId() - 1;	
	generators::osc_type waveformType = static_cast<generators::osc_type>(newId);

	#ifdef _DEBUG
	if (waveformType < 0 || waveformType >= generators::NUM_OSCILLATOR_TYPES) {
		logger::error("invalid lfo waveformtype %d", waveformType);
	}
	#endif

	return waveformType;
}