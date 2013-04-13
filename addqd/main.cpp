
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

	dumpArrayToDisk((char *)snare.data, snare.length*sizeof(float), "output/sample.raw");
	//printf("%d", snare.length*sizeof(float));
	syn_init(8);

	Instrument noise;
	noise.volume=0.15f;
	noise.octave=0;
	noise.waveFunc = *Oscillators::noise;
	noise.env.attack = 0.01f;
	noise.env.release = 0.01f;

	Instrument tri;
	tri.volume=0.5f;
	tri.octave=-2;
	tri.waveFunc = *Oscillators::triangle;
	tri.env.attack = 0.02f;
	tri.env.release = 0.01f;

	Instrument square;
	square.volume=0.3f;
	square.octave=0;
	square.waveFunc = *Oscillators::square;
	square.env.attack = 0.001f;
	square.env.release = 0.001f;

	syn_load_instrument(0, &noise);
	syn_load_instrument(1, &tri);
	syn_load_instrument(2, &square);

	syn_attach_instrument(0, 1);
	syn_attach_instrument(1, 0);
	syn_attach_instrument(2, 2);
	syn_attach_instrument(3, 2);
	syn_attach_instrument(4, 0);
	syn_attach_instrument(5, 0);
	syn_attach_instrument(6, 0);
	syn_attach_instrument(7, 0);

	keys_init();
	init_sound();
	
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