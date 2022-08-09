#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#include "camera.h"
#include "bot_handler.h"
//#include "bots.h"
#include "map.h"
#include "math.h"
#include "minimal_player_info.h"
#include "player.h"
#include "input.h"
#include "render.h"
#include "net.h"
#include "projectile.h"

#include <stdio.h>

#include <netdb.h>
#include <sys/socket.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <poll.h>

#include "include/raylib.h"
#include "include/wasm.h"
#include "include/wasmer.h"
#include "weapon.h"

const float SCREEN_WIDTH = 800;
const float SCREEN_HEIGHT = 450;

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
	camera.offset = (Vector2){ SCREEN_WIDTH / 2.0, SCREEN_HEIGHT / 2.0 };
	camera.rotation = 0.0;
	camera.zoom = 1.0;

	Map map = new_map("maps/fake_map.map");

	uint8_t num_players = 2;
	Player* players = malloc(num_players * sizeof(Player));
	
	uint16_t num_projectiles = 0;
	Projectile* projectiles = NULL;

	players[0] = new_player(200, 200, Teleporation, Shotgun);
	players[1] = new_player(200, 100, Teleporation, AssaultRifle);

	/*if (setup_bot("bots/aggro.wasm", 1) != 0) {
		printf("Failed to setup bot");
		return 1;

	}*/

	SetTargetFPS(60);

	int flag = 0;
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	setsockopt(sockfd, IPPROTO_TCP, TCP_NODELAY, (char *) &flag, sizeof(int));

	struct pollfd pfds[1];

	pfds[0].fd = sockfd;
	pfds[0].events = POLLIN;


	struct sockaddr_in servaddr = { 0 };

	if (sockfd == -1) {
		fprintf(stderr, "Socket creation failed");
		exit(-1);

	}

	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	servaddr.sin_port = htons(42069);


	if (connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) != 0) {
        fprintf(stderr, "connection with the server failed...\n");
        exit(-1);

    }

	// Display the window until ESC is pressed
	while (!WindowShouldClose()) {
		update_player_cooldowns(players, num_players);

		player_input(&players[0], &camera, &DEFAULT_KEY_BINDINGS, &projectiles, &num_projectiles, &map, true);

		
		send_info(players, num_players, &num_projectiles, &projectiles, &map, sockfd, pfds);
		update_bot_info(players, num_players, &projectiles, &num_projectiles, &map);
		update_projectiles(&projectiles, &num_projectiles, players, num_players, &map);
		move_camera(&camera, &map, players[0].pos_x, players[0].pos_y);

		render(camera, players, num_players, projectiles, num_projectiles, &map);

	}

    CloseWindow();
	close(sockfd);

	free(players);
	free(projectiles);
	//free(wasm_data.bot_data_list);

    return 0;
}


