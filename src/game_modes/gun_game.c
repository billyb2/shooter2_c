#include "deps/weapon.h"
#include "deps/drawing_api.h"
#include "deps/minimal_state_info.h"

const char GAME_MODE_NAME[256] = "Gun Game";

void* memset(void* s, int c, unsigned long n) {
	char* sb = s;

	for (char* z = s; z < sb + n; z += 1) {
		*(char*)z = c;

	}

	return s;

}

void* memcpy(void* dst, const void *restrict src, unsigned long n) {
	const char *restrict sb = src;
	char *db = dst;

	for (unsigned long i = 0; i < n; i += 1) {
		*(db + i) = *(sb + i);

	}
	
	return dst;
		
}

uint64_t RNG_SEED;

uint64_t rand(void) {
	return(((RNG_SEED = RNG_SEED * 214013L + 2531011L) >> 16) & 0x7fff);

}

uint64_t rand_range_u64(uint64_t min, uint64_t max) {
        return min + (rand() % (max - min));

}

MinimalTeamInfo TEAMS[255];
uint8_t NUM_TEAMS;

MinimalMapInfo MAP;

uint64_t winning_score(void) {
	return 2;

}

void init_game_mode(void) {
	NUM_TEAMS = 255;

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

MinimalPlayerInfo PLAYER_TO_BE_ADDED;

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

MinimalTeamInfo WINNING_TEAM;

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

DrawableObject DRAWABLE_OBJECTS[256];

uint64_t drawable_objects_ptr(void) {
	return (uint64_t)DRAWABLE_OBJECTS;

}

uint64_t num_drawable_objects(void) {
	return 0;

}

uint64_t player_to_be_added_ptr(void) {
	return (uint64_t)&PLAYER_TO_BE_ADDED;

}

uint64_t winning_team_ptr(void) {
	return (uint64_t)&WINNING_TEAM;

}

uint64_t teams_ptr(void) {
	return (uint64_t)TEAMS;

}

uint32_t num_teams(void) {
	return NUM_TEAMS;

}

uint64_t name_ptr(void) {
	return (uint64_t)GAME_MODE_NAME;

}

uint64_t map_ptr(void) {
	return (uint64_t)&MAP;

}

void set_rng_seed(uint64_t seed) {
	RNG_SEED = seed;

}
