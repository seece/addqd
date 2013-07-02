#include "../JuceLibraryCode/JuceHeader.h"
#include "logger.h"
#include "editor_state.h"
#include "player/addsynth.h"
#include "KnobList.h"
#include "LfoView.h"

LfoView::LfoView(int index, Channel* channel) 
{
	this->channel = channel;
	this->index = index;

	this->setSize(200, 60);
	knobs = new KnobList(2, 150, 70);
	knobs->setTopLeftPosition(0, 16);
	knobs->addKnob(0, KnobList::createSlider(0.0, 1.0, 0.0, this), "freq");
	knobs->addKnob(1, KnobList::createSlider(0.0, 1.0, 0.0, this), "gain");
	addAndMakeVisible(knobs);

	juce::String name = "LFO ";
	name += index;
	nameLabel = new juce::Label("", name);
	nameLabel->setBounds(0, 0, 80, 20);
	addAndMakeVisible(nameLabel);

	waveformSelector = new juce::ComboBox("waveformSelector");
	waveformSelector->setBounds(100, 2, 95, 16);
	waveformSelector->addItem("Sine", generators::osc_type::OSC_SINE + 1);
	waveformSelector->addItem("Saw", generators::osc_type::OSC_SAW + 1);
	waveformSelector->addItem("Square", generators::osc_type::OSC_SQUARE + 1);
	waveformSelector->setSelectedId(generators::osc_type::OSC_SINE + 1);

	addAndMakeVisible(waveformSelector);
}

LfoView::~LfoView()
{
	deleteAllChildren();
}

void LfoView::paint (Graphics& g)
{
	g.fillAll (Colours::grey);
    g.setColour (Colours::black);
    g.setFont (15.0f);
	g.drawRect(0, 0, this->getWidth(), this->getHeight(), 1);
}

void LfoView::comboBoxChanged (ComboBox* box)
{
	int newId = box->getSelectedId() - 1;	
	this->waveformType = static_cast<generators::osc_type>(newId);
	// TODO add oscillator array to oscillators:: namespace with enum indexes
	//this->channel->getLFO(index)->wavefunc = generators::
}

void LfoView::sliderValueChanged(Slider* slider) 
{

	LFO* lfo = this->channel->getLFO(index);
	float val = slider->getValue();

	const GenericScopedLock<CriticalSection> scopedLock(EditorState::editorLock);

	if (slider->getName() == "freq") {
		logger::info("setting freq to %f", val);
		lfo->frequency = val;
	} else if (slider->getName() == "gain") {
		logger::info("setting gain to %f", val);
		lfo->gain = val;
	}


	//int unitsLoaded = processor->getChannel(0)->
	//printf("units loaded on chn 0: %d\n", unitsLoaded);
	
	//editorLock.
}
