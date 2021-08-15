#version 400

layout(triangles) in;
layout(triangle_strip) out;

void main() {
   for(int i = 0; i < gl_in.length(); i++) {
      gl_Position = gl_in[i].gl_Position;
      EmitVertex();
   }
   EndPrimitive();
}

// vim: set ft=glsl:

