#include <stdbool.h>
#include <string.h>
#include "player_ability.h"

/* Returns the length of the string returned*/
int ability_to_text(Ability ability, char* text_buffer) {
	switch (ability) {
		case Warp:
			memcpy(text_buffer, "Warp", 5);
			return 4;
			
		case Stim:
			memcpy(text_buffer, "Stim", 5);
			return 4;

		case Cloak:
			memcpy(text_buffer, "Cloak", 6);
			return 5;

	};

}

bool text_to_ability(const char* ability_text, const int ability_text_len, Ability* ability) {
	if (ability_text_len == 4) {
		if (memcmp(ability_text, "Stim", ability_text_len) == 0) {
			*ability = Stim;
			return true;

		} else if (memcmp(ability_text, "Warp", ability_text_len) == 0) {
			*ability = Warp;
			return true;

		}

	} else if (ability_text_len == 5) {
		if (memcmp(ability_text, "Cloak", ability_text_len) == 0) {
			*ability = Cloak;
			return true;

		}

	}

	return false;
	
}
