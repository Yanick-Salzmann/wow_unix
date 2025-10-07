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

uniform vec3 camera_position;

out vec4 target_color;

void main() {
    vec3 light_dir = normalize(vec3(1.0, 1.0, 1.0));
    float diff = max(dot(normalize(frag_normal), light_dir), 0.2);

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
    vec3 final_color = base_color * diff;

    float shadow = alphas.a;
    final_color *= 1.0f - shadow * 0.5f;

    float distance = length(camera_position - world_position);

    target_color = vec4(final_color * frag_vertex_color * 2.0, 1.0);
}