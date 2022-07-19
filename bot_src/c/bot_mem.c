#include <stdint.h>

#include "minimal_player_info.h"
#include "bot_mem.h"

static uint8_t PLAYER_INDEX;
static uint8_t NUM_PLAYERS;
static MinimalPlayerInfo PLAYERS[4];

void set_byte_at(int64_t mem_addr_int, int32_t val) {
	unsigned char* mem_addr = (unsigned char*)mem_addr_int;
	*mem_addr = (unsigned char)(val & 0xFF);

}

int32_t get_byte_at(int64_t mem_addr_int) {
	unsigned char* mem_addr = (unsigned char*)mem_addr_int;
	return (int32_t)*mem_addr;
	
}

int64_t get_num_players_ptr() {
	return (int64_t)&NUM_PLAYERS;

}

int64_t get_players_ptr() {
	return (int64_t)&PLAYERS;

}

int64_t get_player_index_ptr() {
	return (int64_t)&PLAYER_INDEX;

}
