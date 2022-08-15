#include <stdint.h>
#include <math.h>

#include "minimal_state_info.h"
#include "map.h"
#include "math.h"
#include "player.h"
#include "weapon.h"

#define DEFAULT_PLAYER_SPEED 5.0
#define TELEPORATION_SPEED 250.0

Player new_player(float pos_x, float pos_y, Ability ability, Weapon weapon, Throwable throwable) {
	uint8_t num_throwables;

	switch (throwable) {
		case Grenade:
			num_throwables = 3;
			break;
	};

	Player player = {
		.pos_x = pos_x,
		.pos_y = pos_y,
		.speed = DEFAULT_PLAYER_SPEED,
		.ability = ability,
		.weapon = weapon,
		.num_throwables = num_throwables,
		.throwable = throwable,
		.direction = 0.0,
		.health = 500,
		.remaining_ability_cooldown_frames = 0,
		.remaining_shooting_cooldown_frames = 0,
		.using_ability = false,
		.shooting = false,
		.equipped_weapon = Primary,
		.throw_ratio = 0.5,
	};

	return player;

}

MinimalPlayerInfo get_minimal_player_info(const Player* player) {
	MinimalPlayerInfo minimal_player = {
		.pos_x = player->pos_x,
		.pos_y = player->pos_y,
		.direction = player->direction,
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

		if (player->remaining_ability_cooldown_frames == 0) {
			player->using_ability = false;

			if (player->ability == Stim) {
				player->speed = DEFAULT_PLAYER_SPEED;

			}

		}

	}

}
