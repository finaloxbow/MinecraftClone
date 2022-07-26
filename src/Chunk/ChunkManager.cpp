#include "ChunkManager.h"

/*@TODO:
	- fix chunk rendering around camera position
*/

ChunkManager::ChunkManager(Camera* camera, MousePicker* picker, GLFWwindow* window)
	: camera(camera), picker(picker), window(window)
{
}

static long maxFloats = 0;
static long maxRendered = 0;
static double lastLeftClickTime = 0;
static double lastRightClickTime = 0;

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
	bool endLoop = false;

	for (float i = 0.0f; i <= 8.0f; i += 1) {
		glm::vec3 mouseRay = picker->calcMouseRay(i);

		//find chunk coords (ez)
		glm::vec2 chunkCoords(0.0f, 0.0f);
		
		//x coord
		if (mouseRay.x >= 0)
			chunkCoords.x = static_cast<int>(mouseRay.x)
			- (static_cast<int>(mouseRay.x) % CHUNK_SIZE);
		else {
			chunkCoords.x = static_cast<int>(mouseRay.x - 1);
			while (static_cast<int>(chunkCoords.x) % CHUNK_SIZE != 0)
				chunkCoords.x -= 1;
		}

		//z coord
		if (mouseRay.z >= 0)
			chunkCoords.y = static_cast<int>(mouseRay.z)
			- (static_cast<int>(mouseRay.z) % CHUNK_SIZE);
		else {
			chunkCoords.y = static_cast<int>(mouseRay.z - 1);
			while (static_cast<int>(chunkCoords.y) % CHUNK_SIZE != 0)
				chunkCoords.y -= 1;
		}

		Chunk* contextChunk = chunkList.find({ static_cast<int>(chunkCoords.x),
			static_cast<int>(chunkCoords.y) }) != chunkList.end()
			? chunkList.find({ static_cast<int>(chunkCoords.x),static_cast<int>(chunkCoords.y) })->second
			: nullptr;

		if (contextChunk != nullptr) {
			//iterate over entire grid and do AABB collision testing
			for (int x = 0; x < CHUNK_SIZE; x++) {
				for (int y = 0; y < CHUNK_HEIGHT; y++) {
					for (int z = 0; z < CHUNK_SIZE; z++) {
						//m.x inbetween x + chunk.x and x + 1 + chunk.x
						//m.y inbetween y + chunk.y and y + 1
						//m.z inbetween z and z + 1
						if (mouseRay.x >= x + chunkCoords.x
							&& mouseRay.x <= x + 1 + chunkCoords.x
							&& mouseRay.y >= y
							&& mouseRay.y <= y + 1
							&& mouseRay.z >= z + chunkCoords.y
							&& mouseRay.z <= z + 1 + chunkCoords.y
							&& contextChunk->isActive(x, y, z)) {
							//delete block and end loop
							std::cout << "Block Coords: " << x << "," << y << ", " << z << "\n";
							contextChunk->updateBlock(x, y, z, false);
							contextChunk->setData();
							endLoop = true;
							break;
						}
					}
					if (endLoop)
						break;
				}
				if (endLoop)
					break;
			}
		}
		if (endLoop)
			break;
	}
}

void ChunkManager::addBlockAtRay()
{
	bool endLoop = false;
	bool activeBlockReached = false;
	glm::vec3 previousEmptyBlock = glm::vec3(0.0f, 0.0f, 0.0f);
	Chunk* previousBlockChunk = nullptr;

	for (float i = 0.0f; i <= 8.0f; i++) {
		//find first active block
		//if active block and i >= 1.0f
		//place block at previous empty position
		glm::vec3 mouseRay = picker->calcMouseRay(i);

		//find chunk coords (ez)
		glm::vec2 chunkCoords(0.0f, 0.0f);

		//x coord
		if (mouseRay.x >= 0)
			chunkCoords.x = static_cast<int>(mouseRay.x)
			- (static_cast<int>(mouseRay.x) % CHUNK_SIZE);
		else {
			chunkCoords.x = static_cast<int>(mouseRay.x - 1);
			while (static_cast<int>(chunkCoords.x) % CHUNK_SIZE != 0)
				chunkCoords.x -= 1;
		}

		//z coord
		if (mouseRay.z >= 0)
			chunkCoords.y = static_cast<int>(mouseRay.z)
			- (static_cast<int>(mouseRay.z) % CHUNK_SIZE);
		else {
			chunkCoords.y = static_cast<int>(mouseRay.z - 1);
			while (static_cast<int>(chunkCoords.y) % CHUNK_SIZE != 0)
				chunkCoords.y -= 1;
		}

		Chunk* contextChunk = chunkList.find({ static_cast<int>(chunkCoords.x),
			static_cast<int>(chunkCoords.y) }) != chunkList.end()
			? chunkList.find({ static_cast<int>(chunkCoords.x),static_cast<int>(chunkCoords.y) })->second
			: nullptr;

		if (contextChunk != nullptr) {
			//iterate over entire grid and do AABB collision testing
			for (int x = 0; x < CHUNK_SIZE; x++) {
				for (int y = 0; y < CHUNK_HEIGHT; y++) {
					for (int z = 0; z < CHUNK_SIZE; z++) {
						//COLLISION TESTING: 
						//m.x inbetween x + chunk.x and x + 1 + chunk.x
						//m.y inbetween y + chunk.y and y + 1
						//m.z inbetween z and z + 1
						if (mouseRay.x >= x + chunkCoords.x
							&& mouseRay.x <= x + 1 + chunkCoords.x
							&& mouseRay.y >= y
							&& mouseRay.y <= y + 1
							&& mouseRay.z >= z + chunkCoords.y
							&& mouseRay.z <= z + 1 + chunkCoords.y) {
							if (contextChunk->isActive(x, y, z) && i >= 1.0f) {
								//place block at last empty position
								previousBlockChunk->updateBlock(previousEmptyBlock.x, previousEmptyBlock.y, previousEmptyBlock.z, true);
								previousBlockChunk->setData();
								endLoop = true;
								break;
							}
							else if (contextChunk->isActive(x, y, z)) {
								//break out of loop
								break;
							}
							else {
								//update lastEmptyBlockVector and chunk ptr
								previousEmptyBlock.x = x;
								previousEmptyBlock.y = y;
								previousEmptyBlock.z = z;
								previousBlockChunk = contextChunk;
							}

						}
					}
					if (endLoop)
						break;
				}
				if (endLoop)
					break;
			}
		}
		if (endLoop)
			break;
	}
}

void ChunkManager::procBlockInput()
{
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS && glm::abs(lastLeftClickTime - glfwGetTime()) >= 0.25) {
		lastLeftClickTime = glfwGetTime();
		deleteBlockAtRay();
		printf("LEFT CLICK\n");
	}
	else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS && glm::abs(lastRightClickTime - glfwGetTime()) >= 0.25) {
		lastRightClickTime = glfwGetTime();
		addBlockAtRay();
		printf("RIGHT CLICK\n");
	}
}
