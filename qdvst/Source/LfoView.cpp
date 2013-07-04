#include "../JuceLibraryCode/JuceHeader.h"
#include "logger.h"
#include "editor_state.h"
#include "player/addsynth.h"
#include "KnobList.h"
#include "WaveformSelector.h"
#include "LfoView.h"

LfoView::LfoView(int index, Channel* channel) 
{
	this->channel = channel;
	this->index = index;

	LFO* lfo = this->channel->getLFO(index);

	this->setSize(175, 60);
	knobs = new KnobList(2, 150, 70);
	knobs->setTopLeftPosition(0, 16);
	knobs->addKnob(knobIndex::KNOB_FREQ, KnobList::createSlider(0.0, 16.0, 0.0, this), "freq", &lfo->frequency);
	knobs->addKnob(knobIndex::KNOB_GAIN, KnobList::createSlider(0.0, 5.0, 0.0, this), "gain", &lfo->gain);
	addAndMakeVisible(knobs);

	juce::String name = "LFO ";
	name += index;
	nameLabel = new juce::Label("", name);
	nameLabel->setBounds(0, 0, 80, 20);
	addAndMakeVisible(nameLabel);

	waveformSelector = new WaveformSelector("waveformSelector");
	waveformSelector->addListener(this);
	addAndMakeVisible(waveformSelector);

	fetchValues();
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
	const GenericScopedLock<CriticalSection> scopedLock(EditorState::editorLock);

	generators::osc_type waveformType = waveformSelector->handleChange(box);
	channel->getLFO(index)->wavefunc = generators::osc_functions[waveformType];
	logger::info("wavefunc set to %d", waveformType);
}

void LfoView::sliderValueChanged(Slider* slider) 
{
	const GenericScopedLock<CriticalSection> scopedLock(EditorState::editorLock);

	knobs->updateSlider(slider);
}


void LfoView::fetchValues() 
{
	this->knobs->get(knobIndex::KNOB_FREQ)->setValue(channel->getLFO(index)->frequency);	
	this->knobs->get(knobIndex::KNOB_GAIN)->setValue(channel->getLFO(index)->gain);	
}