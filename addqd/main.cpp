
#include <cstdio>
#include <windows.h>
#include <mmsystem.h>

#include "util.h"
#include "sound.h"
#include "addsynth.h"
#include "keyboard.h"

int main(int argc, char argv[]) {
	dprint(sizeof(Event));
	syn_init(1);

	Instrument ins;
	ins.volume=1.0;
	ins.spectra.keyframe_amount = 1;
	ins.spectra.interpolation = new char[1];
	ins.spectra.spectrum = new Spectrum[1];
	create_spectrum(&ins.spectra.spectrum[0]);

	syn_load_instrument(0, &ins);
	syn_attach_instrument(0, 0);

	keys_init();
	init_sound();
	
	while(!GetAsyncKeyState(VK_ESCAPE)) {
		keys_check_presses();
		poll_sound(syn_render_block);
		Sleep(1);
	}

	syn_free_instrument(&ins);
	free_sound();

	return 0;
}