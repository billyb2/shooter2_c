#define MAIN_C

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include "camera.h"
#include "map.h"
#include "math.h"
#include "net_info.h"
#include "player.h"
#include "input.h"
#include "render.h"
#include "projectile.h"
#include "net.h"
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

int main(const int argc, const char** argv) {
	#define MAX_IPv4_STR_LEN 15
	char ip_str[16] = { 0 };

	bool hosting;

	if (argc == 2) {
		if (strcmp(argv[1], "true") == 0) {
			printf("Hosting server\n");
			hosting = true;

		} else {
			printf("Server IP: ");
			scanf("%s", ip_str);
			printf("Connecting to %s\n", ip_str);
			hosting = false;

		}

	} else {
		printf("Usage: ./main [hosting]\n");
		exit(-1);
	
	}

	init_fast_rand();
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "shooter2");

	Camera2D camera = { 0 };
	camera.offset = (Vector2){ (float)SCREEN_WIDTH / 2.0, (float)SCREEN_HEIGHT / 2.0 };
	camera.rotation = 0.0;
	camera.zoom = 1.0;

	Map map = new_map("maps/untitled.custom"); 

	uint8_t num_players = 2;
	Player* players = malloc(num_players * sizeof(Player));
	
	uint16_t num_projectiles = 0;
	Projectile* projectiles = NULL;

	players[0] = new_player(Warp, Shotgun, Grenade, &map);
	players[1] = new_player(Warp, Shotgun, Grenade, &map);

	SetTargetFPS(60);

	NetworkInfo network_info = init_networking(hosting, ip_str, &players[0]);


	// Display the window until ESC is pressed
	while (!WindowShouldClose()) {
		update_player_cooldowns(players, num_players);

		player_input(&players[0], &DEFAULT_KEY_BINDINGS, &map, true);
		use_weapons(players, num_players, &projectiles, &num_projectiles);
		update_projectiles(&projectiles, &num_projectiles, players, num_players, &map);
		move_camera(&camera, &map, players[0].pos_x, players[0].pos_y);

		handle_networking(&network_info, players, num_players);
		respawn_players(players, num_players);
		

		render(camera, players, num_players, projectiles, num_projectiles, &map);

	}

    CloseWindow();

	free(players);
	free(projectiles);

    return 0;
}


