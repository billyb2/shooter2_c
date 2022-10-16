#ifndef GAME_MODE_H
#define GAME_MODE_H
#include "include/wasm3.h"
#include "game_modes/deps/drawing_api.h"
#include "minimal_state_info.h"
#include "hashmap.h"
#include "player_def.h"
#include "map.h"

MinimalTeamInfo* get_wasm_team_ptr(IM3Runtime rt);

#define game_mode_data_teams get_wasm_team_ptr(game_mode_data->rt)


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
	DrawableObject* drawable_objects;
	uint8_t num_teams;
	IM3Runtime rt;

} GameModeData;

GameModeData init_gamemode_data(IM3Runtime rt, const Map* map);
bool spawn_player(Player* player, const Map* map, GameModeData* game_mode_data);
bool calculate_scores(const Team** winning_team, GameModeData* game_mode_data);
bool add_player_to_team(Player* player, const Map* map, GameModeData* game_mode_data);
Team* find_team_of_id(uint64_t team_id, GameModeData* game_mode_data);
void sync_players_to_teams(Player* players, uint8_t num_players, GameModeData* game_mode_data);
uint64_t get_num_drawable_objects(IM3Runtime rt);

#endif
