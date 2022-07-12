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

void player_input(Player* player, const Camera2D* camera, const KeyBindings* key_bindings, Projectile** projectiles, uint16_t* num_projectiles, const Map* map) {
	Vector2 mouse_pos = GetMousePosition();

	// Ability input
	if (IsKeyDown(key_bindings->ability)) {
		use_ability(player, map);
		
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

	move_player(player, movement_info, map);	

	float angle_target_x;
	float angle_target_y;

	if (player->pos_x < SCREEN_WIDTH / 2) {
		angle_target_x = player->pos_x;
	
	} else if (player->pos_x > map->size_x - SCREEN_WIDTH / 2) {
		angle_target_x = player->pos_x - map->size_x + (float)SCREEN_WIDTH;

	} else {
		angle_target_x = (float)SCREEN_WIDTH / 2.0;

	}

	if (player->pos_y < SCREEN_HEIGHT / 2) {
		angle_target_y = player->pos_y;
	
	} else if (player->pos_y > map->size_x - SCREEN_HEIGHT / 2) {
		angle_target_y = player->pos_y - map->size_y + (float)SCREEN_HEIGHT;

	} else {
		angle_target_y = (float)SCREEN_HEIGHT / 2.0;

	}

	player->direction = get_angle(mouse_pos.x, mouse_pos.y, angle_target_x, angle_target_y);

	if (IsMouseButtonDown(0)) {
		shoot(projectiles, num_projectiles, player, player->direction);

	}

}
