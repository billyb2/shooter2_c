#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "include/raylib.h"
#include "map.h"
#include "projectile.h"
#include "player.h"
#include "render.h"

extern int SCREEN_WIDTH;
extern int SCREEN_HEIGHT;

void render(Camera2D camera, const Player* players, uint8_t num_players, const Projectile* projectiles, uint16_t num_projectiles, const Map* map) {
	BeginDrawing();
		ClearBackground(RAYWHITE);

		BeginMode2D(camera);

		for (uint8_t i = 0; i < num_players; i += 1) {
			const Player* player = &players[i];

			if (player->health > 0 && player->assigned_id) {
				if (player->username != NULL) {
					DrawText(player->username, player->pos_x - (float)strlen(player->username) * 1.5 , player->pos_y - 22, 12, BLACK);

				}

				DrawCircle(player->pos_x, player->pos_y, PLAYER_SIZE, RED);

			}

		}

		for (uint16_t i = 0; i < num_projectiles; i += 1) {
			const Projectile* projectile = &projectiles[i];
			DrawCircle(projectile->pos_x, projectile->pos_y, projectile->size, BLACK);

		}

		for (uint16_t i = 0; i < map->num_objects; i += 1) {
			const MapObject* map_obj = &map->objects[i];

			if (!map_obj->spawn_point) {
				DrawTexture(*map_obj->texture, map_obj->pos_x, map_obj->pos_y, WHITE);

			}

		}

		EndMode2D();

		// Start drawing the UI
		
		// First, draw the health bar
		const Color health_bar_color =  {
			.r = PURPLE.r,
			.g = PURPLE.g,
			.b = PURPLE.b,
			.a = 128,

		};

		const Color health_bar_outline_color =  {
			.r = BLACK.r,
			.g = BLACK.g,
			.b = BLACK.b,
			.a = 128,

		};

		DrawText("Health", SCREEN_WIDTH - 100, 0, 20, health_bar_outline_color);
		Rectangle health_bar_rect = {
			.x = SCREEN_WIDTH - 120,
			.y = 25.0,
			.width = 125.0 * ((float)players[0].health / (float)PLAYER_MAX_HEALTH),
			.height = 20.0,

		};

		DrawRectangleRec(health_bar_rect, health_bar_color);
		DrawRectangleLinesEx(health_bar_rect, 3.5, health_bar_outline_color);

		// Draw the ammo count
		char ammo_count_text[16] = { 0 };

		if (players[0].reloading) {
			memcpy(ammo_count_text, "Reloading", 9);

		} else {
			sprintf(ammo_count_text, "Ammo: %u / %u", players[0].ammo, get_ammo_count(players[0].weapon));

		}

		DrawText(ammo_count_text, SCREEN_WIDTH - 11 * strlen(ammo_count_text), 50, 20, health_bar_outline_color);

	EndDrawing();

}
