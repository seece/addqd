#pragma warning( disable : 4100 ) // disable "unreferenced formal parameter" warning

#include <cmath>
#include <cassert>

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "player/addsynth.h"
#include "addqd_midi.h"


//==============================================================================
QdvstAudioProcessor::QdvstAudioProcessor()
{
	blockSize = -1;
	this->settings.channels = 8;
	maxSamplesPerBlock = 44100;	// maximum length of the temp internal buffer

	sampleTime = 0;

	synthEvents.amount = 0;
	synthEvents.max_events = 4096;	
	synthEvents.event_list = new addqd::Event[synthEvents.max_events];
	tempAudioBuffer = NULL;

	syn_init(this->settings.channels);

	Instrument * tri = &insarr[0];

	*tri = syn_init_instrument(INS_FM_TWO_OP);
	tri->type = INS_FM_TWO_OP;
	tri->volume=0.5f;
	tri->octave=-2;
	tri->fmFunc = *generators::resonant_fm;
	tri->env[0].attack = 0.01f;
	tri->env[0].release = 0.08f;

	listpointer = syn_get_instrument_list_pointer();
	*listpointer = (Instrument *)&insarr;
	//syn_set_instrument_list_pointer(insarr);

	syn_attach_instrument(0, 0);

}

QdvstAudioProcessor::~QdvstAudioProcessor()
{
	syn_free();

	if (tempAudioBuffer != NULL) {
		delete tempAudioBuffer;
	}

	delete synthEvents.event_list;
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

	if (tempAudioBuffer != NULL) {
		delete tempAudioBuffer;
	}

	tempAudioBuffer = new float[maxSamplesPerBlock*2];
	

	#ifdef _DEBUG
		//printf("buffersize: %d, rate: %d\n", blockSize, rate);
	#endif

	assert(rate == 44100);	// lazy input value checking
}

void QdvstAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

void QdvstAudioProcessor::convertMidiEvents(MidiBuffer& midiMessages, addqd::EventBuffer& synthEvents) {
	MidiBuffer::Iterator iter(midiMessages);

	int num = midiMessages.getNumEvents();

	if (num > 0) {
		printf("events: %d\n", num);
	}
	
	const uint8 * mididata;
	int mididata_length;
	int sample_pos;

	while (iter.getNextEvent(mididata, mididata_length, sample_pos)) {
		unsigned char chan = (mididata[0]) & 0x0F; 
		unsigned char msg = (mididata[0] >> 4) & 0x0F; 
		unsigned char id = (mididata[1]); 
		unsigned char velocity = (mididata[1]); 

		if (MIDI::isValidEvent(msg)) {
			addqd::Event e;
			e.channel = chan;
			e.when = sampleTime + sample_pos;
			e.velocity = velocity;

			switch (msg) {
				case MIDI::COMMAND_NOTE_ON:
					e.type = ADQ_EVENT_NOTE_ON;
					e.note = MIDI::convertMidiNotePitch(id);
					break;
				case MIDI::COMMAND_NOTE_OFF:
					e.type = ADQ_EVENT_NOTE_OFF;
					e.note = MIDI::convertMidiNotePitch(id);
					break;
				default:
					e.type = ADQ_EVENT_NONE;
					break;
			}

			if (e.type != ADQ_EVENT_NONE) {
				synthEvents.event_list[synthEvents.amount] = e;
				synthEvents.amount++;

				printf("VST: ");
				print_event(e);
			}

			assert(synthEvents.amount < synthEvents.max_events);
		}

		if (msg == MIDI::COMMAND_NOTE_ON || msg == MIDI::COMMAND_NOTE_OFF) {
			int key = MIDI::convertMidiNotePitch(id);
			//printf("key: %d\n", key);
		}
		
		//printf("chn:\t%d %#x: %d, %d, %d\n", chan, msg, mididata_length, sample_pos, id);
	}
}

void QdvstAudioProcessor::processBlock (AudioSampleBuffer& buffer, MidiBuffer& midiMessages)
{
	synthEvents.amount = 0;
	convertMidiEvents(midiMessages, synthEvents);
		
	int length = buffer.getNumSamples();

	if (length > maxSamplesPerBlock) {
		printf("VST: Warning blocksize too big: %d\n", length);
	}

	//renderStart = GetTickCount();
	//if (sampleTime > 44100*2) {
		syn_render_block(tempAudioBuffer, length, &synthEvents);
	//}

	//printf("%ld:\tbuffersize: %d\n", sampleTime, length);
	
	float* leftChannelData = buffer.getSampleData(0);
	float* rightChannelData = buffer.getSampleData(1);

    for (int i=0;i<length;i++) {
		leftChannelData[i] = tempAudioBuffer[i*2];
		rightChannelData[i] = tempAudioBuffer[i*2+1];
	}


	//printf("type 0: %d\n", (*listpointer)[0].type); 
	//channelData[i] = sinf(440.0 * 2 * 3.14159265 * (i + sampleTime) * 1.0/(double)rate) * 0.8f;
    

    // In case we have more outputs than inputs, we'll clear any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    for (int i = getNumInputChannels(); i < getNumOutputChannels(); ++i)
    {
        buffer.clear (i, 0, buffer.getNumSamples());
    }

	sampleTime += blockSize;
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
