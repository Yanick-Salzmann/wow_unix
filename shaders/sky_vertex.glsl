#version 150

in vec3 position0;
in vec2 tex_coord0;

uniform mat4 view;
uniform mat4 projection;

out vec2 frag_tex_coord;

void main() {
    frag_tex_coord = tex_coord0;
    vec3 world_pos = position0;
    gl_Position = projection * view * vec4(world_pos, 1.0);
}