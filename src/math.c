#include <stdint.h>
#include "math.h"
#ifndef WASM
#include <math.h>
#endif

#ifndef WASM
#include <stdlib.h>
# else
#define NULL 0
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
bool aabb_collision_w_movement(AABB aabb1, AABB aabb2, float aabb1_distance, float aabb1_angle) {
	// TODO
	return aabb_collision(aabb1, aabb2);

}
#endif


bool aabb_collision(AABB aabb1, AABB aabb2) {
	return (
		aabb1.right_edge >= aabb2.left_edge &&
		aabb1.bottom_edge >= aabb2.top_edge &&
		aabb2.right_edge >= aabb1.left_edge &&
		aabb2.bottom_edge >= aabb1.top_edge

	);

}
