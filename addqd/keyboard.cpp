#include <Windows.h>
#include "config.h"
#include "keyboard.h"
#include "addsynth.h"

void keys_init() {
	for (int i=0;i<KEYBOARD_KEYS;i++) {
		keymap[i] = KEY_NULL;
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
}

void keys_check_presses() {
	for (int i=KEYBOARD_BASE;i<KEYBOARD_KEYS;i++) {
		if (keymap[i] == KEY_NULL) {
			continue;
		} 

		if (GetAsyncKeyState(i)) {
			syn_play_note(0, keymap[i]);
		} else {
			syn_end_note(0, keymap[i]);
		}
	}
}
