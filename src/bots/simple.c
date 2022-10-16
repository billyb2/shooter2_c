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

uint64_t bot_actions() {
	uint8_t direction = NO_MOVEMENT;

	const MinimalPlayerInfo* my_player = NULL;
	const MinimalPlayerInfo* enemy_player = NULL;

	// Find the first player that isn't my player
	for (const MinimalTeamInfo* team = TEAMS; team < TEAMS + NUM_TEAMS; team += 1) {
		for (const MinimalPlayerInfo* player = team->players; player < team->players + team->num_players; player += 1) {
			if (player->id == MY_PLAYER_ID) {
				my_player = player;

			} else if (enemy_player == NULL) {	
				enemy_player = player;

			}

			if (my_player != NULL && enemy_player != NULL) {
				break;

			}
		}
	}

	if (enemy_player != NULL) {
		if (enemy_player->pos_x < 500.0) {
			direction = W;

		} else {
			direction = E;

		}
	}
	

	BotActions actions = {
		.direction = direction,
		.shooting = true,
		.using_ability = true,
		.angle = F32_PI,

	};


	uint64_t* info64 = (uint64_t*)&actions;
	return *info64;

}
