#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "amigamod.h"
#include "player.h"

void setDefaultInstrumentValues(PTInstrument *ins) {
	strcpy((char *) ins->name, "instrument");
	ins->envelope = 0;
	ins->finetune = 0;
	ins->length = 0;
	ins->octave = 0;
	ins->pan = 0;
	ins->repeat = 0;
	ins->repeat_length = 0;
	ins->volume = 64;	// 040
}


uint32_t loadInstruments(Sample *sample_info, Instrumentinfo *sinstruments) {
	uint32_t i;

	for (i=0;i<sinstruments->instrument_amount;i++) {
		strcpy((char*) sinstruments->instruments[i].name, (char*) sample_info[i].name);
		sinstruments->instruments[i].volume = sample_info[i].volume;
		sinstruments->instruments[i].finetune = sample_info[i].finetune;
		sinstruments->instruments[i].length = sample_info[i].length;
		sinstruments->instruments[i].repeat = sample_info[i].repeat;
		sinstruments->instruments[i].repeat_length = sample_info[i].repeat_length;
	}

	return 0;
}

void printPattern(Note *synthnotes, Songinfo *ssong, uint32_t pattern) {
	uint32_t p = pattern;
	uint32_t channels = ssong->channels;
	uint32_t r, c;

	for (r=0;r<MOD_ROWS;r++) {	// loop through rows
		for (c=0;c<channels;c++) {
			Note current_note = synthnotes[(p*MOD_ROWS*channels) + r*channels + c];
			uint8_t nname[4];
			getNoteName(current_note.pitch, nname);

			printf("%s", nname);

			printf("  ");
		}

		printf("\n");

	}

	printf("\n");

}
