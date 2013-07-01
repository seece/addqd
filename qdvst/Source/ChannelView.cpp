#include "ChannelView.h"

ChannelView::ChannelView(int index)
{
	this->index = index;
	 setSize (400, 300);

	name = "channel #";
	name += index;
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