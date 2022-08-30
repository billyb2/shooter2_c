#ifndef PLAYER_ABILITY_H
#define PLAYER_ABILITY_H

#define NUM_ABILITIES 3

typedef enum Ability {
	Warp,
	Stim,
	Cloak,
} Ability;

int ability_to_text(Ability ability, char* text_buffer);
#endif
