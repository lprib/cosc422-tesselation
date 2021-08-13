#ifndef CAMERA_H
#define CAMERA_H

#include <cglm/cglm.h>

// Initialise the camera controller
void
camera_init();

// Update the camera with frame delta time
void
camera_update(double delta);

vec4*
camera_get_view_matrix();

float*
camera_get_pos();

#endif
