#ifndef PLAYER_H
#define PLAYER_H

#include <stdint.h>
#include "minimal_player_info.h"

struct Player {
	uint16_t pos_x;
	uint16_t pos_y;
	uint16_t speed;
};

typedef struct Player Player;

Player new_player(uint16_t pos_x, uint16_t pos_y);
MinimalPlayerInfo get_minimal_player_info(const Player* player);
#endif
