#ifndef PLAYER_ABILITY_H
#define PLAYER_ABILITY_H

#define NUM_ABILITIES 2

typedef enum Ability {
	Warp,
	Stim,
} Ability;

int ability_to_text(Ability ability, char* text_buffer);
#endif
