#ifndef BOT_ACTIONS_H
#define BOT_ACTIONS_H
#include <stdint.h>
#include <stdbool.h>
#include "bot_actions_def.h"

#define UP (uint8_t)0x00
#define DOWN (uint8_t)0xFF
#define LEFT (uint8_t)0x00
#define RIGHT (uint8_t)0xFF
#define NONE (uint8_t)0x80

BotActions calc_actions();

/// Since wasm functions *must* return 4 bytes, we just pack all the data into an i32
int64_t actions_to_int();

void inline __attribute__((always_inline)) set_should_shoot(unsigned char* shooting_and_ability, bool should_shoot) {
	unsigned char bool_as_bits = ((unsigned char)(should_shoot) & 1) * 0xFF;
	*shooting_and_ability = (*shooting_and_ability & ~0xF0) | (bool_as_bits & 0xF0); 

}

void inline __attribute__((always_inline)) set_should_use_ability(unsigned char* shooting_and_ability, bool should_use_ability) {
	unsigned char bool_as_bits = ((unsigned char)(should_use_ability) & 1) * 0xFF;
	*shooting_and_ability = (*shooting_and_ability & ~0x0F) | (bool_as_bits & 0x0F); 

}

#endif
