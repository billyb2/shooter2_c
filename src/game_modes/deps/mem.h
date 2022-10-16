#ifndef MEM_H
#define MEM_H
#include <stdint.h>
#include "minimal_state_info.h"
#include "drawing_api.h"

extern MinimalTeamInfo TEAMS[255];
extern MinimalMapInfo MAP;

extern MinimalPlayerInfo PLAYER_TO_BE_ADDED;
extern MinimalTeamInfo WINNING_TEAM;
extern DrawableObject DRAWABLE_OBJECTS[256];

void* memset(void* s, int c, unsigned long n);
void* memcpy(void* dst, const void *restrict src, unsigned long n);

uint64_t rand();
uint64_t rand_range_u64(uint64_t min, uint64_t max);
#endif
