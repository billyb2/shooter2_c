#include <stdint.h>
#include <errno.h>
#include <sys/stat.h>
#include <stdio.h>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>

#include "file_ext.h"
#include "config.h"
#include "game_mode.h"
#include "game_state.h"
#include "include/wasm3.h"
#include "main_menu_state.h"
#include "include/raylib.h"
#include "player_ability.h"
#include "weapon.h"
#include "include/wasm3_ext.h"

#define RAYGUI_IMPLEMENTATION
#include "include/raygui.h"
#undef RAYGUI_IMPLEMENTATION

#define BUTTON_WIDTH 150.0
#define BUTTON_HEIGHT 40.0
#define SPACE_BETWEEEN_BUTTONS 7.5
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
	uint64_t num_game_modes;
	uint64_t current_game_mode_index;
	UninitGameMode* uninit_game_modes = NULL;

	char* ip_addr = malloc(16);
	memcpy(ip_addr, "127.0.0.1", 10);

	MainMenuState* main_menu_state = &game_state->main_menu_state;

	char* game_mode_file_name = NULL;

	if (*game_page == InGame) {
		int username_len = strlen(game_state->in_game_state.players[0].username);
		username = malloc(username_len + 1);
		memcpy(username, game_state->in_game_state.players[0].username, username_len + 1);

		ability = game_state->in_game_state.players[0].ability;
		weapon = game_state->in_game_state.players[0].weapon;

		free(game_state->in_game_state.players);

		hosting = game_state->in_game_state.network_info.is_server;

		current_game_mode_index = game_state->in_game_state.current_game_mode_index;
		num_game_modes = game_state->in_game_state.num_game_modes;
		uninit_game_modes = game_state->in_game_state.uninit_game_modes;

	} else if (*game_page == Settings) {
		ability = main_menu_state->ability;
		weapon = main_menu_state->weapon;
		username = main_menu_state->username;
		hosting = main_menu_state->hosting;
		free(ip_addr);
		ip_addr = main_menu_state->ip_addr;

		current_game_mode_index = main_menu_state->current_game_mode_index;
		num_game_modes = main_menu_state->num_game_modes;
		uninit_game_modes = main_menu_state->uninit_game_modes;
		game_mode_file_name = main_menu_state->game_mode_file_name;


	// Usually means the game just started
	} else {
		char* ability_option = get_config_option("ability");
		char* weapon_option = get_config_option("weapon");
		char* username_option = get_config_option("username");
		char* hosting_option = get_config_option("hosting");
		char* ip_addr_option = get_config_option("ip_addr");
		char* config_game_mode_file_name = get_config_option("game_mode_file_name");

		if (ability_option != NULL) {
			text_to_ability(ability_option, strlen(ability_option), &ability);

		}

		if (weapon_option != NULL) {
			text_to_weapon(weapon_option, strlen(weapon_option), &weapon);

		}

		if (username_option != NULL) {
			int username_len = strlen(username_option);
			username = malloc(username_len + 1); 
			memcpy(username, username_option, username_len + 1);

		}

		if (hosting_option != NULL) {
			hosting = memcmp(hosting_option, "true", 4) == 0;

		}

		if (ip_addr_option != NULL) {
			memcpy(ip_addr, ip_addr_option, strlen(ip_addr_option) + 1);

		}

		if (config_game_mode_file_name != NULL) {
			game_mode_file_name = malloc(256 + 11);
			memcpy(game_mode_file_name, config_game_mode_file_name, strlen(config_game_mode_file_name) + 1);

		}

		free(ability_option);
		free(weapon_option);
		free(username_option);
		free(hosting_option);
		free(ip_addr_option);
		free(config_game_mode_file_name);

	}

	if (username == NULL) {
		username = malloc(20);
		memcpy(username, "shooter2_c player", 18);

	}

	if (game_mode_file_name == NULL) {
		game_mode_file_name = malloc(11 + 256);
		memcpy(game_mode_file_name, "game_modes/deathmatch.wasm", 27);

	}


	char txt_buffer[256] = { 0 };

	ability_to_text(ability, txt_buffer);
	set_config_option("ability", txt_buffer);

	weapon_to_text(weapon, txt_buffer);
	set_config_option("weapon", txt_buffer);
	set_config_option("hosting", hosting ? "true" : "false");

	if (uninit_game_modes == NULL) {
		// Loads all modes in the game_modes directory
		struct dirent* dir_entry;
		DIR* game_mode_dir = opendir("game_modes/");

		if (game_mode_dir  == NULL) {
			fprintf(stderr, "Couldn't open game_modes/\n");
			exit(-1);

		}

		current_game_mode_index = 0;
		uint64_t index = 0;
		uninit_game_modes = malloc(256 * sizeof(UninitGameMode));

		while ((dir_entry = readdir(game_mode_dir)) != NULL) {
			char* full_path = malloc(256 + 11);
			memcpy(full_path, "game_modes/", 11);

			memcpy(full_path + 11, dir_entry->d_name, strlen(dir_entry->d_name) + 1);

			struct stat stats;

			if (stat(full_path, &stats) != 0) {
				fprintf(stderr, "Err calling stat on %s: %d\n", full_path, errno);
				exit(-1);

			}

			// Make sure it's a file and not something else, like a subdirectory
			if (!S_ISREG(stats.st_mode)) {
				continue;

			}


			if (strcmp(full_path, game_mode_file_name) == 0) {
				current_game_mode_index = index;

			}

			IM3Environment env = m3_NewEnvironment();
			IM3Runtime rt = m3_NewRuntime(env, 1000000, NULL); // 1 MB
			IM3Module module = m3_NewModule(env);

			FILE* wasm_file = fopen(full_path, "rb");

			if (wasm_file == NULL) {
				fprintf(stderr, "Couldn't open %s\n", full_path);
				perror("Error opening wasm file: ");
				exit(-1);

			}

			size_t wasm_file_size = get_file_size(wasm_file);

			uint8_t* wasm_file_bytes = malloc(wasm_file_size);

			size_t remaining_bytes_to_read = wasm_file_size;
			
			while (remaining_bytes_to_read > 0) {
				size_t bytes_read = fread(wasm_file_bytes + (wasm_file_size-remaining_bytes_to_read), 1, wasm_file_size, wasm_file);

				if (bytes_read == 0) {
					break;

				} else {
					remaining_bytes_to_read -= bytes_read;

				}

			}

			if (remaining_bytes_to_read > 0) {
				fprintf(stderr, "Failed to read entire wasm file\n");
				exit(-1);

			}

			fclose(wasm_file);

			m3_ParseModule(env, &module, wasm_file_bytes, wasm_file_size);
			m3_LoadModule(rt, module);

			UninitGameMode uninit_game_mode = {
				.full_path = full_path,
				.rt = rt,
			
			};

			if (m3_FindFunction(&uninit_game_mode.name, uninit_game_mode.rt, "name_ptr") != m3Err_none) {
				M3ErrorInfo error_info;
				m3_GetErrorInfo(uninit_game_mode.rt, &error_info);
				printf("Error getting name_ptr: %s\n", error_info.message);
				exit(-1);

			}

			uninit_game_modes[index] = uninit_game_mode;
			index += 1;

		}

		num_game_modes = index;
		closedir(game_mode_dir);

	}

	main_menu_state->ability = ability;
	main_menu_state->weapon = weapon;
	main_menu_state->username = username;
	main_menu_state->hosting = hosting;
	main_menu_state->ip_addr = ip_addr;
	main_menu_state->key_bindings = NULL;
	main_menu_state->game_mode_file_name = game_mode_file_name;

	main_menu_state->uninit_game_modes = uninit_game_modes;
	main_menu_state->current_game_mode_index = current_game_mode_index;
	main_menu_state->num_game_modes = num_game_modes;
	main_menu_state->game_mode_file_name = game_mode_file_name;

	set_config_option("username", username);
	set_config_option("ip_addr", ip_addr);
	set_config_option("game_mode_file_name", game_mode_file_name);

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


		char game_mode_button_text[11 + 256] = "Game Mode: ";

		uint64_t game_mode_name_ptr;

		UninitGameMode* current_game_mode = &main_menu_state->uninit_game_modes[main_menu_state->current_game_mode_index];

		m3_CallV(current_game_mode->name);
		m3_GetResultsV(current_game_mode->name, &game_mode_name_ptr);
		char* game_mode_name = (char*)&m3_GetMemory(current_game_mode->rt, NULL, 0)[game_mode_name_ptr];
		memcpy(game_mode_button_text + 11, game_mode_name, strlen(game_mode_name));

		const int game_mode_name_str_len = strlen(game_mode_button_text);
		const float game_mode_button_width =  BUTTON_WIDTH + 6.0 * (9 + game_mode_name_str_len);
		const float game_mode_button_x = (float)SCREEN_WIDTH / 2.0 - game_mode_button_width / 2.0; 
		const float game_mode_button_y = FIRST_BUTTON_HEIGHT + 5 * BUTTON_HEIGHT;

		Rectangle game_mode_rect = (Rectangle) { game_mode_button_x, game_mode_button_y, game_mode_button_width, BUTTON_HEIGHT }; 

		GuiDrawRectangle(game_mode_rect, 3, username_outline_color, RAYWHITE);
		GuiDrawText(game_mode_button_text, game_mode_rect, TEXT_ALIGN_CENTER, GRAY);

		if (GuiButton((Rectangle){ game_mode_button_x + game_mode_button_width, game_mode_button_y, increment_button_width, BUTTON_HEIGHT }, "->") ) {
			if (main_menu_state->current_game_mode_index == main_menu_state->num_game_modes - 1) {
				main_menu_state->current_game_mode_index = 0;

			} else {
				main_menu_state->current_game_mode_index += 1;

			}

			memcpy(main_menu_state->game_mode_file_name, main_menu_state->uninit_game_modes[main_menu_state->current_game_mode_index].full_path, strlen(main_menu_state->uninit_game_modes[main_menu_state->current_game_mode_index].full_path) + 1);

		}

		if (GuiButton((Rectangle){ game_mode_button_x - increment_button_width, game_mode_button_y, increment_button_width, BUTTON_HEIGHT }, "<-") ) {
			if (main_menu_state->current_game_mode_index == 0) {
				main_menu_state->current_game_mode_index = main_menu_state->num_game_modes - 1;

			} else {
				main_menu_state->current_game_mode_index -= 1;

			}

			memcpy(main_menu_state->game_mode_file_name, main_menu_state->uninit_game_modes[main_menu_state->current_game_mode_index].full_path, strlen(main_menu_state->uninit_game_modes[main_menu_state->current_game_mode_index].full_path) + 1);

		}


		char hosting_text[16] = { "Hosting: " };

		if (main_menu_state->hosting) {
			memcpy(hosting_text + 9, "true", 5);

		} else {
			memcpy(hosting_text + 9, "false", 6);

		}

		if (GuiButton((Rectangle) { CENTER_BUTTON_X, FIRST_BUTTON_HEIGHT + 6 * BUTTON_HEIGHT + SPACE_BETWEEEN_BUTTONS * 2, BUTTON_WIDTH, BUTTON_HEIGHT }, hosting_text)) {
			main_menu_state->hosting = !main_menu_state->hosting;

		}

		char ip_addr_text_to_draw[17] = { 0 };

		if (!main_menu_state->hosting) {
			Rectangle ip_addr_rect = name_textbox_rect;
			ip_addr_rect.y = FIRST_BUTTON_HEIGHT + 7 * BUTTON_HEIGHT + SPACE_BETWEEEN_BUTTONS * 2;

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
