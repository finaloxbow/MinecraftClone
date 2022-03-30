#pragma once
#include "VertexBufferLayout.h"
#include "VertexArray.h"
#include "Shader.h"
#include "Texture.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "Camera/Camera.h"
#include "IndexBuffer.h"

#define VERTICES_COUNT 36

typedef struct {
    int xpos, zpos;
} chunkPos;

//render different textures for each side of block
//also add support for other materials later?

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
    0.0f, 0.0f, 0.0f,   0.0f, 0.0f,
    1.0f, 0.0f, 0.0f,   1.0f, 0.0f,
    1.0f, 0.0f, 1.0f,   1.0f, 1.0f,

    0.0f, 0.0f, 0.0f,   0.0f, 0.0f,
    0.0f, 0.0f, 1.0f,   0.0f, 1.0f,
    1.0f, 0.0f, 1.0f,   1.0f, 1.0f,
};

const static unsigned int CHUNK_SIZE = 16;

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
    
    Camera* camera;

    //holds whether a block is active at a certain chunk position
    bool activeBlockList[CHUNK_SIZE][CHUNK_SIZE][CHUNK_SIZE];

    //chunk positions
    chunkPos pos;

public:

    Chunk();

    Chunk(Camera* camera, int xpos, int ypos);
    

    //sets OpenGL data
    void SetData();
    //render chunk based on boolean above
    void Render();
    //updates block at chunk position
    void UpdateBlock(int xpos, int ypos, int zpos, bool isActive);
    //checks whether block at chunk position is actively rendered
    bool isActive(int xpos, int ypos, int zpos);

    void transFace(const float arr[], float newArr[], int size, int x, int y, int z);

};