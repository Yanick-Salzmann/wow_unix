#version 150

in vec2 frag_tex_coord;

out vec4 fragColor;

uniform sampler2D sky_texture;

void main() {
    fragColor = vec4(texture(sky_texture, vec2(0.0, frag_tex_coord.y)).bgr, 1.0);
}