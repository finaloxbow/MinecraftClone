#pragma once

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include <GLFW/glfw3.h>

class Camera
{
private:
	//camera
	glm::vec3 cameraPos;
	glm::vec3 cameraFront;
	glm::vec3 cameraUp;

	//mouse controls
	bool firstMouse;
	float yaw;
	float pitch;
	float lastX;
	float lastY;
	float fov;

	//time calculations
	static float deltaTime;
	static float lastFrame;

public:


	Camera();

	void Set_Data(float fovIn);
	//mouse callback
	void mouse_callback(GLFWwindow* window, double xposIn, double yposIn);
	//keyboard input
	void keyboard_input(GLFWwindow* window);
	//set cursor callback
	void set_cursor_callback(GLFWwindow* window, Camera* camera);
	//calculate time
	void calc_time(float currentFrame);

	inline float getFrameTime() { return deltaTime; }

	//MVP matrix getters
	glm::mat4 getPerspectiveMatrix();
	glm::mat4 getViewMatrix();
};