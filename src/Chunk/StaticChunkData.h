#pragma once

#include "VertexBufferLayout.h"
#include "Shader.h"
#include "Texture.h"
#include "Noise/Noise.h"

struct StaticChunkData {
	VertexBufferLayout layout;
	Shader shader;
	Texture texture;
	Noise noiseGen;

	StaticChunkData() {
		layout.Push<float>(3);
		layout.Push<float>(2);

		printf("Finished layout...\n");
		shader.Set_Data("res/shaders/Basic.shader");
		texture.Set_Data("res/textures/concreteTexture.png");
	}
};