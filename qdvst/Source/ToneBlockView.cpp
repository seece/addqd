
#include "editor_state.h"
#include "ToneBlockView.h"
#include "player/units/toneblock.h"

ToneBlockView::ToneBlockView(Channel* targetChannel, CToneBlock* targetToneBlock) : 
	UnitView(targetChannel, targetToneBlock), channel(targetChannel), toneBlock(targetToneBlock)
{
	this->channel = targetChannel;
	this->toneBlock = targetToneBlock;

	float* values = toneBlock->param_values;

	knobs = new KnobList(5, 170, 50);
	knobs->setTopLeftPosition(0, 16);
	knobs->addKnob(knobIndex::KNOB_VOL,		KnobList::createSlider(0.0, 1.0, 0.0, this), "vol",		&values[CToneBlock::PARAM_VOLUME]);
	knobs->addKnob(knobIndex::KNOB_OCTAVE,	KnobList::createSlider(-4.0, 4.0, 1.0, this), "oct",	&values[CToneBlock::PARAM_OCTAVE]);
	knobs->addKnob(knobIndex::KNOB_FINE,	KnobList::createSlider(-1.0, 1.0, 0.0, this), "fine",	&values[CToneBlock::PARAM_FINE]);
	knobs->addKnob(knobIndex::KNOB_A,		KnobList::createSlider(0.0, 1.0, 0.0, this), "a",		&values[CToneBlock::PARAM_MOD_A]);
	knobs->addKnob(knobIndex::KNOB_B,		KnobList::createSlider(0.0, 1.0, 0.0, this), "b",		&values[CToneBlock::PARAM_MOD_B]);

	addAndMakeVisible(knobs);

	waveformSelector = new WaveformSelector("waveformSelector");
	waveformSelector->addListener(this);
	waveformSelector->setTopLeftPosition(125, 5);
	addAndMakeVisible(waveformSelector);

	fetchValues();
}

ToneBlockView::~ToneBlockView()
{
	deleteAllChildren();
}

void ToneBlockView::fetchValues()
{

}

void ToneBlockView::paint (Graphics& g)
{
	g.fillAll (Colours::darkkhaki);

	if (toneBlock == NULL) {
		g.fillAll (Colours::darkslategrey);
		g.setColour (Colours::white);
		g.setFont (15.0f);
		g.drawFittedText ("empty",
						  0, 0, getWidth(), getHeight(),
						  Justification::topLeft, 1);
	} else {
		g.fillAll (Colours::yellow);
		g.setColour (Colours::black);
		g.setFont (15.0f);
		g.drawFittedText ("a toneblock!",
						  0, 0, getWidth(), getHeight(),
						  Justification::topLeft, 1);
	}

    g.setColour (Colours::black);
	g.drawRect(0, 0, this->getWidth(), this->getHeight(), 1);
}

void ToneBlockView::sliderValueChanged(Slider* slider) 
{
	float val = slider->getValue();

	const GenericScopedLock<CriticalSection> scopedLock(EditorState::editorLock);
	
	if (knobs->updateSlider(slider)) {
		return;
	}

	// this slider was not found from the knobs value mapping, so one should perform some
	// custom value updating here
}

void ToneBlockView::comboBoxChanged (ComboBox* box)
{
	const GenericScopedLock<CriticalSection> scopedLock(EditorState::editorLock);

	toneBlock->selected_osc = waveformSelector->handleChange(box);
}
