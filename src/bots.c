#include <stdio.h>
#include <stdlib.h>

#include "minimal_player_info.h"
#include "player.h"
#include "bots.h"
#include "bot_actions_def.h"
#include "include/wasm.h"
#include "include/wasmer.h"
#include "wasmer_ext.h"

typedef struct GetByteResult {
	uint8_t byte;
	bool valid;

} GetByteResult;


WasmData setup_wasm() {
	WasmData wasm_data = {
		.num_bots = 0,
		.wasm_engine = wasm_engine_new(),
		.bot_data_list = NULL,
	};

	return wasm_data;

}

bool check_wasm_function(const char* func_name, wasm_func_t* func) {
	if (func == NULL) {
		printf("Error: Function %s not found", func_name);
		return false;

	} else {
		return true;

	}

}

bool setup_bot(const char* wasm_file_path, WasmData* wasm_data, uint8_t player_index) {
	// First, load the wasmfile
	FILE* wasm_file = fopen(wasm_file_path, "r");

	if (wasm_file == NULL) {
		printf("Opening wasm file failed");
		return false;

	}
	
	// Get the size of the file
	// Seek to the end of the file, then see what pos we are in the file, then seek back to the beginning
	// Just C programming things
	fseek(wasm_file, 0, SEEK_END);
	unsigned long wasm_file_size = ftell(wasm_file);
	rewind(wasm_file);

	wasm_byte_vec_t wasm_byte_vec;
	wasm_byte_vec_new_empty(&wasm_byte_vec);

	wasm_byte_vec.size = wasm_file_size;
	wasm_byte_vec.data = malloc(wasm_file_size);

	if (wasm_byte_vec.data == NULL) {
		printf("Error allocating space\n");
		return false;

	}

	fread(wasm_byte_vec.data, wasm_file_size, 1, wasm_file);
	fclose(wasm_file);

	wasm_engine_t* wasm_engine = wasm_data->wasm_engine;
	wasm_store_t* wasm_store = wasm_store_new(wasm_engine);

	wasm_module_t* wasm_module = wasm_module_new(wasm_store, &wasm_byte_vec);

	if (!wasm_module) {
		printf("Error compiling module\n");
		return false;

	}

	wasm_exporttype_vec_t exports_type;
	wasm_module_exports(wasm_module, &exports_type);
	
	if (exports_type.size == 0) {
		printf("Error getting exports_type\n");
		return false;

	}

	wasm_extern_vec_t wasm_import_object = WASM_EMPTY_VEC;
	wasm_instance_t* wasm_instance = wasm_instance_new(wasm_store, wasm_module, &wasm_import_object, NULL);

	wasm_extern_vec_t wasm_exports;

	wasm_instance_exports(wasm_instance, &wasm_exports);

	if (wasm_exports.size == 0) {
		printf("Invalid number of exports, found %lu\n", wasm_exports.size);
		return false;

	}

	size_t* action_to_int_index = get_export_by_name("actions_to_int", exports_type);
	size_t* get_byte_at_index = get_export_by_name("get_byte_at", exports_type);
	size_t* set_byte_at_index = get_export_by_name("set_byte_at", exports_type);
	size_t* get_num_players_ptr_index = get_export_by_name("get_num_players_ptr", exports_type);
	size_t* get_players_ptr_index = get_export_by_name("get_players_ptr", exports_type);
	size_t* get_player_index_ptr_index = get_export_by_name("get_player_index_ptr", exports_type);

	if (action_to_int_index == NULL) {
		printf("Couldn't find actions_to_int index\n");
		return false;

	}

	if (get_byte_at_index == NULL) {
		printf("Couldn't find get_byte_at index\n");
		return false;

	}

	if (set_byte_at_index == NULL) {
		printf("Couldn't find set_byte_at index\n");
		return false;

	}

	if (get_num_players_ptr_index == NULL) {
		printf("Couldn't find get_num_players_ptr index\n");
		return false;

	}

	if (get_players_ptr_index == NULL) {
		printf("Couldn't find get_players_ptr index\n");
		return false;

	}

	if (get_player_index_ptr_index == NULL) {
		printf("Couldn't find get_player_index_ptr index\n");
		return false;

	}

	wasm_func_t* actions_to_int = wasm_extern_as_func(wasm_exports.data[*action_to_int_index]);
	wasm_func_t* get_byte_at = wasm_extern_as_func(wasm_exports.data[*get_byte_at_index]);
	wasm_func_t* set_byte_at = wasm_extern_as_func(wasm_exports.data[*set_byte_at_index]);
	wasm_func_t* get_num_players_ptr = wasm_extern_as_func(wasm_exports.data[*get_num_players_ptr_index]);
	wasm_func_t* get_players_ptr = wasm_extern_as_func(wasm_exports.data[*get_players_ptr_index]);
	wasm_func_t* get_player_index_ptr = wasm_extern_as_func(wasm_exports.data[*get_player_index_ptr_index]);

	if (
			!check_wasm_function("actions_to_int", actions_to_int) || 
			!check_wasm_function("get_byte_at", get_byte_at) || 
			!check_wasm_function("set_byte_at", set_byte_at) || 
			!check_wasm_function("get_num_players_ptr", get_num_players_ptr) || 
			!check_wasm_function("get_players_ptr", get_players_ptr) ||
			!check_wasm_function("get_player_index_ptr", get_player_index_ptr) 
		) {
		return false;

	}

	free(action_to_int_index);
	free(get_byte_at_index);
	free(set_byte_at_index);
	free(get_num_players_ptr_index);
	free(get_players_ptr_index);
	free(get_player_index_ptr_index);

	if (!wasm_instance) {
		printf("Error instantiating moudle\n");
		return false;

	}

	wasm_data->num_bots += 1;
	wasm_data->bot_data_list = realloc(wasm_data->bot_data_list, sizeof(BotWasmData) * wasm_data->num_bots);
	BotWasmData bot_wasm_data = {
		.player_index = player_index,
		.wasm_exports = wasm_exports,
		.wasm_import_object = wasm_import_object,
		.wasm_module = wasm_module,
		.wasm_store = wasm_store,
		.bot_func = actions_to_int,
		.get_byte = get_byte_at,
		.set_byte = set_byte_at,
		.get_num_players_ptr = get_num_players_ptr,
		.get_players_ptr = get_players_ptr,
		.get_player_index_ptr = get_player_index_ptr,
	};
	

	wasm_data->bot_data_list[wasm_data->num_bots - 1] = bot_wasm_data;

	return true;

}

