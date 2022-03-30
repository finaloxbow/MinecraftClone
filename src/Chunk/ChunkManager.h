#pragma once

#include <unordered_map>
#include "Chunk.h"
#include "glm/glm.hpp"
#include <math.h>
#include "glm/gtx/hash.hpp"

class ChunkManager {
private:
	//list of all chunks
	std::unordered_map<glm::vec2, Chunk*> chunkList;
	//list of loaded chunks
	std::unordered_map<glm::vec2, Chunk*> loadedChunks;
	Camera* camera;

public:

	ChunkManager(Camera *camera);

	//render all chunks in visibility list
	void Render_Chunks();

	//update loaded chunk list to contain only chunks within some distance
	//of the camera position
	void Update_Loaded_Chunks();

};