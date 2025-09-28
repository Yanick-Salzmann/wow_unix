#version 330 core

in vec3 frag_normal;
in vec2 frag_tex_coord;
in vec2 frag_alpha_coord;

void main() {
    vec3 light_dir = normalize(vec3(1.0, 1.0, 1.0));
    float diff = max(dot(normalize(frag_normal), light_dir), 0.0);

    vec3 base_color = vec3(0.2, 0.7, 0.2);
    vec3 final_color = base_color * diff;

    gl_FragColor = vec4(final_color, 1.0);
}