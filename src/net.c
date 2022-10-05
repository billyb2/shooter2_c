#include "game_mode.h"
#include "hashset.h"
#include "weapon.h"
#include "projectile.h"
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



NetworkInfo init_networking(bool hosting, const char* ip_addr, Player* my_player, GameModeData* game_mode_data, const Map* map) {
	#ifdef __WIN32__
	WSADATA wsadata;
	if (WSAStartup(MAKEWORD(2,2), &wsadata) != 0) {
		printf("Error creating socket.");
		exit(-1);
	}
	#endif


	int sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
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

		if (bind(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) == INVALID_SOCKET) {
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
		unsigned long mode = 1;
		int result = ioctlsocket(sockfd, FIONBIO, &mode);

		if (result != NO_ERROR) {
			printf("setting non-blocking socket failed\n");

		}
	#endif

	#ifdef __unix__
		fcntl(sockfd, F_SETFL, O_NONBLOCK);
	#endif

	my_player->assigned_id = true;
	my_player->health = PLAYER_MAX_HEALTH;

	
	if (hosting) {
		add_player_to_team(my_player, map, game_mode_data);
		my_player->assigned_team_id = true;

	}

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
		.net_buffer = NULL,

	};

	return network_info;

}

void process_net_packets(const NetPlayer* buffer, Player* players, uint8_t num_players, GameModeData* game_mode_data, bool hosting, const Map* map) {
	const MinimalPlayerInfo* minimal_player_info = &buffer->minimal_player_info;
	bool shooting = buffer->shooting;

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

	// Always copy the team id, since the server can set the teams of other players at any time
	if (!hosting) {
		net_player->assigned_team_id = buffer->assigned_team_id;
		net_player->team_id = buffer->minimal_player_info.team_id;

	}

	// Don't copy most data if it's my ID
	if (players[0].id == minimal_player_info->id) {
		return;

	}

	if (hosting) {
		if (!net_player->assigned_team_id) {
			printf("Adding player to team\n");
			add_player_to_team(net_player, map, game_mode_data);
			net_player->assigned_team_id = true;

		}
	}

	net_player->last_hurt_by = buffer->last_hurt_by;
	net_player->is_net_player = true;
	net_player->ammo = minimal_player_info->ammo;
	net_player->pos_x = minimal_player_info->pos_x;
	net_player->pos_y = minimal_player_info->pos_y;
	net_player->direction = minimal_player_info->direction;
	net_player->weapon = minimal_player_info->weapon;
	net_player->using_ability = minimal_player_info->using_ability;
	net_player->ability = minimal_player_info->ability;
	net_player->cloaking = minimal_player_info->cloaking;
	net_player->shooting = shooting;

	if (minimal_player_info->health == 0 && net_player->health > 0) {
		add_kill(players, num_players, net_player);
		net_player->health = 0;

	} else {
		net_player->health = minimal_player_info->health;

	}


	if (net_player->username == NULL) {
		net_player->username = calloc(1, 30);
		memcpy(net_player->username, buffer->username, strlen(buffer->username));

	}


}

