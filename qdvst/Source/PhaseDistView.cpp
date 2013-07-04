#include "PhaseDistView.h"
#include "editor_state.h"

PhaseDistView::PhaseDistView(Channel* targetChannel, CPhaseDist* targetPhaseDist)
: UnitView(targetChannel, targetPhaseDist), channel(targetChannel), phaseDist(targetPhaseDist)
{

	float * values = phaseDist->param_values;

	knobs = new KnobList(5, 170, 50);
	knobs->setTopLeftPosition(0, 16);
	knobs->addKnob(knobIndex::KNOB_OCTAVE,	KnobList::createSlider(-4.0, 4.0, 1.0, this), "oct",	&values[CPhaseDist::PARAM_OCTAVE]);
	knobs->addKnob(knobIndex::KNOB_FINE,	KnobList::createSlider(-1.0, 1.0, 0.0, this), "fine",	&values[CPhaseDist::PARAM_FINE]);
	knobs->addKnob(knobIndex::KNOB_A,		KnobList::createSlider(0.0, 1.0, 0.0, this), "a",		&values[CPhaseDist::PARAM_MOD_A]);
	knobs->addKnob(knobIndex::KNOB_B,		KnobList::createSlider(0.0, 1.0, 0.0, this), "b",		&values[CPhaseDist::PARAM_MOD_B]);

	addAndMakeVisible(knobs);

}

PhaseDistView::~PhaseDistView() {
	deleteAllChildren();
};

void PhaseDistView::sliderValueChanged(Slider* slider) 
{
	return;

	float val = slider->getValue();

	const GenericScopedLock<CriticalSection> scopedLock(EditorState::editorLock);
	
	if (knobs->updateSlider(slider)) {
		return;
	}

}

void PhaseDistView::paint (Graphics& g)
{
	g.fillAll (Colours::gold);

	g.setColour (Colours::black);
	g.setFont (15.0f);
	g.drawFittedText ("a phasedist!",
					  0, 0, getWidth(), getHeight(),
					  Justification::topLeft, 1);

    g.setColour (Colours::black);
	g.drawRect(0, 0, this->getWidth(), this->getHeight(), 1);
}