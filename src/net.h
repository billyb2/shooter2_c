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
#include "hashset.h"

#ifdef __WIN32__
#define socklen_t int

#ifndef MAIN_C
// winsock2.h conflicts with raylib, so I pull some defs insead
#include <winsock2.h>
#endif
#endif

typedef struct NetPlayer {
	MinimalPlayerInfo minimal_player_info;
	bool shooting;
	uint32_t _padding;

} NetPlayer;

static_assert(sizeof(NetPlayer) == 40, "NetPlayer is the wrong size");

typedef struct Addr {
	struct sockaddr_in sockaddr;
	socklen_t addr_len;

} Addr;

typedef struct NetworkInfo {
	bool is_server;
	int socket;
	HashSet addrs_to_send_to;

} NetworkInfo;


NetworkInfo init_networking(bool hosting, const char* ip_addr, Player* my_player);
int handle_server_networking(NetworkInfo* network_info, uint8_t my_player_id, Player* players, uint8_t num_players);
int handle_networking(NetworkInfo* network_info, Player* players, uint8_t num_players);
#endif 
