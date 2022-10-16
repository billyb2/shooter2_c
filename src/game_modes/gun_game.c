#include "deps/weapon.h"
#include "deps/drawing_api.h"
#include "deps/minimal_state_info.h"
#include "deps/mem.h"

const char GAME_MODE_NAME[256] = "Gun Game";

uint64_t winning_score(void) {
	return 2;

}

uint8_t NUM_TEAMS = 255;
uint8_t NUM_DRAWABLE_OBJECTS = 0;

void init_game_mode(void) {
	for (uint8_t i = 0; i < NUM_TEAMS; i += 1) {
		MinimalTeamInfo team = {
			.num_players = 0,
			.score = 0,
			.color = 0xFF0000FF,
			.id = rand(),

		};

		TEAMS[i] = team;

	}

}

// Returns true or false depending on if the player was added
uint32_t add_player_to_team(void) {
	// Find an empty team
	for (uint8_t i = 0; i < NUM_TEAMS; i += 1) {
		MinimalTeamInfo* team = &TEAMS[i];
		
		if (team->num_players == 0) {
			team->num_players = 1;
			*team->players = PLAYER_TO_BE_ADDED;
			PLAYER_TO_BE_ADDED.team_id = team->id;
			PLAYER_TO_BE_ADDED.weapon = 0;
			return true;

		}

	}
				

	return false;
}

uint32_t spawn_player(void) {
	MinimalMapObject* spawn = &MAP.spawn_points[rand_range_u64(0, MAP.num_spawn_points)];
	PLAYER_TO_BE_ADDED.pos_x = spawn->pos_x + spawn->width / 2.0; 
	PLAYER_TO_BE_ADDED.pos_y = spawn->pos_y + spawn->height / 2.0; 

	return (bool)true;

}

uint32_t calculate_scores(void) {
	for (uint8_t i = 0; i < NUM_TEAMS; i += 1) {
		MinimalTeamInfo* team = &TEAMS[i];
		MinimalPlayerInfo* player = &team->players[0];


		// When the player has gotten a kill withe very weapon, the game loops back around to using the first weapon (currently the AR), so if the player is using a pistol and their num_kills > 0, they've won
		if (player->num_kills >= NUM_WEAPONS) {
			team->score = NUM_WEAPONS;
			WINNING_TEAM = *team;
			return true;

		} else {
			team->score = player->weapon;

		}

	}

	return false;

}

void set_player_stats(void) {
	for (uint8_t i = 0; i < NUM_TEAMS; i += 1) {
		MinimalTeamInfo* team = &TEAMS[i];
		MinimalPlayerInfo* player = &team->players[0];

		if (player->num_kills < NUM_WEAPONS) {
			if (player->weapon != player->num_kills) {
				// When changing a player's weapon, make sure to set max ammo
				player->weapon = player->num_kills;
				player->ammo = get_ammo_count(player->weapon);

			}

		} else {
			player->weapon = Pistol;

		}

	}
}

