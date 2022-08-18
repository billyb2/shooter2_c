#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "include/raylib.h"
#include "math.h"
#include "map.h"
#include "include/raylib.h"
#include "raylib_defs.h"

Map new_map(const char* file_name) {
	Map map = {
		.num_objects = 0,

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

	float tile_width;
	float tile_height;

	uint32_t map_header_size = 18;

	// Get the map's header info
	memcpy(&map.size_x, map_bin, sizeof(float));
	map_bin += sizeof(float);

	memcpy(&map.size_y, map_bin, sizeof(float));
	map_bin += sizeof(float);

	memcpy(&tile_width, map_bin, sizeof(float));
	map_bin += sizeof(float);

	memcpy(&tile_height, map_bin, sizeof(float));
	map_bin += sizeof(float);

	uint16_t* num_tiles_ptr = (uint16_t*)(map_bin);
	uint16_t num_tiles = *num_tiles_ptr;
	uint16_t largest_id = 0;

	map_bin += sizeof(uint16_t);

	// Poor man's hashmap. Honestly a fine solution for now but should FIXME
	char** index_to_file_name = malloc(sizeof(char*));

	while (num_tiles > 0) {
		uint16_t* tile_id = (uint16_t*)(map_bin);
		map_bin += 2;

		if (*tile_id > largest_id) {
			largest_id = *tile_id;
			index_to_file_name = realloc(index_to_file_name, (largest_id + 1) * sizeof(char**));

		}

		char* file_name = (char*)(map_bin);
		int file_name_len = strlen(file_name);

		#define SPRITES_PATH "sprites/"
		#define SPRITES_PATH_LEN 8

		index_to_file_name[*tile_id] = malloc(SPRITES_PATH_LEN + file_name_len + 1);
		memcpy(index_to_file_name[*tile_id], SPRITES_PATH, SPRITES_PATH_LEN);
		memcpy(index_to_file_name[*tile_id] + SPRITES_PATH_LEN, file_name, file_name_len + 1);

		map_header_size += file_name_len + 1;
		map_bin += file_name_len + 1;

		num_tiles -= 1;

	}

	#define MAP_OBJ_SIZE 11

	map.num_objects = (map_file_size - map_header_size) / MAP_OBJ_SIZE;
	map.objects = malloc(map.num_objects * sizeof(MapObject));

	for (uint16_t i = 0; i < map.num_objects; i += 1) {
		uint16_t* tile_id = (uint16_t*)&map_bin[i * MAP_OBJ_SIZE];

		const char* file_name = index_to_file_name[*tile_id];

		Image image = LoadImage(file_name);
		ImageCrop(&image, (Rectangle){ 0.0, 0.0, tile_width, tile_height });

		Texture2D texture = LoadTextureFromImage(image);

		UnloadImage(image);

		bool spawn = map_bin[i * MAP_OBJ_SIZE + 2] == 255; 

		memcpy(&map.objects[i].pos_x, &map_bin[i * MAP_OBJ_SIZE + 3], sizeof(float));
		memcpy(&map.objects[i].pos_y, &map_bin[i * MAP_OBJ_SIZE + 3 + sizeof(float)], sizeof(float));

		map.objects[i].spawn_point = spawn;

		map.objects[i].pos_x *= tile_width;
		map.objects[i].pos_y *= tile_height;

		map.objects[i].size_x = tile_width;
		map.objects[i].size_y = tile_height;

		map.objects[i].texture = texture;

	}

	return map;

}

bool map_collision(float pos_x, float pos_y, float size_x, float size_y, const Map* map) {
	if (pos_x < 0.0 || pos_x + size_x > map->size_x || pos_y < 0.0 || pos_y + size_y > map->size_y) {
		return true;

	}

	for (uint16_t i = 0; i < map->num_objects; i += 1) {
		const MapObject* map_obj = &map->objects[i];

		if (map_obj->spawn_point) {
			continue;

		}

		if (aabb_collision(pos_x, pos_y, size_x, size_y, map_obj->pos_x, map_obj->pos_y, map_obj->size_x, map_obj->size_y)) {
			return true;

		}

	}

	return false;

}
