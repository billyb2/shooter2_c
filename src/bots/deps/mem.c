#include <stdint.h>
#include "bot_wasm.h"
#include "minimal_state_info.h"

uint64_t MY_PLAYER_ID;
MinimalTeamInfo TEAMS[255];
uint8_t NUM_TEAMS = 255;

uint64_t teams_ptr(void) {
	return (uint64_t)TEAMS;

}

uint64_t my_player_id_ptr(void) {
	return (uint64_t)&MY_PLAYER_ID;

}

uint64_t num_teams_ptr(void) {
	return (uint64_t)&NUM_TEAMS;

}

extern BotInfo BOT_INFO;

uint64_t bot_info_ptr() {
	return (uint64_t)&BOT_INFO;

}
