#version 400


uniform vec3 light_pos;

in FRAG_DATA {
   smooth vec4 world_position;
   flat vec3 normal;
} data;

void main() 
{
   vec3 base_color = vec3(1, 1, 1);
   vec3 diffuse_color = vec3(0, 0, 0);

   vec3 light_dir = normalize(light_pos - data.world_position.xyz);
   float n_dot_l = dot(data.normal, light_dir);
   // gl_FragColor = vec4(data.diffuse, 0, 1, 1);
   gl_FragColor = vec4(n_dot_l * base_color, 1);
   // gl_FragColor = vec4(n_dot_l, 0, 1, 1);
}

// vim: set ft=glsl:

