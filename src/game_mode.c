#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#include "game_mode.h"
#include "hashmap.h"
#include "player.h"

GameModeData init_gamemode_data(GameMode game_mode, uint8_t num_players) {
	Team* teams;
	uint8_t num_teams;

	switch (game_mode) {
		case Deathmatch:
			num_teams = num_players;
			teams = malloc(sizeof(Team) * num_players);

			for (uint8_t i = 0; i < num_players; i += 1) {
				Team team = {
					.num_players = 0,
					.num_players_alloc = 0,
					.players = NULL,
					.score = 0,
					.id = rand(),

				};

				teams[i] = team;

			}
			
			break;

		case TeamDeathmatch:
			num_teams = 2;
			teams = malloc(sizeof(Team) * num_teams);

			for (uint8_t i = 0; i < num_teams; i += 1) {
				Team team = {
					.num_players = 0,
					.num_players_alloc = 0,
					.players = NULL,
					.score = 0,
					.id = rand(),

				};

				teams[i] = team;

			}

			break;

	}

	GameModeData game_mode_data = {
		.game_mode = game_mode,
		.num_teams = num_teams,
		.teams = teams,

	};

	return game_mode_data;

}

uint64_t winning_score() {
	return 2;

}

// Don't worry about player->assigned_team = true, this is done by the game
// Returns whether or not a player was able to be added to a team
bool add_player_to_team(Player* player, GameModeData* game_mode_data) {
	switch (game_mode_data->game_mode) {
		case Deathmatch:
			// Find an empty team
			for (uint8_t i = 0; i < game_mode_data->num_teams; i += 1) {
				Team* team = &game_mode_data->teams[i];
				
				if (team->num_players == 0) {
					team->num_players = 1;
					team->num_players_alloc = 1;
					team->players = malloc(sizeof(Player*));
					*team->players = player;
					player->team_id = team->id;
					return true;

				}

			}
				
			break;

		case TeamDeathmatch:
			// Add to whichever team has the lower number of players	
			{
				Team* lower_num_team = &game_mode_data->teams[0];

				for (uint8_t i = 0; i < game_mode_data->num_teams; i += 1) {
					Team* team = &game_mode_data->teams[i];

					if (team->num_players < lower_num_team->num_players) {
						lower_num_team = team;

					}

					printf("Team %lu has %u players\n", team->id, team->num_players);

				}

				lower_num_team->num_players += 1;
				lower_num_team->num_players_alloc = lower_num_team->num_players;
				lower_num_team->players = realloc(lower_num_team->players, lower_num_team->num_players * sizeof(Player*));
				lower_num_team->players[lower_num_team->num_players - 1] = player;
				player->team_id = lower_num_team->id;
				return true;

			}
			break;

	};

	return false;

}

Team* find_team_of_id(uint64_t team_id, GameModeData* game_mode_data) {
	for (uint8_t i = 0; i < game_mode_data->num_teams; i += 1) {
		Team* team = &game_mode_data->teams[i];

		if (team->id == team_id) {
			return team;

		}

	}

	return NULL;

}

bool calculate_scores(const Team** winning_team, GameModeData* game_mode_data) {
	for (uint8_t i = 0; i < game_mode_data->num_teams; i += 1) {
		Team* team = &game_mode_data->teams[i];

		team->score = 0;

		// Count up the number of kills of each team's player
		for (uint8_t j = 0; j < team->num_players; j += 1) {
			Player* player = team->players[j];
			team->score += player->num_kills;

		}

		if (team->score >= winning_score()) {
			*winning_team = team;
			return true;

		}

	}

	return false;

}
