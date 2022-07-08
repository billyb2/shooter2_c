#include <stdlib.h>

#include "include/raylib.h"

#include "player.h"
#include "input.h"
#include "math.h"

void player_input(Player* player, const KeyBindings* key_bindings) {
	if (IsKeyDown(key_bindings->up)) {
		player->pos_y = saturating_sub(player->pos_y, player->speed);
	}

	if (IsKeyDown(key_bindings->down)) {
		player->pos_y += player->speed;
	}

	if (IsKeyDown(key_bindings->left)) {
		player->pos_x = saturating_sub(player->pos_x, player->speed);
	}

	if (IsKeyDown(key_bindings->right)) {
		player->pos_x += player->speed;
	}
}
