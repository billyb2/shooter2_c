#ifndef IN_GAME_STATE_H
#define IN_GAME_STATE_H
#include "game_state.h"

extern int SCREEN_WIDTH;
extern int SCREEN_HEIGHT;

void enter_in_game(GamePage* game_page, GameState* game_state);
void run_in_game_state(GamePage* game_page, GameState* game_state);

#endif
