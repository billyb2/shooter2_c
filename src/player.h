#ifndef PLAYER_H
#define PLAYER_H

#include <stdint.h>
#include "map.h"
#include "minimal_state_info.h"
#include "weapon.h"

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
	float pos_x;
	float pos_y;
	float speed;
	Ability ability;
	Weapon weapon;
	float direction;
	uint16_t health;
	uint16_t remaining_ability_cooldown_frames;
	uint16_t remaining_shooting_cooldown_frames;
} Player;

Player new_player(float pos_x, float pos_y, Ability ability, Weapon weapon);
MinimalPlayerInfo get_minimal_player_info(const Player* player);
void use_ability(Player* player, const Map* map);
void update_player_cooldowns(Player* players, uint8_t num_players);
void move_player(Player* player, PlayerMovementInfo movement_info, const Map* map);
#endif
