#version 150

in vec3 frag_normal;
in vec2 frag_tex_coord;
in vec2 frag_alpha_coord;
in vec3 frag_vertex_color;

in vec3 world_position;

uniform sampler2D shadow_texture;

uniform sampler2D color_texture0;
uniform sampler2D color_texture1;
uniform sampler2D color_texture2;
uniform sampler2D color_texture3;

uniform vec4 camera_position;
uniform vec3 sun_direction;
uniform vec4 fog_color = vec4(0.5, 0.7, 1.0, 1.0);
uniform vec4 diffuse_color = vec4(1.0, 1.0, 1.0, 1.0);
uniform vec4 ambient_color = vec4(0.0);
uniform float fog_distance = 150.0f;

out vec4 target_color;

void main() {
    vec3 light_dir = normalize(sun_direction);
    float light = dot(normalize(frag_normal), light_dir);
    light = clamp(light, 0.0, 1.0);
    light = 1.0 - (1.0 - light) * (1.0 - light);
    light *= 0.6;

    vec3 color0 = texture(color_texture0, frag_tex_coord).rgb;
    vec3 color1 = texture(color_texture1, frag_tex_coord).rgb;
    vec3 color2 = texture(color_texture2, frag_tex_coord).rgb;
    vec3 color3 = texture(color_texture3, frag_tex_coord).rgb;

    vec4 alphas = texture(shadow_texture, frag_alpha_coord);
    float alpha = alphas.b;
    float alpha2 = alphas.g;
    float alpha3 = alphas.r;

    vec3 base_color = color0;
    base_color = mix(base_color, color1, alpha);
    base_color = mix(base_color, color2, alpha2);
    base_color = mix(base_color, color3, alpha3);
    base_color = base_color * frag_vertex_color * 2.0;
    vec3 final_color = base_color * (light * diffuse_color.bgr + ambient_color.bgr);

    float shadow = alphas.a;
    final_color *= 1.0f - shadow * 0.5f;

    float distance = length(camera_position.xyz - world_position);
    float factor = 1.0f - clamp((fog_distance - distance) / 150.0f, 0.0f, 1.0f);
    final_color = mix(final_color, fog_color.bgr, factor);

    target_color = vec4(final_color, 1.0);
}