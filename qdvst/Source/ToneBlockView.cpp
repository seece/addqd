
#include "ToneBlockView.h"
#include "player/units/toneblock.h"

ToneBlockView::ToneBlockView(Channel* targetChannel, CToneBlock* targetToneBlock) : 
	UnitView(targetChannel, targetToneBlock), channel(targetChannel), toneBlock(targetToneBlock)
{
	this->channel = targetChannel;
	this->toneBlock = targetToneBlock;

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
