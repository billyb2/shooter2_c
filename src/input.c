#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "include/raylib.h"

#include "player.h"
#include "input.h"
#include "math.h"
#include "projectile.h"

extern float SCREEN_WIDTH;
extern float SCREEN_HEIGHT;

void keyboard_input(Player* player, const Camera2D* camera, const KeyBindings* key_bindings, Projectile** projectiles, uint16_t* num_projectiles, const Map* map) {
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

	if (player->pos_x < SCREEN_WIDTH / 2.0) {
		angle_target_x = player->pos_x;
	
	} else if (player->pos_x > map->size_x - SCREEN_WIDTH / 2.0) {
		angle_target_x = player->pos_x - map->size_x + SCREEN_WIDTH;

	} else {
		angle_target_x = SCREEN_WIDTH / 2.0;

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

void controller_input(Player* player, const Camera2D* camera, Projectile** projectiles, uint16_t* num_projectiles, const Map* map, int gamepad_id) {
	PlayerMovementInfo movement_info = {
		.x_axis = NoneX,
		.y_axis = NoneY,

	};

	float axis_x = GetGamepadAxisMovement(gamepad_id, GAMEPAD_AXIS_LEFT_X);
	float axis_y = GetGamepadAxisMovement(gamepad_id, GAMEPAD_AXIS_LEFT_Y);
	
	if (IsGamepadButtonDown(gamepad_id, GAMEPAD_BUTTON_LEFT_FACE_UP)) {
		movement_info.y_axis = Up;

	}

	if (IsGamepadButtonDown(gamepad_id, GAMEPAD_BUTTON_LEFT_FACE_DOWN)) {
		movement_info.y_axis = Down;

	}

	if (IsGamepadButtonDown(gamepad_id, GAMEPAD_BUTTON_LEFT_FACE_LEFT)) {
		movement_info.x_axis = Left;

	}

	if (IsGamepadButtonDown(gamepad_id, GAMEPAD_BUTTON_LEFT_FACE_RIGHT)) {
		movement_info.x_axis = Right;

	}

	if (axis_x > 0.0) {
		movement_info.x_axis = Right;

	} else if (axis_x < 0.0) {
		movement_info.x_axis = Left;

	}

	if (axis_y < 0.0) {
		movement_info.y_axis = Up;

	} else if (axis_y > 0.0) {
		movement_info.y_axis = Down;

	}

	move_player(player, movement_info, map);

	float axis_x_right = -GetGamepadAxisMovement(gamepad_id, GAMEPAD_AXIS_RIGHT_X);
	float axis_y_right = -GetGamepadAxisMovement(gamepad_id, GAMEPAD_AXIS_RIGHT_Y);

	if (axis_x_right != 0.0 || axis_y_right != 0.0) {
		player->direction = get_angle(0.0, 0.0, axis_x_right, axis_y_right);

	}

	if (IsGamepadButtonDown(gamepad_id, GAMEPAD_BUTTON_RIGHT_TRIGGER_2)) {
		shoot(projectiles, num_projectiles, player, player->direction);

	}

}

void player_input(Player* player, const Camera2D* camera, const KeyBindings* key_bindings, Projectile** projectiles, uint16_t* num_projectiles, const Map* map, bool using_keyboard) {
	if (using_keyboard) {
		keyboard_input(player, camera, key_bindings, projectiles, num_projectiles, map);

	} else {
		controller_input(player, camera, projectiles, num_projectiles, map, 0);


	}

}



