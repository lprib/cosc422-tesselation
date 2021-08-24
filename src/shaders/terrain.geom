#version 400

layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

in VERTEX_DATA {
   vec4 world_position;
   vec3 normal;
} verts[];

out FRAG_DATA {
   smooth vec4 diffuse_color;
   smooth vec4 world_position;
   smooth vec3 normal;
} out_frag_data;

uniform vec3 light_pos;

void main() {
   vec3 diffuse_color = vec3(1., 1., 1.);

   for(int i = 0; i < gl_in.length(); i++) {
      vec3 light_dir = normalize(light_pos - verts[i].world_position.xyz);
      float n_dot_l = dot(light_dir, verts[i].normal);
      out_frag_data.diffuse_color = vec4(n_dot_l * diffuse_color, 1);
      // out_frag_data.diffuse_color = vec4(verts[i].normal, 1);
      // out_frag_data.diffuse_color = vec4(verts[i].color, 1);


      out_frag_data.world_position = verts[i].world_position;
      gl_Position = gl_in[i].gl_Position;
      EmitVertex();
   }
   EndPrimitive();
}

// vim: set ft=glsl:

