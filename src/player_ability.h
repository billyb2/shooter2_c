#ifndef PLAYER_ABILITY_H
#define PLAYER_ABILITY_H

#define NUM_ABILITIES 3
#include <stdbool.h>

typedef enum Ability {
	Warp,
	Stim,
	Cloak,
} Ability;

int ability_to_text(Ability ability, char* text_buffer);
/* Returns whether or not the ability_text was valid */
bool text_to_ability(const char* ability_text, const int ability_text_len, Ability* ability);
#endif

