
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
	PTSong song = load_PTSong("mods/test2.mod");
	player_load_PTSong(&song);

	char * samplesnare = load16bitWAV("mods/snare.wav");

	syn_init(8);

	Instrument ins;
	ins.volume=0.15f;
	ins.octave=0;
	ins.waveFunc = sin;
	ins.env.attack = 0.01f;
	ins.env.release = 0.1f;

	Instrument ins2;
	ins2.volume=0.14f;
	ins2.octave=0;
	ins2.waveFunc = sin;
	ins2.env.attack = 0.05f;
	ins2.env.release = 0.1f;

	syn_load_instrument(0, &ins);
	syn_load_instrument(1, &ins2);
	syn_attach_instrument(0, 0);
	syn_attach_instrument(1, 0);
	syn_attach_instrument(2, 0);
	syn_attach_instrument(3, 0);
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
	syn_free_instrument(&ins);
	free_sound();

	return 0;
}