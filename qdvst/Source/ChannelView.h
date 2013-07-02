#ifndef CHANNEL_VIEW_H
#define CHANNEL_VIEW_H

#include "../JuceLibraryCode/JuceHeader.h"
#include "player/addsynth.h"
#include "KnobList.h"
#include "LfoView.h"
#include "EnvView.h"

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
	LfoView* lfoView[2];
	EnvView* envView[2];
	
};

#endif
