#include <stdint.h>
#include <stdio.h>

#include "camera.h"
#include "map.h"
#include "include/raylib.h"

extern const int SCREEN_WIDTH;
extern const int SCREEN_HEIGHT;

void move_camera(Camera2D* camera, const Map* map, float pos_x, float pos_y) {
	if (pos_x < SCREEN_WIDTH / 2.0) {
		camera->target.x = SCREEN_WIDTH / 2.0;

	} else if (pos_x > map->size_x - (SCREEN_WIDTH / 2.0)) {
		camera->target.x = map->size_x - SCREEN_WIDTH / 2.0;

	} else {
		camera->target.x = pos_x;

	}
	

	if (pos_y < SCREEN_HEIGHT / 2.0) {
		camera->target.y = SCREEN_HEIGHT / 2.0;

	} else if (pos_y > map->size_y - (SCREEN_HEIGHT / 2.0)) {
		camera->target.y = map->size_y - SCREEN_HEIGHT / 2.0;

	} else {
		camera->target.y = pos_y;

	}

}
