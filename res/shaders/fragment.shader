#version 330 core

in vec2 tex_coord;

out vec4 frag_color;

uniform sampler2D tex_img1;
uniform sampler2D tex_img2;

void main() {
	frag_color = mix(texture(tex_img1, tex_coord), texture(tex_img2, tex_coord), 0.2);
}