#version 400

layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

in VERTEX_DATA {
   vec4 world_position;
   vec3 normal;
   vec2 tex_coords;
   vec4 tex_weights;
} verts[];

out FRAG_DATA {
   smooth vec4 world_position;
   smooth vec3 normal;
   smooth vec2 tex_coords;
   smooth vec4 tex_weights;
} frag;

void main() {
   for(int i = 0; i < gl_in.length(); i++) {
      frag.world_position = verts[i].world_position;
      frag.normal = verts[i].normal;
      frag.tex_coords = verts[i].tex_coords;
      frag.tex_weights = verts[i].tex_weights;
      gl_Position = gl_in[i].gl_Position;
      EmitVertex();
   }
   EndPrimitive();
}

// vim: set ft=glsl:

