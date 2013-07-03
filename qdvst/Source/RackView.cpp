#include "RackView.h"
#include "GenericUnitView.h"
#include "ToneBlockView.h"

RackView::RackView(Channel* targetChannel) 
{
	this->channel = targetChannel;

	this->units = std::vector<UnitView*>(Channel::MAX_UNITS);

	for (int i=0;i<Channel::MAX_UNITS;i++) {
		units[i] = NULL;
	}

	fetchValues();

	setSize(240, UnitView::MAX_HEIGHT * Channel::MAX_UNITS + 5);
}

RackView::~RackView()
{
	clearUnits();
}

void RackView::paint (Graphics& g)
{
	g.fillAll (Colours::darkgrey);
    g.setColour (Colours::black);
	g.drawRect(0, 0, this->getWidth(), this->getHeight(), 1);
}

void RackView::fetchValues()
{
	int height = UnitView::MAX_HEIGHT;
	int margin = 0;

	for (int i=0;i<Channel::MAX_UNITS;i++) {
		CUnit* unit = channel->getUnit(i);

		if (!unit)
			continue;

		UnitView* view; 

		switch (unit->getType()) {
			case addqd::UnitType::UNIT_TONEBLOCK:
				view = new ToneBlockView(channel, dynamic_cast<CToneBlock*> (unit));
				break;
			// NONE and GENERIC
			default:
				view = new GenericUnitView(channel, unit);
				break;
		}

		this->units[i] = view;
		view->setTopLeftPosition(margin, height*i);
		addAndMakeVisible(view);
	}
}

void RackView::clearUnits()
{
	for (int i=0;i<Channel::MAX_UNITS;i++) {
		if (units[i] == NULL) 
			continue;

		delete units[i];
	}
}
