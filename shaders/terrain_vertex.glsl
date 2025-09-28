#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 tex_coord;
layout (location = 3) in vec2 alpha_coord;

out vec3 frag_normal;
out vec2 frag_tex_coord;
out vec2 frag_alpha_coord;

uniform mat4 view_projection;

void main() {
    frag_normal = normal;
    frag_tex_coord = tex_coord;
    frag_alpha_coord = alpha_coord;
    gl_Position = view_projection * vec4(position, 1.0);
}