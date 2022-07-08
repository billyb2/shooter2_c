#ifndef PLAYER_H
#define PLAYER_H

#include <stdint.h>
struct Player {
	uint16_t pos_x;
	uint16_t pos_y;
	uint16_t speed;
};

typedef struct Player Player;

Player new_player(uint16_t pos_x, uint16_t pos_y);
#endif
