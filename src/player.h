#ifndef PLAYER_H
#define PLAYER_H

#include <stdint.h>
#include "minimal_player_info.h"

typedef enum Ability {
	Teleporation,

} Ability;

struct Player {
	uint16_t pos_x;
	uint16_t pos_y;
	uint16_t speed;
	Ability ability;
	// TODO: integer addition only
	float direction;
};

typedef struct Player Player;

Player new_player(uint16_t pos_x, uint16_t pos_y, Ability ability);
MinimalPlayerInfo get_minimal_player_info(const Player* player);
#endif
