#ifndef BOTS_H
#define BOTS_H
#include "include/wasm3.h"
#include "game_mode.h"
#include "map.h"
#include "bot_def.h"
#include "player_def.h"

Bot* new_bot(const char* bot_file, Ability* ability, Weapon* weapon);
void run_bots(Player* players, uint8_t num_players, const Map* map);
void sync_teams_bots(Player* players, uint8_t num_players, const GameModeData* game_mode_data);

#endif
