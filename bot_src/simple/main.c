#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include <math.h>

#include "bot_actions.h"
#include "bot_mem.h"
#include "minimal_player_info.h"

#define PI 3.14159265358979323846264338327950288 

float get_angle(float cx, float cy, float ex, float ey) {
	float dy = ey - cy;
	float dx = ex - cx;

	float angle;

	if (dx != 0.0) {
		angle = atanf(dy / dx); 

	} else {
		if (dy > 0.0) {
			angle = PI / 2.0;

		} else {
			angle = PI;

		}

	}

	if (cx < ex) {
		angle -= PI;

	}

	return angle;
}

float distance(float pos_x1, float pos_y1, float pos_x2, float pos_y2) {
	return sqrtf(powf((pos_x2 - pos_x1), 2) + powf((pos_y2 - pos_y1), 2));
}

BotActions calc_actions() {
	BotActions actions;

	const uint8_t* num_players_ptr = (uint8_t*)get_num_players_ptr();
	uint8_t num_players = *num_players_ptr;

	const uint8_t* player_index_ptr = (uint8_t*)get_player_index_ptr();
	uint8_t player_index = *player_index_ptr;

	const MinimalPlayerInfo* minimal_player_info_list = (MinimalPlayerInfo*)get_players_ptr();
	const MinimalPlayerInfo* my_player = &minimal_player_info_list[player_index];

	actions.movement_direction_left_right = NONE;
	actions.movement_direction_up_down = NONE;
	actions.shooting_and_ability = 0x00;

	const MinimalPlayerInfo* closest_player = NULL;
	float closest_player_distance = 0.0;

	for (uint8_t i = 0; i < num_players; i += 1) {
		const MinimalPlayerInfo* player = &minimal_player_info_list[i];

		if (i == player_index) {
			continue;

		}

		float player_distance = distance(my_player->pos_x, my_player->pos_y, player->pos_x, player->pos_y);
		if (closest_player == NULL || player_distance < closest_player_distance) {
			closest_player_distance = player_distance;
			closest_player = player;

		}
		

	}

	float closest_player_angle = get_angle(closest_player->pos_x, closest_player->pos_y, my_player->pos_x, my_player->pos_y);

	float angle_cos = cosf(closest_player_angle);
	float angle_sin = sinf(closest_player_angle);

	if (closest_player_distance > 50.0) {
		if (angle_cos > 0.0) {
			actions.movement_direction_left_right = RIGHT;

		} else if (angle_cos < 0.0) {
			actions.movement_direction_left_right = LEFT;

		}

		if (angle_sin > 0.0) {
			actions.movement_direction_up_down = DOWN;

		} else if (angle_sin < 0.0) {
			actions.movement_direction_up_down = UP;

		}

	}

	actions.direction = closest_player_angle;

	set_should_shoot(&actions.shooting_and_ability, true);


	return actions;
}
