#include <stdint.h>
#include "player.h"

Player new_player(uint16_t pos_x, uint16_t pos_y) {
	Player player = {
		.pos_x = pos_x,
		.pos_y = pos_y,
		.speed = 1,
	};

	return player;

}
