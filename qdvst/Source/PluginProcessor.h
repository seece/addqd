/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic startup code for a Juce application.

  ==============================================================================
*/

#ifndef __PLUGINPROCESSOR_H_6F279363__
#define __PLUGINPROCESSOR_H_6F279363__

#include "../JuceLibraryCode/JuceHeader.h"
;
#include "player/addsynth.h"

struct PluginSettings {
	int channels;
};


//==============================================================================
/**
*/
class QdvstAudioProcessor  : public AudioProcessor
{
public:
    //==============================================================================
    QdvstAudioProcessor();
    ~QdvstAudioProcessor();

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock);
    void releaseResources();

    void processBlock (AudioSampleBuffer& buffer, MidiBuffer& midiMessages);

    //==============================================================================
    AudioProcessorEditor* createEditor();
    bool hasEditor() const;

    //==============================================================================
    const String getName() const;

    int getNumParameters();

    float getParameter (int index);
    void setParameter (int index, float newValue);

    const String getParameterName (int index);
    const String getParameterText (int index);

    const String getInputChannelName (int channelIndex) const;
    const String getOutputChannelName (int channelIndex) const;
    bool isInputChannelStereoPair (int index) const;
    bool isOutputChannelStereoPair (int index) const;

    bool acceptsMidi() const;
    bool producesMidi() const;

	bool silenceInProducesSilenceOut() const;
	double getTailLengthSeconds() const;

    //==============================================================================
    int getNumPrograms();
    int getCurrentProgram();
    void setCurrentProgram (int index);
    const String getProgramName (int index);
    void changeProgramName (int index, const String& newName);

    //==============================================================================
    void getStateInformation (MemoryBlock& destData);
    void setStateInformation (const void* data, int sizeInBytes);

private:
    //==============================================================================
   

	int blockSize;	// buffer size
	int rate;		// samplerate, must be 44100
	long sampleTime;	// time in audio frames
	PluginSettings settings;
	Instrument insarr[SYN_MAX_INSTRUMENTS];
	Instrument** listpointer;
	addqd::EventBuffer synthEvents; // temporary synth events

	void convertMidiEvents(MidiBuffer& midiMessages, addqd::EventBuffer& synthEvents);


	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (QdvstAudioProcessor);
};

#endif  // __PLUGINPROCESSOR_H_6F279363__
