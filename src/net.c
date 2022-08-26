#include "hashset.h"
#include "weapon.h"
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

#ifdef __unix__
#include <netinet/tcp.h>
#include <netinet/udp.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#endif

#include <unistd.h>
#include <fcntl.h>

#include "minimal_state_info.h"
#include "net.h"
#include "player.h"

#define SERVER_HOST_PORT 42069

// Returns true if it binded sucessfully
bool bind_rand_port(int sockfd, struct sockaddr_in* addr) {
	for (uint16_t port = 1024; port < 65535; port += 1) {
		addr->sin_port = htons(port);

		if (bind(sockfd, (struct sockaddr*)addr, sizeof(struct sockaddr_in)) >= 0) {
			return true;

		}

	}

	return false;

}

bool sockaddrs_are_eq(struct sockaddr_in* addr1, struct sockaddr_in* addr2) {
	return addr1->sin_addr.s_addr == addr2->sin_addr.s_addr && addr1->sin_port == addr2->sin_port;

}

NetworkInfo init_networking(bool hosting, const char* ip_addr, Player* my_player) {
	#ifdef __WIN32__
	WSADATA wsadata;
	if (WSAStartup(MAKEWORD(1,1), &wsadata) == SOCKET_ERROR) {
		printf("Error creating socket.");
		exit(-1);
	}
	#endif


	int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	struct sockaddr_in servaddr = { 0 };

	if (sockfd == -1) {
		fprintf(stderr, "Socket creation failed\n");
		exit(-1);

	}

	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = inet_addr("0.0.0.0");

	struct sockaddr_in sock_to_send_to;
	socklen_t addr_len = 0;

	if (hosting) {
		servaddr.sin_port = htons(SERVER_HOST_PORT);

		if (bind(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) == -1) {
			fprintf(stderr, "Failed to bind to ipaddr\n");
			exit(-1);

		}

	} else {
		sock_to_send_to.sin_family = AF_INET;
		sock_to_send_to.sin_addr.s_addr = inet_addr(ip_addr);
		sock_to_send_to.sin_port = htons(SERVER_HOST_PORT);

		addr_len = sizeof(struct sockaddr_in);

		if (connect(sockfd, (struct sockaddr*)&sock_to_send_to, addr_len) == -1) {
			fprintf(stderr, "Failed to connect to %s\n", ip_addr);
			exit(-1);

		}

	}

	// Set the socket to be non blocking
	#ifdef __WIN32__
		u_long mode = 1;
		int result = ioctlsocket(sockfd, FIONBIO, &mode);

		if (result != NO_ERROR) {
			printf("setting non-blocking socket failed\n");

		}
	#endif

	#ifdef __unix__
		fcntl(sockfd, F_SETFL, O_NONBLOCK);
	#endif

	my_player->assigned_id = true;

	Addr addr = {
		.sockaddr = sock_to_send_to,
		.addr_len = addr_len,

	};

	HashSet addrs_to_send_to = new_hashset();

	if (addr.addr_len > 0) {
		hashset_insert(&addr, sizeof(addr), &addrs_to_send_to);

	}

	NetworkInfo network_info = {
		.socket = sockfd,
		.is_server = hosting,
		.addrs_to_send_to = addrs_to_send_to,

	};

	return network_info;

}

void process_net_packets(const NetPlayer* buffer, Player* players, uint8_t num_players) {
	const MinimalPlayerInfo* minimal_player_info = &buffer->minimal_player_info;
	bool shooting = buffer->shooting;

	if (players[0].id == minimal_player_info->id) {
		return;

	}

	Player* net_player = NULL;

	// First, look to see if the net player has been assigned an id
	for (uint8_t i = 0; i < num_players; i += 1) {
		Player* player = &players[i];

		if (player->assigned_id) {
			if (player->id == minimal_player_info->id) {
				net_player = player;
				break;

			}
		}

	}

	if (net_player == NULL) {
		// If no player was found, then assign an id 
		for (uint8_t i = 0; i < num_players; i += 1) {
			Player* player = &players[i];

			if (!player->assigned_id) {
				player->assigned_id = true;
				player->id = minimal_player_info->id;
				net_player = player;
				break;

			}

		}

	}

	if (net_player == NULL) {
		fprintf(stderr, "Error finding an available player!\n");
		//exit(-1);
		return;

	}

	net_player->is_net_player = true;
	net_player->ammo = minimal_player_info->ammo;
	net_player->pos_x = minimal_player_info->pos_x;
	net_player->pos_y = minimal_player_info->pos_y;
	net_player->health = minimal_player_info->health;
	net_player->direction = minimal_player_info->direction;
	net_player->weapon = minimal_player_info->weapon;
	net_player->ability = minimal_player_info->ability;
	net_player->shooting = shooting;

}

int handle_networking(NetworkInfo* network_info, Player* players, uint8_t num_players) {
	const int buffer_len = sizeof(NetPlayer);
	NetPlayer* buffer = malloc(buffer_len * num_players);

	Player* my_player = &players[0];

	if (network_info->is_server) {
		const Addr* addrs_to_send_to = network_info->addrs_to_send_to.item_list;

		for (uint8_t i = 0; i < num_players; i += 1) {
			buffer[i].minimal_player_info = get_minimal_player_info(&players[i]);
			buffer[i].shooting = players[i].shooting;

		}

		for (uint64_t i = 0; i < network_info->addrs_to_send_to.num_items; i += 1) {
			const Addr* addr_to_send_to = &addrs_to_send_to[i];

			sendto(network_info->socket, buffer, buffer_len * num_players, 0, (struct sockaddr*)&addr_to_send_to->sockaddr, addr_to_send_to->addr_len);

		}

	} else {
		buffer[0].minimal_player_info = get_minimal_player_info(my_player);
		buffer[0].shooting = my_player->shooting;

		send(network_info->socket, buffer, sizeof(NetPlayer), 0);

	}

	int total_bytes_read = 0;

	if (network_info->is_server) {
		Addr addr = {
			.addr_len = sizeof(addr.sockaddr),

		};

		int bytes_read = recvfrom(network_info->socket, buffer, sizeof(NetPlayer), 0, (struct sockaddr*)&addr.sockaddr, &addr.addr_len);

		int buffer_index = 0;

		while (bytes_read > 0) {
			hashset_insert(&addr, sizeof(addr), &network_info->addrs_to_send_to);
			total_bytes_read += bytes_read;

			buffer_index += 1;
			buffer = realloc(buffer, (buffer_index + 2) * sizeof(NetPlayer));

			bytes_read = recvfrom(network_info->socket, &buffer[buffer_index], sizeof(NetPlayer), 0, (struct sockaddr*)&addr.sockaddr, &addr.addr_len);

		}

	} else {
		int bytes_read = recv(network_info->socket, buffer, buffer_len, 0); 
		int buffer_index = 0;

		while (bytes_read > 0) {
			buffer_index += 1;
			total_bytes_read += bytes_read;
			
			buffer = realloc(buffer, (buffer_index + 2) * sizeof(NetPlayer));
			
			bytes_read = recv(network_info->socket, &buffer[buffer_index], buffer_len, 0); 
		}
		
	}


	if (total_bytes_read > 0) {
		uint64_t num_net_players = total_bytes_read / (sizeof(NetPlayer) - 1);

		for (uint64_t i = 0; i < num_net_players; i += 1) {
			process_net_packets(&buffer[i], players, num_players);
			

		}

	}

	free(buffer);

	return 0;

}

