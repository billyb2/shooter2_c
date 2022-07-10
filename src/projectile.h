#ifndef PROJECTILE_H
#define PROJECTILE_H
#include <stdint.h>
#include "include/raylib.h"
#include "player.h"

typedef enum ProjectileType {
	StandardBullet,

} ProjectileType;

typedef struct Projectile {
	uint16_t pos_x;
	uint16_t pos_y;

	int16_t speed_x;
	int16_t speed_y;

	ProjectileType projectile_type;

} Projectile;

void update_projectiles(Projectile** projectiles, uint16_t* num_projectiles);
Projectile new_projectile(uint16_t pos_x, uint16_t pos_y, float angle, ProjectileType projectile_type);
void shoot(Projectile ** projectiles, uint16_t* num_projectiles, const Player* player, float angle);
#endif
