#pragma warning( disable : 4100 ) // disable "unreferenced formal parameter" warning

#include <cmath>
#include <cassert>

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "player/addsynth.h"



//==============================================================================
QdvstAudioProcessor::QdvstAudioProcessor()
{
	blockSize = -1;
	this->settings.channels = 8;
	syn_init(this->settings.channels);

	Instrument * tri = &insarr[0];

	*tri = syn_create_instrument(INS_FM_TWO_OP);
	tri->volume=0.5f;
	tri->octave=-2;
	tri->fmFunc = *generators::resonant_fm;
	tri->env.attack = 0.01f;
	tri->env.release = 0.08f;

	listpointer = syn_get_instrument_list_pointer();
	*listpointer = (Instrument *)&insarr;

}

QdvstAudioProcessor::~QdvstAudioProcessor()
{
	syn_free();
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
    this->blockSize = samplesPerBlock;
	this->rate = (int)sampleRate;

	assert(rate == 44100);	// lazy input value checking
}

void QdvstAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

void QdvstAudioProcessor::processBlock (AudioSampleBuffer& buffer, MidiBuffer& midiMessages)
{

	//int num = midiMessages.getNumEvents();
	//printf("events: %d\n", num);
	MidiBuffer::Iterator * iter = new MidiBuffer::Iterator(midiMessages);

	const uint8 * mididata;
	int mididata_length;
	int sample_pos;

	for (int i=0;iter->getNextEvent(mididata, mididata_length, sample_pos);) {
		printf("%d: %d, %d\n", mididata_length, sample_pos);
	}
	

    for (int channel = 0; channel < getNumInputChannels(); ++channel)
    {
        float* channelData = buffer.getSampleData (channel);

		for (int i=0;i<blockSize;i++) {
			channelData[i] = sinf(440.0 * 2 * 3.14159265 * i * 1.0/(double)rate) * 0.8f;
		}
    }

	delete iter;

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
