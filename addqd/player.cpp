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

static PTSong * loaded_song;

static int last_row = -1;
static int lastnote[8] = {0,0,0,0,0,0,0,0};	// TODO make this use some proper constant
static int row_offset = 0;

static long player_millis = 0;
static long player_samples = 0;

void player_init() {
	player_millis = 0;
	player_samples = 0;
	row_offset = 0;
	loaded_song = NULL;
}

void player_load_PTSong(PTSong * song) {
	loaded_song = song;
}

void play_PTSong(PTSong * song, int time) {}

int player_add_offset(int offset) {
	row_offset += offset;
	return row_offset;
}

// time between rows in MILLISECS
#define TEMPO 40

static int push_event(EventBuffer * buffer, Event e) {
	if (buffer->amount >= buffer->max_events) {
		fprintf(stderr, "Note event stack full!");
		return -1;
	} 

	#ifdef DEBUG_EVENT
	printf("Added event to slot #%d\n", buffer->amount);
	#endif

	buffer->event_list[buffer->amount] = e;
	buffer->amount++;

	return 0;
}


static void traverse_module(EventBuffer * buffer, PTSong * song, long samplecount) {
	int speed = 4;
	int ticklength = (TEMPO * AUDIO_RATE*0.001)*speed;	// tick length in samples
	int channels =	song->song.channels;
	int start_row =	player_samples/ticklength + row_offset;
	int end_row =	(player_samples + samplecount)/ticklength + row_offset;

	int debug_channel_semitones[256];	// used only for printing

	if (start_row == end_row) {
		return;
	}

	for (int r=start_row;r<=end_row;r++) {
		int pattern_row = r % 64;
		int current_position = (r/64) % song->song.length;
		int current_pattern = song->song.orderlist[current_position];

		printf("\tord: %d\n", current_position);

		if (r == last_row) {
			continue;
		}

		for (int c=0;c<channels;c++) {
			int note_array_offset = (current_pattern*MOD_ROWS*channels) + pattern_row*channels + c;
			Note note = song->notedata[note_array_offset];
			long start_samples = r*((TEMPO * AUDIO_RATE*0.001)*speed);
			double start_sec = start_samples/(double)AUDIO_RATE;
			unsigned char volume = 150;
			int third, fifth;
			// new notes to be played on this channel
			int new_notes[3] = {EMPTY_NOTE_VALUE, EMPTY_NOTE_VALUE, EMPTY_NOTE_VALUE};
			debug_channel_semitones[c] = EMPTY_NOTE_VALUE;

			//if (c!=3) { continue; }
			
			switch (note.command) {
				case 0x00:
					if (note.parameters == 0) {
						break;
					}

					third = (note.parameters & 0xF0) >> 4;
					fifth = (note.parameters & 0x0F);
					new_notes[1] = note.pitch + third;
					new_notes[2] = note.pitch + fifth;

					break;
				case 0x0C:
					volume = note.parameters*4;	// in MOD the max volume is 0x40
					push_event(buffer, create_volume_event(start_sec, c, volume));
					break;
				default:
					break;
			}

			if (note.pitch != EMPTY_NOTE_VALUE) {
				new_notes[0] = note.pitch;
				//continue;
			}

			// if there's new notes on this channel, create end_all event first
			for (int i=0;i<3;i++) {
				if (new_notes[i] != EMPTY_NOTE_VALUE) {
					push_event(buffer, create_end_all_event(start_sec, c));
					break;
				}
			}

			for (int i=0;i<3;i++) {
				if (new_notes[i] == EMPTY_NOTE_VALUE) {
					continue;
				}
		
				push_event(buffer, create_note_event(start_sec, c, new_notes[i], true, volume));
			}

			debug_channel_semitones[c] = note.pitch;
		}

		
	}

	printf("start_row: %d\t end_row: %d\t ticklen: %d\n", start_row, end_row, samplecount);
	for (int i=0;i<channels;i++) {
		char name[4];
		getNoteName(debug_channel_semitones[i], (uint8_t *)name);
		printf("%s ", name);
	}	

	printf("\n");

	last_row = end_row;

	//Event e = create_note_event(1.0, 0, 5);
	//short s = (e.data[0] << 8) | e.data[1];
}

// pushes new commands to the command buffer
// only picks commands between start_time and end_time
void player_update(EventBuffer * buffer, long samplecount) {
	//long time = long((double)millis/((double)AUDIO_RATE/1000.0));
	PTSong * song = loaded_song;

	if (loaded_song == NULL) {
		fprintf(stderr, "Song not loaded, cannot play!\n");
		return;
	}

	traverse_module(buffer, song, samplecount); 

	player_samples += samplecount;
}

