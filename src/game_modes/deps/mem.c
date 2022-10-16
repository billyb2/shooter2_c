#include <stdint.h>
#include "mem.h"
#include "drawing_api.h"
#include "minimal_state_info.h"

extern char GAME_MODE_NAME[256];
extern uint8_t NUM_TEAMS;
extern uint8_t NUM_DRAWABLE_OBJECTS;
MinimalTeamInfo TEAMS[255];
MinimalMapInfo MAP;

MinimalPlayerInfo PLAYER_TO_BE_ADDED;
MinimalTeamInfo WINNING_TEAM;
DrawableObject DRAWABLE_OBJECTS[256];

uint64_t RNG_SEED;

uint64_t drawable_objects_ptr(void) {
	return (uint64_t)DRAWABLE_OBJECTS;

}

uint64_t num_drawable_objects(void) {
	return NUM_DRAWABLE_OBJECTS;

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

uint64_t rand(void) {
	return(((RNG_SEED = RNG_SEED * 214013L + 2531011L) >> 16) & 0x7fff);

}

uint64_t rand_range_u64(uint64_t min, uint64_t max) {
        return min + (rand() % (max - min));

}

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
