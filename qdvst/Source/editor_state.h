#ifndef EDITOR_STATE_H
#define EDITOR_STATE_H

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "../JuceLibraryCode/JuceHeader.h"

namespace EditorState {
	extern CriticalSection editorLock;
	extern QdvstAudioProcessor* processor;
	extern QdvstAudioProcessorEditor* editor;
}

#endif