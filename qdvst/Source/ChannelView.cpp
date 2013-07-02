#include "player/addsynth.h"
#include "ChannelView.h"
#include "KnobList.h"
#include "LfoView.h"
#include "EnvView.h"

ChannelView::ChannelView(int index)
{
	this->index = index;
	setSize (400, 420);

	targetChannel = syn_get_channel(index);

	name = "channel #";
	name += index;

	unitAmountLabel = new juce::Label();
	juce::String unitAmountText = "units loaded: ";
	unitAmountText += targetChannel->unitsLoaded();
	unitAmountLabel->setText(unitAmountText, juce::NotificationType::dontSendNotification);
	unitAmountLabel->setBounds(0, 16, 100, 16);
	addAndMakeVisible(unitAmountLabel);

	lfoView[0] = new LfoView(0, targetChannel);
	lfoView[0]->setTopLeftPosition(0, 50);
	addAndMakeVisible(lfoView[0]);
	lfoView[1] = new LfoView(1, targetChannel);
	lfoView[1]->setTopLeftPosition(0, 120);
	addAndMakeVisible(lfoView[1]);

	envView[0] = new EnvView(0, targetChannel);
	envView[0]->setTopLeftPosition(0, 190);
	addAndMakeVisible(envView[0]);

	envView[1] = new EnvView(1, targetChannel);
	envView[1]->setTopLeftPosition(0, 260);
	addAndMakeVisible(envView[1]);

}

ChannelView::~ChannelView()
{
	deleteAllChildren();
}

int ChannelView::getIndex()
{
	return this->index;
}

void ChannelView::paint (Graphics& g)
{
	g.fillAll (Colours::red);
    g.setColour (Colours::black);
    g.setFont (15.0f);
    g.drawFittedText (name,
                      0, 0, getWidth(), getHeight(),
                      Justification::topLeft, 1);
}