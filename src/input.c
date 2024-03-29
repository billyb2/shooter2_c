#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "include/raylib.h"

#include "minimal_state_info.h"
#include "player.h"
#include "input.h"
#include "math.h"
#include "projectile.h"
#include "weapon.h"

extern int SCREEN_WIDTH;
extern int SCREEN_HEIGHT;

void keyboard_input(Player* player, const KeyBindings* key_bindings, const Map* map) {
	Vector2 mouse_pos = GetMousePosition();

	// Ability input
	if (IsKeyDown(key_bindings->ability)) {
		use_ability(player, map);
		
	}

	float y_axis = 0.0;
	float x_axis = 0.0;
	

	// Movement input
	if (IsKeyDown(key_bindings->up)) {
		y_axis = -1.0;
	}

	if (IsKeyDown(key_bindings->down)) {
		y_axis = 1.0;

	}

	if (IsKeyDown(key_bindings->left)) {
		x_axis = -1.0;
	}

	if (IsKeyDown(key_bindings->right)) {
		x_axis = 1.0;

	}

	PlayerMovementInfo movement_info;

	if (x_axis == 0.0 && y_axis == 0.0) {
		movement_info.moving = false;

	} else {
		movement_info.moving = true;
		movement_info.angle = atan2f(y_axis, x_axis);

	}

	move_player(player, movement_info, map);	

	float angle_target_x;
	float angle_target_y;

	if (player->pos_x < SCREEN_WIDTH / 2.0) {
		angle_target_x = player->pos_x;
	
	} else if (player->pos_x > SCREEN_WIDTH / 2.0 && player->pos_x < map->size_x - SCREEN_WIDTH / 2.0) {
		angle_target_x = SCREEN_WIDTH / 2.0;

	} else {
		angle_target_x = player->pos_x + SCREEN_WIDTH - map->size_x;

	}

	if (player->pos_y < SCREEN_HEIGHT / 2.0) {
		angle_target_y = player->pos_y;
	
	} else if (player->pos_y > SCREEN_HEIGHT / 2.0 && player->pos_y < map->size_y - SCREEN_HEIGHT / 2.0) {
		angle_target_y = SCREEN_HEIGHT / 2.0;

	} else {
		angle_target_y = player->pos_y + SCREEN_HEIGHT - map->size_y;

	}

	player->direction = get_angle(mouse_pos.x, mouse_pos.y, angle_target_x, angle_target_y);
	bool shooting_input = false;

	if (IsMouseButtonDown(0)) {
		player->equipped_weapon = Primary;
		shooting_input = true;

	}

	if (IsKeyDown(KEY_G)) {
		player->equipped_weapon = Tertiary;
		shooting_input = true;
		float throw_distance = distance(mouse_pos.x, mouse_pos.y, angle_target_x, angle_target_y);

		if (throw_distance > 350.0) {
			throw_distance = 350.0;

		}

		// Make throw_distance a value between 0 and 1, to set the initial speed of thw throwable
		throw_distance = throw_distance / 350.0;

		player->throw_ratio = throw_distance;

		
	}

	if (IsKeyDown(KEY_R)) {
		reload(player);

	}

	if (IsKeyDown(KEY_TAB)) {
		// Half a second
		player->weapon_switch_cooldown = 30;

	}

	player->shooting = shooting_input;



}

void controller_input(Player* player, const Map* map, int gamepad_id) {
	/*PlayerMovementInfo movement_info = {
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
		player->shooting = true;

	}

	*/
}

void player_input(Player* player, const KeyBindings* key_bindings, const Map* map, bool using_keyboard) {
	if (using_keyboard) {
		keyboard_input(player, key_bindings, map);

	} else {
		controller_input(player, map, 0);


	}

}
