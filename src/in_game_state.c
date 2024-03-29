#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <ctype.h>

#include "bots.h"
#include "camera.h"
#include "game_mode.h"
#include "input.h"
#include "player.h"
#include "player_ability.h"
#include "render.h"
#include "net.h"
#include "game_state.h"
#include "rand.h"
#include "in_game_state.h"
#include "weapon.h"


const KeyBindings DEFAULT_KEY_BINDINGS = {
	.up = KEY_W,
	.down = KEY_S,
	.left = KEY_A,
	.right = KEY_D,
	.ability = KEY_Q,

};

void enter_in_game(GamePage* game_page, GameState* game_state) {
	// If we just came from the main menu
	KeyBindings key_bindings = DEFAULT_KEY_BINDINGS;
	char* username = NULL;
	char* ip_addr = NULL;
	bool hosting = false;

	UninitGameMode* uninit_game_modes = NULL;
	uint64_t num_game_modes;
	uint64_t current_game_mode_index = 0;
	uint8_t num_bots = 0;

	if (*game_page == MainMenu) {
		MainMenuState* main_menu_state = &game_state->main_menu_state;

		if (main_menu_state->key_bindings != NULL) {
			key_bindings = *main_menu_state->key_bindings;

		}

		hosting = main_menu_state->hosting;
		username = main_menu_state->username;
		ip_addr = main_menu_state->ip_addr;
		
		uninit_game_modes = main_menu_state->uninit_game_modes;
		num_bots = main_menu_state->num_bots;
		num_game_modes = main_menu_state->num_game_modes;
		current_game_mode_index = main_menu_state->current_game_mode_index;

	}

	if (username == NULL) {
		username = calloc(1, 20);
		memcpy(username, "shooter2_c player", 18);

	}

	if (ip_addr == NULL) {
		ip_addr = malloc(16);
		memcpy(ip_addr, "127.0.0.1", 10);

	}


	Camera2D camera = { 0 };
	camera.offset = (Vector2){ (float)SCREEN_WIDTH / 2.0, (float)SCREEN_HEIGHT / 2.0 };
	camera.rotation = 0.0;
	camera.zoom = 1.0;

	Map map = new_map("maps/untitled.custom"); 
	GameModeData game_mode_data = init_gamemode_data(uninit_game_modes[current_game_mode_index].rt, &map); 

	uint8_t num_players = 255;
	Player* players = malloc(num_players * sizeof(Player));
	
	uint16_t num_projectiles = 0;
	Projectile* projectiles = NULL;

	for (uint8_t i = 0; i < num_players; i += 1) {
		char* new_player_username = NULL;

		if (i == 0) {
			new_player_username = username;

		} 

		players[i] = new_player(game_state->main_menu_state.ability, game_state->main_menu_state.weapon, Grenade, &map, new_player_username, &game_mode_data, NULL);

	}

	if (hosting) {
		// Start at the second bot since the first one is always the main player
		for (uint8_t i = 1; i < game_state->main_menu_state.num_bots + 1; i += 1) {
			Player* player = &players[i];
			Ability ability;
			Weapon weapon;

			Bot* bot_data = new_bot("bots/simple.wasm", &ability, &weapon);

			char* username = malloc(20);
			sprintf(username, "Bot %u", i);
			*player = new_player(ability, weapon, Grenade, &map, username, &game_mode_data, bot_data);

			player->assigned_id = true;
			player->id = rand();
			player->health = PLAYER_MAX_HEALTH;

			add_player_to_team(player, &map, &game_mode_data);

		}
	}


	NetworkInfo network_info = init_networking(hosting, game_state->main_menu_state.ip_addr, &players[0], &game_mode_data, &map);


	InGameState new_game_state = {
		.default_ability = game_state->main_menu_state.ability,
		.default_weapon = game_state->main_menu_state.weapon,
		.players = players,
		.num_players = num_players,

		.projectiles = projectiles,
		.num_projectiles = num_projectiles,

		.game_mode_data = game_mode_data,

		.camera = camera,
		
		.map = map,
		.network_info = network_info,
		.key_bindings = key_bindings,

		.uninit_game_modes = uninit_game_modes,
		.num_game_modes = num_game_modes,
		.current_game_mode_index = current_game_mode_index,

	};

	game_state->in_game_state = new_game_state;
	*game_page = InGame;

}


void exit_in_game(GameState* game_state, GamePage* game_page, GamePage new_game_page) {
	InGameState* in_game_state = &game_state->in_game_state;	

	free(in_game_state->map.objects);
	free(in_game_state->projectiles);

	free(in_game_state->network_info.addrs_to_send_to.item_list);

	for (uint8_t i = 0; i < in_game_state->game_mode_data.num_teams; i += 1) {
		//free(in_game_state->game_mode_data.teams->players);

	}

	//free(in_game_state->game_mode_data.teams);

	#ifdef __unix__
	int t = true;
	setsockopt(in_game_state->network_info.socket, SOL_SOCKET,SO_REUSEADDR, &t, sizeof(int));

	if (close(in_game_state->network_info.socket) != 0) {
		fprintf(stderr, "Failed to shutdown socket\n");
		printf("errno: %d\n", errno);

	}
	#endif

	#ifdef __WIN32__
	if (closesocket(in_game_state->network_info.socket) != 0) {
		fprintf(stderr, "Failed to shutdown socket\n");
		printf("errno: %d\n", errno);

	}
	#endif

	enter_state(game_page, game_state, new_game_page);

}

void run_in_game_state(GamePage* game_page, GameState* game_state) {
	InGameState* in_game_state = &game_state->in_game_state;

	update_player_cooldowns(in_game_state->players, in_game_state->num_players);

	player_input(&in_game_state->players[0], &in_game_state->key_bindings, &in_game_state->map, true);
	sync_teams_bots(in_game_state->players, in_game_state->num_players, &in_game_state->game_mode_data);
	run_bots(in_game_state->players, in_game_state->num_players, &in_game_state->map);
	handle_networking(&in_game_state->network_info, in_game_state->players, in_game_state->num_players, &in_game_state->game_mode_data, &in_game_state->map);
	use_weapons(in_game_state->players, in_game_state->num_players, &in_game_state->projectiles, &in_game_state->num_projectiles);
	update_projectiles(&in_game_state->projectiles, &in_game_state->num_projectiles, in_game_state->players, in_game_state->num_players, &in_game_state->map, &in_game_state->game_mode_data);
	move_camera(&in_game_state->camera, &in_game_state->map, in_game_state->players[0].pos_x, in_game_state->players[0].pos_y);

	respawn_players(in_game_state->players, in_game_state->num_players, &in_game_state->map, &in_game_state->game_mode_data);

	sync_players_to_teams(in_game_state->players, in_game_state->num_players, &in_game_state->game_mode_data);


	bool player_won = calculate_scores(&in_game_state->winning_team, &in_game_state->game_mode_data);

	if (in_game_state->countdown_frames_to_main_menu == 0) {
		if (player_won) {
			in_game_state->countdown_frames_to_main_menu = 5 * 60;

		}

	} else {
		in_game_state->countdown_frames_to_main_menu -= 1;

	}

	render(in_game_state->camera, in_game_state->players, in_game_state->num_players, &in_game_state->game_mode_data, in_game_state->projectiles, in_game_state->num_projectiles, &in_game_state->map, in_game_state->winning_team, player_won);	

	if (player_won && in_game_state->countdown_frames_to_main_menu == 0) {
		exit_in_game(game_state, game_page, MainMenu);

	}

}

