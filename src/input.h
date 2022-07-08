#ifndef INPUT_H
#define INPUT_H

#include "include/raylib.h"
#include "player.h"

typedef struct {
	KeyboardKey up;
	KeyboardKey down;
	KeyboardKey left;
	KeyboardKey right;
} KeyBindings;

void player_input(Player* player, const KeyBindings* key_bindings);
#endif
