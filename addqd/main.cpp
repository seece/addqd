
#include <cstdio>
#include <windows.h>
#include <mmsystem.h>

#include "util.h"
#include "sound.h"
#include "addsynth.h"

int main(int argc, char argv[]) {
	dprint(sizeof(Event));
	syn_init(1);
	init_sound();

	while(!GetAsyncKeyState(VK_ESCAPE)) {
		Sleep(1);
	}

	free_sound();

	return 0;
}