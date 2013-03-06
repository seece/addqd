#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "amigamod.h"
#include "player.h"
#include "event.h"
#include "addsynth.h"
#include "util.h"

static void setDefaultInstrumentValues(PTInstrument *ins) {
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


static uint32_t loadInstruments(Sample *sample_info, Instrumentinfo *sinstruments) {
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

PTSong load_PTSong(const char * input_path) {
	bool flag_verbose = true;

	PTSong song;
	Songinfo ssong;
	Instrumentinfo sinstruments;

		int sz, i, u, r, offset;

	// Load the MOD to memory

	FILE *fp;
	fp = fopen(input_path, "rb");

	if (fp == NULL) {
			printf("File %s not found!", input_path);
			//fclose(fp);
			goto error;
	}

	sz = getFilesize(fp);

	uint8_t * moduledata = new uint8_t[sz];	// the whole module
	fread(moduledata, sizeof(char), sz, fp);
	fclose(fp);

	uint8_t modulename[20];	// well, uh
	uint8_t initials[5];		// the magic code
	int song_data_offset;
	uint8_t song_length;		// song length in patterns (the length of the order list)
	int pattern_amount;
	int max_orderlist_entries = 128;
	int sample_amount = MAX_SAMPLES;

	// init the instrument variables
	for (i=0;i<31;i++) {
		setDefaultInstrumentValues(&(sinstruments.instruments[i]));
	}

	loadModuleName(moduledata, ssong.name);
	dprints("%s", ssong.name);

	int channels = getChannelAmount(moduledata);

	// load the sample information (not actual PCM audio)
	//sinstruments.instrument_amount = sample_amount; // TODO add check to see how many instruments are actually used
	sinstruments.instrument_amount = channels;	// HARDCODED VALUE
	Sample * sample_info = new Sample[sample_amount];
	loadSampleInfo(moduledata, sample_info, sample_amount);
	loadInstruments(sample_info, &sinstruments);

	if (flag_verbose) {
		printSamples(sample_info, sample_amount);
	}


	song_data_offset = 20 + sizeof(Sample) * sample_amount;

	memcpy(&song_length, &moduledata[song_data_offset + 0], 1);

	dprint(song_length);

	uint8_t * song_order = new uint8_t[max_orderlist_entries];
	loadOrderData(moduledata, song_order, song_data_offset, max_orderlist_entries);
	pattern_amount = getHighestOrder(moduledata, song_order, max_orderlist_entries);
	pattern_amount++;	// atomim's rule (515-3)

	ssong.rows = 64;
	ssong.channels = channels;
	ssong.length = song_length;
	ssong.pattern_amount = pattern_amount;
	ssong.bpm = 120;	// read this from the module somehow?
	memcpy(ssong.orderlist, song_order, max_orderlist_entries);

	if (flag_verbose) {
		printOrderlist(song_order, song_length);
	}

	int pattern_data_offset = song_data_offset + 2 + 128 + 4;	// this should be 1084d for a MOD

	//int note_data;
	uint8_t * note_data = new uint8_t[channels];	// each note is 32-bits long

	// allocate memory for the notes to be exported
	Note * synthnotes = new Note[channels*MOD_ROWS*pattern_amount];
	// initialize the Note array
	uint32_t note_array_length = sizeof(Note) * channels*MOD_ROWS*pattern_amount;
	memset(synthnotes, 9, note_array_length);

	printf("Patterns:\n");

	// show the content of this pattern
	int selected_pattern = 1;

	// load pattern data
	for (i=0;i<pattern_amount;i++) {
		offset = pattern_data_offset+ i*MOD_CHANNEL_SIZE*channels;
		for (r=0;r<64;r++) {	// loop through rows

			for (u=0;u<channels;u++) {
				memcpy(note_data, &moduledata[offset + u*MOD_NOTE_SIZE + r*MOD_NOTE_SIZE*channels], 4);

				Note current_note;
				parseNote(note_data, &current_note);

				uint32_t note_array_offset = (i*MOD_ROWS*channels) + r*channels + u;

				synthnotes[note_array_offset] = current_note;

				if (PATTERN_DEBUG) {
					if (i<=selected_pattern) {
						uint8_t nname[4];
						getNoteName(current_note.pitch, nname);
						printf("%s", nname);
						int blaablaa = 0;

						printf("  ");
					}
				}

			}

			if (i<=selected_pattern && PATTERN_DEBUG) {
				printf("\n");
			}
		}

		if (PATTERN_DEBUG) {
			printf("\n");
		}
	}

	delete note_data;
	//delete synthnotes;
	delete song_order;
	delete sample_info;
	delete moduledata;

	song.ins = sinstruments;
	song.song = ssong;
	song.notedata = synthnotes;

	goto finish;

	error:
	printf("Error occured, can't load module!\n");
	goto finish;


	finish:

	return song;
}

static int lastrow = -1;
static int lastnote[8] = {0,0,0,0,0,0,0,0};	// TODO make this use some proper constant


// pushes new commands to the command buffer
// only picks commands between start_time and end_time
void player_update(EventBuffer * buffer, long samplecount) {

}

void play_PTSong(PTSong * song, int time) {
	int ticklength = 125;
	int current_row = time/ticklength;
	int current_position = (current_row/64) % song->song.length;

	if (current_row == lastrow) {
		return;
	}

	int current_pattern = song->song.orderlist[current_position];
	int channels = song->song.channels;
	int pattern_row = current_row % 64;

	printf("ord: %d\tpat: %d\t row:%d\n", current_position, current_pattern, pattern_row);

	for (int c=0;c<channels;c++) {
		int note_array_offset = (current_pattern*MOD_ROWS*channels) + pattern_row*channels + c;
		Note note = song->notedata[note_array_offset];

		//if (c == 0) {
			//printf("NOTE: %d, %d, %d\n", note.pitch, note_array_offset, note.parameters);
		switch (note.command) {
			case 0xC:
				if (note.parameters == 0) {
					syn_end_all_notes(c);
				}
			default:
				break;
		}

		if (note.pitch == EMPTY_NOTE_VALUE) {
			continue;
		}

		printf("c: %d\n", c);
		syn_end_all_notes(c);
		syn_play_note(c, note.pitch);
		lastnote[c] = note.pitch;
		//}
	}

	lastrow = current_row;
}