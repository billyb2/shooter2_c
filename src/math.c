#include <stdint.h>
#include <math.h>
#include "include/raylib.h"
#include "math.h"

uint16_t saturating_sub(uint16_t x, uint16_t y) {
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
			angle = PI;

		}

	}

	if (cx < ex) {
		angle -= PI;

	}

	return angle;
}
