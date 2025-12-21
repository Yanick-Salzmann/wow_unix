#version 150

in vec2 frag_tex_coord;

out vec4 fragColor;

uniform vec3 sky_color;

void main() {
    fragColor = vec4(frag_tex_coord, 1.0, 1.0);
}