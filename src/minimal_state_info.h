#ifndef MINIMAL_PLAYER_INFO_H
#define MINIMAL_PLAYER_INFO_H
#include <stdint.h>
#include <stdbool.h>
#ifndef WASM
#include <assert.h>
#endif

#define PLAYER_SIZE 26

#include "weapon.h"
#include "player_ability.h"

typedef struct  __attribute__((__packed__)) MinimalPlayerInfo {
	uint64_t id;

	uint64_t team_id;

	uint16_t health;
	bool cloaking;
	uint8_t ammo;
	uint16_t using_ability;
	uint16_t throwing_grenade;

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
	uint32_t color;
	uint8_t num_players;

} MinimalTeamInfo;

typedef struct __attribute__((__packed__)) MinimalMapObject {
	float pos_x;
	float pos_y;

	float width;
	float height;

} MinimalMapObject;

typedef struct __attribute__((__packed__)) MinimalMapInfo {
	float width;
	float height;
	uint64_t num_spawn_points;
	MinimalMapObject spawn_points[256];	

} MinimalMapInfo;

#ifndef WASM
static_assert((sizeof(MinimalPlayerInfo) % 8 == 0), "MinimalPlayerInfo is the wrong size, add padding");
static_assert((sizeof(MinimalMapInfo) % 8 == 0), "MinimalMapInfo is the wrong size, add padding");
static_assert((sizeof(MinimalMapObject) % 8 == 0), "MinimalMapObject is the wrong size, add padding");
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

#ifndef WASM
static_assert((sizeof(PlayerMovementInfo) == 8), "Player movement info is wrong size");
#endif

#define N 0
#define NE 1
#define E 2
#define SE 3
#define S 4
#define SW 5
#define W 6 
#define NW 7
#define NO_MOVEMENT 8

#endif
