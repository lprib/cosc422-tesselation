#version 330

layout (location = 0) in vec4 position;

uniform mat4 mvp_matrix;

void main()
{
   // gl_Position = mvp_matrix * position;
   gl_Position = position;
  
}

// vim: set ft=glsl:

