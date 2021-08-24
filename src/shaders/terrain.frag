#version 330


uniform vec3 light_pos;
uniform vec3 camera_pos;

uniform sampler2D sand_tex;
uniform sampler2D grass_tex;
uniform sampler2D snow_tex;
uniform sampler2D water_normal;

in FRAG_DATA {
   smooth vec4 world_position;
   smooth vec3 normal;
   smooth vec2 tex_coords;
   smooth vec4 tex_weights;
} data;

void main() 
{
   const float diffuse_shading_intensity = 0.8;
   const float texture_zoom = 10;
   const vec4 water_color = vec4(0.1, 0.4, 0.6, 1.);
   const float water_diffuse_normal_strength = 0.1;
   const vec4 specular_color = vec4(1., 1., 1., 1.);
   const float specular_power = 60.0;

   vec4 diffuse_color = data.tex_weights.x * texture(sand_tex, data.tex_coords * texture_zoom);
   diffuse_color += data.tex_weights.y * texture(grass_tex, data.tex_coords * texture_zoom);
   diffuse_color += data.tex_weights.z * texture(snow_tex, data.tex_coords * texture_zoom);
   diffuse_color += data.tex_weights.w * water_color;

   vec3 diffuse_normal = mix(
      data.normal,
      mix(
         vec3(0., 1., 0.),
         texture(water_normal, data.tex_coords * texture_zoom).xyz,
         water_diffuse_normal_strength
      ),
      data.tex_weights.w
   );

   vec3 light_dir = normalize(light_pos - data.world_position.xyz);
   float n_dot_l = diffuse_shading_intensity * dot(diffuse_normal, light_dir) + (1 - diffuse_shading_intensity);

   vec3 specular_normal = mix(
      data.normal,
      texture(water_normal, data.tex_coords * texture_zoom).xyz,
      data.tex_weights.w
   );
   vec3 r = normalize(reflect(light_dir, specular_normal));
   vec3 v = normalize(camera_pos - data.world_position.xyz);
   float specular_coeff = pow(max(dot(r, v), 0.0), specular_power);
   gl_FragColor = n_dot_l * diffuse_color + specular_coeff * specular_color;
   // gl_FragColor = n_dot_l * diffuse_color;
}

// vim: set ft=glsl:

