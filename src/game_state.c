#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "main_menu_state.h"
#include "in_game_state.h"
#include "game_state.h"

void enter_state(GamePage* game_page, GameState* game_state, GamePage new_game_page) {
	switch (new_game_page) {
		case InGame:
			enter_in_game(game_page, game_state);
			break;

		case MainMenu:
			enter_main_menu(game_page, game_state);
			break;

		case Settings:
			enter_settings(game_page, game_state);
			break;

	};

}

void run_state(GamePage* game_page, GameState* game_state) {
	switch (*game_page) {
		case InGame:
			run_in_game_state(game_page, game_state);
			break;

		case MainMenu:
			run_main_menu_state(game_page, game_state);
			break;

		case Settings:
			run_settings_state(game_page, game_state);
			break;

	};

}
