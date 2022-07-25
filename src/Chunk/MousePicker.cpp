#include "MousePicker.h"


MousePicker::MousePicker(Camera* camera, GLFWwindow* window_ptr)
	: window(window_ptr),
	currentRay(glm::vec3(0, 0, 0)),
	camera(camera)
{
}

glm::vec3 MousePicker::getCurrentRay()
{
	return currentRay;
}

void MousePicker::update()
{
	currentRay = calcMouseRay(1);
}

glm::vec3 MousePicker::calcMouseRay(float scalingCoeff)
{
	//ray to use for intersection testing
	glm::vec3 frontRay = scalingCoeff * camera->getCameraFront()
		+ camera->getPosition();

	return frontRay;
}