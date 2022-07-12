#pragma once
#include "VertexBufferLayout.h"
#include "VertexArray.h"
#include "Shader.h"
#include "Texture.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "Camera/Camera.h"
#include "IndexBuffer.h"
#include "Noise/Noise.h"

#include <cmath>

enum blockFace {
	BLOCK_FRONT = 1,
	BLOCK_BACK = 2,
	BLOCK_RIGHT = 4,
	BLOCK_LEFT = 8,
	BLOCK_TOP = 16,
	BLOCK_BOTTOM = 32
};

const static unsigned int CHUNK_SIZE = 32;
const static unsigned int CHUNK_HEIGHT = 128;

class Chunk {
private:
	//chunk position
	glm::vec2 chunkPos;
	
	//OpenGL chunk-specific rendering requirements
	VertexArray va;
	VertexBuffer vb;

	//static vars
	static VertexBufferLayout layout;
	static Shader shader;
	static Texture texture;
	static Noise noiseGen;
	static bool staticInit;

	//camera ptr
	Camera* camera;
	
	//---- 3d array that stores information about blocks in chunk
	/*
	* - face data is stored in leftmost 6 bits
	* - next 2 bits are used by greedy mesher as visit lists
	*/
	unsigned short*** blockData;

	//cache to store vertices to avoid reallocating each frame
	std::vector<float> verts;

public:
	
	//constructor & destructor
	Chunk(Camera* camera, int xpos, int ypos);
	~Chunk();

	//functions to generate and render chunk mesh data
	void setData();
	void generateMesh(std::vector<float>* verts);
	void render();

	//individual block methods
	void updateBlock(int xpos, int ypos, int zpos, bool isActive);
	bool isActive(int xpos, int ypos, int zpos);

	//height generation
	int heightMapGenerator(int xpos, int zpos, int chunkX, int chunkZ);
};