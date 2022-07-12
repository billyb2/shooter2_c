#ifndef CAMERA_H
#define CAMERA_H
#include <stdint.h>
#include "map.h"
#include "include/raylib.h"

void move_camera(Camera2D* camera, const Map* map, uint16_t pos_x, uint16_t pos_y);

#endif
