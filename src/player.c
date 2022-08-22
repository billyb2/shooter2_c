#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <time.h>
#include <math.h>

#include "minimal_state_info.h"
#include "map.h"
#include "math.h"
#include "player.h"
#include "projectile.h"
#include "rand.h"
#include "weapon.h"

#define DEFAULT_PLAYER_SPEED 5.0
#define TELEPORATION_SPEED 250.0

Player new_player(Ability ability, Weapon weapon, Throwable throwable, const Map* map) {
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

	srand(time(NULL));

	Player player = {
		.id = rand(),
		.assigned_id = false,
		.pos_x = spawn_point->pos_x + 25.0,
		.pos_y = spawn_point->pos_y + 25.0,
		.speed = DEFAULT_PLAYER_SPEED,
		.ability = ability,
		.weapon = weapon,
		.num_throwables = num_throwables,
		.throwable = throwable,
		.direction = 0.0,
		.health = PLAYER_MAX_HEALTH,
		.remaining_ability_cooldown_frames = 0,
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

	};

	free(spawn_points);

	return player;

}

MinimalPlayerInfo get_minimal_player_info(const Player* player) {
	MinimalPlayerInfo minimal_player = {
		.pos_x = player->pos_x,
		.pos_y = player->pos_y,
		.health = player->health,
		.ability = player->ability,
		.weapon = player->weapon,
		.direction = player->direction,
		.ammo = player->ammo,
	};

	return minimal_player;

}

void use_ability(Player* player, const Map* map) {
	if (player->health == 0 || player->remaining_ability_cooldown_frames > 0) {
		return;

	}


	switch (player->ability) {
		case Warp: {
			float potential_x = player->pos_x + cosf(player->direction) * TELEPORATION_SPEED;
			float potential_y = player->pos_y + sinf(player->direction) * TELEPORATION_SPEED;

			if (!map_collision(potential_x, potential_y, PLAYER_SIZE, PLAYER_SIZE, map)) {
				player->pos_x = potential_x;
				player->pos_y = potential_y;

				player->using_ability = true;
				player->remaining_ability_cooldown_frames = 4 * 60;

			}

			break;
		}

		case Stim: {
			player->speed *= 1.75;

			player->using_ability = true;
			player->remaining_ability_cooldown_frames = 5 * 60;

			break;
		}

	};

}

void reload(Player* player) {
	if (player->reloading) {
		return;

	}

	uint16_t reload_frames;

	switch (player->weapon) {
		case AssaultRifle:
			// 2.5 seconds
			reload_frames = 60 * 2 + 30;
			break;

		
		case Pistol:
			// 2 seconds
			reload_frames = 60 * 2;
			break;

		case Shotgun:
			// 3 seconds
			reload_frames = 60 * 3;
			break;

		case None:
			reload_frames = 0;
			break;

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

		player->remaining_ability_cooldown_frames = saturating_sub(player->remaining_ability_cooldown_frames, 1);
		player->remaining_shooting_cooldown_frames = saturating_sub(player->remaining_shooting_cooldown_frames, 1);
		player->remaining_throwable_cooldown_frames = saturating_sub(player->remaining_throwable_cooldown_frames, 1);

		if (player->reloading) {
			player->remaining_reload_frames = saturating_sub(player->remaining_reload_frames, 1);

		}

		if (player->remaining_ability_cooldown_frames == 0) {
			player->using_ability = false;

			if (player->ability == Stim) {
				player->speed = DEFAULT_PLAYER_SPEED;

			}

		}


		if (player->reloading && player->remaining_reload_frames == 0) {
			player->ammo = get_ammo_count(player->weapon);
			player->reloading = false;

		}
	}

}

void respawn_players(Player* players, uint8_t num_players) {
	for (uint8_t i = 0; i < num_players; i += 1) {
		Player* player = &players[i];

		if (player->health == 0) {
			player->num_frames_dead += 1;

			if (player->num_frames_dead >= 180) {
				player->health = PLAYER_MAX_HEALTH;
				player->num_frames_dead = 0;

			}

		}

	}

}
