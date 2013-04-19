#include <cstdio>
#include <Windows.h>
#include "player/config.h"
#include "keyboard.h"
#include "player/addsynth.h"
#include "../modplayer.h"

void keys_init() {
	for (int i=0;i<KEYBOARD_KEYS;i++) {
		keymap[i] = KEY_NULL;
		keys_held[i] = false;
	}

	// These are key codes from 
	// http://msdn.microsoft.com/en-us/library/windows/desktop/dd375731(v=vs.85).aspx
	keymap[0x51] = 0;	// Q
	keymap[0x32] = 1;	// 2
	keymap[0x57] = 2;	// W
	keymap[0x33] = 3;	// 3
	keymap[0x45] = 4;	// E
	keymap[0x52] = 5;	// R
	keymap[0x35] = 6;	// 5
	keymap[0x54] = 7;	// T
	keymap[0x36] = 8;	// 6
	keymap[0x59] = 9;	// Y
	keymap[0x37] = 10;	// 7
	keymap[0x55] = 11;	// U
	keymap[0x49] = 12;	// I
	keymap[0x39] = 13;	// 9
	keymap[0x4f] = 14;	// O
	keymap[0x30] = 15;	// 0
	keymap[0x50] = 16;	// P

	keymap[0x5A] = -12 + 0;		// Z
	keymap[0x53] = -12 + 1;		// S
	keymap[0x58] = -12 + 2;		// X
	keymap[0x44] = -12 + 3;		// D
	keymap[0x43] = -12 + 4;		// C
	keymap[0x56] = -12 + 5;		// V
	keymap[0x47] = -12 + 6;		// G
	keymap[0x42] = -12 + 7;		// B
	keymap[0x48] = -12 + 8;		// H
	keymap[0x4E] = -12 + 9;		// N
	keymap[0x4A] = -12 + 10;	// J
	keymap[0x4D] = -12 + 11;	// M
	keymap[VK_OEM_COMMA]	= -12 + 12;	// ,
	keymap[0x4C]			= -12 + 13;	// L
	keymap[VK_OEM_PERIOD]	= -12 + 14;	// .
	//keymap[0x4f]			= -12 + 15;	// Ö
	keymap[VK_OEM_MINUS]	= -12 + 16;	// -
}

void keys_check_transport() {
	int jump = 32;

	if (GetAsyncKeyState(VK_LEFT)) {
		//player_add_offset(-jump);
	} else if (GetAsyncKeyState(VK_RIGHT)) {
		//player_add_offset(jump);
	}
}

void keys_check_presses() {
	int channel = 4;

	for (int i=KEYBOARD_BASE;i<KEYBOARD_KEYS;i++) {
		if (keymap[i] == KEY_NULL) {
			continue;
		} 

		if (GetAsyncKeyState(i)) {
			if (keys_held[i]) {
				continue;
			}
			
			keys_held[i] = true;
			
			syn_play_note(channel, keymap[i]);
			
			printf("key: %x\n", i);
		} else {
			// send the noteoff event only once
			if (keys_held[i]) {
				syn_end_note(channel, keymap[i]);
			}

			keys_held[i] = false;
		}
	}
}
