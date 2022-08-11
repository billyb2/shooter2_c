#ifndef MATH_H
#define MATH_H
#include <stdbool.h>

float saturating_sub(float x, float y);
float get_angle(float cx, float cy, float ex, float ey);
float distance(float x1, float y1, float x2, float y2);
float distance2(float x1, float y1, float x2, float y2);
bool aabb_collision(float pos_x1, float pos_y1, float size1, float pos_x2, float pos_y2, float size2);

#endif
