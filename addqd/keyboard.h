#ifndef KEYBOARD_H
#define KEYBOARD_H

#define KEYBOARD_KEYS 200
#define KEYBOARD_BASE 0x30
#define KEY_NULL -999


static int keymap[KEYBOARD_KEYS];
static bool keys_held[KEYBOARD_KEYS];
void keys_init();
void keys_check_presses();
#endif