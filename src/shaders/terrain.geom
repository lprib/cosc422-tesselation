#version 400

layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

in VERTEX_DATA {
    vec4 world_position;
} verts[];

out FRAG_DATA {
   smooth vec4 world_position;
   flat vec3 normal;
} out_frag_data;

void main() {
   vec3 norm = normalize(cross(
      verts[1].world_position.xyz - verts[0].world_position.xyz,
      verts[2].world_position.xyz - verts[0].world_position.xyz
   ));

   for(int i = 0; i < gl_in.length(); i++) {
      out_frag_data.normal = norm;
      out_frag_data.world_position = verts[i].world_position;
      gl_Position = gl_in[i].gl_Position;
      EmitVertex();
   }
   EndPrimitive();
}

// vim: set ft=glsl:

