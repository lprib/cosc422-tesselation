#include "terrain.h"
#include "load_image.h"
#include "shaders.h"
#include <GL/freeglut.h>
#include <cglm/cglm.h>

GLuint vao_id;

GLuint mvp_matrix_uniform;
GLuint camera_pos_uniform;
GLuint height_map_uniform;

#define MAP_WIDTH 10
#define MAP_HEIGHT 10
#define VERT_SPACING 10

#define NUM_VERTS (MAP_WIDTH * MAP_HEIGHT)
#define NUM_PATCHES ((MAP_WIDTH - 1) * (MAP_HEIGHT - 1))

#define VERTS_PER_PATCH 4

static float verts[NUM_VERTS * 3];
static GLushort vert_indices[NUM_PATCHES * VERTS_PER_PATCH];

void
generate_data()
{
    int index, start;

    for (int i = 0; i < MAP_HEIGHT; i++) {
        for (int j = 0; j < MAP_WIDTH; j++) {
            index = MAP_WIDTH * i + j;
            verts[3 * index] = VERT_SPACING * i;
            verts[3 * index + 1] = 0;
            verts[3 * index + 2] = -VERT_SPACING * j;
        }
    }

    for (int i = 0; i < (MAP_HEIGHT - 1); i++) {
        for (int j = 0; j < (MAP_WIDTH - 1); j++) {
            index = (MAP_WIDTH - 1) * i + j;
            start = MAP_WIDTH * i + j;
            vert_indices[VERTS_PER_PATCH * index] = start;
            vert_indices[VERTS_PER_PATCH * index + 1] = start + MAP_WIDTH;
            vert_indices[VERTS_PER_PATCH * index + 2] = start + MAP_WIDTH + 1;
            vert_indices[VERTS_PER_PATCH * index + 3] = start + 1;
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
    /* load_bmp("terrain.bmp"); */
    load_tga("terrain.tga");

    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
}

void
terrain_init()
{
    load_textures();
    generate_data();

    GLint program = compile_program(
        "src/shaders/terrain.vert", "src/shaders/terrain.frag",
        "src/shaders/terrain.cont", "src/shaders/terrain.eval",
        "src/shaders/terrain.geom");
    glUseProgram(program);

    mvp_matrix_uniform = glGetUniformLocation(program, "mvp_matrix");
    camera_pos_uniform = glGetUniformLocation(program, "camera_pos");
    height_map_uniform = glGetUniformLocation(program, "height_map");
    GLuint object_bounds_uniform =
        glGetUniformLocation(program, "object_bounds");

    glUniform4f(
        object_bounds_uniform, 0, MAP_WIDTH * VERT_SPACING, 0,
        MAP_HEIGHT * VERTS_PER_PATCH);
    glUniform1i(height_map_uniform, 0);

    glGenVertexArrays(1, &vao_id);
    glBindVertexArray(vao_id);

    GLuint vbo_id[2];
    glGenBuffers(2, vbo_id);

    glBindBuffer(GL_ARRAY_BUFFER, vbo_id[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(0); // Vertex position

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_id[1]);
    glBufferData(
        GL_ELEMENT_ARRAY_BUFFER, sizeof(vert_indices), vert_indices,
        GL_STATIC_DRAW);

    glBindVertexArray(0);

    glPatchParameteri(GL_PATCH_VERTICES, VERTS_PER_PATCH);
    // glPatchParameteri(GL_QUADS, VERTS_PER_PATCH);
}

void
terrain_display(mat4 view_proj_matrix, vec3 camera_pos)
{
    glUniformMatrix4fv(
        mvp_matrix_uniform, 1, GL_FALSE, &view_proj_matrix[0][0]);
    glUniform3f(
        camera_pos_uniform, camera_pos[0], camera_pos[1], camera_pos[2]);

    glBindVertexArray(vao_id);
    glDrawElements(
        GL_PATCHES, NUM_PATCHES * VERTS_PER_PATCH, GL_UNSIGNED_SHORT, NULL);
}
