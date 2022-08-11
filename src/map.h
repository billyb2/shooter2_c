#ifndef MAP_H
#define MAP_H

#include <stdint.h>
#include <stdbool.h>

typedef struct Map {
	float size_x;
	float size_y;

} Map;

bool map_collision(float pos_x, float pos_y, float size_x, float size_y, const Map* map);

#endif
