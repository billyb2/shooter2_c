#include "game_state.h"
#include "main_menu_state.h"
#include "include/raylib.h"

#define RAYGUI_IMPLEMENTATION
#include "include/raygui.h"
#undef RAYGUI_IMPLEMENTATION


#define BUTTON_WIDTH 150.0
#define BUTTON_HEIGHT 40.0
#define SPACE_BETWEEEN_BUTTONS 10.0
#define CENTER_BUTTON_X (float)SCREEN_WIDTH / 2.0 - BUTTON_WIDTH / 2.0


extern int SCREEN_WIDTH;
extern int SCREEN_HEIGHT;

void enter_main_menu(GamePage* game_page, GameState* game_state) {
	GuiSetStyle(DEFAULT, TEXT_SIZE, 20);
	*game_page = MainMenu;

}

void run_main_menu_state(GamePage* game_page, GameState* game_state) {
	BeginDrawing();
		ClearBackground(WHITE);
		GuiDrawText("Shooter2_C", (Rectangle){ 0.0, 0.0, (float)SCREEN_WIDTH, 50.0}, TEXT_ALIGN_CENTER, BLACK);

		if (GuiButton((Rectangle){CENTER_BUTTON_X, 50.0, BUTTON_WIDTH, BUTTON_HEIGHT}, "Enter game")) {
			enter_state(game_page, game_state, InGame);

		}

		if (GuiButton((Rectangle){CENTER_BUTTON_X, 50.0 + BUTTON_HEIGHT + SPACE_BETWEEEN_BUTTONS, BUTTON_WIDTH, BUTTON_HEIGHT}, "Settings")) {
			enter_state(game_page, game_state, Settings);
			
		}

	EndDrawing();

}

void enter_settings(GamePage* game_page, GameState* game_state) {
	*game_page = Settings;

}

void run_settings_state(GamePage* game_page, GameState* game_state) {
	BeginDrawing();
		ClearBackground(WHITE);
		GuiDrawText("Settings", (Rectangle){ 0.0, 0.0, (float)SCREEN_WIDTH, 50.0}, TEXT_ALIGN_CENTER, BLACK);

		if (GuiButton((Rectangle){CENTER_BUTTON_X, 50.0, BUTTON_WIDTH, BUTTON_HEIGHT}, "Back")) {
			enter_state(game_page, game_state, MainMenu);

		}


	EndDrawing();

}
