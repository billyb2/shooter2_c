#include <stdio.h>

#include "camera.h"
#include "map.h"
#include "include/raylib.h"

extern const int SCREEN_WIDTH;
extern const int SCREEN_HEIGHT;

void move_camera(Camera2D* camera, const Map* map, float pos_x, float pos_y) {
	bool screen_bigger_than_map_x = false; 
	bool screen_bigger_than_map_y = false;

	if (SCREEN_WIDTH >= map->size_x) {
		camera->target.x = SCREEN_WIDTH / 2.0;
		screen_bigger_than_map_x = true;

	}

	if (SCREEN_HEIGHT >= map->size_y) {
		camera->target.y = SCREEN_HEIGHT / 2.0;
		screen_bigger_than_map_y = true;
	}

	if (!screen_bigger_than_map_x) {
		if (pos_x < SCREEN_WIDTH / 2.0) {
			camera->target.x = (float)SCREEN_WIDTH / 2.0;

		} else if (pos_x > map->size_x - (SCREEN_WIDTH / 2.0)) {
			camera->target.x = (float)map->size_x - (float)SCREEN_WIDTH / 2.0;

		} else {
			camera->target.x = (float)pos_x;

		}

	}	

	if (!screen_bigger_than_map_y) {
		if (pos_y < SCREEN_HEIGHT / 2.0) {
			camera->target.y = (float)SCREEN_HEIGHT / 2.0;

		} else if (pos_y > map->size_y - (SCREEN_HEIGHT / 2.0)) {
			camera->target.y = (float)map->size_y - (float)SCREEN_HEIGHT / 2.0;

		} else {
			camera->target.y = (float)pos_y;

		}

	}

}
