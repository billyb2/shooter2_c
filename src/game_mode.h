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

} GameMode;

typedef struct GameModeData {
	HashMap score;

} GameModeData;

GameModeData init_gamemode_data();
// Returns true if there's a winning player
bool calculate_scores(const Player* players, uint8_t num_players, const Player** winning_player, GameModeData* game_mode_data);


#endif
