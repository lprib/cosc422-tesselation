#ifndef SHADER_H
#define SHADER_H

#include <GL/glew.h>
#include <GL/freeglut.h>

GLuint
load_shader(GLenum shader_type, char* filename);

#endif
