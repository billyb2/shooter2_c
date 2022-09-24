#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "include/wasm3.h"
#include "include/wasm3_ext.h"

#include "file_ext.h"
#include "game_mode.h"
#include "hashmap.h"
#include "minimal_state_info.h"
#include "player.h"

uint8_t get_num_teams(IM3Runtime rt) {
	uint8_t num_teams;

	IM3Function num_teams_fn;
	m3_FindFunction(&num_teams_fn, rt, "num_teams");
	m3_CallV(num_teams_fn);
	m3_GetResultsV(num_teams_fn, &num_teams);

	return num_teams;
	

}

uint8_t* get_wasm_memory(IM3Runtime rt) {
	return m3_GetMemory(rt, NULL, 0);

}

MinimalTeamInfo* get_wasm_team_ptr(IM3Runtime rt) {
	uint64_t teams_ptr;

	IM3Function teams_ptr_fn;
	m3_FindFunction(&teams_ptr_fn, rt, "teams_ptr");
	m3_CallV(teams_ptr_fn);
	m3_GetResultsV(teams_ptr_fn, &teams_ptr);
	return (MinimalTeamInfo*)&get_wasm_memory(rt)[teams_ptr];

}

GameModeData init_gamemode_data(const char* wasm_file_name) {
	Team* teams;
	uint8_t num_teams;

	IM3Environment env = m3_NewEnvironment();
	IM3Runtime rt = m3_NewRuntime(env, 500000, NULL); // 500 KB
	IM3Module module = m3_NewModule(env);

	FILE* wasm_file = fopen(wasm_file_name, "rb");

	if (wasm_file == NULL) {
		perror("Error opening wasm file: ");
		exit(-1);

	}

	size_t wasm_file_size = get_file_size(wasm_file);

	uint8_t* wasm_file_bytes = malloc(wasm_file_size);
	fread(wasm_file_bytes, 1, wasm_file_size, wasm_file);

	fclose(wasm_file);

	m3_ParseModule(env, &module, wasm_file_bytes, wasm_file_size);
	m3_LoadModule(rt, module);
	
	IM3Function init_gamemode;
	m3_FindFunction(&init_gamemode, rt, "init_game_mode");
	m3_CallV(init_gamemode);

	num_teams = get_num_teams(rt);
	teams = get_wasm_team_ptr(rt);

	GameModeData game_mode_data = {
		.num_teams = num_teams,
		.teams = teams,
		.rt = rt,

	};

	return game_mode_data;

}

MinimalPlayerInfo* find_team_player_by_id(uint64_t id, GameModeData* game_mode_data) {
	Team* teams = get_wasm_team_ptr(game_mode_data->rt);

	for (uint8_t i = 0; i < game_mode_data->num_teams; i += 1) {
		Team* team = &teams[i];

		for (uint8_t j = 0; j < team->num_players; j += 1) {
			if (team->players[j].id == id) {
				return &team->players[j];

			}

		}

	}

	return NULL;

}

void sync_players_to_teams(const Player* players, uint8_t num_players, GameModeData* game_mode_data) {
	game_mode_data->num_teams = get_num_teams(game_mode_data->rt);

	for (uint8_t i = 0; i < num_players; i += 1) {
		const Player* player = &players[i];

		if (player->assigned_id || player->is_net_player) {
			MinimalPlayerInfo* team_player = find_team_player_by_id(player->id, game_mode_data);

			if (team_player == NULL) {
				fprintf(stderr, "Failed to find team player\n");
				//exit(-1);

			} else {
				*team_player = get_minimal_player_info(player);

			}

		}

	}

}

uint64_t winning_score(GameModeData* game_mode_data) {
	uint64_t winning_score;

	IM3Function winning_score_fn;
	m3_FindFunction(&winning_score_fn, game_mode_data->rt, "winning_score");
	m3_CallV(winning_score_fn);
	m3_GetResultsV(winning_score_fn, &winning_score);

	return winning_score;

}

// Don't worry about player->assigned_team = true, this is done by the game
// Returns whether or not a player was able to be added to a team
bool add_player_to_team(MinimalPlayerInfo player, GameModeData* game_mode_data, uint64_t* team_id) {
	IM3Function player_to_be_added_ptr_fn;
	m3_FindFunction(&player_to_be_added_ptr_fn, game_mode_data->rt, "player_to_be_added_ptr");
	m3_CallV(player_to_be_added_ptr_fn);

	uint64_t player_to_be_added_ptr;
	m3_GetResultsV(player_to_be_added_ptr_fn, &player_to_be_added_ptr);

	MinimalPlayerInfo* player_to_be_added = (MinimalPlayerInfo*)&get_wasm_memory(game_mode_data->rt)[player_to_be_added_ptr];
	*player_to_be_added = player;
	
	IM3Function add_player_to_team_fn;
	m3_FindFunction(&add_player_to_team_fn, game_mode_data->rt, "add_player_to_team");
	m3_CallV(add_player_to_team_fn);
	
	uint32_t player_added;

	m3_GetResultsV(add_player_to_team_fn, &player_added);

	if ((bool)player_added) {
		*team_id = player_to_be_added->team_id;	
		printf("Added player %lu to team %lu", player.id, *team_id);

	}

	return (bool)player_added;

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
	IM3Function winning_team_fn;
	m3_FindFunction(&winning_team_fn, game_mode_data->rt, "winning_team_ptr");
	m3_CallV(winning_team_fn);

	uint64_t winning_team_ptr;
	m3_GetResultsV(winning_team_fn, &winning_team_ptr);

	Team* wasm_winning_team = (Team*)&get_wasm_memory(game_mode_data->rt)[winning_team_ptr];

	IM3Function calculate_scores;
	m3_FindFunction(&calculate_scores, game_mode_data->rt, "calculate_scores");
	m3_CallV(calculate_scores);

	uint32_t team_won;
	m3_GetResultsV(calculate_scores, &team_won);

	if ((bool)team_won) {
		*winning_team = wasm_winning_team;

	}

	return (bool)team_won;

	
}
