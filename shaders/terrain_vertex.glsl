#version 150

in vec3 position0;
in vec3 normal0;
in vec2 tex_coord0;
in vec2 alpha_coord0;
in vec3 vertex_color0;

out vec3 frag_normal;
out vec2 frag_tex_coord;
out vec2 frag_alpha_coord;
out vec3 frag_vertex_color;
out vec3 world_position;

uniform mat4 view;
uniform mat4 projection;

void main() {
    frag_normal = normal0;
    frag_tex_coord = tex_coord0;
    frag_alpha_coord = alpha_coord0;
    frag_vertex_color = vertex_color0;
    world_position = position0;

    gl_Position = projection * view * vec4(position0, 1.0);
}