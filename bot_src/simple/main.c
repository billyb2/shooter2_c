#include "bot_actions.h"
#include "bot_mem.h"
#include "minimal_player_info.h"
#include <stdint.h>

BotActions calc_actions() {
	BotActions actions;
	uint8_t* num_players_ptr = (uint8_t*)get_num_players_ptr();
	uint8_t num_players = *num_players_ptr;

	MinimalPlayerInfo* minimal_player_info_list = (MinimalPlayerInfo*)get_players_ptr();
	
	if (minimal_player_info_list[1].pos_x < 500) {
		actions.movement_direction_left_right = RIGHT;
		actions.movement_direction_up_down = NONE;

	} else if (minimal_player_info_list[1].pos_x == 500 && minimal_player_info_list[1].pos_y < 300) {
		actions.movement_direction_left_right = NONE;
		actions.movement_direction_up_down = DOWN;

	}

	return actions;
}
