#ifndef GAME_MODE_H
#define GAME_MODE_H
#include "include/wasm3.h"
#include "minimal_state_info.h"
#include "player.h"
#include "hashmap.h"

typedef struct UninitGameMode {
	const char* full_path;
	IM3Runtime rt;
	IM3Function name;

} UninitGameMode;

typedef struct PlayerScore {
	uint64_t id;
	uint64_t score;

} PlayerScore;

typedef enum GameMode {
	Deathmatch,
	TeamDeathmatch,

} GameMode;

#define Team MinimalTeamInfo

typedef struct GameModeData {
	Team* teams;
	uint8_t num_teams;
	IM3Runtime rt;

} GameModeData;

GameModeData init_gamemode_data(IM3Runtime rt);
bool calculate_scores(const Team** winning_team, GameModeData* game_mode_data);
bool add_player_to_team(MinimalPlayerInfo player, GameModeData* game_mode_data, uint64_t* team_id);
Team* find_team_of_id(uint64_t team_id, GameModeData* game_mode_data);
void sync_players_to_teams(Player* players, uint8_t num_players, GameModeData* game_mode_data);

#endif
