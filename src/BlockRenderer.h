#pragma once

#define VERTICES_COUNT 36

#include "VertexBufferLayout.h"
#include "VertexArray.h"
#include "Shader.h"
#include "Texture.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "Camera.h"
#include "IndexBuffer.h"


class BlockRenderer {
private:
    //required rendering utils
    VertexArray va;
    VertexBuffer vb;
    VertexBufferLayout layout;
    Shader shader;
    Texture texture;
    Camera* camera;

public:
    //block renderer obj holds all initial data (vertex array, attrib, etc.)
    BlockRenderer(Camera* camera);

	//draws a block at these world coordinates
	void drawBlock(float xpos, float ypos, float zpos);

};