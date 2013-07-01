#include "player/addsynth.h"
#include "ChannelView.h"
#include "KnobList.h"
#include "LfoView.h"

ChannelView::ChannelView(int index)
{
	this->index = index;
	setSize (400, 300);

	targetChannel = syn_get_channel(index);

	name = "channel #";
	name += index;

	unitAmountLabel = new juce::Label();
	juce::String unitAmountText = "units loaded: ";
	unitAmountText += targetChannel->unitsLoaded();
	unitAmountLabel->setText(unitAmountText, juce::NotificationType::dontSendNotification);
	unitAmountLabel->setBounds(0, 16, 100, 16);
	addAndMakeVisible(unitAmountLabel);

	lfoView = new LfoView(0, targetChannel);
	lfoView->setTopLeftPosition(0, 50);
	addAndMakeVisible(lfoView);

}

ChannelView::~ChannelView()
{

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