#ifndef RENDER_H
#define RENDER_H

#include "game_mode.h"
#include "include/raylib.h"
#include "player.h"
#include "projectile.h"

void render(Camera2D camera, const Player* players, uint8_t num_players, const GameModeData* game_mode_data, const Projectile* projectiles, uint16_t num_projectiles, const Map* map, const Team* winning_team, bool player_won);

#endif
