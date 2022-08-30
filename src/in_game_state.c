#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "camera.h"
#include "input.h"
#include "player.h"
#include "player_ability.h"
#include "render.h"
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

	if (*game_page == MainMenu) {
		MainMenuState* main_menu_state = &game_state->main_menu_state;

		if (main_menu_state->key_bindings != NULL) {
			key_bindings = *main_menu_state->key_bindings;

		}

		hosting = main_menu_state->hosting;
		username = main_menu_state->username;
		ip_addr = main_menu_state->ip_addr;

	}

	if (username == NULL) {
		username = malloc(20);
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

	uint8_t num_players = 2;
	Player* players = malloc(num_players * sizeof(Player));
	
	uint16_t num_projectiles = 0;
	Projectile* projectiles = NULL;

	for (uint8_t i = 0; i < num_players; i += 1) {
		char* new_player_username = NULL;

		if (i == 0) {
			new_player_username = username;

		} 

		players[i] = new_player(game_state->main_menu_state.ability, game_state->main_menu_state.weapon, Grenade, &map, new_player_username);

	}

	NetworkInfo network_info = init_networking(hosting, game_state->main_menu_state.ip_addr, &players[0]);

	InGameState new_game_state = {
		.default_ability = game_state->main_menu_state.ability,
		.default_weapon = game_state->main_menu_state.weapon,
		.players = players,
		.num_players = num_players,

		.projectiles = projectiles,
		.num_projectiles = num_projectiles,

		.camera = camera,
		
		.map = map,
		.network_info = network_info,
		.key_bindings = key_bindings,

	};

	game_state->in_game_state = new_game_state;
	*game_page = InGame;

}

void run_in_game_state(GamePage* game_page, GameState* game_state) {
	InGameState* in_game_state = &game_state->in_game_state;

	update_player_cooldowns(in_game_state->players, in_game_state->num_players);

	player_input(&in_game_state->players[0], &in_game_state->key_bindings, &in_game_state->map, true);
	handle_networking(&in_game_state->network_info, in_game_state->players, in_game_state->num_players);
	use_weapons(in_game_state->players, in_game_state->num_players, &in_game_state->projectiles, &in_game_state->num_projectiles);
	update_projectiles(&in_game_state->projectiles, &in_game_state->num_projectiles, in_game_state->players, in_game_state->num_players, &in_game_state->map);
	move_camera(&in_game_state->camera, &in_game_state->map, in_game_state->players[0].pos_x, in_game_state->players[0].pos_y);

	respawn_players(in_game_state->players, in_game_state->num_players);
	
	render(in_game_state->camera, in_game_state->players, in_game_state->num_players, in_game_state->projectiles, in_game_state->num_projectiles, &in_game_state->map);

}
