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


