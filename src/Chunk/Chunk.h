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

#define VERTICES_COUNT 36

typedef struct {
    int xpos, zpos;
} chunkPos;

//render different textures for each side of block
//also add support for other materials later?

enum blockFace {
    BLOCK_FRONT = 1,
    BLOCK_BACK = 2,
    BLOCK_RIGHT = 4,
    BLOCK_LEFT = 8,
    BLOCK_TOP = 16,
    BLOCK_BOTTOM = 32
};

//front is side on the xy-plane
const static float blockFront[] = {
    1.0f, 0.0f, 0.0f,   0.0f, 0.0f,
    0.0f, 0.0f, 0.0f,   1.0f, 0.0f,
    0.0f, 1.0f, 0.0f,   1.0f, 1.0f,

    1.0f, 0.0f, 0.0f,   0.0f, 0.0f,
    1.0f, 1.0f, 0.0f,   0.0f, 1.0f,
    0.0f, 1.0f, 0.0f,   1.0f, 1.0f
};

const static float blockBack[] = {
    0.0f, 0.0f, 1.0f,   0.0f, 0.0f,
    1.0f, 0.0f, 1.0f,   1.0f, 0.0f,
    1.0f, 1.0f, 1.0f,   1.0f, 1.0f,

    0.0f, 0.0f, 1.0f,   0.0f, 0.0f,
    0.0f, 1.0f, 1.0f,   0.0f, 1.0f,
    1.0f, 1.0f, 1.0f,   1.0f, 1.0f
};

const static float blockRight[] = {
    0.0f, 0.0f, 0.0f,   0.0f, 0.0f,
    0.0f, 0.0f, 1.0f,   1.0f, 0.0f,
    0.0f, 1.0f, 1.0f,   1.0f, 1.0f,

    0.0f, 0.0f, 0.0f,   0.0f, 0.0f,
    0.0f, 1.0f, 0.0f,   0.0f, 1.0f,
    0.0f, 1.0f, 1.0f,   1.0f, 1.0f
};

const static float blockLeft[] = {
    1.0f, 0.0f, 1.0f,   0.0f, 0.0f,
    1.0f, 0.0f, 0.0f,   1.0f, 0.0f,
    1.0f, 1.0f, 0.0f,   1.0f, 1.0f,

    1.0f, 0.0f, 1.0f,   0.0f, 0.0f,
    1.0f, 1.0f, 1.0f,   0.0f, 1.0f,
    1.0f, 1.0f, 0.0f,   1.0f, 1.0f
};

const static float blockTop[] = {
    0.0f, 1.0f, 0.0f,   0.0f, 0.0f,
    1.0f, 1.0f, 0.0f,   1.0f, 0.0f,
    1.0f, 1.0f, 1.0f,   1.0f, 1.0f,

    0.0f, 1.0f, 0.0f,   0.0f, 0.0f,
    0.0f, 1.0f, 1.0f,   0.0f, 1.0f,
    1.0f, 1.0f, 1.0f,   1.0f, 1.0f,
};

const static float blockBottom[] = {
    0.0f, 0.0f, 0.0f,   0.0f, 0.0f,   //bottom left
    1.0f, 0.0f, 0.0f,   1.0f, 0.0f,   //bottom right
    1.0f, 0.0f, 1.0f,   1.0f, 1.0f,   //top right

    0.0f, 0.0f, 0.0f,   0.0f, 0.0f,   //bottom left
    0.0f, 0.0f, 1.0f,   0.0f, 1.0f,   //top left
    1.0f, 0.0f, 1.0f,   1.0f, 1.0f,   //top right
};

const static unsigned int CHUNK_SIZE = 16;
const static unsigned int CHUNK_HEIGHT = 64;

class Chunk {
private:
    //OpenGL rendering requirements
    VertexArray va;
    VertexBuffer vb;

    //make layout, shader, texture static to be used by all chunks after an initial load
    static bool staticInit;

    static VertexBufferLayout layout;
    static Shader shader;
    static Texture texture;
    
    //camera ptr
    Camera* camera;

    //holds whether a block is active at a certain chunk position
    bool*** activeBlockList;

    //chunk positions
    chunkPos pos;

    //noise generation
    static Noise noiseGen;

    //height map
    int** heightMap;

    //tracks which faces are to be rendered
    int*** blockFaceList;

    //keeps track of visited blocks in the greedy mesher
    bool*** visited;

public:

    Chunk();
    ~Chunk();

    Chunk(Camera* camera, int xpos, int ypos);
    

    //sets OpenGL data
    void SetData();
    //render chunk based on boolean above
    void Render();
    //updates block at chunk position
    void UpdateBlock(int xpos, int ypos, int zpos, bool isActive);
    //checks whether block at chunk position is actively rendered
    bool isActive(int xpos, int ypos, int zpos);

    //translates the vertices of the faces above by (x, y, z)
    void transFace(const float arr[], float newArr[], int size, int x, int y, int z);
    //generates height map
    int heightMapGenerator(int xpos, int zpos, int chunkX, int chunkZ);

    //generates the blockk face list
    void blockFaceGenerator();

    //generate greedy meshes for all chunk sides
    void GreedyMeshGeneratorBottomToTop(std::vector<float>* coordsList);
    void GreedyMeshGeneratorBackToFront(std::vector<float>* coordsList);
    void GreedyMeshGeneratorLeftToRight(std::vector<float>* coordsList);
};