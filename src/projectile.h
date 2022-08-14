#ifndef PROJECTILE_H
#define PROJECTILE_H
#include <stdint.h>
#include "include/raylib.h"
#include "player.h"
#include "weapon.h"

typedef enum ProjectileType {
	StandardBullet,
	GrenadeProj,

} ProjectileType;

typedef struct Projectile {
	float pos_x;
	float pos_y;

	float angle;
	float speed;

	uint8_t size;
	uint16_t damage;

	uint16_t num_frames_existed; 

	ProjectileType projectile_type;

} Projectile;

void update_projectiles(Projectile** projectiles, uint16_t* num_projectiles, Player* players, uint8_t num_players, const Map* map);
void use_weapons(Player* players, uint8_t num_players, Projectile** projectiles, uint16_t* num_projectiles);
#endif
