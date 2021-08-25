#include "terrain.h"
#include "load_image.h"
#include "shaders.h"
#include <GL/freeglut.h>
#include <stdbool.h>
#include <cglm/cglm.h>

static GLuint vao_id;

static GLuint mvp_matrix_uniform;
static GLuint camera_pos_uniform;

// Geometry parameters
#define MAP_WIDTH 10
#define MAP_HEIGHT 10
#define VERT_SPACING 10
#define NUM_VERTS (MAP_WIDTH * MAP_HEIGHT)
#define NUM_PATCHES ((MAP_WIDTH - 1) * (MAP_HEIGHT - 1))
#define VERTS_PER_PATCH 4
static float verts[NUM_VERTS * 3];
static GLushort vert_indices[NUM_PATCHES * VERTS_PER_PATCH];

// Water animation parameters
#define WATER_ANIMATION_LENGTH 32
static GLuint water_normal_uniform;
static int water_animation_start;
static int water_animation_offset = 0;
static double water_animation_delta = 0.0;
static double water_animation_framerate = 15.0;

// Keyboard control parameters
#define LEVEL_DELTA 0.05
#define WATER_LEVEL_MIN 0.0
#define WATER_LEVEL_MAX 0.99
#define SNOW_LEVEL_MIN 0.37
#define SNOW_LEVEL_MAX 0.8
static float water_level = WATER_LEVEL_MIN;
static float snow_level = SNOW_LEVEL_MIN;
static GLuint water_level_uniform;
static GLuint snow_level_uniform;

static bool wireframe = false;

static void
update_levels();

static void
process_keys(unsigned char key, int x, int y);

static void
generate_geometry();

static void
load_texture(GLuint uniform, GLuint texture_unit, char* filename);

void
load_water_textures(GLuint first_texture_unit);

void
terrain_init()
{
    generate_geometry();

    GLint program = compile_program(
        "src/shaders/terrain.vert",
        "src/shaders/terrain.frag",
        "src/shaders/terrain.cont",
        "src/shaders/terrain.eval",
        "src/shaders/terrain.geom");
    glUseProgram(program);

    mvp_matrix_uniform = glGetUniformLocation(program, "mvp_matrix");
    camera_pos_uniform = glGetUniformLocation(program, "camera_pos");
    water_normal_uniform = glGetUniformLocation(program, "water_normal");
    water_level_uniform = glGetUniformLocation(program, "water_level");
    snow_level_uniform = glGetUniformLocation(program, "snow_level");

    glUniform4f(
        glGetUniformLocation(program, "object_bounds"),
        0.0,
        (MAP_HEIGHT - 1) * VERT_SPACING,
        0.0,
        -(MAP_WIDTH - 1) * VERT_SPACING);

    glUniform3f(
        glGetUniformLocation(program, "light_pos"),
        -100.0,
        100.0,
        100.0);
    glUniform1f(
        glGetUniformLocation(program, "patch_point_spacing"),
        VERT_SPACING);

    load_texture(
        glGetUniformLocation(program, "height_map"),
        0,
        "assets/terrain2.bmp");
    load_texture(
        glGetUniformLocation(program, "grass_tex"),
        1,
        "assets/grass.bmp");
    load_texture(
        glGetUniformLocation(program, "sand_tex"),
        2,
        "assets/sand.bmp");
    load_texture(
        glGetUniformLocation(program, "snow_tex"),
        3,
        "assets/snow.bmp");
    load_water_textures(4);
    glUniform1i(water_normal_uniform, 4);

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
        GL_ELEMENT_ARRAY_BUFFER,
        sizeof(vert_indices),
        vert_indices,
        GL_STATIC_DRAW);

    glBindVertexArray(0);
    glPatchParameteri(GL_PATCH_VERTICES, VERTS_PER_PATCH);

    update_levels();
    glutKeyboardFunc(&process_keys);
}

void
terrain_display(mat4 view_proj_matrix, vec3 camera_pos)
{
    glUniformMatrix4fv(
        mvp_matrix_uniform,
        1,
        GL_FALSE,
        &view_proj_matrix[0][0]);
    glUniform3f(
        camera_pos_uniform,
        camera_pos[0],
        camera_pos[1],
        camera_pos[2]);

    glBindVertexArray(vao_id);
    glDrawElements(
        GL_PATCHES,
        NUM_PATCHES * VERTS_PER_PATCH,
        GL_UNSIGNED_SHORT,
        NULL);
}

void
terrain_update(double delta)
{
    water_animation_delta += delta;
    if (water_animation_delta > 1 / water_animation_framerate) {
        water_animation_delta -= 1 / water_animation_framerate;
        water_animation_offset++;
        glUniform1i(
            water_normal_uniform,
            water_animation_start + water_animation_offset % 32);
        glutPostRedisplay();
    }
}

static void
update_levels()
{
    if (water_level > WATER_LEVEL_MAX) {
        water_level = WATER_LEVEL_MAX;
    } else if (water_level < WATER_LEVEL_MIN) {
        water_level = WATER_LEVEL_MIN;
    }
    if (snow_level > SNOW_LEVEL_MAX) {
        snow_level = SNOW_LEVEL_MAX;
    } else if (snow_level < SNOW_LEVEL_MIN) {
        snow_level = SNOW_LEVEL_MIN;
    }
    glUniform1f(water_level_uniform, water_level);
    glUniform1f(snow_level_uniform, snow_level);
}

static void
process_keys(unsigned char key, int x, int y)
{
    switch (key) {
    case 'q':
        water_level += LEVEL_DELTA;
        update_levels();
        break;
    case 'a':
        water_level -= LEVEL_DELTA;
        update_levels();
        break;
    case 'w':
        snow_level += LEVEL_DELTA;
        update_levels();
        break;
    case 's':
        snow_level -= LEVEL_DELTA;
        update_levels();
        break;
    case ' ':
        wireframe = !wireframe;
        if (wireframe) {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        } else {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }
        break;
    case '1':
        break;
    case '2':
        break;
    }
}

static void
generate_geometry()
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

static void
load_texture(GLuint uniform, GLuint texture_unit, char* filename)
{
#ifdef TRACE
    printf("loading %s as texture unit %d\n", filename, texture_unit);
#endif
    GLuint texture_id;
    glGenTextures(1, &texture_id);
    glActiveTexture(GL_TEXTURE0 + texture_unit);
    glBindTexture(GL_TEXTURE_2D, texture_id);
    load_bmp(filename);

    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glUniform1i(uniform, texture_unit);
}

void
load_water_textures(GLuint first_texture_unit)
{
    water_animation_start = first_texture_unit;
    GLuint water_textures[WATER_ANIMATION_LENGTH];
    glGenTextures(WATER_ANIMATION_LENGTH, water_textures);

    for (int i = 0; i < WATER_ANIMATION_LENGTH; i++) {
        int texture_unit = GL_TEXTURE0 + first_texture_unit + i;
        glActiveTexture(texture_unit);
        glBindTexture(GL_TEXTURE_2D, water_textures[i]);
        char filename[30];
        snprintf(filename, 30, "assets/water_normal/%04d.bmp", i + 1);
#ifdef TRACE
        printf("loading %s as texture unit %d\n", filename, texture_unit);
#endif
        load_bmp(filename);

        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    }
}
