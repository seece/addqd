#define WIN32_LEAN_AND_MEAN
#define WIN32_EXTRA_LEAN

#include <cstdio>
#include <Windows.h>
#include "synth.h"
#include "introsong.h"
#include "notedata.h"


int main(int argc, char * argv[]) {
	#ifdef MAGIC
		printf("MAGIC IS REAL!\n");
	#endif

	player::init();
	syn_init(8);
	EventBuffer buf = load_serialized_song((char *)notedata);
	player::load_song(&buf);

	// notedata_len is not needed since the amount of events
	// is saved in the first 4 bytes of the data array

	init_sound();

	printf("event amount %d\n", buf.amount );

	int start = GetTickCount();

	while(!GetAsyncKeyState(VK_ESCAPE)) {
		int t = GetTickCount()-start;

		poll_sound(syn_render_block, player::update);
		Sleep(10);
	}

	free_sound();

	return 0;
}