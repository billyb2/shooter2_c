#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "include/raylib.h"
#include "math.h"
#include "map.h"
#include "projectile.h"
#include "player.h"
#include "render.h"
#include "weapon.h"

extern int SCREEN_WIDTH;
extern int SCREEN_HEIGHT;

void render(Camera2D camera, const Player* players, uint8_t num_players, const Projectile* projectiles, uint16_t num_projectiles, const Map* map) {
	camera.zoom = 0.85;
	
	if (players[0].weapon == Sniper) {
		camera.zoom = 0.65;

	}

	BeginDrawing();
		ClearBackground(RAYWHITE);

		BeginMode2D(camera);

		for (uint8_t i = 0; i < num_players; i += 1) {
			const Player* player = &players[i];

			if (player->health > 0 && player->assigned_id) {
				// Don't draw usernames of cloaking players
				if (player->username != NULL && !player->cloaking) {
					DrawText(player->username, player->pos_x - (float)strlen(player->username) * 1.5 , player->pos_y - 22, 12, BLACK);

				}

				const Color LIGHT_RED = {
					.r = 255,
					.g = 0,
					.b = 0,
					.a = 70,

				};

				if (player->cloaking) {
					DrawCircle(player->pos_x, player->pos_y, PLAYER_SIZE / 2.0, LIGHT_RED);

				} else {
					DrawCircle(player->pos_x, player->pos_y, PLAYER_SIZE / 2.0, RED);

				}

				if (player->weapon == Sniper) {
					Rectangle laser = {
						.x = player->pos_x,
						.y = player->pos_y,
						.width = 500.0,
						.height = 1.0,

					};


					DrawRectanglePro(laser, (Vector2){ 0.0, 0.0 }, player->direction * (180.0 / PI), LIGHT_RED);

				}

			}

		}

		for (uint16_t i = 0; i < num_projectiles; i += 1) {
			const Projectile* projectile = &projectiles[i];
			DrawCircle(projectile->pos_x, projectile->pos_y, projectile->size / 2.0, BLACK);

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


		// Draw the weapon being used
		char weapon_text[256];
		int weapon_text_len = weapon_to_text(players[0].weapon,weapon_text);

		DrawText(weapon_text, SCREEN_WIDTH - 11 * weapon_text_len, 50, 20, health_bar_outline_color);


		// Draw the ammo count
		char ammo_count_text[16] = { 0 };

		if (players[0].reloading) {
			memcpy(ammo_count_text, "Reloading", 9);

		} else if (players[0].weapon_switch_cooldown > 0) {
			memcpy(ammo_count_text, "Switching...", 12);

		} else {
			sprintf(ammo_count_text, "Ammo: %u / %u", players[0].ammo, get_ammo_count(players[0].weapon));

		}

		DrawText(ammo_count_text, SCREEN_WIDTH - 11 * strlen(ammo_count_text), 75, 20, health_bar_outline_color);

		// Draw the ability charge
		const Color ability_charge_color = {
			.r = GREEN.r,
			.g = GREEN.g,
			.b = GREEN.b,
			.a = 128,

		};

		DrawText("Charge", SCREEN_WIDTH - 100, 100, 20, health_bar_outline_color);
		Rectangle ability_charge_rect = {
			.x = SCREEN_WIDTH - 120,
			.y = 125.0,
			.width = 125.0 * ((float)players[0].ability_charge / (float)get_max_ability_charge(players[0].ability)),
			.height = 20.0,

		};
		DrawRectangleRec(ability_charge_rect, ability_charge_color);
		DrawRectangleLinesEx(ability_charge_rect, 3.5, health_bar_outline_color);

	EndDrawing();

}
