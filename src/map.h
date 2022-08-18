#ifndef MAP_H
#define MAP_H

#include "include/raylib.h"
#include <stdint.h>
#include <stdbool.h>

typedef struct MapObject {
	float pos_x;
	float pos_y;

	float size_x;
	float size_y;

	Texture2D texture;
	bool spawn_point;

} MapObject;

typedef struct Map {
	float size_x;
	float size_y;
	uint16_t num_objects;
	MapObject* objects;

} Map;

Map new_map(const char* file_name);
bool map_collision(float pos_x, float pos_y, float size_x, float size_y, const Map* map);

#endif
