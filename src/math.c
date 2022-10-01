#include <stdint.h>
#include "math.h"
#ifndef WASM
#include <math.h>
#endif

#ifndef WASM
float get_angle(float cx, float cy, float ex, float ey) {
	float dy = ey - cy;
	float dx = ex - cx;

	float angle;

	if (dx != 0.0) {
		angle = atanf(dy / dx); 

	} else {
		if (dy > 0.0) {
			angle = F32_PI / 2.0;

		} else {
			angle = 0.0;

		}

	}

	if (cx < ex) {
		angle -= F32_PI;

	}

	return angle;
}

float distance2(float x1, float y1, float x2, float y2) {
	return powf((x2 - x1), 2.0) + powf((y2 - y1), 2.0);
}

float distance(float x1, float y1, float x2, float y2) {
	return sqrtf(distance2(x1, y1, x2, y2));

}

/* Checks if an object will collide with another object on it's movement path */
bool aabb_collision_w_movement(float pos_x1, float pos_y1, float size_x1, float size_y1, float pos_x2, float pos_y2, float size_x2, float size_y2, float obj1_distance, float obj1_angle) {
	while (obj1_distance > F32_EPSILON) {
		float distance_x = size_x1 * cosf(obj1_angle);
		float distance_y = size_y1 * sinf(obj1_angle);

		pos_x1 += distance_x;
		pos_y1 += distance_y;

		obj1_distance -= fabsf(distance_x);
		obj1_distance -= fabsf(distance_y);

		if (aabb_collision(pos_x1, pos_y1, size_x1, size_y1, pos_x2, pos_y2, size_x2, size_y2)) {
			return true;

		}

	}

	return false;
}
#endif


bool aabb_collision(float pos_x1, float pos_y1, float size_x1, float size_y1, float pos_x2, float pos_y2, float size_x2, float size_y2) {
	return (
		pos_x1 - size_x1 <= pos_x2 + size_x2 &&
    	pos_x1 + size_x1 >= pos_x2 &&
    	pos_y1 - size_y1 <= pos_y2 + size_y2 &&
    	pos_y1 + size_y1 >= pos_y2

	);

}


