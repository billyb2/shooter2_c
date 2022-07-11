#ifndef PLAYER_H
#define PLAYER_H

#include <stdint.h>
#include "minimal_player_info.h"

#define PLAYER_SIZE 10

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
	uint16_t health;
	uint16_t remaining_ability_cooldown_frames;
};

typedef struct Player Player;

Player new_player(uint16_t pos_x, uint16_t pos_y, Ability ability);
MinimalPlayerInfo get_minimal_player_info(const Player* player);
void use_ability(Player* player);
void update_player_cooldowns(Player* players, uint8_t num_players);
#endif
