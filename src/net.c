#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <poll.h>
#include <assert.h>

#include "minimal_player_info.h"
#include "player.h"
#include "net.h"
#include "projectile.h"

void send_info(Player* players, uint8_t num_players, uint16_t* num_projectiles_ptr, Projectile** projectiles_ptr, const Map* map, const int sockfd, struct pollfd* pfds) {

	uint16_t num_projectiles = *num_projectiles_ptr;
	int len = sizeof(num_players) + sizeof(float) * num_players * 2 + sizeof(num_projectiles) + sizeof(float) * num_projectiles * (2 + 1);

	unsigned char* msg = malloc(len);

	memcpy(msg, &num_players, sizeof(num_players));

	for (uint8_t i = 0; i < num_players; i += 1) {
		unsigned char* pos_index = msg + sizeof(num_players) + (i * 8);

		unsigned char* pos_x = pos_index;
		unsigned char* pos_y = pos_index + sizeof(float); 

		memcpy(pos_x, &players[i].pos_x, sizeof(float));
		memcpy(pos_y, &players[i].pos_y, sizeof(float));

	}

	unsigned char* projectiles_start = msg + num_players * 8 + 1;
	memcpy(projectiles_start, &num_projectiles, sizeof(num_projectiles));

	for (uint16_t i = 0; i < num_projectiles; i += 1) {
		const Projectile* projectiles = *projectiles_ptr;
		unsigned char* pos_index = projectiles_start + sizeof(num_projectiles) + (i * 9);

		unsigned char* pos_x = pos_index;
		unsigned char* pos_y = pos_index + sizeof(float); 
		unsigned char* size = pos_index + sizeof(float) * 2;

		memcpy(pos_x, &projectiles[i].pos_x, sizeof(float));
		memcpy(pos_y, &projectiles[i].pos_y, sizeof(float));
		memcpy(size, &projectiles[i].size, sizeof(uint8_t));

	}

	int bytes_sent = send(sockfd, msg, len, 0);

	free(msg);

	// Next, read input
	char buf[5] = { 0 };

	int num_events = poll(pfds, 1, 10);
	
	if (num_events == 0) {
		return;

	}

	int bytes_read = recv(sockfd, buf, 5, 0);

	char direction = buf[0];

	float* angle = (float*)&buf[1];

	if (bytes_read > 0) {
		Player* player = &players[1];

		if (direction == 0) {
			return;

		}

		
		PlayerMovementInfo movement_info = { 
			.x_axis = NoneX,
			.y_axis = NoneY,
		};

		if (direction == 1) {
			movement_info.y_axis = Up;

		} else if (direction == 2) {
			movement_info.y_axis = Down;

		} else if (direction == 3) {
			movement_info.x_axis = Left;

		} else if (direction == 4) {
			movement_info.x_axis = Right;

		}

		move_player(player, movement_info, map);
		player->direction = *angle;
		//shoot(projectiles_ptr, num_projectiles_ptr, player, *angle);

	}

}
