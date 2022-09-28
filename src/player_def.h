#ifndef PLAYER_DEF_H
#define PLAYER_DEF_H

#include "player_ability.h"
#include "weapon.h"
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

	uint16_t num_frames_ability_in_use;
	uint16_t ability_charge;

	uint16_t remaining_shooting_cooldown_frames;
	uint16_t remaining_throwable_cooldown_frames;
	uint16_t num_frames_dead;

	uint64_t id;
	uint64_t team_id;
	bool assigned_id;
	bool assigned_team_id;

	bool shooting;
	bool using_ability;
	WeaponIndex equipped_weapon;
	float throw_ratio;

	bool is_net_player;

	char* username;
	uint64_t last_hurt_by;

	bool cloaking;

	PlayerKill* kills;
	uint16_t num_kills;

	uint16_t weapon_switch_cooldown;
	
} Player;
#endif
