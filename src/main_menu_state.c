#include <stdlib.h>
#include <string.h>

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
	char* username = NULL;
	bool hosting = false;

	if (*game_page == InGame) {
		username = game_state->in_game_state.players[0].username;
		ability = game_state->in_game_state.players[0].ability;
		weapon = game_state->in_game_state.players[0].weapon;
		hosting = game_state->in_game_state.network_info.is_server;

	}

	if (*game_page != Settings) {
		game_state->main_menu_state.ability = ability;
		game_state->main_menu_state.weapon = weapon;

	}

	if (username == NULL) {
		username = malloc(20);
		memcpy(username, "shooter2_c player", 18);

	}

	char* ip_addr = malloc(16);
	memcpy(ip_addr, "127.0.0.1", 10);

	if (*game_page != Settings) {
		MainMenuState* main_menu_state = &game_state->main_menu_state;

		main_menu_state->ability = ability;
		main_menu_state->weapon = weapon;
		main_menu_state->username = username;
		main_menu_state->hosting = hosting;
		main_menu_state->ip_addr = ip_addr;

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

		// Handle username stuff
		GuiDrawText("Name: ", (Rectangle){ 0.0, FIRST_BUTTON_HEIGHT + 2 * BUTTON_HEIGHT + SPACE_BETWEEEN_BUTTONS, SCREEN_WIDTH, 50.0 }, TEXT_ALIGN_CENTER, BLACK);

		const Rectangle name_textbox_rect = {
			.x = (float)SCREEN_WIDTH / 2.0 - (float)SCREEN_WIDTH * (2.0 / 5.0) * 0.5,
			.y = FIRST_BUTTON_HEIGHT + 3 * BUTTON_HEIGHT + SPACE_BETWEEEN_BUTTONS,
			.width = (float)SCREEN_WIDTH * (2.0 / 5.0),
			.height = 50.0,

		};

		bool hovering_over_username = CheckCollisionPointRec(GetMousePosition(), name_textbox_rect);

		DrawRectangleRec(name_textbox_rect, RAYWHITE);

		Color username_outline_color = GRAY;

		if (hovering_over_username) {
			username_outline_color = DARKGRAY;

		}

		DrawRectangleLinesEx(name_textbox_rect, 3.0, username_outline_color);

		char text_to_draw[25] = { 0 };
		int username_len = strlen(main_menu_state->username);

		memcpy(text_to_draw, main_menu_state->username, username_len);
		
		if (hovering_over_username) {
			text_to_draw[username_len] = '|';

			int key = GetCharPressed();

			while (key > 0) {
				if (username_len < 20) {
					main_menu_state->username[username_len] = (char)key;

				}

				key = GetCharPressed();
			}

			if (IsKeyPressed(KEY_BACKSPACE) && username_len > 0) {
				// If the player's username is equal to the default one, just clear the whole username string
				// Also just clear the whole screen if we press CTRL at the same time
				if ((username_len == 17 && memcmp(main_menu_state->username, "shooter2_c player", 17) == 0) || IsKeyDown(KEY_LEFT_CONTROL)) {
					memset(main_menu_state->username, 0, 17);

				} else {
					*(main_menu_state->username + username_len - 1) = 0;

				}

				

			}

		}

		GuiDrawText(text_to_draw, name_textbox_rect, TEXT_ALIGN_CENTER, username_outline_color);

		char hosting_text[16] = { "Hosting: " };

		if (main_menu_state->hosting) {
			memcpy(hosting_text + 9, "true", 5);

		} else {
			memcpy(hosting_text + 9, "false", 6);

		}

		if (GuiButton((Rectangle) { CENTER_BUTTON_X, FIRST_BUTTON_HEIGHT + 5 * BUTTON_HEIGHT + SPACE_BETWEEEN_BUTTONS, BUTTON_WIDTH, BUTTON_HEIGHT }, hosting_text)) {
			main_menu_state->hosting = !main_menu_state->hosting;

		}

		char ip_addr_text_to_draw[17] = { 0 };

		if (!main_menu_state->hosting) {
			Rectangle ip_addr_rect = name_textbox_rect;
			ip_addr_rect.y = FIRST_BUTTON_HEIGHT + 6 * BUTTON_HEIGHT + SPACE_BETWEEEN_BUTTONS;

			bool hovering_over_ip_addr = CheckCollisionPointRec(GetMousePosition(), ip_addr_rect);

			Color ip_addr_outline_color = GRAY;

			strcpy(ip_addr_text_to_draw, main_menu_state->ip_addr);
			int ip_addr_str_len = strlen(ip_addr_text_to_draw);


			if (hovering_over_ip_addr) {
				ip_addr_outline_color = DARKGRAY;
				ip_addr_text_to_draw[ip_addr_str_len] = '|';

				int key = GetCharPressed();

				while (((key >= '0' && key <= '9') || key == '.')) {
					if (ip_addr_str_len < 15) {
						main_menu_state->ip_addr[ip_addr_str_len] = (char)key;

					}

					key = GetCharPressed();
				}

				if (IsKeyPressed(KEY_BACKSPACE) && ip_addr_str_len > 0) {
					if (IsKeyDown(KEY_LEFT_CONTROL)) {
						memset(main_menu_state->ip_addr, 0, 16);

					} else {
						*(main_menu_state->ip_addr + ip_addr_str_len - 1) = 0;

					}

					

				}

			}

			GuiDrawRectangle(ip_addr_rect, 3, ip_addr_outline_color, RAYWHITE);
			GuiDrawText(ip_addr_text_to_draw, ip_addr_rect, TEXT_ALIGN_CENTER, ip_addr_outline_color);


		}

		if (GuiButton((Rectangle) { CENTER_BUTTON_X, SCREEN_HEIGHT - BUTTON_HEIGHT - SPACE_BETWEEEN_BUTTONS, BUTTON_WIDTH, BUTTON_HEIGHT }, "Back")) {
			enter_state(game_page, game_state, MainMenu);

		}
		

	EndDrawing();

}
