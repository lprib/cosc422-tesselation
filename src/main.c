#include "load_image.h"
#include "shader.h"

#define GLM_FORCE_RADIANS
#include <GL/freeglut.h>
#include <GL/glew.h>
#include <cglm/cglm.h>
#include <stdio.h>
#include <string.h>

GLuint vaoID;
GLuint mvpMatrixLoc;

// Conversion from degrees to rad (required in GLM 0.9.6)
float CDR = 3.14159265 / 180.0;

// 10x10 grid (100 vertices)
float verts[100 * 3];

// Element array for 81 quad patches
GLushort elems[81 * 4];

mat4 projView;

void
generateData()
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
    mat4 proj, view;

    load_textures();

    GLuint vertex_shader = load_shader(GL_VERTEX_SHADER, "src/terrain_patches.vert");
    GLuint fragment_shader = load_shader(GL_FRAGMENT_SHADER, "src/terrain_patches.frag");

    GLuint program = glCreateProgram();
    glAttachShader(program, vertex_shader);
    glAttachShader(program, fragment_shader);

    glLinkProgram(program);
    GLint status;
    glGetProgramiv(program, GL_LINK_STATUS, &status);
    if (status == GL_FALSE) {
        GLint info_log_len;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &info_log_len);
        GLchar* info_log = (GLchar*)malloc(sizeof(GLchar) * (info_log_len + 1));
        memset(info_log, 0, info_log_len + 1);
        glGetProgramInfoLog(program, info_log_len, NULL, info_log);
        fprintf(stderr, "Linker failure: %s\n", info_log);
        free(info_log);
    }
    glUseProgram(program);

    mvpMatrixLoc = glGetUniformLocation(program, "mvpMatrix");
    GLuint texLoc = glGetUniformLocation(program, "heightMap");
    glUniform1i(texLoc, 0);
    glm_perspective(30.0 * CDR, 1.25, 20.0, 500.0, proj);
    glm_lookat((vec3){0.0, 20.0, 30.0}, (vec3){0.0, 0.0, -40.0},
        (vec3){0.0, 1.0, 0.0}, view);
    glm_mat4_mul(proj, view, projView);

    generateData();

    GLuint vboID[2];
    glGenVertexArrays(1, &vaoID);
    glBindVertexArray(vaoID);

    glGenBuffers(2, vboID);

    glBindBuffer(GL_ARRAY_BUFFER, vboID[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(0); // Vertex position

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboID[1]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elems), elems, GL_STATIC_DRAW);

    glBindVertexArray(0);

    glPatchParameteri(GL_PATCH_VERTICES, 4);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
}

void
display()
{
    glUniformMatrix4fv(mvpMatrixLoc, 1, GL_FALSE, &projView[0][0]);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glBindVertexArray(vaoID);
    glDrawElements(GL_QUADS, 81 * 4, GL_UNSIGNED_SHORT, NULL);
    glFlush();
}

void
gl_init(int argc, char** argv)
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
    gl_init(argc, argv);
    init();
    glutDisplayFunc(display);
    glutMainLoop();
}
