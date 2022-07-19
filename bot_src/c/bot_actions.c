#include "bot_actions.h"
#include <stdint.h>

void* memcpy(void* dst, const void* src, unsigned long n) {
	char* d = dst;
	const char* s = src;
	for (uint64_t i = 0; i < n; i++) {
		d[i] = s[i];

	}
	return dst;
}

void bytecpy(void* dst, const void* src) {
	unsigned char* d = dst;
	const unsigned char* s = src;

	*d = *s;

}

/// Since wasm functions *must* return 8 bytes, we just pack all the data into an i64
int64_t actions_to_int() {
	BotActions actions = calc_actions();

	int64_t res;
	void* res_ptr = &res;

	bytecpy(res_ptr, &actions.movement_direction_up_down);
	bytecpy(res_ptr + 1, &actions.movement_direction_left_right);
	memcpy(res_ptr + 2, &actions.direction, sizeof(uint32_t));
	bytecpy(res_ptr + 6, &actions.shooting_and_ability);

	return res;
}
