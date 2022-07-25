#include "ChunkManager.h"

/*@TODO:
	- fix chunk rendering around camera position
*/

ChunkManager::ChunkManager(Camera* camera, MousePicker* picker, GLFWwindow* window)
	: camera(camera), picker(picker), window(window)
{
	
	for (auto& iter : chunkList) {
		iter.second->setData();
		loadedChunks.insert({ iter.first, iter.second});
	}

}

static long maxFloats = 0;
static long maxRendered = 0;

void ChunkManager::Render_Chunks()
{
	int tempRendered = 0;

	

	for (auto& iter: loadedChunks) {
		iter.second->render();

		//test code to find the max number of floats used in mesh generation
		if (iter.second->numFloats() > maxFloats)
			maxFloats = iter.second->numFloats();
		tempRendered++;
	}

	if (tempRendered > maxRendered)
		maxRendered = tempRendered;
}

void ChunkManager::printMaxFloats() {
	printf("Max floats: %ld \n", maxFloats);
	printf("Max chunks rendered: %d", maxRendered);
}

//clear
void ChunkManager::Clear_Loaded_Chunks() {
	loadedChunks.clear();
}

/*
	- update load list
	- update unload list
	- add extra chunks if necessary?
*/
void ChunkManager::Update_Loaded_Chunks()
{
	//clear loaded chunks before computation
	Clear_Loaded_Chunks();

	//need to create new chunks around player and use ones already in chunkList
	glm::vec3 camPos = camera->getPosition();

	//loop through square centered at player position and only render chunks in circle around camera
	//@TODO: 
	//	- only update the loaded chunks list when a chunk border is crossed
	//	- change rendering to center at cameraPos
	for (int i = static_cast<int>(camPos.x) - radius; i < static_cast<int>(camPos.x) + radius; i++) {
		for (int j = static_cast<int>(camPos.z) - radius; j < static_cast<int>(camPos.z) + radius; j++) {
			//chunks only generate at integer multiples of the chunk size
			if (i % CHUNK_SIZE == 0 && j % CHUNK_SIZE == 0) {

				if (chunkList.find({ i, j }) != chunkList.end()) {
					loadedChunks.insert({ glm::vec2(i, j) , chunkList.find({i, j})->second });
				}
				else 
				{
					//inserts new chunk into each chunk list and sets its data
					chunkList.insert({ glm::vec2{i , j}, new Chunk(camera, i, j)});
					loadedChunks.insert({ {glm::vec2{i, j}, chunkList.find({i,j})->second} });
					loadedChunks.find(glm::vec2{ i,j })->second->setData();
				}
			}
		}
	}
}

void ChunkManager::deleteBlockAtRay()
{
	//get blocks from rayStart to rayEnd
	//check whether block can be activated or deactivated or is outofbounds
	
	//given 3d coords for ray:
		//1. find which block it refers to
		//2. find chunk for corresponding block
		//3. find block or declare out of bounds
		//4. deactive block
		//5. if block already deactivated, continue along ray until end of ray
		// or block deleted

	glm::vec3 currentRay = glm::vec3(0.0f, 0.0f, 0.0f);
	//iterates from camera to reach distance
	for (float i = 0.0f; i <= 4.0f; i++) {
		//get block coords
		//get chunk for block
		//test whether block active
		
		currentRay = picker->calcMouseRay(i);
		
		//find block coords
		glm::vec3 blockCoords = glm::vec3();
		glm::vec2 chunkCoords = glm::vec2();

		//x coord
		if (currentRay.x >= 0) {
			blockCoords.x = static_cast<int>(currentRay.x);
			chunkCoords.x = blockCoords.x - 
				(static_cast<int>(blockCoords.x) % CHUNK_SIZE);
		}
		else {
			blockCoords.x = static_cast<int>(currentRay.x - 1);
			chunkCoords.x = static_cast<int>(blockCoords.x);

			//finds lower multiple of CHUNK_SIZE
			while (static_cast<int>(chunkCoords.x) % CHUNK_SIZE != 0)
				chunkCoords.x -= 1;
		}
		//y coord
		if (currentRay.y >= 0)
			blockCoords.y = static_cast<int>(currentRay.y);
		else
			blockCoords.y = static_cast<int>(currentRay.y - 1);
		//z coord
		if (currentRay.z >= 0) {
			blockCoords.z = static_cast<int>(currentRay.z);
			chunkCoords.y = blockCoords.z -
				(static_cast<int>(blockCoords.z) % CHUNK_SIZE);
		}
		else {
			blockCoords.z = static_cast<int>(currentRay.z - 1);
			chunkCoords.y = static_cast<int>(blockCoords.z);

			//finds lower multiple of CHUNK_SIZE
			while (static_cast<int>(chunkCoords.y) % CHUNK_SIZE != 0)
				chunkCoords.y -= 1;
		}

		//finds the chunk for the block
		Chunk* contextChunk = chunkList.find({ static_cast<int>(chunkCoords.x),
			static_cast<int>(chunkCoords.y) }) != chunkList.end() 
			? chunkList.find({ static_cast<int>(chunkCoords.x),static_cast<int>(chunkCoords.y) })->second 
			: nullptr;

		//find coords of block inside the chunk
		//if blockCoord +, just mod with CHUNK_SIZE
		//if blockCoord -, do CHUNK_SIZE - (-blockCoord.y) % CHUNK_SIZE
		glm::vec3 blockCoordsInsideChunk = glm::vec3();

		blockCoordsInsideChunk.x = blockCoords.x >= 0
			? static_cast<int>(blockCoords.x) % CHUNK_SIZE
			: CHUNK_SIZE - ((-static_cast<int>(blockCoords.x)) % CHUNK_SIZE);

		blockCoordsInsideChunk.y = static_cast<int>(blockCoords.y);

		blockCoordsInsideChunk.z = blockCoords.z >= 0
			? static_cast<int>(blockCoords.z) % CHUNK_SIZE
			: CHUNK_SIZE - ((-static_cast<int>(blockCoords.z)) % CHUNK_SIZE);

		//TEST----------------------
		printf("Block Coords Inside Chunk: %f %f %f\n", blockCoordsInsideChunk.x, blockCoordsInsideChunk.y, blockCoordsInsideChunk.z);
		printf("Chunk Coords: %f %f\n", chunkCoords.x, chunkCoords.y);

		//x and z guaranteed to be within chunk boundaries
		//just need to check that y-value is within bounds of 0 to CHUNK_HEIGHT, inclusive
		if (blockCoordsInsideChunk.y >= 0
			&& blockCoordsInsideChunk.y < CHUNK_HEIGHT
			&& contextChunk != nullptr
			&& contextChunk->isActive(blockCoordsInsideChunk.x, blockCoordsInsideChunk.y, blockCoordsInsideChunk.z)) {
			//if the block is active, set to inactive and end loop
			contextChunk->updateBlock(blockCoordsInsideChunk.x, blockCoordsInsideChunk.y, blockCoordsInsideChunk.z, false);
			break;
		}

	}
}

void ChunkManager::addBlockAtRay()
{

}

void ChunkManager::procBlockInput()
{
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
		deleteBlockAtRay();
		printf("LEFT CLICK\n");
	}
}
