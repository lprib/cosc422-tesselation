#version 330


uniform vec3 light_pos;

uniform sampler2D sand_tex;
uniform sampler2D grass_tex;
uniform sampler2D snow_tex;

in FRAG_DATA {
   smooth vec4 diffuse_color;
   smooth vec4 world_position;
   smooth vec3 normal;
} data;

void main() 
{
   gl_FragColor = data.diffuse_color;
   // gl_FragColor = vec4(data.normal, 1);
   // gl_FragColor = vec4(data.world_position.xyz / 100, 1);
   // gl_FragColor = vec4(n_dot_l, 0, 1, 1);
   // gl_FragColor = vec4(data.world_position.xyz / 100, 1);
   // gl_FragColor = vec4(0, 0, data.world_position.x / 100, 1);
   // gl_FragColor = vec4(data.world_position.x / 100, -data.world_position.z / 100, 0, 1);

}

// vim: set ft=glsl:

