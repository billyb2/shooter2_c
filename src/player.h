#ifndef PLAYER_H
#define PLAYER_H

#include <stdint.h>
#include "map.h"
#include "player_ability.h"
#include "minimal_state_info.h"
#include "weapon.h"

#define PLAYER_SIZE 10
#define PLAYER_MAX_HEALTH 500

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

typedef struct PlayerKill {
	uint64_t player_id_killed;

} PlayerKill;

typedef struct Player {
	float pos_x;
	float pos_y;
	float speed;
	Ability ability;
	Weapon weapon;

	uint8_t ammo;
	bool reloading;
	uint16_t remaining_reload_frames;

	uint8_t num_throwables;
	Throwable throwable;
	float direction;
	uint16_t health;

	uint16_t remaining_ability_cooldown_frames;
	uint16_t remaining_shooting_cooldown_frames;
	uint16_t remaining_throwable_cooldown_frames;
	uint16_t num_frames_dead;

	uint64_t id;
	bool assigned_id;

	bool shooting;
	bool using_ability;
	WeaponIndex equipped_weapon;
	float throw_ratio;

	bool is_net_player;

	char* username;
	uint64_t last_hurt_by;

	PlayerKill* kills;
	uint16_t num_kills;

} Player;

Player new_player(Ability ability, Weapon weapon, Throwable throwable, const Map* map, char* player_name);
MinimalPlayerInfo get_minimal_player_info(const Player* player);
void use_ability(Player* player, const Map* map);
void reload(Player* player);
void update_player_cooldowns(Player* players, uint8_t num_players);
void move_player(Player* player, PlayerMovementInfo movement_info, const Map* map);
void respawn_players(Player* players, uint8_t num_players);

#endif
