#pragma once

#define TRIANGLE_COUNT 36

#include "VertexBufferLayout.h"
#include "VertexArray.h"
#include "Shader.h"
#include "Texture.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "Camera.h"


class BlockRenderer {
private:
    //required rendering utils
    VertexArray va;
    VertexBuffer vb;
    VertexBufferLayout layout;
    Shader shader;
    Texture texture;
    Camera* camera;

    //block world coords
    float xpos, ypos, zpos;
public:
    //block renderer obj holds all initial data (vertex array, attrib, etc.)
    BlockRenderer(Camera* camera, float xposIn, float yposIn, float zposIn);

	//draws a block at these world coordinates
	void drawBlock();
};