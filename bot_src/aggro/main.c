#include <stdint.h>

#include "minimal_player_info.h"
#include "bot_actions.h"
#include "bot_actions_def.h"

int32_t get_num_players_ptr();
int32_t get_players_ptr();
int32_t get_num_projectiles_ptr();
int32_t get_projectiles_ptr();

float sqrtf(float x);

uint16_t powu(uint16_t num, uint16_t base) {
	uint16_t result = num;

	if (base == 0) {
		return 1;

	}
	
	for (uint16_t i = 0; i < base - 1; i += 1) {
		result *= num;

	}

	return result;

}

uint16_t saturating_sub(uint16_t x, uint16_t y) {
	if (y > x) {
		return 0;

	} else {
		return x - y;

	}

}

float distance(uint16_t pos1_x, uint16_t pos1_y, uint16_t pos2_x, uint16_t pos2_y) {
	return saturating_sub(pos1_x, pos2_x);
	//return sqrtf( powu((pos2_x - pos1_x), 2) + powu((pos2_y - pos1_y), 2) );

}

BotActions calc_actions() {
	BotActions actions = {
		.direction = 0.0,
		.movement_direction_left_right = NONE,
		.movement_direction_up_down = NONE,

	};

	const uint16_t* num_projectiles = (uint16_t*)get_num_projectiles_ptr();
	const MinimalProjectileInfo* projectiles = (MinimalProjectileInfo*)get_projectiles_ptr();

	const uint8_t* num_players = (uint8_t*)get_num_players_ptr();
	const MinimalPlayerInfo* players = (MinimalPlayerInfo*)get_players_ptr();


	//if (sqrtf(9.0) == 3.0 && *num_players == 2) {
	if (distance(players[0].pos_x, players[0].pos_y, players[1].pos_x, players[1].pos_y) < 10.0) {
		actions.movement_direction_left_right = RIGHT;

	}



	return actions;

}


