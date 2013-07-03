#include <vector>
#include "EnvView.h"
#include "logger.h"
#include "editor_state.h"

EnvView::EnvView(int index, Channel* channel)
{
	this->index = index;
	this->channel = channel;

	this->setSize(175, 60);

	Envelope* env = this->channel->getEnvelope(index);

	juce::String name = "ENV ";
	name += index;
	nameLabel = new juce::Label("", name);
	nameLabel->setBounds(0, 0, 80, 20);

	addAndMakeVisible(nameLabel);
	knobs = new KnobList(5, 150, 70);
	knobs->setTopLeftPosition(0, 16);
	knobs->addKnob(knobIndex::KNOB_ATT, KnobList::createSlider(0.0, 4.0, 0.0, this), "att", &env->attack);
	knobs->addKnob(knobIndex::KNOB_DEC, KnobList::createSlider(0.0, 3.0, 0.0, this), "dec", &env->decay);
	knobs->addKnob(knobIndex::KNOB_HOLD, KnobList::createSlider(0.0, 5.0, 0.0, this),"hold",&env->hold);
	knobs->addKnob(knobIndex::KNOB_SUS, KnobList::createSlider(0.0, 1.0, 0.0, this), "sus", &env->sustain);
	knobs->addKnob(knobIndex::KNOB_REL, KnobList::createSlider(0.0, 4.0, 0.0, this), "rel", &env->release);
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
	const GenericScopedLock<CriticalSection> scopedLock(EditorState::editorLock);

	knobs->updateSlider(slider);
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