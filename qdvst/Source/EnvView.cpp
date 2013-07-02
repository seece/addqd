#include <vector>
#include "EnvView.h"
#include "logger.h"
#include "editor_state.h"

EnvView::EnvView(int index, Channel* channel)
{
	this->index = index;
	this->channel = channel;

	this->setSize(200, 60);

	juce::String name = "ENV ";
	name += index;
	nameLabel = new juce::Label("", name);
	nameLabel->setBounds(0, 0, 80, 20);

	addAndMakeVisible(nameLabel);
	knobs = new KnobList(5, 150, 70);
	knobs->setTopLeftPosition(0, 16);
	knobs->addKnob(knobIndex::KNOB_ATT, KnobList::createSlider(0.0, 1.0, 0.0, this), "att");
	knobs->addKnob(knobIndex::KNOB_DEC, KnobList::createSlider(0.0, 1.0, 0.0, this), "dec");
	knobs->addKnob(knobIndex::KNOB_HOLD, KnobList::createSlider(0.0, 1.0, 0.0, this), "hold");
	knobs->addKnob(knobIndex::KNOB_SUS, KnobList::createSlider(0.0, 1.0, 0.0, this), "sus");
	knobs->addKnob(knobIndex::KNOB_REL, KnobList::createSlider(0.0, 1.0, 0.0, this), "rel");
	addAndMakeVisible(knobs);

	fetchValues();
}

EnvView::~EnvView()
{
	deleteAllChildren();
}

void EnvView::paint (Graphics& g)
{
	g.fillAll (Colours::beige);
    g.setColour (Colours::black);
    g.setFont (15.0f);
	g.drawRect(0, 0, this->getWidth(), this->getHeight(), 1);
}

void EnvView::comboBoxChanged (ComboBox* box)
{
	int newId = box->getSelectedId() - 1;	
}

void EnvView::sliderValueChanged(Slider* slider) 
{

	Envelope* env = this->channel->getEnvelope(index);
	float val = slider->getValue();

	juce::String name = slider->getName();

	const GenericScopedLock<CriticalSection> scopedLock(EditorState::editorLock);
	
	if (name == "att") {
		env->attack = val;
	} else if (name == "dec") {
		env->decay = val;
	} else if (name == "hold") {
		env->hold = val;
	} else if (name == "sus") {
		env->sustain = val;
	} else if (name == "rel") {
		env->release = val;
	}

}

void EnvView::fetchValues()
{
	Envelope* env = &this->channel->env[index];
	knobs->get(knobIndex::KNOB_ATT)->setValue(env->attack);
	knobs->get(knobIndex::KNOB_DEC)->setValue(env->decay);
	knobs->get(knobIndex::KNOB_HOLD)->setValue(env->hold);
	knobs->get(knobIndex::KNOB_SUS)->setValue(env->sustain);
	knobs->get(knobIndex::KNOB_REL)->setValue(env->release);
	//knobs->get(knobIndex::KNOB_ATT) = env->attack;

}