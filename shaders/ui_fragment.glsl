#version 330 core

in vec2 frag_tex_coord;
out vec4 frag_color;

uniform sampler2D ui_texture;

void main() {
    frag_color = texture(ui_texture, frag_tex_coord);
}
