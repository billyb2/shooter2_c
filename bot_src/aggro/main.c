#include <stdint.h>

#include "bot_actions.h"
#include "bot_actions_def.h"

int32_t get_num_projectiles_ptr();

BotActions calc_actions() {
	BotActions actions = {
		.direction = 0.0,
		.movement_direction_left_right = NONE,
		.movement_direction_up_down = NONE,

	};

	const uint16_t* num_projectiles = (uint16_t*)get_num_projectiles_ptr();

	if (*num_projectiles > 0) {
		actions.movement_direction_left_right = RIGHT;

	}



	return actions;

}


