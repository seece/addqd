
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

	char * samplesnare = load16bitWAV("mods/snare.wav");

	syn_init(8);

	Instrument whitenoise;
	whitenoise.volume=0.15f;
	whitenoise.octave=0;
	whitenoise.waveFunc = *Oscillators::noise;
	whitenoise.env.attack = 0.01f;
	whitenoise.env.release = 0.01f;

	Instrument tri;
	tri.volume=0.2f;
	tri.octave=-2;
	tri.waveFunc = *Oscillators::triangle;
	tri.env.attack = 0.05f;
	tri.env.release = 0.1f;

	Instrument square;
	square.volume=0.2f;
	square.octave=-2;
	square.waveFunc = *Oscillators::square;
	square.env.attack = 0.05f;
	square.env.release = 0.1f;

	syn_load_instrument(0, &whitenoise);
	syn_load_instrument(1, &tri);
	syn_attach_instrument(0, 1);
	syn_attach_instrument(1, 0);
	syn_attach_instrument(2, 1);
	syn_attach_instrument(3, 1);
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