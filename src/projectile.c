#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <math.h>

#include "include/raylib.h"
#include "player.h"
#include "projectile.h"
#include "math.h"

extern int SCREEN_WIDTH;
extern int SCREEN_HEIGHT;

// A cheap and lazy way of doing this, but it's simple
void update_projectiles(Projectile** projectiles, uint16_t* num_projectiles, Player* players, uint8_t num_players) {
	Projectile* buff_projectile_list = malloc(*num_projectiles * sizeof(Projectile));
	uint16_t new_num_projectiles = 0;

	for (uint16_t i = 0; i < *num_projectiles; i += 1) {
		Projectile* projectile = &(*projectiles)[i];

		bool projectile_should_be_copied = true;

		if (projectile->projectile_type == StandardBullet) {
			projectile->pos_x = (int16_t)projectile->pos_x + projectile->speed_x;
			projectile->pos_y = (int16_t)projectile->pos_y + projectile->speed_y;

			if (projectile->pos_x > SCREEN_WIDTH || projectile->pos_x < 0 || projectile->pos_y < 0 || projectile->pos_y > SCREEN_HEIGHT) {
				projectile_should_be_copied = false;
				
			} else if (projectile_should_be_copied) {
				for(uint8_t i = 0; i < num_players; i += 1) {
					Player* player = &players[i];

					if (player->health == 0) {
						continue;

					}

					uint16_t half_proj_size = (uint16_t)(projectile->size) / 2;

					bool collision = aabb_collision(player->pos_x - PLAYER_SIZE / 2, player->pos_y - PLAYER_SIZE / 2, PLAYER_SIZE, projectile->pos_x - half_proj_size, projectile->pos_y - half_proj_size, projectile->size);

					if (collision) {
						player->health = saturating_sub(player->health, 50);

					}

					projectile_should_be_copied = !collision;

					if (!projectile_should_be_copied) {
						break;

					}

				}

			}

		}

		if (projectile_should_be_copied) {
			buff_projectile_list[new_num_projectiles] = *projectile;
			new_num_projectiles += 1;
			
			
		}

	}

	free(*projectiles);

	*projectiles = buff_projectile_list;
	*num_projectiles = new_num_projectiles;

}

int16_t get_projectile_speed(ProjectileType projectile_type) {
	int16_t speed = 0;

	if (projectile_type == StandardBullet) {
		speed = 8;

	}

	return speed;

}

Projectile new_projectile(uint16_t pos_x, uint16_t pos_y, float angle, ProjectileType projectile_type) {
	int16_t speed = get_projectile_speed(projectile_type);

	Projectile new_projectile = {
		.pos_x = pos_x,
		.pos_y = pos_y,
		.projectile_type = StandardBullet,
		.speed_x = (int16_t)(cosf(angle) * (float)speed),
		.speed_y = (int16_t)(sinf(angle) * (float)speed),
		.size = 2,

	};

	return new_projectile;

}

void shoot(Projectile ** projectiles, uint16_t* num_projectiles, const Player* player, float angle) { 
	if (player->health > 0) {
		*num_projectiles += 1;
		*projectiles = realloc(*projectiles, *num_projectiles * sizeof(Projectile));

		(*projectiles)[*num_projectiles - 1] = new_projectile(player->pos_x, player->pos_y, angle, StandardBullet);

	}

}
