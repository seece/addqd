#ifndef MODPLAYER_H
#define MODPLAYER_H

#include <stdint.h>
#include "amigamod.h"	// for the Note struct
#include "player/sound.h"
#include "event.h"

#define PATTERN_DEBUG 0
#define SYNTH_EMPTY_NOTE_VALUE 112	// please note that this differs from the one defined in amigamod.h



struct Songinfo {
	uint8_t name[256];
	uint8_t bpm;
	uint8_t pattern_amount;
	uint8_t length;	// song length in patterns
	uint8_t orderlist[256];	// MOD pattern length is inside the range 1-128
	uint8_t channels;
	uint8_t rows;	// rows per pattern, usually 64
};

struct PTInstrument {
	// amigamod fields
	uint8_t name[256];
	uint16_t length;
	int8_t finetune;
	int8_t octave; // octave offset from C-4
	uint8_t volume;
	uint16_t repeat;
	uint16_t repeat_length;	// converted to little-endian, needs to be multiplied by two
	// additional fields
	uint8_t envelope;
	int8_t pan;
};

struct Instrumentinfo {
	uint8_t instrument_amount;
	PTInstrument instruments[256];	// usually only 31 is in use
};

struct PTSong {
	Songinfo song;
	Instrumentinfo ins;
	Note * notedata;
};

namespace mod {

PTSong load_PTSong(const char * input_path);
void player_init();
void player_load_PTSong(PTSong * song);
int player_add_offset(int offset);	// applies a row offset to the player, returns new offset
//void update_player(PTSong * song, int start_time, int end_time);
//void setDefaultInstrumentValues(PTInstrument *ins);
//uint32_t loadInstruments(Sample *sample_info, Instrumentinfo *sinstruments);
void printPattern(Note *synthnotes,Songinfo *ssong, uint32_t pattern);
void play_PTSong(PTSong * song, int time);
void player_update(EventBuffer * buffer, long samplecount);

}
#endif
