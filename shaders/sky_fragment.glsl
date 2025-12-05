#version 150

out vec4 fragColor;

uniform vec3 sky_color;

void main() {
    fragColor = vec4(sky_color, 1.0);
}