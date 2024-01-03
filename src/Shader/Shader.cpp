#include "shader.h"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

shader::shader(const char* vs_filepath, const char* fs_filepath) {
	std::ifstream vs_file(vs_filepath), fs_file(fs_filepath);
	std::stringstream vs_buffer, fs_buffer;
	std::string vs_str, fs_str;

	vs_file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	fs_file.exceptions(std::ifstream::failbit | std::ifstream::badbit);

	vs_buffer << vs_file.rdbuf();
	fs_buffer << fs_file.rdbuf();

	vs_str = vs_buffer.str();
	fs_str = fs_buffer.str();

	const char* vs_code = vs_str.c_str();
	const char* fs_code = fs_str.c_str();

	int success;
	char info_log[512];

	unsigned int vs = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vs, 1, &vs_code, nullptr);
	glCompileShader(vs);

	glGetShaderiv(vs, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(vs, 512, nullptr, info_log);
		std::cout << "Vertex Shader Compilation Failed\n" << info_log << std::endl;
	}

	unsigned int fs = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fs, 1, &fs_code, nullptr);
	glCompileShader(fs);

	glGetShaderiv(fs, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(fs, 512, nullptr, info_log);
		std::cout << "Fragment Shader Compilation Failed\n" << info_log << std::endl;
	}

	unsigned int shader_program = glCreateProgram();
	glAttachShader(shader_program, vs);
	glAttachShader(shader_program, fs);
	glLinkProgram(shader_program);

	glGetProgramiv(shader_program, GL_LINK_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(shader_program, 512, nullptr, info_log);
		std::cout << "Shader Program Linking Failed\n" << info_log << std::endl;
	}

	glDeleteShader(vs);
	glDeleteShader(fs);

	m_id = shader_program;
}

void shader::use() {
	glUseProgram(m_id);
}

void shader::set_uniform4f(const char* name, float x, float y, float z, float t) {
	glUniform4f(glGetUniformLocation(m_id, name), x, y, z, t);
}

void shader::set_uniform1i(const char* name, int i)
{
	glUniform1i(glGetUniformLocation(m_id, name), i);
}

void shader::set_uniform_mat4(const char* name, glm::mat4 matrix) {
	unsigned int transform_loc = glGetUniformLocation(m_id, name);
	glUniformMatrix4fv(transform_loc, 1, GL_FALSE, glm::value_ptr(matrix));
}