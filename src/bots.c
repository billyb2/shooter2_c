#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "bot_wasm.h"
#include "bots.h"
#include "file_ext.h"
#include "game_mode.h"
#include "include/wasm3.h"
#include "include/wasm3_ext.h"
#include "minimal_state_info.h"
#include "player.h"
#include "player_ability.h"
#include "player_def.h"

uint8_t* get_bot_wasm_memory(IM3Runtime rt) {
	return m3_GetMemory(rt, NULL, 0);

}

MinimalTeamInfo* get_bot_wasm_team_ptr(IM3Runtime rt) {
	uint64_t teams_ptr;

	IM3Function teams_ptr_fn;
	m3_FindFunction(&teams_ptr_fn, rt, "teams_ptr");
	m3_CallV(teams_ptr_fn);
	m3_GetResultsV(teams_ptr_fn, &teams_ptr);
	return (MinimalTeamInfo*)&get_bot_wasm_memory(rt)[teams_ptr];

}

uint8_t* get_bot_wasm_num_teams_ptr(IM3Runtime rt) {
	uint64_t num_teams_ptr;

	IM3Function num_teams_ptr_fn;
	m3_FindFunction(&num_teams_ptr_fn, rt, "num_teams_ptr");
	m3_CallV(num_teams_ptr_fn);
	m3_GetResultsV(num_teams_ptr_fn, &num_teams_ptr);
	return &get_bot_wasm_memory(rt)[num_teams_ptr];

}

uint64_t* get_bot_player_id_ptr(IM3Runtime rt) {
	uint64_t player_id_ptr;

	IM3Function player_id_ptr_fn;
	m3_FindFunction(&player_id_ptr_fn, rt, "my_player_id_ptr");
	m3_CallV(player_id_ptr_fn);
	m3_GetResultsV(player_id_ptr_fn, &player_id_ptr);
	return (uint64_t*)&get_bot_wasm_memory(rt)[player_id_ptr];
}

BotInfo* get_bot_info(IM3Runtime rt) {
	uint64_t bot_info_ptr;

	IM3Function bot_info_ptr_fn;
	m3_FindFunction(&bot_info_ptr_fn, rt, "bot_info_ptr");
	m3_CallV(bot_info_ptr_fn);
	m3_GetResultsV(bot_info_ptr_fn, &bot_info_ptr);
	return (BotInfo*)&get_bot_wasm_memory(rt)[bot_info_ptr];
}

Bot* new_bot(const char* bot_file, Ability* ability, Weapon* weapon) {
	FILE* wasm_file = fopen(bot_file, "rb");
	
	if (wasm_file == NULL) {
		fprintf(stderr, "Couldn't open bot file: %s\b", bot_file);
		exit(-1);

	}

	size_t wasm_file_size = get_file_size(wasm_file);
	uint8_t* wasm_file_bytes = malloc(wasm_file_size);

	size_t remaining_bytes_to_read = wasm_file_size;

	while (remaining_bytes_to_read > 0) {
		size_t bytes_read = fread(wasm_file_bytes + (wasm_file_size-remaining_bytes_to_read), 1, wasm_file_size, wasm_file);

		if (bytes_read == 0) {
			break;

		} else {
			remaining_bytes_to_read -= bytes_read;

		}

	}

	if (remaining_bytes_to_read > 0) {
		fprintf(stderr, "Failed to read entire wasm file\n");
		exit(-1);

	}

	fclose(wasm_file);

	IM3Environment env = m3_NewEnvironment();
	IM3Runtime rt = m3_NewRuntime(env, 1000000, NULL); // 1 MB
	IM3Module module = m3_NewModule(env);

	m3_ParseModule(env, &module, wasm_file_bytes, wasm_file_size);
	m3_LoadModule(rt, module);

	BotInfo* bot_info = get_bot_info(rt);

	*ability = bot_info->ability;
	*weapon = bot_info->weapon;


	Bot* bot = malloc(sizeof(Bot));
	bot->rt = rt;

	return bot;

}

void run_bots(Player* players, uint8_t num_players, const Map* map) {
	for (uint8_t i = 0; i < num_players; i += 1) {
		Player* player = &players[i];

		if (player->bot_data == NULL) {
			continue;

		}

		IM3Function bot_actions_fn;
		if (m3_FindFunction(&bot_actions_fn, player->bot_data->rt, "bot_actions") != m3Err_none) {
			fprintf(stderr, "Failed to find bot_actions fn\n");
			exit(-1);

		}

		if (m3_CallV(bot_actions_fn) != m3Err_none) {
			M3ErrorInfo err;
			m3_GetErrorInfo(player->bot_data->rt, &err);
			fprintf(stderr, "Failed to call movement bot_actions_fn: %s\n", err.message);
			exit(-1);

		}


		BotActions bot_actions;
		m3_GetResultsV(bot_actions_fn, &bot_actions);

		PlayerMovementInfo movement_info = {
			.x_axis = NoneX,
			.y_axis = NoneY,

		};
		
		
		switch (bot_actions.direction) {
			case W:
				movement_info.x_axis = Left;
				break;

			case E:
				movement_info.x_axis = Right;
				break;

			case N:
				movement_info.y_axis = Up;
				break;

			case S:
				movement_info.y_axis = Down;
				break;

			case NE:
				movement_info.x_axis = Right;
				movement_info.y_axis = Up;
				break;

			case NW:
				movement_info.x_axis = Left;
				movement_info.y_axis = Up;
				break;

			case SE:
				movement_info.x_axis = Right;
				movement_info.y_axis = Down;
				break;

			case SW:
				movement_info.x_axis = Left;
				movement_info.y_axis = Down;
				break;

		};

		move_player(player,	movement_info, map);
		player->shooting = bot_actions.shooting;
		player->direction = bot_actions.direction;

	}
}

void sync_teams_bots(Player* players, uint8_t num_players, const GameModeData* game_mode_data) {
	for (uint8_t i = 0; i < num_players; i += 1) {
		Player* player = &players[i];

		if (player->bot_data == NULL) {
			continue;

		}

		Team* team_ptr = get_bot_wasm_team_ptr(player->bot_data->rt);
		uint8_t* num_teams = get_bot_wasm_num_teams_ptr(player->bot_data->rt);

		*num_teams = game_mode_data->num_teams;
		memcpy(team_ptr, game_mode_data->teams, sizeof(Team) * game_mode_data->num_teams);

		uint64_t* player_id_ptr = get_bot_player_id_ptr(player->bot_data->rt);
		*player_id_ptr = player->id;

	}

}
