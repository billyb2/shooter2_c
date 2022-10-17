#ifndef MATH_H
#define MATH_H
#include <stdbool.h>

#define F32_PI 3.14159265358979323846264338327950288
#define F32_EPSILON 1.1920929E-7

// Generic math babyyyy
#define saturating_sub(x, y) ( (x < y) ? 0 : (x - y) );

#ifdef WASM
float atanf(float x);
#endif

typedef struct AABB {
	float left_edge;
	float right_edge;
	float top_edge;
	float bottom_edge;

} AABB;

float get_angle(float cx, float cy, float ex, float ey);
float distance(float x1, float y1, float x2, float y2);
float distance2(float x1, float y1, float x2, float y2);
bool aabb_collision(AABB aabb1, AABB aabb2);
bool aabb_collision_w_movement(AABB aabb1, AABB aabb2, float aabb1_angle, float aabb1_movement);
#endif