int handle_networking(NetworkInfo* network_info, Player* players, uint8_t num_players, GameModeData* game_mode_data, const Map* map) {	
	#define BUFFER_LEN 1 + (game_mode_data->num_teams * sizeof(TeamScore)) + (sizeof(NetPlayer) * num_players)
	if (network_info->net_buffer == NULL) {
		network_info->net_buffer = malloc(BUFFER_LEN);

	}

	char* buffer = network_info->net_buffer;

	if (network_info->is_server) {
		if (network_info->addrs_to_send_to.num_items > 0) {
			// First, copy the team data info
			buffer[0] = game_mode_data->num_teams;

			TeamScore* current_team_score = (TeamScore*)&buffer[1];

			for (uint8_t i = 0; i < game_mode_data->num_teams; i += 1) {
				current_team_score->score = game_mode_data->teams[i].score;
				current_team_score->id = game_mode_data->teams[i].id;

				current_team_score += 1;

			}

			NetPlayer* net_players = (NetPlayer*)(buffer + 1 + (game_mode_data->num_teams * sizeof(TeamScore)));
			uint8_t num_players_to_send = 0;

			for (uint8_t i = 0; i < num_players; i += 1) {
				Player* player = &players[i];

				// TODO: Do real serialization not this 
				if (player->assigned_id) {
					memcpy(net_players[num_players_to_send].username, player->username, strlen(player->username) + 1);

					net_players[num_players_to_send].minimal_player_info = get_minimal_player_info(player);
					net_players[num_players_to_send].shooting = players[i].shooting;
					net_players[num_players_to_send].assigned_team_id = players[i].assigned_team_id;
					net_players[num_players_to_send].last_hurt_by = players[i].last_hurt_by;

					num_players_to_send += 1;

				}

			}

			const Addr* addrs_to_send_to = network_info->addrs_to_send_to.item_list;

			// Send state info to each player we've received a message from
			for (uint64_t i = 0; i < network_info->addrs_to_send_to.num_items; i += 1) {
				const Addr* addr_to_send_to = &addrs_to_send_to[i];

				sendto(network_info->socket, buffer, 1 + (game_mode_data->num_teams * sizeof(TeamScore)) + (sizeof(NetPlayer) * num_players_to_send) , 0, (struct sockaddr*)&addr_to_send_to->sockaddr, addr_to_send_to->addr_len);

			}

		}

	} else {
		NetPlayer* net_player = (NetPlayer*)buffer;
		const Player* my_player = &players[0];

		memcpy(net_player[0].username, my_player->username, strlen(my_player->username) + 1);
		net_player[0].minimal_player_info = get_minimal_player_info(my_player);
		net_player[0].shooting = my_player->shooting;
		net_player[0].last_hurt_by = my_player->last_hurt_by;

		// Don't send a message if nothing's changed
		if (network_info->prev_msg == NULL || memcmp(net_player, network_info->prev_msg, network_info->prev_msg_len) != 0 || !net_player->assigned_team_id) {
			network_info->prev_msg = realloc(network_info->prev_msg, sizeof(NetPlayer));
			
			network_info->prev_msg_len = sizeof(NetPlayer);
			memcpy(network_info->prev_msg, net_player, sizeof(NetPlayer));

			send(network_info->socket, net_player, sizeof(NetPlayer), 0);

		}


	}

	int total_bytes_read = 0;

	Addr addr = {
		.addr_len = sizeof(addr.sockaddr),

	};

	if (network_info->is_server) {
		// Since servers just receive the NetPlayer struct, just ask for like all of those lol
		int bytes_read = recvfrom(network_info->socket, buffer, sizeof(NetPlayer), 0, (struct sockaddr*)&addr.sockaddr, &addr.addr_len);

		while (bytes_read > 0) {
			if (bytes_read != sizeof(NetPlayer)) {
				fprintf(stderr, "Bad read size\n");
				continue;

			}

			hashset_insert(&addr, sizeof(addr), &network_info->addrs_to_send_to);
			total_bytes_read += bytes_read;

			bytes_read = recvfrom(network_info->socket, buffer + bytes_read, sizeof(NetPlayer), 0, (struct sockaddr*)&addr.sockaddr, &addr.addr_len);

		}


	} else {
		// Clients on the other hand should just try to get the entire BUFFER_LEN, since that's what the server sends
		int bytes_read = recvfrom(network_info->socket, buffer, BUFFER_LEN, 0, (struct sockaddr*)&addr.sockaddr, &addr.addr_len);
		
		// Discard the rest of any messages received
		bool finished_discarding = recv(network_info->socket, NULL, 0, O_TRUNC) <= 0;
		while (!finished_discarding) {
			int true_size = recv(network_info->socket, NULL, 0, O_TRUNC) <= 0;
			finished_discarding = true_size <= 0;

		}

		total_bytes_read = bytes_read;


	}

	if (total_bytes_read > 0) {
		const NetPlayer* net_players;
		uint64_t num_net_players;

		if (!network_info->is_server) {
			// Client messages include both team composition data as well as regular player data, so to calculate the net_player buffer we have to do some math
			uint8_t num_teams = buffer[0];

			if (game_mode_data->num_teams != num_teams) {
				game_mode_data->teams = realloc(game_mode_data->teams, num_teams * sizeof(Team));
				game_mode_data->num_teams = num_teams;

			}


			uint64_t start_of_net_player_buffer = 1 + (num_teams * sizeof(TeamScore));
			uint64_t total_net_player_bytes = total_bytes_read - start_of_net_player_buffer;

			num_net_players = total_net_player_bytes / (sizeof(NetPlayer) - 1);
			net_players = (NetPlayer*)(buffer + start_of_net_player_buffer);

		} else {
			// Servers, on the other hand, just have a long list of NetPlayers
			num_net_players = total_bytes_read / (sizeof(NetPlayer) - 1);
			net_players = (NetPlayer*)buffer;

		}

		// Run through each NetPlayer and synchronize the actual player list with their info
		for (uint64_t i = 0; i < num_net_players; i += 1) {
			process_net_packets(&net_players[i], players, num_players, game_mode_data, network_info->is_server, map);
			

		}


		// After running through all the players, have clients synchronize teams with the server
		if (!network_info->is_server) {
			// TODO: Just send a smaller version of MinimalTeamInfo and memcpy it to save time
			const TeamScore* team_scores = (TeamScore*)(buffer + 1);

			// First, properly set up teams and their IDs and scores
			for (uint64_t i = 0; i < game_mode_data->num_teams; i += 1) {
				const TeamScore* team_score = &team_scores[i];
				Team* team = &game_mode_data->teams[i];

				team->id = team_score->id;
				team->score = team_score->score;
				team->num_players = 0;

			}

			// Then, move players into their proper teams
			for (uint64_t i = 0; i < num_net_players; i += 1) {
				const NetPlayer* net_player = &net_players[i];
				Team* team = find_team_of_id(net_player->minimal_player_info.team_id, game_mode_data);

				if (team == NULL) {
					fprintf(stderr, "Invalid team ID: %lu\n", net_player->minimal_player_info.team_id);
					for (uint8_t i = 0; i < game_mode_data->num_teams; i += 1) {
						printf("Real team: %lu\n", game_mode_data->teams[i].id);

					}

					exit(-1);

				} else {
					team->num_players += 1;	
					const Player* player = find_player_by_id(net_player->minimal_player_info.id, players, num_players);

					if (player == NULL) {
						fprintf(stderr, "Failed to find net_player's id\n");
						exit(-1);

					} else {
						team->players[team->num_players - 1] = get_minimal_player_info(player);

					}

				}

			}

		}

	}

	return 0;

}

