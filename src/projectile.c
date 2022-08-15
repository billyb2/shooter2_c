#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <math.h>

#include "include/raylib.h"
#include "rand.h"
#include "map.h"
#include "player.h"
#include "projectile.h"
#include "math.h"
#include "weapon.h"

extern int SCREEN_WIDTH;
extern int SCREEN_HEIGHT;

// A cheap and lazy way of doing this, but it's simple
void update_projectiles(Projectile** projectiles, uint16_t* num_projectiles, Player* players, uint8_t num_players, const Map* map) {
	Projectile* buff_projectile_list = malloc(*num_projectiles * sizeof(Projectile));
	uint16_t new_num_projectiles = 0;

	for (uint16_t i = 0; i < *num_projectiles; i += 1) {
		Projectile* projectile = &(*projectiles)[i];
		projectile->num_frames_existed += 1;

		bool projectile_should_be_copied = true;

		projectile->pos_x += projectile->speed * cosf(projectile->angle);
		projectile->pos_y += projectile->speed * sinf(projectile->angle);

		switch (projectile->projectile_type) {
			case StandardBullet: {
				if (projectile->pos_x > map->size_x || projectile->pos_x < 0.0 || projectile->pos_y < 0.0 || projectile->pos_y > map->size_y) {
					projectile_should_be_copied = false;
					
				} else if (projectile_should_be_copied) {
					for(uint8_t i = 0; i < num_players; i += 1) {
						Player* player = &players[i];

						if (player->health == 0) {
							continue;

						}

						float half_proj_size = projectile->size / 2.0;

						bool player_collision = aabb_collision(player->pos_x - PLAYER_SIZE / 2.0, player->pos_y - PLAYER_SIZE / 2.0, PLAYER_SIZE, PLAYER_SIZE, projectile->pos_x - half_proj_size, projectile->pos_y - half_proj_size, projectile->size, projectile->size);
						bool collided_with_map = map_collision(projectile->pos_x - half_proj_size, projectile->pos_y - half_proj_size, projectile->size, projectile->size, map);

						if (player_collision) {
							player->health = saturating_sub(player->health, projectile->damage);

						}

						projectile_should_be_copied = !player_collision && !collided_with_map;

					}

				}

				break;
			}

			case GrenadeProj: {
				// 2.5 seconds
				#define FRAMES_TILL_EXPLOSION 3 * 30
				projectile->speed = saturating_sub(projectile->speed, 0.15);

				for (uint8_t i = 0; i < num_players; i += 1) {
					Player* player = &players[i];

					bool should_explode = projectile->num_frames_existed >= FRAMES_TILL_EXPLOSION;

					float half_proj_size = projectile->size / 2.0;
					bool collided_with_map = map_collision(projectile->pos_x - half_proj_size, projectile->pos_y - half_proj_size, projectile->size, projectile->size, map);

					if (should_explode) {
						#define MAX_GRENADE_DAMAGE_DISTANCE 200.0

						float distance_to_player = distance(player->pos_x, player->pos_y, projectile->pos_x, projectile->pos_y);
						
						if (distance_to_player >= MAX_GRENADE_DAMAGE_DISTANCE) {
							distance_to_player = MAX_GRENADE_DAMAGE_DISTANCE;

						}

						float damage_ratio = powf(1.0 - (distance_to_player / MAX_GRENADE_DAMAGE_DISTANCE), 1.2);
						player->health = saturating_sub(player->health, projectile->damage * damage_ratio);

					}

					projectile_should_be_copied = !collided_with_map && !should_explode;

				}

				break;

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

Projectile new_projectile(float pos_x, float pos_y, float angle, ProjectileType projectile_type, float speed, uint16_t damage) {
	uint8_t size;

	switch (projectile_type) {
		case StandardBullet:
			size = 2;
			break;

		case GrenadeProj:
			size = 5;
			break;

	};

	Projectile new_projectile = {
		.pos_x = pos_x,
		.pos_y = pos_y,
		.angle = angle,
		.speed = speed,
		.projectile_type = projectile_type,
		.size = size,
		.damage = damage,
		.num_frames_existed = 0,

	};

	return new_projectile;

}

void shoot(Projectile ** projectiles, uint16_t* num_projectiles, Player* player, float angle, WeaponIndex weapon_index, float throw_distance) { 
	if (player->health == 0) {
		return;

	}

	// Use primary weapon
	if (weapon_index == Primary)  {
		if (player->remaining_shooting_cooldown_frames > 0) {
			return;

		}

		switch (player->weapon) {
			case AssaultRifle:
				*num_projectiles += 1;
				*projectiles = realloc(*projectiles, *num_projectiles * sizeof(Projectile));

				(*projectiles)[*num_projectiles - 1] = new_projectile(player->pos_x, player->pos_y, angle, StandardBullet, 8.0, 60);

				player->remaining_shooting_cooldown_frames = 5;

				break;
			
			case Pistol:
				*num_projectiles += 1;
				*projectiles = realloc(*projectiles, *num_projectiles * sizeof(Projectile));

				(*projectiles)[*num_projectiles - 1] = new_projectile(player->pos_x, player->pos_y, angle, StandardBullet, 10.0, 200);

				player->remaining_shooting_cooldown_frames = 5;

				break;

			case Shotgun:
				#define NUM_SHOTGUN_PROJECTILES 8
				#define RECOIL_ANGLE_AMT 0.085

				*num_projectiles += NUM_SHOTGUN_PROJECTILES;
				*projectiles = realloc(*projectiles, *num_projectiles * sizeof(Projectile));

				float recoil_angle = angle - (RECOIL_ANGLE_AMT * ((float)NUM_SHOTGUN_PROJECTILES / 2.0));
				float rand_jitter = ((float)RECOIL_ANGLE_AMT) * ((float)fast_rand() / (float)FAST_RAND_MAX);

				for (uint16_t i = *num_projectiles - NUM_SHOTGUN_PROJECTILES; i < *num_projectiles; i += 1) {
					Projectile* projectile = &(*projectiles)[i];

					if (fast_rand() > FAST_RAND_MAX / 2) {
						recoil_angle += RECOIL_ANGLE_AMT + rand_jitter;

					} else {
						recoil_angle += RECOIL_ANGLE_AMT - rand_jitter;

					}

					*projectile = new_projectile(player->pos_x, player->pos_y, recoil_angle, StandardBullet, 11.0, 110);

				}

				player->remaining_shooting_cooldown_frames = 45;
				break;


			case None:
				break;

		};

	// Using throwable
	} else if (weapon_index == Tertiary) {
		if (player->remaining_throwable_cooldown_frames > 0) {
			return;

		}

		if (player->num_throwables == 0) {
			return;

		}

		player->num_throwables -= 1;

		switch (player->throwable) {
			case Grenade: {
				*num_projectiles += 1;
				*projectiles = realloc(*projectiles, *num_projectiles * sizeof(Projectile));

				(*projectiles)[*num_projectiles - 1] = new_projectile(player->pos_x, player->pos_y, angle, GrenadeProj, 15.0 * throw_distance, 450);

				player->remaining_throwable_cooldown_frames = 80;

				break;
			};

		};

	}

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
			//projectile_type = Melee;
			break;
	};

	return projectile_type;

}

void use_weapons(Player* players, uint8_t num_players, Projectile** projectiles, uint16_t* num_projectiles) {
	for (uint8_t i = 0; i < num_players; i += 1) { 
		Player* player = &players[i];

		if (player->shooting) {
			switch (player->equipped_weapon) {
				case Primary: 
					shoot(projectiles, num_projectiles, player, player->direction, Primary, 0.0);
					break;

				case Tertiary: {
					shoot(projectiles, num_projectiles, player, player->direction, Tertiary, player->throw_ratio);
					break;

				}


			}

		}

	}
}
