#ifndef INPUT_H
#define INPUT_H

#include "include/raylib.h"
#include "map.h"
#include "player.h"
#include "projectile.h"

typedef struct {
	KeyboardKey up;
	KeyboardKey down;
	KeyboardKey left;
	KeyboardKey right;
	KeyboardKey ability;
} KeyBindings;

void player_input(Player* player, const KeyBindings* key_bindings, const Map* map, bool using_keyboard);
#endif
