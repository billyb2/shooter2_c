#ifndef BOT_WASM_H
#define BOT_WASM_H
#include <stdint.h>
#include "minimal_state_info.h"
#include "player_ability.h"
#include "weapon.h"

typedef struct __attribute__((__packed__)) BotInfo {
	char name[16];
	Ability ability;
	Weapon weapon;

} BotInfo;

typedef struct __attribute__((__packed__)) BotActions {
	uint8_t direction;
	bool shooting;
	bool using_ability;
	uint8_t _padding0;
	float angle;

} BotActions;

#ifndef WASM 
static_assert(sizeof(BotActions) == 8, "BotActions is the wrong size");
static_assert(sizeof(BotInfo) == 24, "BotInfo is the wrong size");
#endif

uint64_t move_bot();

#endif
