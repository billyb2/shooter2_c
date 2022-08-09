#include <stdint.h>

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

			Vector4 color_normalized = {
				.x = 1.0,
				.y = 0.0,
				.z = 0.0,
				.w = (float)(player->health) / 500.0,

			};

			Color color =  ColorFromNormalized(color_normalized);

			DrawCircle(player->pos_x + PLAYER_SIZE / 2.0, player->pos_y + PLAYER_SIZE / 2.0, PLAYER_SIZE, color);

		}

		for (uint16_t i = 0; i < num_projectiles; i += 1) {
			const Projectile* projectile = &projectiles[i];
			DrawCircle(projectile->pos_x, projectile->pos_y, projectile->size, BLACK);
		}

		for (uint16_t i = 0; i < map->num_objects; i += 1) {
			const MapObject* object = &map->objects[i];
			Color color = {
				.r = object->color[0],
				.g = object->color[1],
				.b = object->color[2],
				.a = object->color[3],

			};

			DrawRectangle(object->pos_x, object->pos_y, object->size_x, object->size_y, color);

		}

	EndDrawing();

}
