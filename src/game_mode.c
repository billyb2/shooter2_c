#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "game_modes/deps/drawing_api.h"
#include "include/wasm3.h"
#include "include/wasm3_ext.h"

#include "file_ext.h"
#include "game_mode.h"
#include "hashmap.h"
#include "map.h"
#include "minimal_state_info.h"
#include "player.h"
#include "rand.h"

uint8_t get_num_teams(IM3Runtime rt) {
	uint32_t num_teams;

	IM3Function num_teams_fn = NULL;
	if (m3_FindFunction(&num_teams_fn, rt, "num_teams") != m3Err_none || num_teams_fn == NULL) {
		fprintf(stderr, "Error getting num_teams\n");
		exit(-1);

	}

	if (m3_CallV(num_teams_fn) != m3Err_none) {
		fprintf(stderr, "Error calling num_teams\n");

	}

	m3_GetResultsV(num_teams_fn, &num_teams);

	return num_teams;
	
}

uint64_t get_num_drawable_objects(IM3Runtime rt) {
	IM3Function num_drawable_objects_fn;
	m3_FindFunction(&num_drawable_objects_fn, rt, "num_drawable_objects");
	m3_CallV(num_drawable_objects_fn);

	uint64_t num_drawable_objects;
	m3_GetResultsV(num_drawable_objects_fn, &num_drawable_objects);

	return num_drawable_objects;

}

uint8_t* get_wasm_memory(IM3Runtime rt) {
	return m3_GetMemory(rt, NULL, 0);

}

void update_map(const Map* map, IM3Runtime rt) {
	IM3Function map_ptr_fn; 
	if (m3_FindFunction(&map_ptr_fn, rt, "map_ptr") != m3Err_none) {
		fprintf(stderr, "map_ptr fn not found\n");
		exit(-1);

	}

	m3_CallV(map_ptr_fn);

	uint64_t map_ptr;
	m3_GetResultsV(map_ptr_fn, &map_ptr);

	MinimalMapInfo* minimal_map = (MinimalMapInfo*)&get_wasm_memory(rt)[map_ptr];
	*minimal_map = get_minimal_map_info(map);


}

MinimalTeamInfo* get_wasm_team_ptr(IM3Runtime rt) {
	uint64_t teams_ptr;

	IM3Function teams_ptr_fn;
	m3_FindFunction(&teams_ptr_fn, rt, "teams_ptr");
	m3_CallV(teams_ptr_fn);
	m3_GetResultsV(teams_ptr_fn, &teams_ptr);
	return (MinimalTeamInfo*)&get_wasm_memory(rt)[teams_ptr];

}

DrawableObject* get_drawable_objects_ptr(IM3Runtime rt) {
	IM3Function drawable_objects_fn;
	m3_FindFunction(&drawable_objects_fn, rt, "drawable_objects_ptr");
	m3_CallV(drawable_objects_fn);

	uint64_t drawable_objects_ptr;
	m3_GetResultsV(drawable_objects_fn, &drawable_objects_ptr);

	DrawableObject* drawable_objects = (DrawableObject*)&get_wasm_memory(rt)[drawable_objects_ptr];

	return drawable_objects;
}

