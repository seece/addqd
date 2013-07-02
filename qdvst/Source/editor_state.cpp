#include "../JuceLibraryCode/JuceHeader.h"
#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "editor_state.h"


CriticalSection EditorState::editorLock;
QdvstAudioProcessor* EditorState::processor = NULL;
QdvstAudioProcessorEditor* EditorState::editor = NULL;


