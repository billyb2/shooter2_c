#include <stdlib.h>
#include "map.h"
#include "math.h"

Map new_map(const char* file_name)  {
	Map map = {
		.size_x	= 1000.0,
		.size_y = 1000.0,

	};

	map.num_objects = 1;
	map.objects = malloc(map.num_objects * sizeof(MapObject));

	MapObject object = {
		.pos_x = 50.0,
		.pos_y = 50.0,
		.size_x = 50.0,
		.size_y = 50.0,
		.color = { 255, 0, 0, 255 },

	};

	map.objects[0] = object;

	return map;

}


bool map_collision(const Map* map, float pos_x, float pos_y, float size) {
	for (uint16_t i = 0; i < map->num_objects; i += 1) {
		MapObject* object = &map->objects[i];
		
		if (aabb_collision(pos_x, pos_y, size, object->pos_x, object->pos_y, object->size_x)) {
			return true;

		}

	}

	return false;

}

