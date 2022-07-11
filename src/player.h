#ifndef PLAYER_H
#define PLAYER_H

#include <stdint.h>
#include "minimal_player_info.h"

#define PLAYER_SIZE 10

typedef enum Ability {
	Teleporation,

} Ability;

typedef enum XAxisMovement {
	Left = 0,
	Right = 0xFF,
	NoneX = 0x80,

} XAxisMovement;

typedef enum YAxisMovement {
	Up = 0,
	Down = 0xFF,
	NoneY = 0x80,

} YAxisMovement;

typedef struct PlayerMovementInfo {
	XAxisMovement x_axis;
	YAxisMovement y_axis;
	
} PlayerMovementInfo;

typedef struct Player {
	uint16_t pos_x;
	uint16_t pos_y;
	uint16_t speed;
	Ability ability;
	// TODO: integer addition only
	float direction;
	uint16_t health;
	uint16_t remaining_ability_cooldown_frames;
} Player;

Player new_player(uint16_t pos_x, uint16_t pos_y, Ability ability);
MinimalPlayerInfo get_minimal_player_info(const Player* player);
void use_ability(Player* player);
void update_player_cooldowns(Player* players, uint8_t num_players);
void move_player(Player* player, PlayerMovementInfo movement_info);
#endif
