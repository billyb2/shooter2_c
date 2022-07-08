#include "bot_actions.h"

BotActions calc_actions() {
	BotActions actions;

	actions.movement_direction_up_down = UP;
	actions.movement_direction_left_right = RIGHT;

	return actions;
}
