#ifndef MAP_H
#define MAP_H

#include <stdint.h>
#include <stdbool.h>

typedef struct MapObject {
	float pos_x;
	float pos_y;

	float size_x;
	float size_y;

	uint8_t color[4];

} MapObject;

typedef struct Map {
	float size_x;
	float size_y;

	MapObject* objects;
	uint16_t num_objects;

} Map;

Map new_map(const char* file_name);
bool map_collision(const Map* map, float pos_x, float pos_y, float size);

#endif
