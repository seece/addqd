
#include <cstdio>
#include <windows.h>
#include <mmsystem.h>
#include <cmath>

#include "util.h"
#include "sound.h"
#include "addsynth.h"
#include "keyboard.h"

int main(int argc, char argv[]) {
	dprint(sizeof(Event));
	syn_init(1);

	Instrument ins;
	ins.volume=1.0;
	ins.waveFunc = sinf;
	ins.env.attack = 0.01f;
	ins.env.release = 0.4f;

	syn_load_instrument(0, &ins);
	syn_attach_instrument(0, 0);

	keys_init();
	init_sound();
	
	while(!GetAsyncKeyState(VK_ESCAPE)) {
		keys_check_presses();
		poll_sound(syn_render_block);
		Sleep(10);
	}

	syn_free_instrument(&ins);
	free_sound();

	return 0;
}