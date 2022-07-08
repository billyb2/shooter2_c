#include "bot_actions.h"

/// Since wasm functions *must* return 4 bytes, we just pack all the data into an i32
int32_t actions_to_int() {
	unsigned char actions_bytes[4] = { 0 };
	
	BotActions actions = calc_actions();
	/// This info could definetely be packed into 4 bits instead of 16 bits, but idc enough right now
	actions_bytes[0] = actions.movement_direction_up_down;
	actions_bytes[1] = actions.movement_direction_left_right;

	// Only works for little endian
	int32_t res = (uint8_t)actions_bytes[0] + ((uint8_t)actions_bytes[1] << 8) + ((uint8_t)actions_bytes[2] << 16) + ((uint8_t)actions_bytes[3] << 24);

	return res;
}
