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
	bool shooting;
	bool using_ability;
	uint16_t _padding;
	float angle;
	PlayerMovementInfo movement_info;

} BotActions;

#ifndef WASM 
static_assert(sizeof(BotActions) % 8 == 0, "BotActions is the wrong size");
static_assert(sizeof(BotInfo) == 24, "BotInfo is the wrong size");
#endif

uint64_t move_bot();

#endif
