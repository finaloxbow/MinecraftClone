#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class shader {
private:
	unsigned int m_id;

public:
	shader(const char* vs_filepath, const char* fs_filepath);

	void use();

	void set_uniform4f(const char* name, float x, float y, float z, float t);
	void set_uniform1i(const char* name, int i);
	void set_uniform_mat4(const char* name, glm::mat4 matrix);
};