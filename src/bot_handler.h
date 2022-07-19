#ifndef BOT_HANDLER_H
#define BOT_HANDLER_H
#include <stdint.h>

#include "minimal_player_info.h"
#include "projectile.h"
#include "player.h"

uint8_t setup_bot(const char* wasm_file_name, uint8_t player_index);
void update_bot_info(Player* minimal_player_info_list, uint8_t num_players, Projectile** projectiles, uint16_t* num_projectiles, const Map* map);

#endif
