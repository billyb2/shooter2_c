#ifndef BOTS_H
#define BOTS_H
#include "player.h"
#include "bot_actions_def.h"
#include "include/wasmer.h"
#include <stdint.h>

typedef struct BotWasmData {
	wasm_store_t* wasm_store;
	wasm_module_t* wasm_module;
	wasm_extern_vec_t wasm_import_object;
	wasm_extern_vec_t wasm_exports;
	wasm_func_t* bot_func;
	wasm_func_t* set_byte;
	wasm_func_t* get_byte;
	
} BotWasmData;

typedef struct WasmData {
	wasm_engine_t* wasm_engine;
	uint16_t num_bots;
	BotWasmData* bot_data_list;
} WasmData;

WasmData setup_wasm();
bool setup_bot(const char* wasm_file_path, WasmData* wasm_data);
BotActions int_to_actions(int32_t actions_int);
void act_on_bot(int32_t actions_int, Player* player);

void set_byte(char val, int64_t addr, BotWasmData* bot_wasm_data);
char get_byte(int64_t addr, BotWasmData* bot_wasm_data);
#endif
