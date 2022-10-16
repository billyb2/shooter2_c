#ifndef MAP_H
#define MAP_H

#include "raylib_defs.h"
#include "minimal_state_info.h"
#include "math.h"
#include <stdint.h>
#include <stdbool.h>

typedef struct MapObject {
	float pos_x;
	float pos_y;

	float size_x;
	float size_y;

	Texture2D* texture;
	bool spawn_point;

	bool collidable;

} MapObject;

typedef struct Map {
	float size_x;
	float size_y;
	uint16_t num_objects;
	uint8_t num_spawn_points;
	uint16_t num_textures;
	MapObject* objects;
	Texture* textures;
	MapObject** spawn_points;

} Map;

Map new_map(const char* file_name);
bool map_collision(AABB aabb, const Map* map);
bool map_collision_w_movement(AABB aabb, float distance, float angle, const Map* map);
MinimalMapInfo get_minimal_map_info(const Map* map);


#endif
