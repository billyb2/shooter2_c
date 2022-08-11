#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#include "camera.h"
#include "map.h"
#include "math.h"
#include "player.h"
#include "input.h"
#include "render.h"
#include "projectile.h"
#include "weapon.h"
#include "rand.h"

#include "include/raylib.h"

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 450;


const KeyBindings DEFAULT_KEY_BINDINGS = {
	.up = KEY_W,
	.down = KEY_S,
	.left = KEY_A,
	.right = KEY_D,
	.ability = KEY_Q,

};

int main() {
	init_fast_rand();
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "shooter2");

	Camera2D camera = { 0 };
	camera.offset = (Vector2){ (float)SCREEN_WIDTH / 2.0, (float)SCREEN_HEIGHT / 2.0 };
	camera.rotation = 0.0;
	camera.zoom = 1.0;

	Map map = {
		.size_x = 1000,
		.size_y = 1000,
	};

	uint8_t num_players = 2;
	Player* players = malloc(num_players * sizeof(Player));
	
	uint16_t num_projectiles = 0;
	Projectile* projectiles = NULL;

	players[0] = new_player(200, 200, Stim, Shotgun, Grenade);
	players[1] = new_player(200, 100, Warp, AssaultRifle, Grenade);

	SetTargetFPS(60);

	// Display the window until ESC is pressed
	while (!WindowShouldClose()) {
		update_player_cooldowns(players, num_players);

		player_input(&players[0], &camera, &DEFAULT_KEY_BINDINGS, &projectiles, &num_projectiles, &map, true);

		update_projectiles(&projectiles, &num_projectiles, players, num_players, &map);
		move_camera(&camera, &map, players[0].pos_x, players[0].pos_y);

		render(camera, players, num_players, projectiles, num_projectiles, &map);

	}

    CloseWindow();

	free(players);
	free(projectiles);

    return 0;
}


