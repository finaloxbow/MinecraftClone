#version 330 core

layout (location = 0) in vec3 in_pos;
layout (location = 1) in vec2 in_tex_coord;

out vec2 tex_coord;

uniform mat4 transform;

void main() {
	gl_Position = transform * vec4(in_pos, 1.0);
	tex_coord = in_tex_coord;
}