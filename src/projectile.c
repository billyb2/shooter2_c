#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <math.h>

#include "game_mode.h"
#include "rand.h"
#include "map.h"
#include "player.h"
#include "projectile.h"
#include "math.h"
#include "weapon.h"

extern int SCREEN_WIDTH;
extern int SCREEN_HEIGHT;

void add_kill(Player* players, uint8_t num_players, Player* player_who_died) {
	Player* killer = NULL;

	for (uint8_t j = 0; j < num_players; j += 1) {
		Player* potential_killer = &players[j];

		if (potential_killer->id == player_who_died->last_hurt_by) {
			killer = potential_killer;
			break;

		}

	}

	if (killer == NULL) {
		fprintf(stderr, "Couldn't find killer player\n");
		exit(-1);

	}

	if (killer == player_who_died) {
		return;

	}

	killer->num_kills += 1;
	killer->kills = realloc(killer->kills, killer->num_kills * sizeof(PlayerKill));

	killer->kills[killer->num_kills - 1].player_id_killed = player_who_died->id;

	printf("Player %s has %d kills\n", killer->username, killer->num_kills);

}

void damage_player(Player* players, uint8_t num_players, Player* player_who_took_damage, uint64_t attacking_player_id, uint16_t damage) {
	if (player_who_took_damage->health == 0 || player_who_took_damage->is_net_player || !player_who_took_damage->assigned_id || damage == 0) {
		return;

	}

	player_who_took_damage->last_hurt_by = attacking_player_id;
	player_who_took_damage->health = saturating_sub(player_who_took_damage->health, damage);

	if (player_who_took_damage->health == 0) {
		add_kill(players, num_players, player_who_took_damage);

	}

}

AABB projectile_to_aabb(const Projectile* projectile) {
	AABB aabb = {
		.top_edge = projectile->pos_y,
		.left_edge = projectile->pos_x,
		.right_edge = projectile->pos_x + projectile->size,
		.bottom_edge = projectile->pos_y + projectile->size,

	};

	return aabb;

}

