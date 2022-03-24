#pragma once
#include "VertexBufferLayout.h"
#include "VertexArray.h"
#include "Shader.h"
#include "Texture.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "Camera.h"
#include "IndexBuffer.h"

#define VERTICES_COUNT 36


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
    VertexBufferLayout layout;
    Shader shader;
    Texture texture;
    Camera* camera;

    //holds whether a block is active at a certain chunk position
    bool activeBlockList[CHUNK_SIZE][CHUNK_SIZE][CHUNK_SIZE];

public:

    Chunk(Camera* camera);
    ~Chunk();

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