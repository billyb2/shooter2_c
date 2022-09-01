#ifndef MATH_H
#define MATH_H
#include <stdbool.h>

#define F32_PI 3.14159265358979323846264338327950288
#define F32_EPSILON 1.1920929E-7

// Generic math babyyyy
#define saturating_sub(x, y) ( (x < y) ? 0 : (x - y) );

float get_angle(float cx, float cy, float ex, float ey);
float distance(float x1, float y1, float x2, float y2);
float distance2(float x1, float y1, float x2, float y2);
bool aabb_collision(float pos_x1, float pos_y1, float size_x1, float size_y1, float pos_x2, float pos_y2, float size_x2, float size_y2);
bool aabb_collision_w_movement(float pos_x1, float pos_y1, float size_x1, float size_y1, float pos_x2, float pos_y2, float size_x2, float size_y2, float obj1_distance, float obj1_angle);
#endif
