#include <stdint.h>
#include <math.h>
#include "include/raylib.h"
#include "math.h"

float saturating_sub(float x, float y) {
	if (y > x) {
		return 0;

	} else {
		return x - y;

	}

}

float get_angle(float cx, float cy, float ex, float ey) {
	float dy = ey - cy;
	float dx = ex - cx;

	float angle;

	if (dx != 0.0) {
		angle = atanf(dy / dx); 

	} else {
		if (dy > 0.0) {
			angle = PI / 2.0;

		} else {
			angle = 0.0;

		}

	}

	if (cx < ex) {
		angle -= PI;

	}

	return angle;
}

float distance2(float x1, float y1, float x2, float y2) {
	return powf((x2 - x1), 2.0) + powf((y2 - y1), 2.0);
}

float distance(float x1, float y1, float x2, float y2) {
	return sqrtf(distance2(x1, y1, x2, y2));

}


bool aabb_collision(float pos_x1, float pos_y1, float size_x1, float size_y1, float pos_x2, float pos_y2, float size_x2, float size_y2) {
	return (
		pos_x1 - size_x1 < pos_x2 + size_x2 &&
    	pos_x1 + size_x1 > pos_x2 &&
    	pos_y1 - size_y1 < pos_y2 + size_y2 &&
    	pos_y1 + size_y1 > pos_y2

	);

}
