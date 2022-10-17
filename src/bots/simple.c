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

	// Find the first player that isn't my player and isn't on my team
	for (const MinimalTeamInfo* team = TEAMS; team < TEAMS + NUM_TEAMS; team += 1) {
		for (const MinimalPlayerInfo* player = team->players; player < team->players + team->num_players; player += 1) {
			if (player->id == MY_PLAYER_ID) {
				my_player = player;
				continue;

			} else if(player->health > 0) {	
				enemy_player = player;

			}

			if (my_player != NULL && enemy_player != NULL) {
				break;

			}
		}
	}

	if (enemy_player != NULL) {
		float angle = get_angle(enemy_player->pos_x, enemy_player->pos_y, my_player->pos_x, my_player->pos_y);
		
		ACTIONS.movement_info.angle = angle;
		ACTIONS.movement_info.moving = true;

		ACTIONS.angle = angle;
		ACTIONS.using_ability = true;
		ACTIONS.shooting = true;

	}	


	return (uint64_t)&ACTIONS;

}
