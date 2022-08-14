#ifndef MINIMAL_PLAYER_INFO_H
#define MINIMAL_PLAYER_INFO_H
#include <stdint.h>
#include <assert.h>

typedef struct MinimalPlayerInfo {
	float pos_x;
	float pos_y;
	float direction;

} MinimalPlayerInfo;


static_assert((sizeof(MinimalPlayerInfo) == 12), "MinimalPlayerInfo is the wrong size, add padding");

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
