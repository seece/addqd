#ifndef EDITOR_STATE_H
#define EDITOR_STATE_H

#include "../JuceLibraryCode/JuceHeader.h"

namespace EditorState {
	extern CriticalSection editorLock;
	extern QdvstAudioProcessor* processor;
	extern QdvstAudioProcessorEditor* editor;
}

#endif