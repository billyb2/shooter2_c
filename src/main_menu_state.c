#include "game_state.h"
#include "main_menu_state.h"
#include "include/raylib.h"
#include "player_ability.h"
#include "weapon.h"

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

	// If we're coming from InGame, copy the ability used
	Ability ability = 0;
	Weapon weapon = 0;

	if (*game_page == InGame) {
		ability = game_state->in_game_state.players[0].ability;
		weapon = game_state->in_game_state.players[0].weapon;

	}

	if (*game_page != Settings) {
		game_state->main_menu_state.ability = ability;
		game_state->main_menu_state.weapon = weapon;

	}

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
	MainMenuState* main_menu_state = &game_state->main_menu_state;

	BeginDrawing();
		ClearBackground(WHITE);
		GuiDrawText("Settings", (Rectangle){ 0.0, 0.0, (float)SCREEN_WIDTH, 50.0}, TEXT_ALIGN_CENTER, BLACK);

		const Color BUTTON_COLOR = {
			.r = 230,
			.g = 233,
			.b = 233,
			.a = 255,

		};


		#define FIRST_BUTTON_HEIGHT 50.0

		char ability_text[256] = { "Ability: " };
		int ability_str_len = ability_to_text(main_menu_state->ability, ability_text + 9);

		const float ability_button_width =  BUTTON_WIDTH + 6.0 * (9 + ability_str_len);
		const float ability_button_x = (float)SCREEN_WIDTH / 2.0 - ability_button_width / 2.0; 

		GuiDrawRectangle((Rectangle){ability_button_x, 50.0, ability_button_width, BUTTON_HEIGHT}, 3, DARKGRAY, BUTTON_COLOR);
		GuiDrawText(ability_text, (Rectangle){ability_button_x, 50.0, ability_button_width, BUTTON_HEIGHT}, TEXT_ALIGN_CENTER, BLACK);

		char weapon_text[256] = { "Weapon: " };
		int weapon_str_len = weapon_to_text(main_menu_state->weapon, weapon_text + 8);

		const float weapon_button_width =  BUTTON_WIDTH + 4.0 * (8 + weapon_str_len);
		const float weapon_button_x = (float)SCREEN_WIDTH / 2.0 - weapon_button_width / 2.0; 

		GuiDrawRectangle((Rectangle){weapon_button_x, 50.0 + BUTTON_HEIGHT + SPACE_BETWEEEN_BUTTONS, weapon_button_width, BUTTON_HEIGHT}, 3, DARKGRAY, BUTTON_COLOR);
		GuiDrawText(weapon_text, (Rectangle){weapon_button_x, 50.0 + BUTTON_HEIGHT + SPACE_BETWEEEN_BUTTONS, weapon_button_width, BUTTON_HEIGHT}, TEXT_ALIGN_CENTER, BLACK);

		const float increment_button_width = 20.0;
		const float increment_ability_button_x = ability_button_x + ability_button_width;

		if (GuiButton((Rectangle){ increment_ability_button_x, FIRST_BUTTON_HEIGHT, increment_button_width, BUTTON_HEIGHT }, "->") ) {
			if (main_menu_state->ability == NUM_ABILITIES - 1) {
				main_menu_state->ability = 0;

			} else {
				main_menu_state->ability += 1;

			}

		}

		if (GuiButton((Rectangle){ ability_button_x - increment_button_width, FIRST_BUTTON_HEIGHT, increment_button_width, BUTTON_HEIGHT }, "<-") ) {
			if (main_menu_state->ability == 0) {
				main_menu_state->ability = NUM_ABILITIES - 1;

			} else {
				main_menu_state->ability -= 1;

			}

		}

		const float increment_weapon_button_x = weapon_button_x + weapon_button_width;


		if (GuiButton((Rectangle){ increment_weapon_button_x , FIRST_BUTTON_HEIGHT + BUTTON_HEIGHT + SPACE_BETWEEEN_BUTTONS, increment_button_width, BUTTON_HEIGHT }, "->") ) {
			if (main_menu_state->weapon == NUM_WEAPONS - 1) {
				main_menu_state->weapon = 0;

			} else {
				main_menu_state->weapon += 1;

			}

		}

		if (GuiButton((Rectangle){ weapon_button_x - increment_button_width, FIRST_BUTTON_HEIGHT + BUTTON_HEIGHT + SPACE_BETWEEEN_BUTTONS, increment_button_width, BUTTON_HEIGHT }, "<-") ) {
			if (main_menu_state->weapon == 0) {
				main_menu_state->weapon = NUM_WEAPONS - 1;

			} else {
				main_menu_state->weapon -= 1;

			}

		}

		if (GuiButton((Rectangle) { weapon_button_x, FIRST_BUTTON_HEIGHT + 2 * (BUTTON_HEIGHT + SPACE_BETWEEEN_BUTTONS), weapon_button_width, BUTTON_HEIGHT }, "Back")) {
			enter_state(game_page, game_state, MainMenu);

		}

	EndDrawing();

}
