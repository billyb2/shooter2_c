#ifndef RENDER_H
#define RENDER_H

#include "include/raylib.h"
#include "player.h"
#include "projectile.h"

void render(Camera2D camera, const Player* players, uint8_t num_players, const Projectile* projectiles, uint16_t num_projectiles, const Map* map);

#endif
