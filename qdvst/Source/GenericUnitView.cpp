
#include "GenericUnitView.h"

GenericUnitView::GenericUnitView(Channel* targetChannel, CUnit* targetUnit) : UnitView(targetChannel, targetUnit),
	unit(targetUnit)
{
}

GenericUnitView::~GenericUnitView()
{

}

void GenericUnitView::paint (Graphics& g)
{
	g.fillAll (Colours::darkkhaki);

	if (!unit) {
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
		String type = "unit type: ";
		type += unit->getType();
		g.drawFittedText (type,
						  0, 0, getWidth(), getHeight(),
						  Justification::topLeft, 1);
	}

    g.setColour (Colours::black);
	g.drawRect(0, 0, this->getWidth(), this->getHeight(), 1);
}