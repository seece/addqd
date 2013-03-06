
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
	PTSong song = load_PTSong("mods/test1.mod");
	syn_init(4);

	Instrument ins;
	ins.volume=0.15;
	ins.waveFunc = sin;
	ins.env.attack = 0.01f;
	ins.env.release = 0.2f;

	syn_load_instrument(0, &ins);
	syn_attach_instrument(0, 0);
	syn_attach_instrument(1, 0);
	syn_attach_instrument(2, 0);
	syn_attach_instrument(3, 0);

	keys_init();
	init_sound();
	
	int start = GetTickCount();

	while(!GetAsyncKeyState(VK_ESCAPE)) {
		int t = GetTickCount()-start;
		keys_check_presses();
		play_PTSong(&song, t);
		poll_sound(syn_render_block);
		Sleep(10);
	}

	delete song.notedata;
	syn_free_instrument(&ins);
	free_sound();

	return 0;
}