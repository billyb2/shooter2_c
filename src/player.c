#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <time.h>
#include <math.h>

#include "game_mode.h"
#include "include/raylib.h"
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

Player new_player(Ability ability, Weapon weapon, Throwable throwable, const Map* map, char* player_name, GameModeData* game_mode_data, Bot* bot_data) {
	uint8_t num_throwables;

	switch (throwable) {
		case Grenade:
			num_throwables = 3;
			break;
	};

	if (map->num_spawn_points == 0) {
		fprintf(stderr, "Failed to find any spawn points\n");
		exit(-1);

	}

	Player player = {
		.id = rand(),
		.team_id = 0,
		.username = player_name,
		.assigned_team_id = false,
		.assigned_id = false,
		.speed = DEFAULT_PLAYER_SPEED,
		.ability = ability,
		.weapon = weapon,
		.num_throwables = num_throwables,
		.throwable = throwable,
		.direction = 0.0,
		.health = 0,
		.pos_x = 0.0,
		.pos_y = 0.0,
		.ability_charge = get_max_ability_charge(ability),
		.num_frames_ability_in_use = 0,
		.remaining_shooting_cooldown_frames = 0,
		.num_frames_dead = 0,
		.using_ability = false,
		.shooting = false,
		.throwing_grenade = false,
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
		.bot_data = bot_data,

	};

	return player;

}

AABB player_to_aabb(const Player* player) {
	AABB aabb = {
		.left_edge = player->pos_x - PLAYER_SIZE / 2.0,
		.right_edge = player->pos_x + PLAYER_SIZE / 2.0,
		.top_edge = player->pos_y - PLAYER_SIZE / 2.0,
		.bottom_edge = player->pos_y + PLAYER_SIZE / 2.0,
	};

	return aabb;

}

MinimalPlayerInfo get_minimal_player_info(const Player* player) {
	MinimalPlayerInfo minimal_player = {
		.id = player->id,
		.team_id = player->team_id,
		.pos_x = player->pos_x,
		.pos_y = player->pos_y,
		.health = player->health,
		.ability = player->ability,
		.using_ability = (uint32_t)player->using_ability,
		.weapon = player->weapon,
		.direction = player->direction,
		.ammo = player->ammo,
		.cloaking = player->cloaking,
		.num_kills = player->num_kills,
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

			player->pos_x += cosf(player->direction) * TELEPORATION_SPEED;
			player->pos_y += sinf(player->direction) * TELEPORATION_SPEED;

			if (!map_collision(player_to_aabb(player), map)) {
				player->using_ability = true;
				player->ability_charge = saturating_sub(player->ability_charge, half_max_ability_charge);

			} else {
				player->pos_x -= cosf(player->direction) * TELEPORATION_SPEED;
				player->pos_y -= sinf(player->direction) * TELEPORATION_SPEED;

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

	if (!movement_info.moving) {
		return;

	}

	float x_movement = cosf(movement_info.angle) * player->speed;
	float y_movement = sinf(movement_info.angle) * player->speed;

	player->pos_x += x_movement;

	if (map_collision(player_to_aabb(player), map)) {
		player->pos_x -= x_movement;

	}

	player->pos_y += y_movement;

	if (map_collision(player_to_aabb(player), map)) {
		player->pos_y -= y_movement;

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

void respawn_players(Player* players, uint8_t num_players, const Map* map, GameModeData* game_mode_data) {
	for (uint8_t i = 0; i < num_players; i += 1) {
		Player* player = &players[i];

		if (player->is_net_player) {
			continue;

		}

		if (player->health == 0 && !player->is_net_player && player->assigned_id) {
			player->num_frames_dead += 1;

			if (player->num_frames_dead >= 180) {
				player->health = PLAYER_MAX_HEALTH;
				spawn_player(player, map, game_mode_data);
				player->num_frames_dead = 0;

			}

		}

	}

}



const Player* find_const_player_by_id(uint64_t player_id, const Player* players, uint8_t num_players) {
	for (uint8_t i = 0; i < num_players; i += 1) {
		if (player_id == players[i].id) {
			return &players[i];

		}

	}

	return NULL;

}

Player* find_player_by_id(uint64_t player_id, Player* players, uint8_t num_players) {
	for (uint8_t i = 0; i < num_players; i += 1) {
		if (player_id == players[i].id) {
			return &players[i];

		}

	}

	return NULL;

}
