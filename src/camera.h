#ifndef CAMERA_H
#define CAMERA_H
#include "map.h"
#include "include/raylib.h"

void move_camera(Camera2D* camera, const Map* map, float pos_x, float pos_y);

#endif
