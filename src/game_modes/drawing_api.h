#ifndef DRAWING_API_H
#define DRAWING_API_H
#include <stdint.h>

typedef enum Shape {
	DrawableCircle,
	DrawableRectangle,

} Shape;

typedef struct DrawableObject {
	float pos_x;
	float pos_y;
	float width;
	float height;
	uint32_t color;
	Shape shape;

} DrawableObject;
#endif
