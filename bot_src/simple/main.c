#include <stdbool.h>
#include <stdint.h>

#include "bot_actions.h"
#include "bot_mem.h"
#include "minimal_player_info.h"

BotActions calc_actions() {
	static bool moving_up = true;

	BotActions actions;

	const uint8_t* num_players_ptr = (uint8_t*)get_num_players_ptr();
	uint8_t num_players = *num_players_ptr;

	const uint8_t* player_index_ptr = (uint8_t*)get_player_index_ptr();
	uint8_t player_index = *player_index_ptr;

	const MinimalPlayerInfo* minimal_player_info_list = (MinimalPlayerInfo*)get_players_ptr();
	const MinimalPlayerInfo my_player = minimal_player_info_list[player_index];

	
	actions.movement_direction_left_right = NONE;
	actions.movement_direction_up_down = NONE;
	actions.shooting_and_ability = 0x00;


	if (my_player.pos_x < 600) {
		actions.movement_direction_left_right = RIGHT;

	} 

	if (moving_up) {
		actions.movement_direction_up_down = UP;
		
		if (my_player.pos_y < 20) {
			moving_up = false;

		}

	} else {
		actions.movement_direction_up_down = DOWN;

		if (my_player.pos_y > 400) {
			moving_up = true;

		}


	}

	
	actions.direction = 3.1415926535;

	for (uint8_t i = 0; i < num_players; i += 1) {
		if (i == player_index) { continue; }

		MinimalPlayerInfo enemy_player = minimal_player_info_list[i];

		uint16_t low_range = enemy_player.pos_y - 25;
		uint16_t high_range = enemy_player.pos_y + 25;

		if (my_player.pos_y > low_range && my_player.pos_y < high_range) {
			set_should_shoot(&actions.shooting_and_ability, true);

		}

	}

	return actions;
}
