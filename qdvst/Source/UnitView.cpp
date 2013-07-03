#include "UnitView.h"
#include "player/../unit_types.h"

UnitView::UnitView(Channel* targetChannel, CUnit* targetUnit) : channel(targetChannel), unit(targetUnit), 
	unitKnobs(KnobList(2, 32, 64))
{
	setSize(240, 80);
	//unitKnobs->addKnob(knobIndex::KNOB_VOL, KnobList::createSlider(0.0, 1.0, 0.0, this), "unitVol");
	// TODO handle unit VOL/PAN knob control & creation here
}

UnitView::~UnitView()
{
	for (int i=0;i<unitKnobs.knobs.size();i++) {
		if (unitKnobs.knobs[i] != NULL)	{
			delete unitKnobs.knobs[i];
		}
	}

	deleteAllChildren();
}

void UnitView::fetchUnitKnobValues()
{

}

/*
void UnitView::paint (Graphics& g)
{
	g.fillAll (Colours::darkkhaki);

	if (unit == NULL) {
		g.fillAll (Colours::darkslategrey);
		g.setColour (Colours::white);
		g.setFont (15.0f);
		g.drawFittedText ("empty",
						  0, 0, getWidth(), getHeight(),
						  Justification::topLeft, 1);
	} else {
		addqd::UnitType type = unit->getType();
		g.setColour (Colours::black);
		g.setFont (15.0f);
		String title = "type: ";
		title += type;
		g.drawFittedText (title,
						  0, 0, getWidth(), getHeight(),
						  Justification::topLeft, 1);
	}

    g.setColour (Colours::black);
	g.drawRect(0, 0, this->getWidth(), this->getHeight(), 1);
}
*/