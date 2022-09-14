#ifndef GAME_MODE_H
#define GAME_MODE_H
#include "player.h"
#include "hashmap.h"

typedef struct PlayerScore {
	uint64_t id;
	uint64_t score;

} PlayerScore;

typedef enum GameMode {
	Deathmatch,
	TeamDeathmatch,

} GameMode;

typedef struct Team {
	uint64_t id;
	uint64_t score;
	Player** players;
	uint8_t num_players_alloc;
	uint8_t num_players;

} Team;

typedef struct GameModeData {
	GameMode game_mode;
	Team* teams;
	uint8_t num_teams;

} GameModeData;

GameModeData init_gamemode_data(GameMode game_mode, uint8_t num_players);
bool calculate_scores(const Team** winning_team, GameModeData* game_mode_data);
bool add_player_to_team(Player* player, GameModeData* game_mode_data);
Team* find_team_of_id(uint64_t team_id, GameModeData* game_mode_data);


#endif
