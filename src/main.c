#define MAIN_C

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include "rand.h"
#include "game_state.h"
#include "include/raylib.h"

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 450;

int main() {
	init_fast_rand();
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "shooter2");

	SetTargetFPS(60);

	GameState game_state;
	GamePage game_page = MainMenu;

	enter_state(&game_page, &game_state, MainMenu);

	// Display the window until ESC is pressed
	while (!WindowShouldClose()) {
		run_state(&game_page, &game_state);

	}

    CloseWindow();

    return 0;
}


