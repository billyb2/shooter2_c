#ifndef NET_H
#define NET_H
#ifdef __unix__
#include <netinet/tcp.h>
#include <netinet/udp.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#endif



#include <stdint.h>
#include "player.h"

#ifdef __WIN32__
#define socklen_t int

// winsock2.h conflicts with raylib
#ifndef MAIN_C
#include <winsock2.h>
#endif
#endif

typedef struct NetworkInfo {
	bool is_server;
	int socket;
	struct sockaddr_in* socks_to_send_to;
	socklen_t addr_len;


} NetworkInfo;


NetworkInfo init_networking(bool hosting, const char* ip_addr);
int handle_server_networking(NetworkInfo* network_info, uint8_t my_player_id, Player* players, uint8_t num_players);
int handle_client_networking(NetworkInfo* network_info, const Player* my_player, Player* other_player);
#endif 
