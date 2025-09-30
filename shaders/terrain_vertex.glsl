#version 150

in vec3 position0;
in vec3 normal0;
in vec2 tex_coord0;
in vec2 alpha_coord0;

out vec3 frag_normal;
out vec2 frag_tex_coord;
out vec2 frag_alpha_coord;

uniform mat4 view;
uniform mat4 projection;

void main() {
    frag_normal = normal0;
    frag_tex_coord = tex_coord0;
    frag_alpha_coord = alpha_coord0;
    gl_Position = projection * view * vec4(position0, 1.0);
}