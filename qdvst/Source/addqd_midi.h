#ifndef ADDQD_MIDI_H
#define ADDQD_MIDI_H

namespace MIDI {
	const int COMMAND_NOTE_UNKNOWN = 0x0;
	const int COMMAND_NOTE_ON = 0x9;
	const int COMMAND_NOTE_OFF = 0x8;
	const int COMMAND_NOTE_CC = 0xB;

	int convertMidiNotePitch(unsigned char midiNote);
}

#endif