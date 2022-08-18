#include <stdint.h>
#include <string.h>

#include "include/raylib.h"
#include "map.h"
#include "projectile.h"
#include "player.h"
//#include "render.h"

void render(Camera2D camera, const Player* players, uint8_t num_players, const Projectile* projectiles, uint16_t num_projectiles, const Map* map) {
	BeginDrawing();
		ClearBackground(RAYWHITE);
		BeginMode2D(camera);

		for (uint8_t i = 0; i < num_players; i += 1) {
			const Player* player = &players[i];

			Color color = {
				.r = 255,
				.g = 0,
				.b = 0,
				.a = ((float)(player->health) / 500.0) * 255.0,

			};

			DrawCircle(player->pos_x, player->pos_y, PLAYER_SIZE, color);

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

	EndDrawing();

}
