#ifndef MATH_H
#define MATH_H
#include <stdint.h>
#include <stdbool.h>

uint16_t saturating_sub(uint16_t x, uint16_t y);
float get_angle(float cx, float cy, float ex, float ey);
bool aabb_collision(uint16_t pos_x1, uint16_t pos_y1, uint16_t size1, uint16_t pos_x2, uint16_t pos_y2, uint16_t size2);
#endif
