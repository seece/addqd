
#include <cstdio>
#include <windows.h>
#include <mmsystem.h>

#include "util.h"
#include "sound.h"
#include "addsynth.h"

static short sampleBuffer[SYN_BUFFERSIZE + 22];

int main(int argc, char argv[]) {
	dprint(sizeof(Event));
	init_sound();

	while(!GetAsyncKeyState(VK_ESCAPE)) {
		Sleep(1);
	}

	free_sound();

	return 0;
}