void act_on_bot(int32_t actions_int, Player* player) {
	BotActions actions;

	actions.movement_direction_up_down = actions_int & 0xFF;
	actions.movement_direction_left_right = (actions_int >> 8) & 0xFF;

	if (actions.movement_direction_left_right == 0) {
		player->pos_x -= player->speed;

	} else if (actions.movement_direction_left_right == 0xFF) {
		player->pos_x += player->speed;

	}

	if (actions.movement_direction_up_down == 0) {
		player->pos_y -= player->speed;

	} else if (actions.movement_direction_up_down == 0xFF) {
		player->pos_y += player->speed;

	}

}

GetByteResult get_byte(int64_t addr, BotWasmData* bot_wasm_data) {
	GetByteResult byte_result;

	wasm_val_t args_val[1] = { WASM_I64_VAL(addr) };
	wasm_val_vec_t args = WASM_ARRAY_VEC(args_val);

	wasm_val_t results_val[1] = { WASM_INIT_VAL };
	wasm_val_vec_t results = WASM_ARRAY_VEC(results_val);

	if (wasm_func_call(bot_wasm_data->get_byte, &args, &results)) {
		printf("Error calling func\n");
		byte_result.valid = false;
		return byte_result;

	}

	byte_result.valid = true;
	byte_result.byte = (uint8_t)results.data->of.i32;

	return byte_result;

}

void set_byte(uint8_t val, int64_t addr, BotWasmData* bot_wasm_data) {
	wasm_val_t args_val[2] = { WASM_I64_VAL(addr), WASM_I32_VAL(val) };
	wasm_val_vec_t args = WASM_ARRAY_VEC(args_val);

	wasm_val_t results_val[0];
	wasm_val_vec_t results = WASM_ARRAY_VEC(results_val);

	if (wasm_func_call(bot_wasm_data->set_byte, &args, &results)) {
		printf("Error calling func\n");

	}


}

void memcpy_to_wasm(const int64_t dst, const void* src, const size_t num_bytes, BotWasmData* bot_data) {
	const uint8_t* src_ptr = src;

	for (size_t i = 0; i < num_bytes; i += 1) {
		set_byte(src_ptr[i], dst + i, bot_data);

	}

}

void update_bot_info(const Player* players, uint8_t num_players, WasmData* wasm_data) {
	// First, generate the minimal player info from the normal player info
	MinimalPlayerInfo* minimal_player_info_list = malloc(num_players * sizeof(MinimalPlayerInfo));

	for (uint8_t i = 0; i < num_players; i += 1) {
		const Player* player = &players[i];
		minimal_player_info_list[i] = get_minimal_player_info(player);

	}

	wasm_val_t empty_args[0];
	wasm_val_vec_t args = WASM_ARRAY_VEC(empty_args);

	wasm_val_t results_val[1] = { WASM_INIT_VAL };
	wasm_val_vec_t results = WASM_ARRAY_VEC(results_val);

	// Then start updating the bot's information
	for (uint8_t i = 0; i < wasm_data->num_bots; i += 1) {
		BotWasmData* bot_data = &wasm_data->bot_data_list[i];

		if (wasm_func_call(bot_data->get_num_players_ptr, &args, &results)) {
			printf("Error getting num_players_ptr\n");
			return;

		}

		int64_t num_players_ptr = results.data->of.i64;

		if (wasm_func_call(bot_data->get_players_ptr, &args, &results)) {
			printf("Error getting num_players_ptr\n");
			return;

		}

		int64_t players_ptr = results.data->of.i64;

		if (wasm_func_call(bot_data->get_player_index_ptr, &args, &results)) {
			printf("Error getting player_index_ptr\n");
			return;

		}

		int64_t player_index_ptr = results.data->of.i64;

		memcpy_to_wasm(num_players_ptr, &num_players, sizeof(uint8_t), bot_data);
		memcpy_to_wasm(players_ptr, minimal_player_info_list, num_players * sizeof(MinimalPlayerInfo), bot_data);
		memcpy_to_wasm(player_index_ptr, &bot_data->player_index, sizeof(uint8_t), bot_data);

	}

	free(minimal_player_info_list);

}
