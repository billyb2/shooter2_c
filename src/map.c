#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "math.h"
#include "map.h"

Map new_map(const char* file_name) {
	Map map = {
		.objects = NULL,
		.num_objects = 0,
		.size_x = 1000.0,
		.size_y = 1000.0,

	};

	FILE* map_file_ptr = fopen(file_name, "rb");
	size_t map_file_size = 0;

	if (map_file_ptr == NULL) {
		printf("Opening map file failed\n");
		exit(-1);

	}

	fseek(map_file_ptr, 0, SEEK_END);
	map_file_size = ftell(map_file_ptr);
	rewind(map_file_ptr);

	uint8_t* map_bin = malloc(map_file_size);

	size_t total_num_read_bytes = 0;

	while (total_num_read_bytes < map_file_size) {
		size_t amt_read = fread(&map_bin[total_num_read_bytes], 1, map_file_size, map_file_ptr);
		total_num_read_bytes += amt_read;

		if (amt_read == 0) {
			break;

		}
	}

	if (total_num_read_bytes != map_file_size) {
		printf("Error reading file: Tried to read %ld, instead read %ld\n", map_file_size, total_num_read_bytes);
		exit(-1);

	}

	#define MAP_OBJ_SIZE 9

	map.num_objects = map_file_size / 9;
	map.objects = malloc(map.num_objects * sizeof(MapObject));

	for (uint16_t i = 0; i < map.num_objects; i += 1) {
		#define MAP_TILE_SIZE 50.0

		uint8_t color[4] = { 0 };

		uint8_t tile_id = map_bin[i * MAP_OBJ_SIZE];

		if (tile_id == 0) {
			color[0] = 255;
			color[3] = 255;

		} else if (tile_id == 1) {
			color[1] = 255;
			color[3] = 255;

		}


		memcpy(&map.objects[i].color, &color, 4);
		memcpy(&map.objects[i].pos_x, &map_bin[i * MAP_OBJ_SIZE + 1], sizeof(float));
		memcpy(&map.objects[i].pos_y, &map_bin[i * MAP_OBJ_SIZE + 1 + sizeof(float)], sizeof(float));

		map.objects[i].pos_x *= MAP_TILE_SIZE;
		map.objects[i].pos_y *= MAP_TILE_SIZE;
		map.objects[i].size_x = MAP_TILE_SIZE;
		map.objects[i].size_y = MAP_TILE_SIZE;

	}

	return map;

}

bool map_collision(float pos_x, float pos_y, float size_x, float size_y, const Map* map) {
	if (pos_x < 0.0 || pos_x + size_x > map->size_x || pos_y < 0.0 || pos_y + size_y > map->size_y) {
		return true;

	}

	for (uint16_t i = 0; i < map->num_objects; i += 1) {
		const MapObject* map_obj = &map->objects[i];

		if (aabb_collision(pos_x, pos_y, size_x, size_y, map_obj->pos_x, map_obj->pos_y, map_obj->size_x, map_obj->size_y)) {
			return true;

		}

	}

	return false;

}
