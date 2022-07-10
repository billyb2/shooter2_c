#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "include/raylib.h"

#include "player.h"
#include "input.h"
#include "math.h"
#include "projectile.h"

extern int SCREEN_WIDTH;
extern int SCREEN_HEIGHT;

void player_input(Player* player, const KeyBindings* key_bindings, Projectile** projectiles, uint16_t* num_projectiles) {
	uint16_t old_speed = player->speed;

	Vector2 mouse_pos = GetMousePosition();

	// Ability input
	if (IsKeyDown(key_bindings->ability)) {
		player->speed = player->speed * 15;
		
	}

	// Movement input
	if (IsKeyDown(key_bindings->up)) {
		player->pos_y = saturating_sub(player->pos_y, player->speed);
	}

	if (IsKeyDown(key_bindings->down)) {
		player->pos_y += player->speed;
	}

	if (IsKeyDown(key_bindings->left)) {
		player->pos_x = saturating_sub(player->pos_x, player->speed);
	}

	if (IsKeyDown(key_bindings->right)) {
		player->pos_x += player->speed;
	}

	player->direction = get_angle(mouse_pos.x, mouse_pos.y, (float)player->pos_x, (float)player->pos_y); 


	if (IsMouseButtonDown(0)) {
		shoot(projectiles, num_projectiles, player, player->direction);

	}

	player->speed = old_speed;	
}
