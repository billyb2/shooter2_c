#include <stdint.h>
#include <stdio.h>

#include "game_mode.h"
#include "hashmap.h"
#include "player.h"

GameModeData init_gamemode_data() {
	GameModeData game_mode_data = {
		.score = {
			.num_items = 0,
			.item_list = NULL,

		},

	};

	return game_mode_data;

}

uint64_t winning_score() {
	return 2;

}

bool calculate_scores(const Player* players, uint8_t num_players, const Player** winning_player, GameModeData* game_mode_data) {
	// First, calculate scores for all players
	for (uint8_t i = 0; i < num_players; i += 1) {
		const Player* player = &players[i];

		if (!player->assigned_id) {
			continue;

		}

		hashmap_insert(&player->id, sizeof(player->id), &game_mode_data->score, player->num_kills);

	}

	
	for (uint8_t i = 0; i < num_players; i += 1) {
		const Player* player = &players[i];

		if (!player->assigned_id) {
			continue;

		}

		uint64_t score;
		hashmap_get(&player->id, sizeof(player->id), &game_mode_data->score, &score);

		// If any player's score is > winning_score(), they win
		if (score >= winning_score()) {
			*winning_player = player;
			return true;

		}

	}

	return false;

}
