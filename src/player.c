#include <stdint.h>
#include "minimal_player_info.h"
#include "player.h"

Player new_player(uint16_t pos_x, uint16_t pos_y, Ability ability) {
	Player player = {
		.pos_x = pos_x,
		.pos_y = pos_y,
		.speed = 5,
		.ability = ability,
		.direction = 0.0,
	};

	return player;

}

MinimalPlayerInfo get_minimal_player_info(const Player* player) {
	MinimalPlayerInfo minimal_player = {
		.pos_x = player->pos_x,
		.pos_y = player->pos_y,
		.direction = player->direction,
	};

	return minimal_player;

}

