#ifndef GAME_STATE_H
#define GAME_STATE_H
#include "game_mode.h"
#include "include/raylib.h"
#include "net.h"
#include "player.h"
#include "map.h"
#include "input.h"
#include "projectile.h"


typedef enum GamePage {
	MainMenu,
	InGame,
	Settings,

} GamePage;

typedef struct InGameState {
	Player* players;
	Ability default_ability;
	Weapon default_weapon;
	uint8_t num_players;

	Projectile* projectiles;
	uint16_t num_projectiles;

	const Team* winning_team;

	GameModeData game_mode_data;

	Camera2D camera;

	Map map;
	NetworkInfo network_info;

	KeyBindings key_bindings;

	uint16_t countdown_frames_to_main_menu;


} InGameState;

typedef struct MainMenuState {
	KeyBindings* key_bindings;
	char* username;
	Ability ability;
	Weapon weapon;
	bool hosting;
	char* ip_addr;

} MainMenuState;

typedef union GameState {
	InGameState in_game_state;
	MainMenuState main_menu_state;

} GameState;


void enter_state(GamePage* game_page, GameState* game_state, GamePage new_game_page);
void run_state(GamePage* game_page, GameState* game_state);

#endif
