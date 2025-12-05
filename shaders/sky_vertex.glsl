#version 150

in vec3 position0;

uniform mat4 view;
uniform mat4 projection;
uniform vec3 camera_position;
uniform float radius;

void main() {
    vec3 world_pos = camera_position + position0 * radius;
    gl_Position = projection * view * vec4(world_pos, 1.0);
}