#version 330

in FRAG_DATA {
   smooth vec4 world_position;
   smooth vec3 normal;
   smooth vec2 tex_coords;
   smooth vec4 tex_weights;
   smooth float water_depth;
} data;

uniform vec3 light_pos;
uniform vec3 camera_pos;

uniform sampler2D sand_tex;
uniform sampler2D grass_tex;
uniform sampler2D snow_tex;
uniform sampler2DArray water_normal;
uniform float water_animation_idx;

const float diffuse_shading_intensity = 0.8;
const float texture_zoom = 10;
const float water_texture_zoom = 5;
const float water_diffuse_normal_strength = 0.3;

const float specular_power = 200.0;
const vec4 specular_color = 0.6 * vec4(0.9, 0.85, 0.7, 1.0);

const vec4 water_color_shallow = vec4(0.51, 0.9, 1., 1.);
const vec4 water_color_deep = vec4(0., 0., 0.05, 0.1);

void main() 
{

   vec4 water_color = mix(water_color_shallow, water_color_deep, data.water_depth);

   vec4 diffuse_color = data.tex_weights.x * texture(sand_tex, data.tex_coords * texture_zoom);
   diffuse_color += data.tex_weights.y * texture(grass_tex, data.tex_coords * texture_zoom);
   diffuse_color += data.tex_weights.z * texture(snow_tex, data.tex_coords * texture_zoom);
   diffuse_color += data.tex_weights.w * water_color;

   vec3 water_normal_frag = texture(water_normal, vec3(data.tex_coords.xy * water_texture_zoom, water_animation_idx)).xyz;

   // diffuse lighting normal is either terrain normal (`data.normal`) or water
   // normal based on whether this fragment is part of the water
   // (`data.tex_weights.w`)
   vec3 diffuse_normal = mix(
      data.normal,

      // Create water normals: mix between straight up (no variation) and
      // normal map with `water_diffuse_normal_strength`
      mix(
         vec3(0., 1., 0.),
         water_normal_frag,
         water_diffuse_normal_strength
      ),
      data.tex_weights.w
   );

   vec3 light_dir = normalize(light_pos - data.world_position.xyz);
   float n_dot_l = diffuse_shading_intensity * dot(diffuse_normal, light_dir) + (1 - diffuse_shading_intensity);

   // reflection vec
   vec3 reflection = normalize(reflect(light_dir, water_normal_frag));
   // view vec
   vec3 view = normalize(camera_pos - data.world_position.xyz);
   float specular_coeff = pow(max(dot(reflection, view), 0.0), specular_power);

   // multiply the overall specular color by `data.tex_weights.w` so that only
   // the water is specular
   vec4 specular = specular_coeff * specular_color * data.tex_weights.w;

   gl_FragColor = n_dot_l * diffuse_color + specular;
}

// vim: set ft=glsl:

