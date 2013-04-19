#define WIN32_LEAN_AND_MEAN
#define WIN32_EXTRA_LEAN

#include <cstdio>
#include <Windows.h>
#include "synth.h"


int main(int argc, char * argv[]) {
	#ifdef MAGIC
		printf("MAGIC IS REAL!\n");
	#endif

	init_sound();

	while (!GetAsyncKeyState(VK_ESCAPE)) {
		Sleep(10);
	}

	free_sound();

	return 0;
}