#include <stdint.h>
#include <math.h>

#include "minimal_state_info.h"
#include "map.h"
#include "math.h"
#include "player.h"

#define TELEPORATION_SPEED 250.0

Player new_player(float pos_x, float pos_y, Ability ability, Weapon weapon) {
	Player player = {
		.pos_x = pos_x,
		.pos_y = pos_y,
		.speed = 5,
		.ability = ability,
		.weapon = weapon,
		.direction = 0.0,
		.health = 500,
		.remaining_ability_cooldown_frames = 0,
		.remaining_shooting_cooldown_frames = 0,
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
		case Teleporation: {
			float potential_x = player->pos_x + cosf(player->direction) * TELEPORATION_SPEED;
			float potential_y = player->pos_y + sinf(player->direction) * TELEPORATION_SPEED;

			if (!map_collision(potential_x, potential_y, PLAYER_SIZE, PLAYER_SIZE, map)) {
				player->pos_x = potential_x;
				player->pos_y = potential_y;

				player->remaining_ability_cooldown_frames = 5 * 60;

			}

			break;
		}

	};

}

void move_player(Player* player, PlayerMovementInfo movement_info, const Map* map) {
	if (player->health == 0) {
		return;

	}

	if (movement_info.x_axis == Left) {
		player->pos_x = saturating_sub(player->pos_x, player->speed);

	}

	if (movement_info.x_axis == Right) {
		if (player->pos_x + player->speed < map->size_x) {
			player->pos_x += player->speed;

		}
	
	}

	if (movement_info.y_axis == Up) {
		player->pos_y = saturating_sub(player->pos_y, player->speed);

	}

	if (movement_info.y_axis == Down) {
		if (player->pos_y + player->speed < map->size_y) {
			player->pos_y += player->speed;

		}

	}

}

void update_player_cooldowns(Player* players, uint8_t num_players) {
	for (uint8_t i = 0; i < num_players; i += 1) {
		Player* player = &players[i];

		player->remaining_ability_cooldown_frames = saturating_sub(player->remaining_ability_cooldown_frames, 1);
		player->remaining_shooting_cooldown_frames = saturating_sub(player->remaining_shooting_cooldown_frames, 1);

	}

}
