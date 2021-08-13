#ifndef SHADERS_H
#define SHADERS_H

#include <GL/glew.h>
#include <GL/freeglut.h>

GLuint
load_shader(GLenum shader_type, char* filename);

GLint
compile_program(char* vertex_filename, char* fragment_filename,
    char* tess_control_filename, char* tess_eval_filename);

#endif
