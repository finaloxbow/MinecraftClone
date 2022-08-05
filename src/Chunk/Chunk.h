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

//stores which block faces to render
enum blockFace {
	BLOCK_FRONT = 1 << 0,
	BLOCK_BACK = 1 << 1,
	BLOCK_RIGHT = 1 << 2,
	BLOCK_LEFT = 1 << 3,
	BLOCK_TOP = 1 << 4,
	BLOCK_BOTTOM = 1 << 5
};

//chunk size data
const static unsigned int CHUNK_SIZE = 16;
const static unsigned int CHUNK_HEIGHT = 512;

//visited bit positions
const static unsigned int FIRST_VISITED = 1 << 6;
const static unsigned int SECOND_VISITED = 1 << 7;
//active block bit
const static unsigned int IS_ACTIVE = 1 << 8;

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
	void generateFaceData();

	//individual block methods
	void updateBlock(int xpos, int ypos, int zpos, bool isActive);
	bool isActive(int xpos, int ypos, int zpos);

	//helper methods for greedy mesher
	bool firstVisited(int xpos, int ypos, int zpos);
	bool secondVisited(int xpos, int ypos, int zpos);
	bool blockHasFace(int xpos, int ypos, int zpos, blockFace face);

	//height generation
	int heightMapGenerator(int xpos, int zpos, int chunkX, int chunkZ);

	//greedy meshing helper functions
	void greedyMesherBottomToTop(std::vector<float>* coordsList);
	void greedyMesherLeftToRight(std::vector<float>* coordsList);
	void greedyMesherBackToFront(std::vector<float>* coordsList);

	//test functions
	long numFloats();
};