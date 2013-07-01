#ifndef LFOVIEW_H
#define LFOVIEW_H

#include "KnobList.h"
#include "../JuceLibraryCode/JuceHeader.h"
#include "player/addsynth.h"

class LfoView  : public Component,
				public ComboBox::Listener
{
	public:
	LfoView(int index, Channel* channel);
	virtual ~LfoView();
	void paint (Graphics& g);
	void comboBoxChanged (ComboBox* box);

	private:
	int index;

	KnobList* knobs;
	Label* nameLabel;
	Channel* channel;
	ComboBox* waveformSelector;
	generators::osc_type waveformType;

	
};

#endif