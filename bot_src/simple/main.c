#include "bot_actions.h"
#include "bot_mem.h"
#include "minimal_player_info.h"

BotActions calc_actions() {
	static float direction = 0.0;

	BotActions actions;

	const uint8_t* num_players_ptr = (uint8_t*)get_num_players_ptr();
	uint8_t num_players = *num_players_ptr;

	const uint8_t* player_index_ptr = (uint8_t*)get_player_index_ptr();
	uint8_t player_index = *player_index_ptr;

	const MinimalPlayerInfo* minimal_player_info_list = (MinimalPlayerInfo*)get_players_ptr();
	const MinimalPlayerInfo my_player = minimal_player_info_list[player_index];
	
	if (my_player.pos_x < 500) {
		actions.movement_direction_left_right = RIGHT;
		actions.movement_direction_up_down = NONE;

	} else if (my_player.pos_y < 300) {
		actions.movement_direction_left_right = NONE;
		actions.movement_direction_up_down = DOWN;

	} else {
		actions.movement_direction_left_right = NONE;
		actions.movement_direction_up_down = NONE;

	}

	direction += 0.1;

	actions.direction = direction;
	actions.shooting = true;

	return actions;
}
