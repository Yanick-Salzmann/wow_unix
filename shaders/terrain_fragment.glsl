#version 150

in vec3 frag_normal;
in vec2 frag_tex_coord;
in vec2 frag_alpha_coord;
in vec3 frag_vertex_color;

uniform sampler2D shadow_texture;

out vec4 target_color;

void main() {
    vec3 light_dir = normalize(vec3(1.0, 1.0, 1.0));
    float diff = max(dot(normalize(frag_normal), light_dir), 0.2);

    vec3 base_color = vec3(1.0, 1.0, 1.0);
    vec3 final_color = base_color * diff;

    float shadow = texture(shadow_texture, frag_alpha_coord).a;
    float shadow2 = texture(shadow_texture, frag_tex_coord).a * 0.0001;
    final_color *= 1.0f - (shadow + shadow2) * 0.5f;

    target_color = vec4(final_color * frag_vertex_color * 2.0, 1.0);
}