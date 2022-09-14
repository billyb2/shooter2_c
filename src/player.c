#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <time.h>
#include <math.h>

#include "minimal_state_info.h"
#include "map.h"
#include "math.h"
#include "player.h"
#include "player_ability.h"
#include "projectile.h"
#include "rand.h"
#include "weapon.h"

uint16_t get_max_ability_charge(Ability ability) {
	uint16_t max_ability_charge = 0;

	switch (ability) {
		case Warp:
			// 6 seconds to recharge
			max_ability_charge = 6 * 60;
			break;

		case Stim:
			// 3 seconds to recharge
			max_ability_charge = 3 * 60;
			break;

		case Cloak:
			// 8 seconds to recharge
			max_ability_charge = 8 * 60;
			break;
	};

	return max_ability_charge;

}

uint16_t get_max_ability_use_frames(Ability ability) {
	uint16_t max_frames_in_use = 0;

	switch (ability) {
		case Warp:
			// Use is a quarter of a second
			max_frames_in_use = 15;
			break;

		case Stim:
			// 3 seconds of use
			max_frames_in_use = 180;
			break;

		case Cloak:
			// 3 seconds of use
			max_frames_in_use = 300;
			break;
	};

	return max_frames_in_use;

}

Player new_player(Ability ability, Weapon weapon, Throwable throwable, const Map* map, char* player_name) {
	uint8_t num_throwables;

	switch (throwable) {
		case Grenade:
			num_throwables = 3;
			break;
	};

	uint16_t num_spawn_points = 0;
	const MapObject** spawn_points = malloc(sizeof(MapObject*) * map->num_objects);

	for (uint16_t i = 0; i < map->num_objects; i += 1) {
		MapObject* map_obj = &map->objects[i];
		
		if (map_obj->spawn_point) {
			spawn_points[num_spawn_points] = map_obj;
			num_spawn_points += 1;
		}

	}

	if (num_spawn_points == 0) {
		fprintf(stderr, "Failed to find any spawn points\n");
		exit(-1);

	}

	const MapObject* spawn_point = spawn_points[rand_range_u64(0, num_spawn_points)];

	Player player = {
		.id = rand(),
		.team_id = 0,
		.username = player_name,
		.assigned_team_id = false,
		.assigned_id = false,
		.pos_x = fmaf(spawn_point->size_x, 0.5, spawn_point->pos_x),
		.pos_y = fmaf(spawn_point->size_y, 0.5, spawn_point->pos_y),
		.speed = DEFAULT_PLAYER_SPEED,
		.ability = ability,
		.weapon = weapon,
		.num_throwables = num_throwables,
		.throwable = throwable,
		.direction = 0.0,
		.health = 0,
		.ability_charge = get_max_ability_charge(ability),
		.num_frames_ability_in_use = 0,
		.remaining_shooting_cooldown_frames = 0,
		.num_frames_dead = 0,
		.using_ability = false,
		.shooting = false,
		.equipped_weapon = Primary,
		.throw_ratio = 0.5,
		.reloading = false,
		.ammo = get_ammo_count(weapon),
		.remaining_reload_frames = 0,
		.is_net_player = false,
		.kills = NULL,
		.num_kills = 0,
		.cloaking = false,
		.weapon_switch_cooldown = 0,

	};

	free(spawn_points);

	return player;

}

MinimalPlayerInfo get_minimal_player_info(const Player* player) {
	MinimalPlayerInfo minimal_player = {
		.id = player->id,
		.team_id = player->team_id,
		.pos_x = player->pos_x,
		.pos_y = player->pos_y,
		.health = player->health,
		.ability = player->ability,
		.weapon = player->weapon,
		.direction = player->direction,
		.ammo = player->ammo,
		.cloaking = player->cloaking,
	};

	return minimal_player;

}

void use_ability(Player* player, const Map* map) {
	if (player->health == 0 || player->using_ability) {
		return;

	}


	switch (player->ability) {
		case Warp: {
			#define TELEPORATION_SPEED 350.0

			uint16_t half_max_ability_charge = get_max_ability_charge(player->ability) / 2;

			if (player->ability_charge < half_max_ability_charge) {
				return;

			}

			float potential_x = player->pos_x + cosf(player->direction) * TELEPORATION_SPEED;
			float potential_y = player->pos_y + sinf(player->direction) * TELEPORATION_SPEED;

			if (!map_collision(potential_x, potential_y, PLAYER_SIZE, PLAYER_SIZE, map)) {
				player->pos_x = potential_x;
				player->pos_y = potential_y;

				player->using_ability = true;
				player->ability_charge = saturating_sub(player->ability_charge, half_max_ability_charge);

			}

			break;
		}

		case Stim: {
			if (player->ability_charge < get_max_ability_charge(player->ability)) {
				return;

			}

			player->speed *= 1.75;

			player->using_ability = true;
			player->ability_charge = 0;

			break;
		}

		case Cloak: {
			if (player->ability_charge < get_max_ability_charge(player->ability)) {
				return;

			}

			player->cloaking = true;
			player->using_ability = true;
			player->ability_charge = 0;


			break;
		}


	};

}

