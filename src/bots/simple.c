#include <stdint.h>

#include "deps/bot_wasm.h"
#include "deps/mem.h"
#include "deps/math.h"
#include "deps/minimal_state_info.h"
#include "deps/weapon.h"

BotInfo BOT_INFO = {
	.name = "booty bot",
	.ability = Warp,
	.weapon = Shotgun,

};

BotActions ACTIONS = {
	.shooting = false,
	.using_ability = false,
	.angle = 0.0,
	.movement_info = {
		.moving = false,
		.angle = 0.0,

	},

};

uint64_t bot_actions(void) {
	const MinimalPlayerInfo* my_player = NULL;
	const MinimalPlayerInfo* enemy_player = NULL;
	float enemy_player_distance2 = 0.0;

	// First, find my player (TODO: Just copy this over)
	for (const MinimalTeamInfo* team = TEAMS; team < TEAMS + NUM_TEAMS; team += 1) {
		for (const MinimalPlayerInfo* player = team->players; player < team->players + team->num_players; player += 1) {
			if (player->id == MY_PLAYER_ID) {
				my_player = player;
				break;

			}
		}
	}


	// Next, find the closest enemy player
	for (const MinimalTeamInfo* team = TEAMS; team < TEAMS + NUM_TEAMS; team += 1) {
		if (team->id == my_player->team_id) {
			continue;

		}
		
		for (const MinimalPlayerInfo* player = team->players; player < team->players + team->num_players; player += 1) {
			if (enemy_player == NULL) {
				enemy_player = player;
				enemy_player_distance2 = distance2(my_player->pos_x, my_player->pos_y, enemy_player->pos_x, enemy_player->pos_y);

			} else {
				float distance = distance2(my_player->pos_x, my_player->pos_y, enemy_player->pos_x, enemy_player->pos_y);

				if (distance < enemy_player_distance2) {
					enemy_player = player;
					enemy_player_distance2 = distance;

				}

			}
			
		}
	}

	if (enemy_player != NULL) {
		float angle = get_angle(enemy_player->pos_x, enemy_player->pos_y, my_player->pos_x, my_player->pos_y);
		ACTIONS.angle = angle;
		
		if (enemy_player_distance2 > 2500.0) {
			ACTIONS.movement_info.angle = angle;
			ACTIONS.movement_info.moving = true;

			if (enemy_player_distance2 > 5000.0) {
				ACTIONS.using_ability = true;

			}

		} else {
			ACTIONS.shooting = true;

		}


	}	


	return (uint64_t)&ACTIONS;

}
