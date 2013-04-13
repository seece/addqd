
#include <cstdio>
#include <windows.h>
#include <mmsystem.h>
#include <cmath>

#include "util.h"
#include "sound.h"
#include "addsynth.h"
#include "keyboard.h"
#include "amigamod.h"
#include "player.h"

int main(int argc, char argv[]) {
	player_init();
	PTSong song = load_PTSong("mods/daveys.mod");
	player_load_PTSong(&song);

	int samplelength=-1;
	char * samplesnare = load16bitWAV("mods/snare.wav", &samplelength);
	dprint(samplelength);

	Sample snare;
	convert_sample_to_float(&snare, samplesnare, samplelength);

	//dumpArrayToDisk((char *)snare.data, snare.length*sizeof(float), "output/sample.raw");
	//dumpArrayToDisk(samplesnare, samplelength, "output/sample2.raw");
	//printf("%d", snare.length*sizeof(float));
	syn_init(8);

	Instrument noise = syn_create_instrument(INS_SAMPLER);
	noise.volume=0.8f;
	//noise.waveFunc = *Generators::noise;
	noise.samplerFunc = *Generators::sampler;
	noise.sample = &snare;
	noise.env.attack = 0.01f;
	noise.env.release = 0.1f;

	Instrument tri = syn_create_instrument(INS_OSC);
	tri.volume=0.5f;
	tri.octave=-2;
	tri.waveFunc = *Generators::sinsquare;
	tri.env.attack = 0.02f;
	tri.env.release = 0.01f;

	Instrument square = syn_create_instrument(INS_OSC);
	square.volume=0.3f;
	square.waveFunc = *Generators::sinsquare;
	square.env.attack = 0.001f;
	square.env.release = 0.001f;

	syn_load_instrument(0, &noise);
	syn_load_instrument(1, &tri);
	syn_load_instrument(2, &square);

	syn_attach_instrument(0, 1);
	syn_attach_instrument(1, 0);
	syn_attach_instrument(2, 2);
	syn_attach_instrument(3, 2);
	syn_attach_instrument(4, 1);
	syn_attach_instrument(5, 0);
	syn_attach_instrument(6, 0);
	syn_attach_instrument(7, 0);

	keys_init();
	init_sound();
	
	// MUTE channels
	//syn_get_channel(0)->volume = 0.0f;
	syn_get_channel(1)->volume = 0.0f;
	//syn_get_channel(2)->volume = 0.0f;
	//syn_get_channel(3)->volume = 0.0f;


	int start = GetTickCount();

	while(!GetAsyncKeyState(VK_ESCAPE)) {
		int t = GetTickCount()-start;
		keys_check_transport();
		//keys_check_presses();
		//play_PTSong(&song, t);
		poll_sound(syn_render_block, player_update);
		Sleep(10);
	}

	delete song.notedata;
	//syn_free_instrument(&whitenoise);	
	free_sound();

	return 0;
}