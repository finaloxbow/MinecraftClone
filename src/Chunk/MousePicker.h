#pragma once
#include "glm/glm.hpp"
#include "Camera/Camera.h"


/*use video*/

/*use calculated ray (and its endpoints) to get all blocks along ray from the chunk manager at that point*/
	//if the block exists in bounds
	  //if left click
		//set block to inactive
	  //if right click, set block to active

class MousePicker {
private:
	GLFWwindow *window;
	glm::vec3 currentRay;
	glm::mat4 viewMatrix;
	Camera* camera;

	glm::vec2 getNormalizedDeviceCoords(double mouseX, double mouseY);

public:

	MousePicker(Camera* camera, GLFWwindow *window);

	/*return current ray*/
	glm::vec3 getCurrentRay();

	/*updates view matrix and ray*/
	void update();

	//calculates mouse ray
	glm::vec3 calcMouseRay();
	glm::vec3 toWorldCoords(glm::vec4 eyeCoords);
	glm::vec4 toEyeCoords(glm::vec4 clipCoords);
};