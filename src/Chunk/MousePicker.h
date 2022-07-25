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
	Camera* camera;


public:

	MousePicker(Camera* camera, GLFWwindow *window);

	/*return current ray*/
	glm::vec3 getCurrentRay();

	/*updates view matrix and ray*/
	void update();

	//calculates mouse ray
	glm::vec3 calcMouseRay(float scalingCoeff);
};