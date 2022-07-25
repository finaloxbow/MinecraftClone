#pragma once

#include <unordered_map>
#include "Chunk.h"
#include "glm/glm.hpp"
#include <math.h>
#include "glm/gtx/hash.hpp"
#include "MousePicker.h"
#include "GLFW/glfw3.h"


class ChunkManager {
private:
	//list of all chunks
	std::unordered_map<glm::vec2, Chunk*> chunkList;
	//list of loaded chunks
	std::unordered_map<glm::vec2, Chunk*> loadedChunks;


	Camera* camera;
	MousePicker* picker;
	GLFWwindow* window;

	const static int radius = 16 * CHUNK_SIZE;

public:

	ChunkManager(Camera *camera, MousePicker* picker, GLFWwindow* window);

	//render all chunks in visibility list
	void Render_Chunks();

	void Clear_Loaded_Chunks();

	//update loaded chunk list to contain only chunks within some distance
	//of the camera position
	void Update_Loaded_Chunks();

	//----These methods will be used in the glfw mouse callback
	//helper method to delete block at some chunk
	void deleteBlockAtRay();
	//helper method to add block at some chunk
	void addBlockAtRay();

	//test function to print the max number of floats used by a single chunk
	void printMaxFloats();

	void procBlockInput();
};