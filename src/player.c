#include <stdint.h>
#include "minimal_player_info.h"
#include "player.h"

Player new_player(uint16_t pos_x, uint16_t pos_y) {
	Player player = {
		.pos_x = pos_x,
		.pos_y = pos_y,
		.speed = 1,
	};

	return player;

}

MinimalPlayerInfo get_minimal_player_info(Player* player) {
	MinimalPlayerInfo minimal_player = {
		.pos_x = player->pos_x,
		.pos_y = player->pos_y,

	};

	return minimal_player;

}

