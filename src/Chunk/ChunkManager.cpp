#include "ChunkManager.h"

/*@TODO:
	- fix chunk rendering around camera position
*/

ChunkManager::ChunkManager(Camera* camera)
	: camera(camera)
{
	//initially add 4 chunks around camera
	chunkList.insert({ glm::vec2(0, 0) , new Chunk(camera, 0, 0)});
	chunkList.insert({ glm::vec2(0, -1), new Chunk(camera, 0, -1)});
	chunkList.insert({ glm::vec2(-1, 0), new Chunk(camera, -1, 0)});
	chunkList.insert({ glm::vec2(-1, -1), new Chunk(camera, -1, -1)});
	
	for (auto& iter : chunkList) {
		iter.second->SetData();
		loadedChunks.insert({ iter.first, iter.second});
	}

}

void ChunkManager::Render_Chunks()
{
	for (auto& iter: loadedChunks) {
		iter.second->Render();
	}
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

	int radius = 4 * CHUNK_SIZE;

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
				else {
					/*loadedChunks.insert({ glm::vec2{i, j}, new Chunk(camera, i, j)});
					chunkList.insert({ glm::vec2{i, j}, loadedChunks.find(glm::vec2(i,j))->second});
					loadedChunks.find(glm::vec2(i, j))->second->SetData();*/

					//inserts new chunk into each chunk list and sets its data
					chunkList.insert({ glm::vec2{i , j}, new Chunk(camera, i, j)});
					loadedChunks.insert({ {glm::vec2{i, j}, chunkList.find({i,j})->second} });
					loadedChunks.find(glm::vec2{ i,j })->second->SetData();
				}
			}
		}
	}





}
