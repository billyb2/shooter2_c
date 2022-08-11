#ifndef MINIMAL_PLAYER_INFO_H
#define MINIMAL_PLAYER_INFO_H
#include <stdint.h>

typedef struct MinimalPlayerInfo {
	float pos_x;
	float pos_y;
	float direction;

} MinimalPlayerInfo;

typedef struct MinimalProjectileInfo {
	float pos_x;
	float pos_y;
	float angle;	
	float speed;	
	uint16_t damage;
	uint16_t size;

} MinimalProjectileInfo;

#endif
