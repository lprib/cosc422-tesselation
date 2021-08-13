#include "shader.h"

#include <GL/freeglut.h>
#include <GL/glew.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

GLuint load_shader(GLenum shader_type, char *filename) {
  FILE *file = fopen(filename, "r");
  if (file == NULL) {
    printf("Error opening shader file %s\n", filename);
    exit(1);
  }
  fseek(file, 0, SEEK_END);
  long file_size = ftell(file);
  fseek(file, 0, SEEK_SET);

  char *file_string = malloc(file_size + 1);
  fread(file_string, 1, file_size, file);
  fclose(file);

  const char *file_string_const = (const char *)file_string;

  GLuint shader = glCreateShader(shader_type);
  glShaderSource(shader, 1, &file_string_const, NULL);
  glCompileShader(shader);
  GLint status;

  glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
  if (status == GL_FALSE) {
    GLint info_log_len;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &info_log_len);
    GLchar *info_log = (GLchar *)malloc(sizeof(GLchar) * (info_log_len + 1));
    memset(info_log, 0, info_log_len + 1);
    glGetShaderInfoLog(shader, info_log_len, NULL, info_log);
    const char *strShaderType = NULL;
    printf("Comile error in shader %s: %s\n", filename, info_log);
    free(info_log);
  } else {
  }

  return shader;
}
