#ifndef CHANNEL_VIEW_H
#define CHANNEL_VIEW_H

#include "../JuceLibraryCode/JuceHeader.h"
#include "player/addsynth.h"
#include "KnobList.h"

class ChannelView : public Component
{
	public:
	ChannelView(int index);
	~ChannelView();
	int getIndex();

	void paint (Graphics& g);

	private:
	int index;
	juce::String name;
	juce::Label* unitAmountLabel;
	Channel* targetChannel;
	
	KnobList* knobs;
};

#endif
