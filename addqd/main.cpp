
#include <cstdio>
#include <windows.h>
#include <mmsystem.h>
#include <cassert>
#include <cmath>

#include "util.h"
#include "player/sound.h"
#include "player/addsynth.h"
#include "player/player.h"
#include "editor/keyboard.h"
#include "amigamod.h"
#include "modplayer.h"
#include "serialize.h"
#include "tests.h"

int main(int argc, char argv[]) {
	mod::player_init();
	PTSong song = mod::load_PTSong("mods/daveys.mod");
	mod::player_load_PTSong(&song);

	test_serialization();

	int samplelength=-1;
	//char * samplesnare = load16bitWAV("mods/snare.wav", &samplelength);
	//dprint(samplelength);

	Sample snare;
	generate_noise_sample(&snare, 4410);
	//convert_sample_to_float(&snare, samplesnare, samplelength);

	//dumpArrayToDisk((char *)snare.data, snare.length*sizeof(float), "output/sample.raw");
	//dumpArrayToDisk(samplesnare, samplelength, "output/sample2.raw");
	//printf("%d", snare.length*sizeof(float));
	syn_init(8);

	Instrument insarr[SYN_MAX_INSTRUMENTS];	// the size of this must match the variable 
											// instrument_list_max_length in addsynth.cpp

	Instrument * noise = &insarr[0];
	Instrument * tri = &insarr[1];
	Instrument * square = &insarr[2];
	Instrument * square2 = &insarr[3];

	*noise = syn_init_instrument(INS_SAMPLER);
	noise->volume=0.3f;
	//noise.waveFunc = *Generators::noise;
	noise->octave = -3;
	noise->samplerFunc = *generators::sampler;
	noise->sample = &snare;
	noise->env[0].attack = 0.01f;
	noise->env[0].release = 0.03f;

	*tri = syn_init_instrument(INS_FM_TWO_OP);
	tri->volume=0.5f;
	tri->octave=-2;
	tri->fmFunc = *generators::resonant_fm;
	tri->env[0].attack = 0.1f;
	tri->env[0].release = 0.8f;
	//tri->env[0].decay = 0.5f;

	tri->matrix.routes[0].enabled = true;
	tri->matrix.routes[0].source = MOD_ENV1;
	tri->matrix.routes[0].target.device = MOD_DEVICE_LOCAL;
	tri->matrix.routes[0].target.param_index = PARAM_VOLUME;

	*square = syn_init_instrument(INS_OSC);
	square->volume=0.3f;
	square->octave = -1;
	square->waveFunc = *generators::sinsquare;
	square->env[0].attack = 0.001f;
	square->env[0].release = 0.1f;

	*square2 = syn_init_instrument(INS_OSC);
	square2->volume=0.3f;
	square2->octave = -1;
	square2->waveFunc = *generators::square;
	square2->env[0].attack = 0.001f;
	square2->env[0].release = 0.1f;

	Instrument** listpointer = syn_get_instrument_list_pointer();
	*listpointer = (Instrument *)&insarr;

	syn_attach_instrument(0, 1);
	syn_attach_instrument(1, 0);
	syn_attach_instrument(2, 2);
	syn_attach_instrument(3, 3);
	syn_attach_instrument(4, 1);
	syn_attach_instrument(5, 0);
	syn_attach_instrument(6, 0);
	syn_attach_instrument(7, 0);
	
	keys_init();
	init_sound();

	/*
	// EXPORT TEST
	EventBuffer newevents;
	record_events(mod::player_update, AUDIO_RATE*60, &newevents);
	printf("doned!\n");

	for (int i=0;i<50;i++) {
		char estring[512];
		event_to_string(newevents.event_list[i], estring, 512);
		fprintf(stdout, "%s\n", estring);
	}

	long fsize = -1;
	dprint(newevents.event_list[0].channel);
	dprint(newevents.amount);
	dprint(newevents.max_events);
	char * recorddata = serialize_event_array(newevents.event_list, newevents.amount, &fsize);
	assert(recorddata != NULL);
	dumpArrayToDisk(recorddata, fsize, "output/notedata.dat");

	player::init();
	player::load_song(&newevents);

	WAITKEY();
	printf("playin' the song\n");
	
	*/

	mod::player_init();
	mod::player_load_PTSong(&song);
	
	// MUTE channels
	//syn_get_channel(0)->volume = 0.0f;
	//syn_get_channel(1)->volume = 0.0f;
	//syn_get_channel(2)->volume = 0.0f;
	//syn_get_channel(3)->volume = 0.0f;

	int start = GetTickCount();

	while(!GetAsyncKeyState(VK_ESCAPE)) {
		int t = GetTickCount()-start;
		keys_check_transport();
		//keys_check_presses();
		poll_sound(syn_render_block, mod::player_update);
		//poll_sound(syn_render_block, player::update);
		Sleep(10);
	}

	delete song.notedata;
	//syn_free_instrument(&whitenoise);	
	free_sound();

	return 0;
}