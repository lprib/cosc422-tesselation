#include "load_image.h"
#include "camera.h"
#include "terrain.h"

#define GLM_FORCE_RADIANS
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <cglm/cglm.h>
#include <stdio.h>
#include <string.h>

int old_elapsed_time = 0;
mat4 proj_matrix;

void
opengl_init(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_DEPTH);
    glutInitWindowSize(1000, 800);
    glutCreateWindow("terrain");
    glutInitContextVersion(4, 2);
    glutInitContextProfile(GLUT_CORE_PROFILE);

    if (glewInit() == GLEW_OK) {
        printf("GLEW initialization successful\n");
        printf("Using GLEW version %s\n", glewGetString(GLEW_VERSION));
    } else {
        printf("Unable to initialize GLEW\n");
        exit(EXIT_FAILURE);
    }

    glm_perspective(30.0 * 3.14159265 / 180.0, 1.25, 5.0, 500.0, proj_matrix);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
}

void
update(int value)
{
    int elapsed_time = glutGet(GLUT_ELAPSED_TIME);
    int delta = elapsed_time - old_elapsed_time;
    old_elapsed_time = elapsed_time;
    glutTimerFunc(16, &update, 0);
    double deltaSeconds = delta / 1000.0;
    terrain_update(deltaSeconds);
    camera_update(deltaSeconds);
}

void
display()
{
    mat4 view_proj_matrix;
    glm_mat4_mul(proj_matrix, camera_get_view_matrix(), view_proj_matrix);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    terrain_display(view_proj_matrix, camera_get_pos());
    glFlush();
}

int
main(int argc, char** argv)
{
    opengl_init(argc, argv);
    camera_init();
    terrain_init();
    glutDisplayFunc(display);
    update(0);
    glutMainLoop();
}
