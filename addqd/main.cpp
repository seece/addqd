
#include <cstdio>
#include <windows.h>
#include <mmsystem.h>
#include <cmath>

#include "util.h"
#include "player/sound.h"
#include "player/addsynth.h"
#include "editor/keyboard.h"
#include "amigamod.h"
#include "modplayer.h"
#include "serialize.h"
#include "tests.h"

int main(int argc, char argv[]) {
	player_init();
	PTSong song = load_PTSong("mods/daveys.mod");
	player_load_PTSong(&song);

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

	Instrument noise = syn_create_instrument(INS_SAMPLER);
	noise.volume=0.3f;
	//noise.waveFunc = *Generators::noise;
	noise.octave = -3;
	noise.samplerFunc = *generators::sampler;
	noise.sample = &snare;
	noise.env.attack = 0.01f;
	noise.env.release = 0.03f;

	Instrument tri = syn_create_instrument(INS_OSC);
	tri.volume=0.5f;
	tri.octave=-2;
	tri.waveFunc = *generators::triangle;
	tri.env.attack = 0.01f;
	tri.env.release = 0.08f;

	Instrument square = syn_create_instrument(INS_OSC);
	square.volume=0.3f;
	square.octave = -1;
	square.waveFunc = *generators::sinsquare;
	square.env.attack = 0.001f;
	square.env.release = 0.001f;

	Instrument square2 = syn_create_instrument(INS_OSC);
	square2.volume=0.3f;
	square2.octave = -1;
	square2.waveFunc = *generators::square;
	square2.env.attack = 0.001f;
	square2.env.release = 0.001f;

	syn_load_instrument(0, &noise);
	syn_load_instrument(1, &tri);
	syn_load_instrument(2, &square2);
	syn_load_instrument(3, &square);

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

	// EXPORT TEST
	EventBuffer newevents;
	record_events(player_update, AUDIO_RATE*5, &newevents);
	printf("doned!\n");
	for (int i=0;i<5;i++) {
		char estring[256];
		event_to_string(newevents.event_list[0], estring, 256);
		fprintf(stdout, "%s\n", estring);
	}

	WAITKEY();

	player_init();
	player_load_PTSong(&song);
	
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
		poll_sound(syn_render_block, player_update);
		Sleep(10);
	}

	delete song.notedata;
	//syn_free_instrument(&whitenoise);	
	free_sound();

	return 0;
}