#pragma once

#define VERTICES_COUNT 36

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