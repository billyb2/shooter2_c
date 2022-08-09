#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <math.h>

#include "include/raylib.h"
#include "player.h"
#include "projectile.h"
#include "math.h"
#include "weapon.h"

// A cheap and lazy way of doing this, but it's simple
void update_projectiles(Projectile** projectiles, uint16_t* num_projectiles, Player* players, uint8_t num_players, const Map* map) {
	Projectile* buff_projectile_list = malloc(*num_projectiles * sizeof(Projectile));
	uint16_t new_num_projectiles = 0;

	for (uint16_t i = 0; i < *num_projectiles; i += 1) {
		Projectile* projectile = &(*projectiles)[i];

		bool projectile_should_be_copied = true;

		if (projectile->projectile_type == StandardBullet) {
			float speed_x = cosf(projectile->angle) * projectile->speed;
			float speed_y = sinf(projectile->angle) * projectile->speed;

			projectile->pos_x = projectile->pos_x + speed_x;
			projectile->pos_y = projectile->pos_y + speed_y;

			if (projectile->pos_x > map->size_x || projectile->pos_x < 0 || projectile->pos_y < 0 || projectile->pos_y > map->size_y) {
				projectile_should_be_copied = false;
				
			} else if (projectile_should_be_copied) {
				for(uint8_t i = 0; i < num_players; i += 1) {
					Player* player = &players[i];

					if (player->health == 0) {
						continue;

					}

					uint16_t half_proj_size = (uint16_t)(projectile->size) / 2;

					bool collision = aabb_collision(player->pos_x - PLAYER_SIZE / 2.0, player->pos_y - PLAYER_SIZE / 2.0, PLAYER_SIZE, projectile->pos_x - half_proj_size, projectile->pos_y - half_proj_size, projectile->size);

					if (collision) {
						player->health = saturating_sub(player->health, projectile->damage);

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

Projectile new_projectile(uint16_t pos_x, uint16_t pos_y, float angle, ProjectileType projectile_type, float speed, uint16_t damage) {
	float speed_x = cosf(angle) * speed;
	float speed_y = sinf(angle) * speed;

	Projectile new_projectile = {
		.pos_x = pos_x + speed_x,
		.pos_y = pos_y + speed_y,
		.speed = speed,
		.angle = angle,
		.projectile_type = projectile_type,
		.size = 2,
		.damage = damage,

	};

	return new_projectile;

}

void shoot(Projectile ** projectiles, uint16_t* num_projectiles, Player* player, float angle) { 
	if (player->health == 0) {
		return;

	}

	if (player->remaining_shooting_cooldown_frames > 0) {
		return;

	}

	switch (player->weapon) {
		case AssaultRifle:
			#define DAMAGE 30
			#define RECOIL 0.045

			*num_projectiles += 1;
			*projectiles = realloc(*projectiles, *num_projectiles * sizeof(Projectile));

			float recoil = RECOIL * ((float)rand() / (float)RAND_MAX);
			
			if (rand() > RAND_MAX / 2) {
				recoil = -recoil;
				
			}

			angle += recoil;

			(*projectiles)[*num_projectiles - 1] = new_projectile(player->pos_x, player->pos_y, angle, StandardBullet, 14.0, DAMAGE);

			player->remaining_shooting_cooldown_frames = 3;

			break;
		
		case Pistol:
			*num_projectiles += 1;
			*projectiles = realloc(*projectiles, *num_projectiles * sizeof(Projectile));

			(*projectiles)[*num_projectiles - 1] = new_projectile(player->pos_x, player->pos_y, angle, StandardBullet, 10.0, 200);

			player->remaining_shooting_cooldown_frames = 5;

			break;

		case Shotgun:
			#define NUM_SHOTGUN_PROJECTILES 9
			#define PROJECTILE_SPEED 16.0
			#define RECOIL_ANGLE_AMT 0.025

			*num_projectiles += NUM_SHOTGUN_PROJECTILES;
			*projectiles = realloc(*projectiles, *num_projectiles * sizeof(Projectile));

			float recoil_angle = angle - (RECOIL_ANGLE_AMT * ((float)NUM_SHOTGUN_PROJECTILES / 2.0));
			float rand_jitter = ((float)RECOIL_ANGLE_AMT) * ((float)rand() / (float)RAND_MAX);

			for (uint8_t i = *num_projectiles - NUM_SHOTGUN_PROJECTILES; i < *num_projectiles; i += 1) {
				Projectile* projectile = &(*projectiles)[i];

				if (rand() > RAND_MAX / 2) {
					recoil_angle += RECOIL_ANGLE_AMT + rand_jitter;

				} else {
					recoil_angle += RECOIL_ANGLE_AMT - rand_jitter;

				}

				*projectile = new_projectile(player->pos_x + ceilf(cosf(recoil_angle) * PROJECTILE_SPEED), player->pos_y + ceilf(sinf(recoil_angle) * PROJECTILE_SPEED), recoil_angle, StandardBullet, PROJECTILE_SPEED, 110);

			}

			player->remaining_shooting_cooldown_frames = 45;


		case None:
			break;

	};

}

ProjectileType get_projectile_for_weapon(Weapon weapon) {
	ProjectileType projectile_type = StandardBullet;

	switch(weapon) {
		case AssaultRifle:
			projectile_type = StandardBullet;
			break;

		case Pistol:
			projectile_type = StandardBullet;
			break;

		case Shotgun:
			projectile_type = StandardBullet;
			break;

		case None:
			projectile_type = Melee;
			break;
	};

	return projectile_type;

}
