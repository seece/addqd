#include "UnitView.h"
#include "player/../unit_types.h"

UnitView::UnitView(Channel* targetChannel, CUnit* targetUnit)
{
	this->channel = targetChannel;
	this->unit = targetUnit;

	setSize(240, 80);
}

UnitView::~UnitView()
{
	deleteAllChildren();
}

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