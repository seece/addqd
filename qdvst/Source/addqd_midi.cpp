#include "addqd_midi.h"

char MIDI::convertMidiNotePitch(unsigned char midiNote) {
	return (int)midiNote - 69 -3;
}

bool MIDI::isValidEvent(unsigned char eventMsg) {
	if (eventMsg == MIDI::COMMAND_NOTE_OFF) {
		return true;
	}

	if (eventMsg == MIDI::COMMAND_NOTE_ON) {
		return true;
	}

	if (eventMsg == MIDI::COMMAND_CC) {
		return true;
	}

	return false;
}