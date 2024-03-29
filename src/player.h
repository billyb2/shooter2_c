#ifndef PLAYER_H
#define PLAYER_H

#include <stdint.h>
#include "map.h"
#include "player_def.h"
#include "game_mode.h"
#include "player_ability.h"
#include "minimal_state_info.h"
#include "weapon.h"

#define DEFAULT_PLAYER_SPEED 4.2
#define PLAYER_MAX_HEALTH 500

Player new_player(Ability ability, Weapon weapon, Throwable throwable, const Map* map, char* player_name, GameModeData* game_mode_data, Bot* bot_data);
MinimalPlayerInfo get_minimal_player_info(const Player* player);
void use_ability(Player* player, const Map* map);
void reload(Player* player);
void update_player_cooldowns(Player* players, uint8_t num_players);
void move_player(Player* player, PlayerMovementInfo movement_info, const Map* map);
void respawn_players(Player* players, uint8_t num_players, const Map* map, GameModeData* game_mode_data);
uint16_t get_max_ability_charge(Ability ability);
AABB player_to_aabb(const Player* player);
Player* find_player_by_id(uint64_t player_id, Player* players, uint8_t num_players);
const Player* find_const_player_by_id(uint64_t player_id, const Player* players, uint8_t num_players);

#endif
