#ifndef TERRAIN_H
#define TERRAIN_H

#include <cglm/cglm.h>

void
terrain_init();

void
terrain_display(mat4 view_proj_matrix, vec3 camera_pos);

void
terrain_update(double delta);

#endif
