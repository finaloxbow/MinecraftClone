#include "ChunkManager.h"

/*@TODO:
	- fix chunk rendering around camera position
*/

ChunkManager::ChunkManager(Camera* camera, MousePicker* picker)
	: camera(camera), picker(picker)
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

	//TEST CODE: prints position for now
	deleteBlockAtRay();


}

void ChunkManager::deleteBlockAtRay()
{
	//find which chunk to use as context
	//find which block first intersects with ray
	//set block to inactive
	glm::vec3 camPos = camera->getPosition();
	glm::vec3 mouseRay = picker->getCurrentRay();

	glm::vec3 chunkContextRay = camPos + mouseRay;


	//if >=0 , round down to integer with smaller modulo => static_cast<int>(float)
	// if < 0, round down to integer with larger modulo => static_cast<int>(float - 1)
	glm::vec2 chunkKey = glm::vec2(
		static_cast<int>(chunkContextRay.x >= 0 ? chunkContextRay.x : chunkContextRay.x - 1.0f),
		static_cast<int>(chunkContextRay.y >= 0 ? chunkContextRay.y : chunkContextRay.y - 1.0f)
	);

	//finds closest multiple of CHUNK_SIZE below coord
	if (chunkKey.x < 0) {
		while (static_cast<int>(chunkKey.x) % CHUNK_SIZE != 0)
			chunkKey.x -= 1;
	}
	else
		chunkKey.x = chunkKey.x - (static_cast<int>(chunkKey.x) % CHUNK_SIZE);

	if (chunkKey.y < 0) {
		while (static_cast<int>(chunkKey.y) % CHUNK_SIZE != 0)
			chunkKey.y -= 1;
	}
	else
		chunkKey.y = chunkKey.y - (static_cast<int>(chunkKey.y) % CHUNK_SIZE);


	if (chunkList.find(
		{
			static_cast<int>(chunkKey.x),
			static_cast<int>(chunkKey.y)
		}) != chunkList.end()) {

		//chunk context to check ray for
		Chunk* contextChunk = chunkList.find(
			{
				static_cast<int>(chunkKey.x),
				static_cast<int>(chunkKey.y)
			})->second;




	}

	printf("{ %d, %d }\n", static_cast<int>(chunkKey.x), static_cast<int>(chunkKey.y));
}

void ChunkManager::addBlockAtRay()
{
}

