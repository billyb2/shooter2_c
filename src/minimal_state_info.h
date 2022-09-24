#ifndef MINIMAL_PLAYER_INFO_H
#define MINIMAL_PLAYER_INFO_H
#include <stdint.h>
#include <stdbool.h>
#ifndef WASM
#include <assert.h>
#endif

#include "weapon.h"
#include "player_ability.h"

typedef struct  __attribute__((__packed__)) MinimalPlayerInfo {
	uint64_t id;

	uint64_t team_id;

	uint16_t health;
	bool cloaking;
	uint8_t ammo;

	uint32_t _padding;


	uint32_t num_kills;
	Weapon weapon;
	Ability ability;
	float pos_x;
	float pos_y;
	float direction;

} MinimalPlayerInfo;

typedef struct __attribute__((__packed__)) MinimalTeamInfo {
	uint64_t id;
	uint64_t score;
	MinimalPlayerInfo players[255];
	uint8_t num_players;

} MinimalTeamInfo;

#ifndef WASM
static_assert((sizeof(MinimalPlayerInfo) % 8 == 0), "MinimalPlayerInfo is the wrong size, add padding");
#endif

typedef struct MinimalProjectileInfo {
	float pos_x;
	float pos_y;
	float angle;	
	float speed;	
	uint16_t damage;
	uint16_t size;

} MinimalProjectileInfo;


#ifndef WASM
static_assert((sizeof(MinimalProjectileInfo) == 20), "MinimalProjectileInfo is the wrong size, add padding");
#endif

#endif
