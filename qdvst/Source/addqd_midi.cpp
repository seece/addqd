#include "addqd_midi.h"

int MIDI::convertMidiNotePitch(unsigned char midiNote) {
	return (int)midiNote - 69;
}