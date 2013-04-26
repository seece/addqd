/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic startup code for a Juce application.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"


//==============================================================================
QdvstAudioProcessor::QdvstAudioProcessor()
{
}

QdvstAudioProcessor::~QdvstAudioProcessor()
{
}

//==============================================================================
const String QdvstAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

int QdvstAudioProcessor::getNumParameters()
{
    return 0;
}

float QdvstAudioProcessor::getParameter (int index)
{
    return 0.0f;
}

void QdvstAudioProcessor::setParameter (int index, float newValue)
{
}

const String QdvstAudioProcessor::getParameterName (int index)
{
    return String::empty;
}

const String QdvstAudioProcessor::getParameterText (int index)
{
    return String::empty;
}

const String QdvstAudioProcessor::getInputChannelName (int channelIndex) const
{
    return String (channelIndex + 1);
}

const String QdvstAudioProcessor::getOutputChannelName (int channelIndex) const
{
    return String (channelIndex + 1);
}

bool QdvstAudioProcessor::isInputChannelStereoPair (int index) const
{
    return true;
}

bool QdvstAudioProcessor::isOutputChannelStereoPair (int index) const
{
    return true;
}

bool QdvstAudioProcessor::acceptsMidi() const
{
#if JucePlugin_WantsMidiInput
    return true;
#else
    return false;
#endif
}

bool QdvstAudioProcessor::producesMidi() const
{
#if JucePlugin_ProducesMidiOutput
    return true;
#else
    return false;
#endif
}

bool QdvstAudioProcessor::silenceInProducesSilenceOut() const {
	return false;
}

double QdvstAudioProcessor::getTailLengthSeconds() const {
	return 0.0;
}

int QdvstAudioProcessor::getNumPrograms()
{
    return 0;
}

int QdvstAudioProcessor::getCurrentProgram()
{
    return 0;
}

void QdvstAudioProcessor::setCurrentProgram (int index)
{
}

const String QdvstAudioProcessor::getProgramName (int index)
{
    return String::empty;
}

void QdvstAudioProcessor::changeProgramName (int index, const String& newName)
{
}

//==============================================================================
void QdvstAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
}

void QdvstAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

void QdvstAudioProcessor::processBlock (AudioSampleBuffer& buffer, MidiBuffer& midiMessages)
{
    // This is the place where you'd normally do the guts of your plugin's
    // audio processing...
    for (int channel = 0; channel < getNumInputChannels(); ++channel)
    {
        float* channelData = buffer.getSampleData (channel);

        // ..do something to the data...
    }

    // In case we have more outputs than inputs, we'll clear any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    for (int i = getNumInputChannels(); i < getNumOutputChannels(); ++i)
    {
        buffer.clear (i, 0, buffer.getNumSamples());
    }
}

//==============================================================================
bool QdvstAudioProcessor::hasEditor() const
{
    return true;
}

AudioProcessorEditor* QdvstAudioProcessor::createEditor()
{
    return new QdvstAudioProcessorEditor (this);
}

//==============================================================================
void QdvstAudioProcessor::getStateInformation (MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void QdvstAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new QdvstAudioProcessor();
}
