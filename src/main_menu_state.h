#ifndef MAIN_MENU_H
#define MAIN_MENU_H
#include "game_state.h"
void enter_main_menu(GamePage* game_page, GameState* game_state);
void run_main_menu_state(GamePage* game_page, GameState* game_state);

void enter_settings(GamePage* game_page, GameState* game_state);
void run_settings_state(GamePage* game_page, GameState* game_state);

#endif
