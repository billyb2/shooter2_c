#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>

#include "wasm_strings.h"
#include "bots.h"

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
