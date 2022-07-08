#include "minimal_player_info.h"

uint8_t NUM_PLAYERS;
MinimalPlayerInfo PLAYERS[4];

void set_byte_at(int64_t mem_addr_int, int32_t val) {
	char* mem_addr = (char*)mem_addr_int;
	*mem_addr = (char)(val & 0xFF);

}

int32_t get_byte_at(int64_t mem_addr_int) {
	char* mem_addr = (char*)mem_addr_int;
	return (int32_t)*mem_addr;
	
}

int64_t get_num_players_ptr() {
	return (int64_t)&NUM_PLAYERS;

}

int64_t get_players_ptr() {
	return (int64_t)&PLAYERS;

}
