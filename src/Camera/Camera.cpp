#include "camera.h"

#include <stdexcept>

static bool mouse_init = false;

static void static_mouse_callback(GLFWwindow* window, double xpos, double ypos) {
	if (camera* cam = reinterpret_cast<camera*>(glfwGetWindowUserPointer(window)))
		cam->mouse_callback(static_cast<float>(xpos), static_cast<float>(ypos));
	else
		throw new std::runtime_error("glfwGetWindowUserPointer() did not return a camera ptr");
}

camera::camera(GLFWwindow* window, unsigned int window_width, unsigned int window_height)
:	m_window(window),
	m_window_width(window_width),
	m_window_height(window_height),
	m_camera_front(glm::vec3(0.0f, 0.0f, -1.0f)), 
	m_camera_up(glm::vec3(0.0f, 1.0f, 0.0f)),
	m_camera_pos(glm::vec3(0.0f, 0.0f, 3.0f)),
	m_delta_time(0.0f),
	m_last_frame(0.0f),
	m_yaw(-90.0f),
	m_pitch(0.0f),
	m_last_x((float)window_width / 2.0f),
	m_last_y((float)window_height / 2.0f),
	m_fov(45.0f),
	m_sens(0.1f)
{
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	if (!glfwGetWindowUserPointer(window))
		glfwSetWindowUserPointer(window, reinterpret_cast<void*>(this));
	else
		throw new std::runtime_error("window user ptr has already been set");

	glfwSetCursorPosCallback(window, static_mouse_callback);
}

void camera::frame_update()
{
	float current_frame = glfwGetTime();
	m_delta_time = current_frame - m_last_frame;
	m_last_frame = current_frame;

	//input handling
	float camera_speed = 2.5f * m_delta_time;
	if (glfwGetKey(m_window, GLFW_KEY_W) == GLFW_PRESS)
		m_camera_pos += camera_speed * m_camera_front;
	if (glfwGetKey(m_window, GLFW_KEY_S) == GLFW_PRESS)
		m_camera_pos -= camera_speed * m_camera_front;
	if (glfwGetKey(m_window, GLFW_KEY_A) == GLFW_PRESS)
		m_camera_pos -= glm::normalize(glm::cross(m_camera_front, m_camera_up)) * camera_speed;
	if (glfwGetKey(m_window, GLFW_KEY_D) == GLFW_PRESS)
		m_camera_pos += glm::normalize(glm::cross(m_camera_front, m_camera_up)) * camera_speed;
}

void camera::mouse_callback(float xpos, float ypos) {
	if (!mouse_init) {
		m_last_x = xpos;
		m_last_y = ypos;
		mouse_init = true;
	}

	float xoffset = xpos - m_last_x;
	float yoffset = m_last_y - ypos;
	m_last_x = xpos;
	m_last_y = ypos;
	xoffset *= m_sens;
	yoffset *= m_sens;

	m_yaw += xoffset;
	m_pitch += yoffset;

	if (m_pitch > 89.0f)
		m_pitch = 89.0f;
	if (m_pitch < -89.0f)
		m_pitch = -89.0f;

	glm::vec3 front(0.0f);
	front.x = cos(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
	front.y = sin(glm::radians(m_pitch));
	front.z = sin(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
	m_camera_front = glm::normalize(front);
}


glm::mat4 camera::get_view_matrix() {
	return glm::lookAt(m_camera_pos, m_camera_pos + m_camera_front, m_camera_up);
}
