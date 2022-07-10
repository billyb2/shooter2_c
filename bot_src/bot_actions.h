#ifndef BOT_ACTIONS_H
#define BOT_ACTIONS_H
#include <stdint.h>
#include <stdbool.h>
#include "bot_actions_def.h"

#define UP (uint8_t)0x00
#define DOWN (uint8_t)0xFF
#define LEFT (uint8_t)0x00
#define RIGHT (uint8_t)0xFF
#define NONE (uint8_t) 0x80

BotActions calc_actions();

/// Since wasm functions *must* return 4 bytes, we just pack all the data into an i32
int64_t actions_to_int();

#endif
