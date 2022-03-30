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


void ChunkManager::Update_Loaded_Chunks()
{
	//need to create new chunks around player and use ones already in chunkList
	glm::vec3 camPos = camera->getPosition();

	//loop through square centered at player position and only render chunks in circle around camera
	for (float i = camPos.x - 16 * 2; i < camPos.x + 16 * 2; i += 16.0) {
		for (float j = camPos.z - 16 * 2; j < camPos.z + 16 * 2; j += 16.0) {
			if (pow(camPos.x - i, 2) + pow(camPos.z - j, 2) < 4 * 4) {
				if (chunkList.find({ i, j }) != chunkList.end()) {
					loadedChunks.insert({ glm::vec2(i, j) , chunkList.find({i, j})->second });
				}
				else {
					loadedChunks.insert({ glm::vec2{i, j}, new Chunk(camera, i, j)});
					chunkList.insert({ glm::vec2{i, j}, loadedChunks.find(glm::vec2(i,j))->second});
					loadedChunks.find(glm::vec2(i, j))->second->SetData();
				}
			}
		}
	}




}
