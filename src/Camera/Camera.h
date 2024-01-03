#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <GLFW/glfw3.h>


class camera {
private:
	GLFWwindow* m_window;
	unsigned int m_window_width;
	unsigned int m_window_height;

	glm::vec3 m_camera_front;
	glm::vec3 m_camera_up;
	glm::vec3 m_camera_pos;

	float m_delta_time;
	float m_last_frame;

	float m_yaw;
	float m_pitch;
	float m_last_x;
	float m_last_y;
	float m_fov;
	float m_sens;

public:
	camera(GLFWwindow* window, unsigned int window_width, unsigned int window_height);

	void frame_update();
	void mouse_callback(float xpos, float ypos);
	glm::mat4 get_view_matrix();
};