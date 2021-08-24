#include "camera.h"

#include <cglm/cglm.h>
#include <GL/freeglut.h>
#include <math.h>
#include <stdbool.h>

// In units per second (used with frame delta time)
#define MOVE_SPEED 50
#define ANGLE_SPEED 1.5
#define LOOK_DISTANCE 90.0

// camera angle
static double angle = M_PI / 4.0;
static vec3 pos = {0.0, 15.0, 0.0};

// look vector
static double look_x = 0.0;
static double look_z = 0.0;

static mat4 view_matrix;

static double aspect_ratio;

// Which keys are currently held down (left, right, up, down) arrows
// respectively
static bool keysDown[] = {false, false, false, false};

// Updates the state of pressed keys in keysDownArray, based on the key
// provided. if press is true, the specified key will be set. if press is false
// it will be reset.
static void
update_key_array(int key, bool press)
{
    switch (key) {
    case GLUT_KEY_LEFT:
        keysDown[0] = press;
        break;
    case GLUT_KEY_RIGHT:
        keysDown[1] = press;
        break;
    case GLUT_KEY_UP:
        keysDown[2] = press;
        break;
    case GLUT_KEY_DOWN:
        keysDown[3] = press;
        break;
    }
}

void
camera_special_keys(int key, int x, int y)
{
    update_key_array(key, true);
}

void
camera_special_keys_up(int key, int x, int y)
{
    update_key_array(key, false);
}

void
update_look_dir()
{
    look_x = sin(angle);
    look_z = -cos(angle);
}

void
camera_init()
{
    glutIgnoreKeyRepeat(true);
    glutSpecialFunc(&camera_special_keys);
    glutSpecialUpFunc(&camera_special_keys_up);
    update_look_dir();
}

void
camera_update(double delta)
{
    // left
    if (keysDown[0]) {
        angle -= ANGLE_SPEED * delta;
        update_look_dir();
        glutPostRedisplay();
    }

    // right
    if (keysDown[1]) {
        angle += ANGLE_SPEED * delta;
        update_look_dir();
        glutPostRedisplay();
    }

    // up
    if (keysDown[2]) {
        pos[0] += look_x * MOVE_SPEED * delta;
        pos[2] += look_z * MOVE_SPEED * delta;
        glutPostRedisplay();
    }

    // down
    if (keysDown[3]) {
        pos[0] -= look_x * MOVE_SPEED * delta;
        pos[2] -= look_z * MOVE_SPEED * delta;
        glutPostRedisplay();
    }
}

vec4*
camera_get_view_matrix()
{
    glm_lookat(
        pos,
        (vec3){
            pos[0] + LOOK_DISTANCE * look_x, 1.0f,
            pos[2] + LOOK_DISTANCE * look_z},
        (vec3){0.0f, 1.0f, 0.0f}, view_matrix);
    return view_matrix;
}

float*
camera_get_pos()
{
    return pos;
}
