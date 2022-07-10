#ifndef BOT_ACTIONS_DEF_H
#define BOT_ACTIONS_DEF_H
#include <stdbool.h>

typedef  struct __attribute__((__packed__)) BotActions {
	// 2 bytes
	unsigned char movement_direction_up_down;
	unsigned char movement_direction_left_right;
	// 4 bytes
	float direction;
	// 1 byte
	bool shooting;
} BotActions;
#endif
