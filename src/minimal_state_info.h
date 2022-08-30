#ifndef MINIMAL_PLAYER_INFO_H
#define MINIMAL_PLAYER_INFO_H
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>

#include "weapon.h"
#include "player_ability.h"

typedef struct MinimalPlayerInfo {
	uint64_t id;
	uint16_t health;
	bool cloaking;
	uint8_t ammo;
	Weapon weapon;
	Ability ability;
	float pos_x;
	float pos_y;
	float direction;

} MinimalPlayerInfo;


static_assert((sizeof(MinimalPlayerInfo) == 32), "MinimalPlayerInfo is the wrong size, add padding");

typedef struct MinimalProjectileInfo {
	float pos_x;
	float pos_y;
	float angle;	
	float speed;	
	uint16_t damage;
	uint16_t size;

} MinimalProjectileInfo;


static_assert((sizeof(MinimalProjectileInfo) == 20), "MinimalProjectileInfo is the wrong size, add padding");

#endif
