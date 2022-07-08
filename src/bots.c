#include <stdio.h>
#include <stdlib.h>

#include "player.h"
#include "bots.h"
#include "bot_actions_def.h"
#include "include/wasm.h"
#include "include/wasmer.h"

WasmData setup_wasm() {
	WasmData wasm_data = {
		.num_bots = 0,
		.wasm_engine = wasm_engine_new(),
		.bot_data_list = NULL,
	};

	return wasm_data;

}

bool setup_bot(const char* wasm_file_path, WasmData* wasm_data) {
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

	wasm_extern_vec_t wasm_import_object = WASM_EMPTY_VEC;
	wasm_instance_t* wasm_instance = wasm_instance_new(wasm_store, wasm_module, &wasm_import_object, NULL);

	wasm_extern_vec_t wasm_exports;
	wasm_instance_exports(wasm_instance, &wasm_exports);

	if (wasm_exports.size == 0) {
		printf("Invalid number of exports, found %lu\n", wasm_exports.size);
		return false;

	}

	wasm_func_t* calc_actions = wasm_extern_as_func(wasm_exports.data[wasm_exports.size - 1]);

	if (calc_actions == NULL) {
		printf("Failed to get calc_actions function\n");
		return false;

	}

	if (!wasm_instance) {
		printf("Error instantiating moudle\n");
		return false;

	}

	wasm_data->num_bots += 1;
	wasm_data->bot_data_list = realloc(wasm_data->bot_data_list, wasm_data->num_bots);
	BotWasmData bot_wasm_data = {
		.wasm_exports = wasm_exports,
		.wasm_import_object = wasm_import_object,
		.wasm_module = wasm_module,
		.wasm_store = wasm_store,
		.bot_func = calc_actions,
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

char get_byte(int64_t addr, BotWasmData* bot_wasm_data) {
	

}


