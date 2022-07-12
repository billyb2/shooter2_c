#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#include "camera.h"
#include "bots.h"
#include "map.h"
#include "math.h"
#include "player.h"
#include "input.h"
#include "projectile.h"

#include "include/raylib.h"
#include "include/wasm.h"
#include "include/wasmer.h"

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
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "shooter2");

	Camera2D camera = { 0 };
	camera.offset = (Vector2){ (float)SCREEN_WIDTH / 2.0, (float)SCREEN_HEIGHT / 2.0 };
	camera.rotation = 0.0;
	camera.zoom = 1.0;

	Map map = {
		.size_x = 1000,
		.size_y = 1000,
	};

	WasmData wasm_data = setup_wasm();

	uint8_t num_players = 3;
	Player* players = malloc(num_players * sizeof(Player));
	
	uint16_t num_projectiles = 0;
	Projectile* projectiles = NULL;

	players[0] = new_player(200, 200, Teleporation);
	players[1] = new_player(200, 100, Teleporation);
	players[2] = new_player(200, 300, Teleporation);


	if (!setup_bot("bots/simple_bot.wasm", &wasm_data, 1)) {
		printf("Failed to setup bot");
		return 1;

	}

	SetTargetFPS(60);

	// Display the window until ESC is pressed
	while (!WindowShouldClose()) {
		update_player_cooldowns(players, num_players);

		player_input(&players[0], &DEFAULT_KEY_BINDINGS, &projectiles, &num_projectiles, &map);
		update_bot_info(players, num_players, &wasm_data, &projectiles, &num_projectiles, &map);
		update_projectiles(&projectiles, &num_projectiles, players, num_players, &map);
		move_camera(&camera, &map, players[0].pos_x, players[0].pos_y);

		BeginDrawing();
			ClearBackground(RAYWHITE);
			BeginMode2D(camera);

			for (uint8_t i = 0; i < num_players; i += 1) {
				Player* player = &players[i];

				Vector4 color_normalized = {
					.x = 1.0,
					.y = 0.0,
					.z = 0.0,
					.w = (float)(player->health) / 500.0,

				};

				Color color =  ColorFromNormalized(color_normalized);

				DrawCircle((float)player->pos_x, (float)player->pos_y, PLAYER_SIZE, color);

			}

			for (uint16_t i = 0; i < num_projectiles; i += 1) {
				Projectile* projectile = &projectiles[i];
				DrawCircle((float)projectile->pos_x, (float)projectile->pos_y, projectile->size, BLACK);
			}

		EndDrawing();
	}

    CloseWindow();

	free(players);
	free(projectiles);
	free(wasm_data.bot_data_list);

    return 0;
}


