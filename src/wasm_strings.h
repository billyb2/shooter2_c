#ifndef WASM_STRINGS_H
#define WASM_STRINGS_H
#include <stdint.h>
#include <stdbool.h>
#include "bots.h"

typedef struct GetByteResult {
	uint8_t byte;
	bool valid;

} GetByteResult;


GetByteResult get_byte(int64_t addr, BotData* bot_wasm_data);
void memcpy_to_wasm(const int64_t dst, const void* src, const size_t num_bytes, BotData* bot_data);

#endif
