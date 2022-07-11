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

	static const int SCR_HEIGHT = 1080;
	static const int SCR_WIDTH = 1920;

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
	inline glm::vec3 getPosition() { return cameraPos; };

	//MVP matrix getters
	glm::mat4 getPerspectiveMatrix();
	glm::mat4 getViewMatrix();
};