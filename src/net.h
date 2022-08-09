#ifndef NET_H
#define NET_H
#include <stdio.h>
#include <poll.h>

#include "player.h"
#include "projectile.h"

void send_info(Player* players, uint8_t num_players, uint16_t* num_projectiles_ptr, Projectile** projectiles_ptr, const Map* map, const int sockfd, struct pollfd* pfds);
#endif