GameModeData init_gamemode_data(IM3Runtime rt, const Map* map) {
	Team* teams;
	uint8_t num_teams;

	update_map(map, rt);

	IM3Function set_rng_seed_fn;
	m3_FindFunction(&set_rng_seed_fn, rt, "set_rng_seed");
	srand(time(0));
	m3_CallV(set_rng_seed_fn, rand());

	IM3Function init_gamemode;
	m3_FindFunction(&init_gamemode, rt, "init_game_mode");

	if (m3_CallV(init_gamemode) != m3Err_none) {
		M3ErrorInfo err;
		m3_GetErrorInfo(rt, &err);
		fprintf(stderr, "Error calling init_game_mode: %s with error code: %s\n", err.message, err.result);
		exit(-1);

	}

	num_teams = get_num_teams(rt);
	teams = get_wasm_team_ptr(rt);
	DrawableObject* drawable_objects = get_drawable_objects_ptr(rt);


	GameModeData game_mode_data = {
		.num_teams = num_teams,
		.teams = teams,
		.rt = rt,
		.drawable_objects = drawable_objects,

	};

	IM3Function name_fn;
	uint64_t name_ptr;
	m3_FindFunction(&name_fn, rt, "name_ptr");
	m3_CallV(name_fn);
	m3_GetResultsV(name_fn, &name_ptr);
	printf("Playing %s\n", (char*)&get_wasm_memory(rt)[name_ptr]);

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

void sync_players_to_teams(Player* players, uint8_t num_players, GameModeData* game_mode_data) {
	game_mode_data->teams = get_wasm_team_ptr(game_mode_data->rt);
	game_mode_data->num_teams = get_num_teams(game_mode_data->rt);
	game_mode_data->drawable_objects = get_drawable_objects_ptr(game_mode_data->rt);

	for (uint8_t i = 0; i < num_players; i += 1) {
		Player* player = &players[i];

		if (player->assigned_id || player->is_net_player) {
			MinimalPlayerInfo* team_player = find_team_player_by_id(player->id, game_mode_data);	

			if (team_player != NULL) {
				// Allows the game mode to set the player's weapons, abilities, etc.
				*team_player = get_minimal_player_info(player);

				IM3Function set_player_stats;
				m3_FindFunction(&set_player_stats, game_mode_data->rt, "set_player_stats"); 
				m3_CallV(set_player_stats);

				player->weapon = team_player->weapon;
				player->ability = team_player->ability;
				player->ammo = team_player->ammo;
				player->pos_x = team_player->pos_x;
				player->pos_y = team_player->pos_y;

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
bool add_player_to_team(Player* player, const Map* map, GameModeData* game_mode_data) {
	IM3Function player_to_be_added_ptr_fn;
	m3_FindFunction(&player_to_be_added_ptr_fn, game_mode_data->rt, "player_to_be_added_ptr");
	m3_CallV(player_to_be_added_ptr_fn);

	uint64_t player_to_be_added_ptr;
	m3_GetResultsV(player_to_be_added_ptr_fn, &player_to_be_added_ptr);

	MinimalPlayerInfo* player_to_be_added = (MinimalPlayerInfo*)&get_wasm_memory(game_mode_data->rt)[player_to_be_added_ptr];
	*player_to_be_added = get_minimal_player_info(player);
	
	IM3Function add_player_to_team_fn;
	m3_FindFunction(&add_player_to_team_fn, game_mode_data->rt, "add_player_to_team");
	m3_CallV(add_player_to_team_fn);
	
	uint32_t player_added;

	m3_GetResultsV(add_player_to_team_fn, &player_added);

	if ((bool)player_added) {
		player->team_id = player_to_be_added->team_id;	
		if (!spawn_player(player, map, game_mode_data)) {
			fprintf(stderr, "Failed to spawn player\n");
			exit(-1);

		}

		printf("Added player %lu to team %lu\n", player->id, player->team_id);

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
	IM3Function calculate_scores_fn = NULL;
	m3_FindFunction(&calculate_scores_fn, game_mode_data->rt, "calculate_scores");

	if (calculate_scores_fn == NULL) {
		fprintf(stderr, "Error finding calculate scores fn\n");
		exit(-1);

	}

	if (m3_CallV(calculate_scores_fn) != m3Err_none) {
		M3ErrorInfo err;
		m3_GetErrorInfo(game_mode_data->rt, &err);
		fprintf(stderr, "Error calling calculate_scores with err: %s\n", err.message);
		exit(-1);

	}

	uint32_t team_won;

	if (m3_GetResultsV(calculate_scores_fn, &team_won) != m3Err_none) {
		fprintf(stderr, "Error getting calculate_scores results\n");
		exit(-1);

	}

	if ((bool)team_won) {
		IM3Function winning_team_fn;
		if (m3_FindFunction(&winning_team_fn, game_mode_data->rt, "winning_team_ptr") != m3Err_none) {
			fprintf(stderr, "Error getting winning_team_ptr\n");
			exit(-1);

		}
		m3_CallV(winning_team_fn);

		uint64_t winning_team_ptr;
		if (m3_GetResultsV(winning_team_fn, &winning_team_ptr) != m3Err_none) {
			M3ErrorInfo err;
			m3_GetErrorInfo(game_mode_data->rt, &err);
			fprintf(stderr, "Error calling winning_team_ptr: %s\n", err.message);
			exit(-1);

		}
		Team* wasm_winning_team = (Team*)&get_wasm_memory(game_mode_data->rt)[winning_team_ptr];
		*winning_team = wasm_winning_team;

	}

	return (bool)team_won;

	
}

bool spawn_player(Player* player, const Map* map, GameModeData* game_mode_data) {
	IM3Function player_to_be_added_ptr_fn;
	m3_FindFunction(&player_to_be_added_ptr_fn, game_mode_data->rt, "player_to_be_added_ptr");
	m3_CallV(player_to_be_added_ptr_fn);

	uint64_t player_to_be_added_ptr;
	m3_GetResultsV(player_to_be_added_ptr_fn, &player_to_be_added_ptr);

	MinimalPlayerInfo* player_to_be_added = (MinimalPlayerInfo*)&get_wasm_memory(game_mode_data->rt)[player_to_be_added_ptr];
	*player_to_be_added = get_minimal_player_info(player);

	update_map(map, game_mode_data->rt);

	IM3Function spawn_player_fn;
	m3_FindFunction(&spawn_player_fn, game_mode_data->rt, "spawn_player");

	if (m3_CallV(spawn_player_fn) != m3Err_none) {
		fprintf(stderr, "Failed to call spawn_player\n");
		exit(-1);

	}
	uint32_t spawned_player;

	m3_GetResultsV(spawn_player_fn, &spawned_player);

	if ((bool)spawned_player == true) {
		m3_CallV(player_to_be_added_ptr_fn);

		uint64_t player_to_be_added_ptr;
		m3_GetResultsV(player_to_be_added_ptr_fn, &player_to_be_added_ptr);

		MinimalPlayerInfo* player_to_be_added = (MinimalPlayerInfo*)&get_wasm_memory(game_mode_data->rt)[player_to_be_added_ptr];

		player->pos_x = player_to_be_added->pos_x;
		player->pos_y = player_to_be_added->pos_y;

	} else {
		fprintf(stderr, "Failed to spawn player\n");
		exit(-1);

	}

	return (bool)spawned_player;

}
