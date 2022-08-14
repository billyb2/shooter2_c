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

#ifdef __WIN32__
#include <winsock2.h>
#endif

#include <unistd.h>
#include <fcntl.h>

#include "minimal_state_info.h"
#include "net.h"
#include "player.h"

#define SERVER_HOST_PORT 42069

NetworkInfo init_networking(bool hosting, const char* ip_addr) {
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

	struct sockaddr_in* socks_to_send_to = NULL;

	socklen_t addr_len = 0;

	socks_to_send_to = calloc(1, sizeof(struct sockaddr_in));

	if (hosting) {
		servaddr.sin_port = htons(SERVER_HOST_PORT);
		socks_to_send_to->sin_port = htons(SERVER_HOST_PORT);

		if (bind(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) == -1) {
			fprintf(stderr, "Failed to bind to ipaddr\n");
			exit(-1);

		}

	} else {
		servaddr.sin_port = htons(SERVER_HOST_PORT);

		socks_to_send_to->sin_family = AF_INET;
		socks_to_send_to->sin_addr.s_addr = inet_addr(ip_addr);
		socks_to_send_to->sin_port = htons(SERVER_HOST_PORT);

		addr_len = sizeof(struct sockaddr_in);

		if (connect(sockfd,  (struct sockaddr*)socks_to_send_to, addr_len) == -1) {
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

	NetworkInfo network_info = {
		.socket = sockfd,
		.is_server = hosting,
		.socks_to_send_to = socks_to_send_to,
		.addr_len = addr_len,

	};

	return network_info;

}

void process_net_packets(char* buffer, int bytes_read, Player* net_player) {
	MinimalPlayerInfo* minimal_player_info = (MinimalPlayerInfo*)buffer;

	net_player->pos_x = minimal_player_info->pos_x;
	net_player->pos_y = minimal_player_info->pos_y;
	net_player->direction = minimal_player_info->direction;

	memcpy(&net_player->shooting, buffer + sizeof(MinimalPlayerInfo), sizeof(bool));

	
}


int handle_client_networking(NetworkInfo* network_info, const Player* my_player, Player* other_player) {
	int buffer_len = sizeof(MinimalPlayerInfo) + sizeof(bool);
	char* buffer = malloc(buffer_len);

	MinimalPlayerInfo minimal_player_info = get_minimal_player_info(my_player);

	memcpy(buffer, &minimal_player_info, sizeof(minimal_player_info));
	memcpy(buffer + sizeof(minimal_player_info), &my_player->shooting, sizeof(my_player->shooting));

	if (send(network_info->socket, buffer, buffer_len, 0) == -1) {
		fprintf(stderr, "Error sending\n");

	}

	int bytes_read = recv(network_info->socket, buffer, buffer_len, 0); 

	if (bytes_read > 0) {
		process_net_packets(buffer, bytes_read, other_player);

	}


	free(buffer);

	return 0;

}

int handle_server_networking(NetworkInfo* network_info, uint8_t my_player_id, Player* players, uint8_t num_players) {
	char* buffer = malloc(sizeof(MinimalPlayerInfo) + sizeof(bool));

	int bytes_to_send = sizeof(MinimalPlayerInfo) + sizeof(bool);

	int bytes_read = recvfrom(network_info->socket, buffer, bytes_to_send, 0, (struct sockaddr*)&network_info->socks_to_send_to, &network_info->addr_len);

	if (bytes_read > 0) {
		process_net_packets(buffer, bytes_read, &players[1]);
	
	}


	MinimalPlayerInfo minimal_player_info = get_minimal_player_info(&players[0]);

	memcpy(buffer, &minimal_player_info, sizeof(minimal_player_info));
	memcpy(buffer + sizeof(minimal_player_info), &players[0].shooting, sizeof(bool));


	if (sendto(network_info->socket, buffer, bytes_to_send, 0, (struct sockaddr*)&network_info->socks_to_send_to, network_info->addr_len) == -1) {
		//fprintf(stderr, "Error sending data back\n");
		//exit(-1);

	}
	
	free(buffer);

	return 0;

}

