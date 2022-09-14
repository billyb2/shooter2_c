#ifndef NET_H
#define NET_H
#include "minimal_state_info.h"
#ifdef __unix__
#include <netinet/tcp.h>
#include <netinet/udp.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#endif

#include <stdint.h>
#include "player.h"
#include "game_mode.h"
#include "hashset.h"

#define INVALID_SOCKET -1

#ifdef __WIN32__
#define socklen_t int

// winsock2.h conflicts with raylib, so I pull some defs insead
#include "winsock.h"
#endif

typedef struct NetPlayer {
	char username[20];
	uint64_t last_hurt_by;
	bool shooting;
	bool assigned_team_id;
	MinimalPlayerInfo minimal_player_info;

} NetPlayer;

typedef struct TeamScore {
	uint64_t id;
	uint64_t score;

} TeamScore;

static_assert(sizeof(NetPlayer) == 80, "NetPlayer is the wrong size");

typedef struct Addr {
	struct sockaddr_in sockaddr;
	socklen_t addr_len;

} Addr;

typedef struct NetworkInfo {
	void* prev_msg;
	uint64_t prev_msg_len;

	bool is_server;
	int socket;
	HashSet addrs_to_send_to;

} NetworkInfo;


NetworkInfo init_networking(bool hosting, const char* ip_addr, Player* my_player, GameModeData* game_mode_data);
int handle_networking(NetworkInfo* network_info, Player* players, uint8_t num_players, GameModeData* game_mode_data);
#endif 
