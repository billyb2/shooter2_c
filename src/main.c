#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#include "bots.h"
#include "math.h"
#include "player.h"
#include "input.h"

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

};

int main() {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "shooter2");

	WasmData wasm_data = setup_wasm();

	if (!setup_bot("bots/simple_bot.wasm", &wasm_data)) {
		printf("Failed to setup bot");
		return 1;

	}

	uint8_t num_players = 2;
	Player* players = malloc(num_players * sizeof(Player));

	players[0] = new_player(200, 200);
	players[1] = new_player(200, 100);

    SetTargetFPS(60);

	wasm_val_t args_val[0];
	wasm_val_vec_t args = WASM_ARRAY_VEC(args_val);

	// Display the window until ESC is pressed
	while (!WindowShouldClose()) {
		player_input(&players[0], &DEFAULT_KEY_BINDINGS);
		update_bot_info(players, num_players, &wasm_data);

		for (uint16_t i = 0; i < wasm_data.num_bots; i += 1) {
			BotWasmData* bot = &wasm_data.bot_data_list[i];

			wasm_val_t results_val[1] = { WASM_INIT_VAL };
			wasm_val_vec_t results = WASM_ARRAY_VEC(results_val);

			if (wasm_func_call(bot->bot_func, &args, &results)) {
				printf("Error calling func\n");
				return 1;

			}

			act_on_bot(results.data[0].of.i32, &players[i + 1]);

		}

		
		BeginDrawing();
			
			ClearBackground(RAYWHITE);

			for (int i = 0; i < num_players; i += 1) {
				Player player = players[i];
				DrawCircle((float)player.pos_x, (float)player.pos_y, 10.0, RED);

			}

		EndDrawing();
	}

    CloseWindow();

	free(players);

    return 0;
}


