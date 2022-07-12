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

void player_input(Player* player, const Camera2D* camera, const KeyBindings* key_bindings, Projectile** projectiles, uint16_t* num_projectiles, const Map* map, bool using_keyboard);
#endif
