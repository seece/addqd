
#include <cstdio>
#include <Windows.h>
#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "editor_state.h"
#include "logger.h"

void initConsole(void) {
    AllocConsole();
    freopen( "CON", "w", stdout );
    freopen( "CON", "w", stderr );
}

//==============================================================================
QdvstAudioProcessorEditor::QdvstAudioProcessorEditor (QdvstAudioProcessor* ownerFilter)
    : AudioProcessorEditor (ownerFilter)
{
	
	#ifdef DEBUG
	initConsole();
	logger::info("starting GUI\n");
	#endif

	channelAmount = ownerFilter->getNumChannels();
	
	logger::info("creating %d channel views", channelAmount);
	for (int i=0;i<channelAmount;i++) {
		this->channels.push_back(new ChannelView(i));
		channels[i]->setVisible(false);
		channels[i]->setTopLeftPosition(50, 50);
		this->addChildComponent(channels[i]);
	}

    setSize (640, 480);
	testknob = new Slider ("scale");
	testknob->setBounds(10, 100, 150, 32);
	testknob->setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
	testknob->addListener(this);
	//addAndMakeVisible(testknob);

	channelLabel = new Label("channelLabel", "channels");
	channelLabel->setBounds(10, 0, 150, 80);
	
	EditorState::editor = this;

	juce::String channelText("channels: ");
	channelText += EditorState::processor->getNumChannels();
	channelLabel->setText(channelText, dontSendNotification);

	addAndMakeVisible(channelLabel);

	channelSelector = new juce::ComboBox("channelSelector");
	channelSelector->setBounds(150, 2, 150, 16);

	for (int i=0;i<channelAmount;i++) {
		juce::String text = "channel ";
		text += (i);

		channelSelector->addItem(text, i + 1);
	}

	// Load the current channel number to the combobox
	channelSelector->setSelectedId(EditorState::selectedChannelId + 1);
	channelSelector->addListener(this);
	addAndMakeVisible(channelSelector);

	logger::info("GUI system nominal.");
}

QdvstAudioProcessorEditor::~QdvstAudioProcessorEditor()
{
	deleteAllChildren();

	/*
	int channelNum = EditorState::processor->getNumChannels();
	
	for (int i=0;i<channelNum;i++) {
		
	}
	*/

	#ifdef DEBUG
	FreeConsole();
	#endif
}

//==============================================================================
void QdvstAudioProcessorEditor::paint (Graphics& g)
{
    g.fillAll (Colours::white);
    g.setColour (Colours::black);
    g.setFont (15.0f);
    g.drawFittedText ("PEISIK ADDQD VST",
                      0, 0, getWidth(), getHeight(),
                      Justification::topLeft, 1);


}

void QdvstAudioProcessorEditor::sliderValueChanged(Slider* slider) 
{
	#ifdef DEBUG
	printf("slider value changse! %lf\n", slider->getValue());
	#endif

	//const GenericScopedLock<CriticalSection> scopedLock(EditorState::editorLock);

	if (!EditorState::processor) {
		return;
	}

	QdvstAudioProcessor* processor = EditorState::processor;
	//int unitsLoaded = processor->getChannel(0)->
	//printf("units loaded on chn 0: %d\n", unitsLoaded);
	
	//editorLock.
}

void QdvstAudioProcessorEditor::comboBoxChanged (ComboBox* box)
{
	int newId = box->getSelectedId() - 1;	// channel numbers begin from 0

	if (newId >= this->channelAmount) {
		logger::error("trying to change to an invalid channel: %d", newId);
		return;
	}

	this->channels[EditorState::selectedChannelId]->setVisible(false);
	EditorState::selectedChannelId = newId;
	this->channels[newId]->setVisible(true);
	logger::info("changed to channel %d", newId);
}