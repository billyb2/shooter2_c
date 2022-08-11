#include "map.h"

bool map_collision(float pos_x, float pos_y, float size_x, float size_y, const Map* map) {
	return (pos_x < 0.0 || pos_x + size_x > map->size_x || pos_y < 0.0 || pos_y + size_y > map->size_y);

}
