#include "load_image.h"
#include "shaders.h"
#include "camera.h"

#define GLM_FORCE_RADIANS
#include <GL/freeglut.h>
#include <GL/glew.h>
#include <cglm/cglm.h>
#include <stdio.h>
#include <string.h>

GLuint vao_id;
GLuint mvp_matrix_uniform;
GLuint camera_pos_uniform;

int old_elapsed_time = 0;

// Conversion from degrees to rad (required in GLM 0.9.6)
float deg_to_rad = 3.14159265 / 180.0;

// 10x10 grid (100 vertices)
float verts[100 * 3];

// Element array for 81 quad patches
GLushort elems[81 * 4];

mat4 proj_matrix, view_matrix;

void
generate_data()
{
    int indx, start;
    // verts array
    for (int i = 0; i < 10; i++) // 100 vertices on a 10x10 grid
    {
        for (int j = 0; j < 10; j++) {
            indx = 10 * i + j;
            verts[3 * indx] = 10 * i - 45; // x
            verts[3 * indx + 1] = 0;       // y
            verts[3 * indx + 2] = -10 * j; // z
        }
    }

    // elems array
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            indx = 9 * i + j;
            start = 10 * i + j;
            elems[4 * indx] = start;
            elems[4 * indx + 1] = start + 10;
            elems[4 * indx + 2] = start + 11;
            elems[4 * indx + 3] = start + 1;
        }
    }
}

void
load_textures()
{
    GLuint texture_id;
    glGenTextures(1, &texture_id);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture_id);
    load_bmp("terrain.bmp");

    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
}

void
init()
{
    load_textures();

    GLint program = compile_program(
        "src/shaders/terrain.vert", "src/shaders/terrain.frag",
        "src/shaders/terrain.cont", "src/shaders/terrain.eval");
    glUseProgram(program);

    GLuint height_map_uniform = glGetUniformLocation(program, "height_map");
    glUniform1i(height_map_uniform, 0);

    mvp_matrix_uniform = glGetUniformLocation(program, "mvp_matrix");
    camera_pos_uniform = glGetUniformLocation(program, "camera_pos");

    glm_perspective(30.0 * deg_to_rad, 1.25, 5.0, 500.0, proj_matrix);

    generate_data();

    GLuint vbo_id[2];
    glGenVertexArrays(1, &vao_id);
    glBindVertexArray(vao_id);

    glGenBuffers(2, vbo_id);

    glBindBuffer(GL_ARRAY_BUFFER, vbo_id[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(0); // Vertex position

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_id[1]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elems), elems, GL_STATIC_DRAW);

    glBindVertexArray(0);

    glPatchParameteri(GL_PATCH_VERTICES, 4);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
}

void
update(int value)
{
    int elapsed_time = glutGet(GLUT_ELAPSED_TIME);
    int delta = elapsed_time - old_elapsed_time;
    old_elapsed_time = elapsed_time;
    glutTimerFunc(16, &update, 0);
    double deltaSeconds = delta / 1000.0;
    camera_update(deltaSeconds);
    vec4* new_view_matrix = camera_get_view_matrix();
    glm_mat4_copy(new_view_matrix, view_matrix);
}

void
display()
{
    mat4 proj_view_matrix;
    glm_mat4_mul(proj_matrix, view_matrix, proj_view_matrix);

    glUniformMatrix4fv(
        mvp_matrix_uniform, 1, GL_FALSE, &proj_view_matrix[0][0]);

    float* new_camera_pos = camera_get_pos();
    glUniform3f(
        camera_pos_uniform, new_camera_pos[0], new_camera_pos[1],
        new_camera_pos[2]);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glBindVertexArray(vao_id);
    glDrawElements(GL_PATCHES, 81 * 4, GL_UNSIGNED_SHORT, NULL);
    glFlush();
}

void
gl_context_init(int argc, char** argv)
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
}

int
main(int argc, char** argv)
{
    gl_context_init(argc, argv);
    init();
    camera_init();
    glutDisplayFunc(display);
    update(0);
    glutMainLoop();
}