void reload(Player* player) {
	if (player->reloading || player->ammo >= get_ammo_count(player->weapon) || player->weapon_switch_cooldown > 0) {
		return;

	}

	uint16_t reload_frames = get_num_reload_frames(player->weapon);	

	// If your mag isn't empty, decrease reload time
	if (player->ammo > 0) {
		reload_frames -= reload_frames / 4;

	}

	player->remaining_reload_frames = reload_frames;
	player->reloading = true;

}

void move_player(Player* player, PlayerMovementInfo movement_info, const Map* map) {
	if (player->health == 0) {
		return;

	}

	if (movement_info.x_axis == Left) {
		float potential_x = saturating_sub(player->pos_x, player->speed);

		if (!map_collision(potential_x, player->pos_y, PLAYER_SIZE, PLAYER_SIZE, map)) {
			player->pos_x = potential_x;

		}

	}

	if (movement_info.x_axis == Right) {
		float potential_x = player->pos_x + player->speed;

		if (!map_collision(potential_x, player->pos_y, PLAYER_SIZE, PLAYER_SIZE, map)) {
			player->pos_x = potential_x;

		}
	
	}

	if (movement_info.y_axis == Up) {
		float potential_y = saturating_sub(player->pos_y, player->speed);

		if (!map_collision(player->pos_x, potential_y, PLAYER_SIZE, PLAYER_SIZE, map)) {
			player->pos_y = potential_y;

		}

	}

	if (movement_info.y_axis == Down) {
		float potential_y = player->pos_y + player->speed;

		if (!map_collision(player->pos_x, potential_y, PLAYER_SIZE, PLAYER_SIZE, map)) {
			player->pos_y = potential_y;

		}

	}

}

void update_player_cooldowns(Player* players, uint8_t num_players) {
	for (uint8_t i = 0; i < num_players; i += 1) {
		Player* player = &players[i];

		if (player->ability_charge < get_max_ability_charge(player->ability) && !player->using_ability) {
			player->ability_charge += 1;

		}

		player->remaining_shooting_cooldown_frames = saturating_sub(player->remaining_shooting_cooldown_frames, 1);
		player->remaining_throwable_cooldown_frames = saturating_sub(player->remaining_throwable_cooldown_frames, 1);

		if (player->reloading) {
			player->remaining_reload_frames = saturating_sub(player->remaining_reload_frames, 1);

		}

		if (player->using_ability) {
			player->num_frames_ability_in_use += 1;

			if (player->num_frames_ability_in_use >= get_max_ability_use_frames(player->ability)) {
				player->using_ability = false;
				player->num_frames_ability_in_use = 0;

				if (player->ability == Stim) {
					player->speed /= 1.75;

				} else if (player->ability == Cloak) {
					player->cloaking = false;

				}

			}

		}

		if (player->reloading && player->remaining_reload_frames == 0) {
			player->ammo = get_ammo_count(player->weapon);
			player->reloading = false;

		}

		if (player->weapon_switch_cooldown > 0) {
			player->weapon_switch_cooldown = saturating_sub(player->weapon_switch_cooldown, 1);

			// Swap weapons
			if (player->weapon_switch_cooldown == 0) {
				if (player->weapon == NUM_WEAPONS - 1) {
					player->weapon = 0;

				} else {
					player->weapon += 1;

				}

				player->ammo = 0;

			}

		}

	}

}

void respawn_players(Player* players, uint8_t num_players, const Map* map) {
	for (uint8_t i = 0; i < num_players; i += 1) {
		Player* player = &players[i];

		if (player->is_net_player) {
			continue;

		}

		if (player->health == 0 && !player->is_net_player && player->assigned_id) {
			player->num_frames_dead += 1;

			if (player->num_frames_dead >= 180) {
				player->health = PLAYER_MAX_HEALTH;

				uint16_t num_spawn_points = 0;
				const MapObject** spawn_points = malloc(sizeof(MapObject*) * map->num_objects);

				for (uint16_t i = 0; i < map->num_objects; i += 1) {
					MapObject* map_obj = &map->objects[i];
					
					if (map_obj->spawn_point) {
						spawn_points[num_spawn_points] = map_obj;
						num_spawn_points += 1;
					}

				}

				if (num_spawn_points == 0) {
					fprintf(stderr, "Failed to find any spawn points\n");
					exit(-1);

				}

				const MapObject* spawn_point = spawn_points[rand_range_u64(0, num_spawn_points)];

				player->pos_x = fmaf(spawn_point->size_x, 0.5, spawn_point->pos_x);
				player->pos_y = fmaf(spawn_point->size_y, 0.5, spawn_point->pos_y);

				player->num_frames_dead = 0;

			}

		}

	}

}

Player* find_player_by_id(uint64_t player_id, Player* players, uint8_t num_players) {
	for (uint8_t i = 0; i < num_players; i += 1) {
		if (player_id == players[i].id) {
			return &players[i];

		}

	}

	return NULL;

}
