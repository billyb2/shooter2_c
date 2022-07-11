#include <stdint.h>
#include <math.h>

#include "minimal_player_info.h"
#include "math.h"
#include "player.h"

#define TELEPORATION_SPEED 250.0

Player new_player(uint16_t pos_x, uint16_t pos_y, Ability ability) {
	Player player = {
		.pos_x = pos_x,
		.pos_y = pos_y,
		.speed = 5,
		.ability = ability,
		.direction = 0.0,
		.health = 500,
		.remaining_ability_cooldown_frames = 0,
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

void use_ability(Player* player) {
	if (player->remaining_ability_cooldown_frames > 0) {
		return;

	}

	if (player->ability == Teleporation) {
		int16_t speed_x = (uint16_t)(cosf(player->direction) * TELEPORATION_SPEED);
		int16_t speed_y = (uint16_t)(sinf(player->direction) * TELEPORATION_SPEED);

		player->pos_x = (uint16_t)((int16_t)player->pos_x + speed_x);
		player->pos_y = (uint16_t)((int16_t)player->pos_y + speed_y);

		// 5 seconds times 60 fps
		player->remaining_ability_cooldown_frames = 5 * 60;

	}

}

void update_player_cooldowns(Player* players, uint8_t num_players) {
	for (uint8_t i = 0; i < num_players; i += 1) {
		Player* player = &players[i];

		player->remaining_ability_cooldown_frames = saturating_sub(player->remaining_ability_cooldown_frames, 1);

	}

}
