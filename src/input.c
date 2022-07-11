#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "include/raylib.h"

#include "player.h"
#include "input.h"
#include "math.h"
#include "projectile.h"

extern int SCREEN_WIDTH;
extern int SCREEN_HEIGHT;

void player_input(Player* player, const KeyBindings* key_bindings, Projectile** projectiles, uint16_t* num_projectiles) {
	Vector2 mouse_pos = GetMousePosition();

	// Ability input
	if (IsKeyDown(key_bindings->ability)) {
		use_ability(player);
		
	}

	PlayerMovementInfo movement_info = {
		.x_axis = NoneX,
		.y_axis = NoneY,

	};

	// Movement input
	if (IsKeyDown(key_bindings->up)) {
		movement_info.y_axis = Up;
	}

	if (IsKeyDown(key_bindings->down)) {
		movement_info.y_axis = Down;

	}

	if (IsKeyDown(key_bindings->left)) {
		movement_info.x_axis = Left;
	}

	if (IsKeyDown(key_bindings->right)) {
		movement_info.x_axis = Right;

	}

	move_player(player, movement_info);	

	player->direction = get_angle(mouse_pos.x, mouse_pos.y, (float)player->pos_x, (float)player->pos_y); 


	if (IsMouseButtonDown(0)) {
		shoot(projectiles, num_projectiles, player, player->direction);

	}

}
