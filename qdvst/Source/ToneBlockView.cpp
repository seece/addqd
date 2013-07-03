
#include "editor_state.h"
#include "ToneBlockView.h"
#include "player/units/toneblock.h"

ToneBlockView::ToneBlockView(Channel* targetChannel, CToneBlock* targetToneBlock) : 
	UnitView(targetChannel, targetToneBlock), channel(targetChannel), toneBlock(targetToneBlock)
{
	this->channel = targetChannel;
	this->toneBlock = targetToneBlock;

	KnobList* knobs = new KnobList(5, 170, 50);
	knobs->setTopLeftPosition(0, 16);
	knobs->addKnob(knobIndex::KNOB_VOL, KnobList::createSlider(0.0, 1.0, 0.0, this), "vol");
	knobs->addKnob(knobIndex::KNOB_OCTAVE, KnobList::createSlider(-4.0, 4.0, 1.0, this), "oct");
	knobs->addKnob(knobIndex::KNOB_FINE, KnobList::createSlider(-1.0, 1.0, 0.0, this), "fine");
	knobs->addKnob(knobIndex::KNOB_A, KnobList::createSlider(0.0, 1.0, 0.0, this), "a");
	knobs->addKnob(knobIndex::KNOB_B, KnobList::createSlider(0.0, 1.0, 0.0, this), "b");

	addAndMakeVisible(knobs);

	knobMap["vol"] = &toneBlock->param_values[CToneBlock::PARAM_VOLUME];
	knobMap["oct"] = &toneBlock->param_values[CToneBlock::PARAM_OCTAVE];
	knobMap["fine"] = &toneBlock->param_values[CToneBlock::PARAM_FINE];
	knobMap["a"] = &toneBlock->param_values[CToneBlock::PARAM_MOD_A];
	knobMap["b"] = &toneBlock->param_values[CToneBlock::PARAM_MOD_B];

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

	if (knobMap.find(slider->getName()) == knobMap.end()) {
		logger::info("knob %s is not in the map", slider->getName().getCharPointer());
		return;
	} else {
		float* target = knobMap[slider->getName()];
		*target = val;
		//logger::info("knob %s mapped to %p", slider->getName().getCharPointer(), knobMap[slider->getName()]);
	}
}