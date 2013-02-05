
#include <cstdio>
#include <windows.h>
#include <mmsystem.h>

#include "util.h"
#include "sound.h"
#include "addsynth.h"

static short sampleBuffer[SYN_BUFFERSIZE + 22];

int main(int argc, char argv[]) {
	dprint(sizeof(Event));
	InitSound();

	WAITKEY();

	sndPlaySound( 0, 0 );
	return 0;
}