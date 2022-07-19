#ifndef MINIMAL_PLAYER_INFO_H
#define MINIMAL_PLAYER_INFO_H
#include <stdint.h>

typedef struct MinimalPlayerInfo {
	uint16_t pos_x;
	uint16_t pos_y;
	float direction;

} MinimalPlayerInfo;

typedef struct MinimalProjectileInfo {
	uint16_t pos_x;
	uint16_t pos_y;

} MinimalProjectileInfo;
#endif