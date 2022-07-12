#include "ChunkRewrite.h"

//static vars init
VertexBufferLayout Chunk::layout;
Shader Chunk::shader;
Texture Chunk::texture;
Noise Chunk::noiseGen;
bool Chunk::staticInit = false;

Chunk::Chunk(Camera* cameraIn, int xpos, int ypos) {
	
	camera = cameraIn;
	chunkPos = { xpos, ypos};

	//initializes shader, texture, and layout for all chunks
	if (!staticInit) {
		layout.Push<float>(3);
		layout.Push<float>(2);

		shader.Set_Data("res/shaders/Basic.shader");
		texture.Set_Data("res/textures/concreteTexture.png");

		staticInit = true;
	}

	//init blockData 3d array
	blockData = new unsigned short** [CHUNK_SIZE];
	for (int i = 0; i < CHUNK_SIZE; i++) {
		blockData[i] = new unsigned short* [CHUNK_HEIGHT];
		for (int j = 0; j < CHUNK_HEIGHT; j++) {
			blockData[i][j] = new unsigned short[CHUNK_SIZE];
			for (int k = 0; k < CHUNK_SIZE; k++) {
				blockData[i][j][k] = 0;
			}
		}
	}

	//initialize blockData
	for (int x = 0; x < CHUNK_SIZE; x++) {
		for (int z = 0; z < CHUNK_SIZE; z++) {
			//chunk coords
			int xpos = chunkPos.x;
			int zpos = chunkPos.y;

			//heights of surrounding 4 columns around block
			int heightRight = heightMapGenerator(x + 1, z, xpos, zpos);
			int heightLeft = heightMapGenerator(x - 1, z, xpos, zpos);
			int heightFront = heightMapGenerator(x, z + 1, xpos, zpos);
			int heightBack = heightMapGenerator(x, z - 1, xpos, zpos);

			//TODO: set block faces
		}
	}

}

Chunk::~Chunk() {
	//free blockData
}

void Chunk::setData() {
	verts.clear();

	generateMesh(&verts);

	//sets the buffer data
	vb.Set_Data(&verts[0], (unsigned int)verts.size() * sizeof(verts[0]));

	//sets buffer layout with buffer
	va.AddBuffer(vb, layout);

	//creates and starts shader program
	shader.Bind();

	//loads texture and binds to slot 0
	texture.Bind();
	shader.SetUniform1i("u_Texture", 0);
	shader.SetUniform1i("u_Texture", 0);
}

void Chunk::generateMesh(std::vector<float>* verts) {

}

void Chunk::render() {
	//sets model-view-projection matrix and sends to shader
	//MVP = proj * view * model
	shader.SetUniformMat4f("u_MVP",
		camera->getPerspectiveMatrix()
		* camera->getViewMatrix()
		* glm::mat4(1.0f));

	//binds VAO
	va.Bind();

	//draws vertices to screen
	//num_vertices = verts.size / 5
	glDrawArrays(GL_TRIANGLES, 0, verts.size() / 5);
}

int Chunk::heightMapGenerator(int xpos, int zpos, int chunkX, int chunkZ) {
	
	//out of bounds noise check
	if (xpos < 0 || xpos >= CHUNK_SIZE || zpos < 0 || zpos >= CHUNK_SIZE)
		return 0;

	//-1.5 to 1.5
	float elev = (noiseGen.getNoise(xpos + chunkX, zpos + chunkZ)
		+ 0.5 * noiseGen.getNoise(2 * (xpos + chunkX), 2 * (zpos + chunkZ))
		+ 0.25 * noiseGen.getNoise(4 * (xpos + chunkX), 4 * (zpos + chunkZ)));
	//-1 to 1
	elev /= 1.75;
	//0 to 2
	elev += 1;

	//0 to 1
	elev /= 2;

	//0 to 1
	elev = elev * elev * elev;

	return (int)(((CHUNK_HEIGHT - 2)) * elev) + 1;
}