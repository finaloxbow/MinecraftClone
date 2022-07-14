#include "MousePicker.h"

//"reach" of the mouse picker ray
const static float REACH_DISTANCE = 4.0f;


MousePicker::MousePicker(Camera* camera, GLFWwindow *window_ptr)
	: window(window_ptr),
		currentRay(glm::vec3(0.0f, 0.0f, 0.0f)),
		camera(camera),
		viewMatrix(camera->getViewMatrix())
{
}

glm::vec3 MousePicker::getCurrentRay()
{
	return currentRay;
}

void MousePicker::update()
{
	viewMatrix = camera->getViewMatrix();
	currentRay = calcMouseRay();
}

glm::vec3 MousePicker::calcMouseRay()
{
	double mouseX, mouseY;
	glfwGetCursorPos(window, &mouseX, &mouseY);
	
	glm::vec2 normCoords = getNormalizedDeviceCoords(mouseX, mouseY);
	glm::vec4 clipCoords = glm::vec4(normCoords.x, normCoords.y,
			-1.0f, 1.0f);
	glm::vec4 eyeCoords = toEyeCoords(clipCoords);
	glm::vec3 worldRay = toWorldCoords(eyeCoords);
	worldRay = glm::normalize(worldRay);
	worldRay = glm::vec3(REACH_DISTANCE * worldRay.x, 
						 REACH_DISTANCE * worldRay.y,
						 REACH_DISTANCE * worldRay.z);

	return worldRay;

}

glm::vec3 MousePicker::toWorldCoords(glm::vec4 eyeCoords) {
	glm::mat4 inverseView = glm::inverse(camera->getViewMatrix());
	glm::vec4 rayWorld = inverseView * eyeCoords;
	glm::vec3 mouseRay = glm::vec3(rayWorld.x, rayWorld.y, rayWorld.z);

	return glm::normalize(mouseRay);
}

glm::vec4 MousePicker::toEyeCoords(glm::vec4 clipCoords) {
	glm::mat4 inverseProj = glm::inverse(camera->getPerspectiveMatrix());

	/*check this for correctness*/
	glm::vec4 eyeCoords = inverseProj * clipCoords;
	
	return glm::vec4(eyeCoords.x, eyeCoords.y, -1.0f, 0.0f);
}

glm::vec2 MousePicker::getNormalizedDeviceCoords(double mouseX, double mouseY)
{
	int scrWidth, scrHeight;
	glfwGetWindowSize(window, &scrWidth, &scrHeight);
	
	float x = (2 * (float)mouseX) / scrWidth - 1;
	float y = (2 * (float)mouseY) / scrHeight - 1;

	return glm::vec2(x, y);
}