// A cheap and lazy way of doing this, but it's simple
void update_projectiles(Projectile** projectiles, uint16_t* num_projectiles, Player* players, uint8_t num_players, const Map* map, GameModeData* game_mode_data) {
	Projectile* buff_projectile_list = malloc(*num_projectiles * sizeof(Projectile));
	uint16_t new_num_projectiles = 0;
	bool team_killing_allowed = team_kill(game_mode_data->rt);

	for (uint16_t i = 0; i < *num_projectiles; i += 1) {
		Projectile* projectile = &(*projectiles)[i];
		projectile->num_frames_existed += 1;

		bool projectile_should_be_copied = true;

		AABB projectile_aabb = projectile_to_aabb(projectile);

		switch (projectile->projectile_type) {
			case StandardBullet: {
				projectile->pos_x += projectile->speed * cosf(projectile->angle);
				projectile->pos_y += projectile->speed * sinf(projectile->angle);

				bool collided_with_map = map_collision(projectile_aabb, map);
				bool player_collision = false;

				if (!collided_with_map) {
					for(uint8_t i = 0; i < num_players; i += 1) {
						Player* player = &players[i];
						AABB player_aabb = player_to_aabb(player);

						if (player->health == 0 || !player->assigned_id) {
							continue;

						}


						if ( (projectile->shot_by_team != player->team_id) || team_killing_allowed) {
							player_collision = aabb_collision(projectile_aabb, player_aabb);

							if (player_collision) {
								damage_player(players, num_players, player, projectile->shot_by, projectile->damage);
								break;

							}

						}

					}

				}

				projectile_should_be_copied = !player_collision && !collided_with_map;

				break;
			}

			case SniperBullet: {
				bool player_collision = false;
				bool collided_with_map = map_collision_w_movement(projectile_aabb, projectile->speed, projectile->angle, map);

				if (!collided_with_map) {
					for (uint8_t i = 0; i < num_players; i += 1) {
						Player* player = &players[i];
						AABB player_aabb = player_to_aabb(player);

						if (player->health == 0) {
							continue;

						}

						if ( (projectile->shot_by_team != player->team_id) || team_killing_allowed) {
							player_collision = aabb_collision_w_movement(projectile_aabb, player_aabb, projectile->speed, projectile->angle);

							if (player_collision) {
								damage_player(players, num_players, player, projectile->shot_by, projectile->damage);

								// No break statement so snipers can pierce players

							}

						}

					}

				}

				projectile_should_be_copied = !player_collision && !collided_with_map;

				if (projectile_should_be_copied) {
					projectile->pos_x += projectile->speed * cosf(projectile->angle);
					projectile->pos_y += projectile->speed * sinf(projectile->angle);

				}

				break;

			}

			case GrenadeProj: {
				projectile->pos_x += projectile->speed * cosf(projectile->angle);
				projectile->pos_y += projectile->speed * sinf(projectile->angle);

				// 2.5 seconds
				#define FRAMES_TILL_EXPLOSION 3 * 30
				projectile->speed = saturating_sub(projectile->speed, 0.15);

				for (uint8_t i = 0; i < num_players; i += 1) {
					Player* player = &players[i];

					bool should_explode = projectile->num_frames_existed >= FRAMES_TILL_EXPLOSION;

					bool collided_with_map = map_collision(projectile_aabb, map);


					if (should_explode) {

						if ( (projectile->shot_by_team != player->team_id) || team_killing_allowed) {
							float distance_to_player = distance(player->pos_x, player->pos_y, projectile->pos_x, projectile->pos_y);
							
							if (distance_to_player >= MAX_GRENADE_DAMAGE_DISTANCE) {
								distance_to_player = MAX_GRENADE_DAMAGE_DISTANCE;

							}

							float damage_ratio = (1.0 - (distance_to_player / MAX_GRENADE_DAMAGE_DISTANCE)) * 1.8;
							float damage = projectile->damage * damage_ratio;
							damage_player(players, num_players, player, projectile->shot_by, damage);

						}
						

					} else if (collided_with_map) {
						projectile->angle += F32_PI / 2.0;

					}

					projectile_should_be_copied = !should_explode;

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

Projectile new_projectile(float pos_x, float pos_y, float angle, ProjectileType projectile_type, float speed, uint16_t damage, uint64_t shot_by, uint64_t shot_by_team) {
	uint8_t size;

	switch (projectile_type) {
		case StandardBullet:
			size = 5;
			break;

		case GrenadeProj:
			size = 7;
			break;

		case SniperBullet:
			size = 5;
			break;

	};

	pos_x += cosf(angle) * PLAYER_SIZE * 1.5;
	pos_y += sinf(angle) * PLAYER_SIZE * 1.5;

	Projectile new_projectile = {
		.pos_x = pos_x,
		.pos_y = pos_y,
		.angle = angle,
		.speed = speed,
		.projectile_type = projectile_type,
		.size = size,
		.damage = damage,
		.num_frames_existed = 0,
		.shot_by = shot_by,
		.shot_by_team = shot_by_team,

	};

	return new_projectile;

}

void shoot(Projectile ** projectiles, uint16_t* num_projectiles, Player* player, float angle, WeaponIndex weapon_index, float throw_distance) { 
	if (player->health == 0 || player->weapon_switch_cooldown > 0) {
		return;

	}

	// Use primary weapon
	if (weapon_index == Primary)  {
		// Only perform specific checks on net players
		if (player->is_net_player) {
			if (player->remaining_shooting_cooldown_frames > 0 || player->ammo == 0) {
				return;

			}
		} else {
			if (player->remaining_shooting_cooldown_frames > 0 || player->reloading) {
				return;

			}

			if (player->ammo == 0) {
				reload(player);
				return;

			}
		}

		player->ammo -= 1;

		float proj_speed = get_projectile_speed(player->weapon);
		float proj_damage = get_projectile_damage(player->weapon);
		uint16_t cooldown_frames = get_cooldown_frames(player->weapon);

		switch (player->weapon) {
			case AssaultRifle:
				*num_projectiles += 1;
				*projectiles = realloc(*projectiles, *num_projectiles * sizeof(Projectile));

				(*projectiles)[*num_projectiles - 1] = new_projectile(player->pos_x, player->pos_y, angle, StandardBullet, proj_speed, proj_damage, player->id, player->team_id);

				player->remaining_shooting_cooldown_frames = cooldown_frames;

				break;
			
			case Pistol:
				*num_projectiles += 1;
				*projectiles = realloc(*projectiles, *num_projectiles * sizeof(Projectile));

				(*projectiles)[*num_projectiles - 1] = new_projectile(player->pos_x, player->pos_y, angle, StandardBullet, proj_speed, proj_damage, player->id, player->team_id);

				player->remaining_shooting_cooldown_frames = cooldown_frames;

				break;

			case Shotgun:
				#define NUM_SHOTGUN_PROJECTILES 8
				#define RECOIL_ANGLE_AMT 0.070

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

					*projectile = new_projectile(player->pos_x, player->pos_y, recoil_angle, StandardBullet, proj_speed, proj_damage, player->id, player->team_id);

				}

				player->remaining_shooting_cooldown_frames = cooldown_frames;
				break;

			case Sniper:
				*num_projectiles += 1;
				*projectiles = realloc(*projectiles, *num_projectiles* sizeof(Projectile));

				(*projectiles)[*num_projectiles - 1] = new_projectile(player->pos_x, player->pos_y, angle, SniperBullet, proj_speed, proj_damage, player->id, player->team_id);

				player->remaining_shooting_cooldown_frames = cooldown_frames;

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

				(*projectiles)[*num_projectiles - 1] = new_projectile(player->pos_x, player->pos_y, angle, GrenadeProj, 15.0 * throw_distance, 450, player->id, player->team_id);

				player->remaining_throwable_cooldown_frames = 80;

				break;
			};

		};

	}

}


void use_weapons(Player* players, uint8_t num_players, Projectile** projectiles, uint16_t* num_projectiles) {
	for (uint8_t i = 0; i < num_players; i += 1) { 
		Player* player = &players[i];

		if (player->shooting) {
			shoot(projectiles, num_projectiles, player, player->direction, player->equipped_weapon, player->throw_ratio);

		}

	}

}
