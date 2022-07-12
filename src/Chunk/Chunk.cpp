#include "Chunk.h"

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
	//for (int x = 0; x < CHUNK_SIZE; x++) {
	//	for (int z = 0; z < CHUNK_SIZE; z++) {
	//		//chunk coords
	//		int xpos = chunkPos.x;
	//		int zpos = chunkPos.y;
	//		int height = heightMapGenerator(x, z, xpos, zpos);

	//		//heights of surrounding 4 columns around block
	//		int heightRight = heightMapGenerator(x + 1, z, xpos, zpos);
	//		int heightLeft = heightMapGenerator(x - 1, z, xpos, zpos);
	//		int heightFront = heightMapGenerator(x, z + 1, xpos, zpos);
	//		int heightBack = heightMapGenerator(x, z - 1, xpos, zpos);

	//		//TODO: set block faces
	//		for(int y = 0; y < height; y++){
	//			if (x == 0 || y <= heightLeft)
	//				blockData[x][y][z] |= BLOCK_LEFT;
	//			if (x < CHUNK_SIZE || y <= heightRight)
	//				blockData[x][y][z] |= BLOCK_RIGHT;
	//			if (z == 0 || z <= heightBack)
	//				blockData[x][y][z] |= BLOCK_BACK;
	//			if (z < CHUNK_SIZE || z <= heightFront)
	//				blockData[x][y][z] |= BLOCK_FRONT;
	//			if (y > 0 && blockData[x][y - 1][z] == 0)
	//				blockData[x][y][z] |= BLOCK_BOTTOM;
	//			if (y < CHUNK_HEIGHT - 1 && blockData[x][y + 1][z] == 0)
	//				blockData[x][y][z] |= BLOCK_TOP;
	//		}
	//	}
	//}
    
    //init activeBlockList bit based on height map
    for (int x = 0; x < CHUNK_SIZE; x++) {
        for (int z = 0; z < CHUNK_SIZE; z++) {
            int xpos = chunkPos.x;
            int zpos = chunkPos.y;
            int height = heightMapGenerator(x, z, xpos, zpos);

            for (int y = 0; y < height; y++) {
                //sets blocks up to height as active
                blockData[x][y][z] |= IS_ACTIVE;
            }
        }
    }

    //generate face data
    for (int x = 0; x < CHUNK_SIZE; x++) {
        for (int z = 0; z < CHUNK_SIZE; z++) {
            int xpos = chunkPos.x;
            int zpos = chunkPos.y;
            int height = heightMapGenerator(x, z, xpos, zpos);

            for (int y = 0; y < height; y++) {
                //blockData[x][y][z] & IS_ACTIVE != 0
                
                //bottom face
                if (y == 0 || (blockData[x][y - 1][z] & IS_ACTIVE) == 0)
                    blockData[x][y][z] |= BLOCK_BOTTOM;
                //top face
                if (y < CHUNK_HEIGHT - 1 && (blockData[x][y + 1][z] & IS_ACTIVE) == 0)
                    blockData[x][y][z] |= BLOCK_TOP;
                //left face (-x)
                if (x == 0 || (blockData[x - 1][y][z] & IS_ACTIVE) == 0)
                    blockData[x][y][z] |= BLOCK_LEFT;
                //right face (+x)
                if (x < CHUNK_SIZE - 1 && (blockData[x + 1][y][z] & IS_ACTIVE) == 0)
                    blockData[x][y][z] |= BLOCK_RIGHT;
                //back face (-z)
                if (z == 0 || (blockData[x][y][z - 1] & IS_ACTIVE) == 0)
                    blockData[x][y][z] |= BLOCK_BACK;
                //front face (+z)
                if (z < CHUNK_SIZE - 1 && (blockData[x][y][z + 1] & IS_ACTIVE) == 0)
                    blockData[x][y][z] |= BLOCK_FRONT;
            }
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

void Chunk::generateMesh(std::vector<float>* coordsList) {
	
    //meshing helper functions
    greedyMesherBottomToTop(coordsList);
    greedyMesherBackToFront(coordsList);
    greedyMesherLeftToRight(coordsList);
}

void Chunk::greedyMesherBottomToTop(std::vector<float>* coordsList){
    int chunkX = chunkPos.x;
    int chunkZ = chunkPos.y;

    //fills entire array with false entries
    for (int i = 0; i < CHUNK_SIZE; i++) {
        for (int j = 0; j < CHUNK_HEIGHT; j++) {
            for (int h = 0; h < CHUNK_SIZE; h++) {
                //sets visited bits to 0
                blockData[i][j][h] &= ~(FIRST_VISITED + 0xff00);
                blockData[i][j][h] &= ~(SECOND_VISITED + 0xff00);
            }
        }
    }

    for (int layer = 0; layer < CHUNK_HEIGHT; layer++) {
        for (int row = 0; row < CHUNK_SIZE; row++) {
            for (int col = 0; col < CHUNK_SIZE; col++) {

                // activeblocklist => (blockData & BLOCK_BOTTOM) != 0
                // !firstVisited => (blockData & FIRST_VISITED) == 0
                // 
                //BOTTOM LAYER
                if ((blockData[row][layer][col] & BLOCK_BOTTOM) != 0 && (blockData[row][layer][col] & FIRST_VISITED) == 0) {
                    int row_temp = row;
                    int col_temp = col;
                    bool endFlag = false;

                    //iterates right until end of chunk or visited block or inactive block reached
                    while (row_temp < CHUNK_SIZE && (blockData[row_temp][layer][col] & FIRST_VISITED) == 0
                        && (blockData[row_temp][layer][col] & BLOCK_BOTTOM) != 0)
                        row_temp++;


                    //iterate upwards until incomplete row or end of chunk reached
                    while (col_temp < CHUNK_SIZE) {
                        for (int i = row; i < row_temp; i++) {
                            //makes sure every element in rectangle is visited

                            //end if already visited or inactive block
                            if ((blockData[i][layer][col_temp] & FIRST_VISITED) != 0 || (blockData[i][layer][col_temp] & BLOCK_BOTTOM) == 0) {
                                endFlag = true;
                            }


                            if (endFlag)
                                break;
                        }


                        if (endFlag)
                            break;
                        else {
                            for (int i = row; i < row_temp; i++) {
                                blockData[i][layer][col_temp] |= FIRST_VISITED;
                            }
                        }

                        col_temp++;
                    }



                    //vertices and texture mappings
                    float vertices[] = {
                        row, 0, col,                        0, 0,                                          //bottom left
                        row_temp, 0, col,                   row_temp - row, 0,                        //bottom right
                        row_temp, 0, col_temp,              row_temp - row, col_temp - col,      //top right
                        row, 0, col,                        0, 0,                                          //bottom left
                        row, 0, col_temp,                   0, col_temp - col,                        //top left
                        row_temp, 0, col_temp,              row_temp - row, col_temp - col       //top right
                    };

                    float newArr[30] = { 0 };

                    //translates block faces to their correct positions
                    for (int i = 0; i < 30; i += 5) {

                        //block positions translated
                        newArr[i] = vertices[i] + chunkX;
                        newArr[i + 1] = vertices[i + 1] + layer;
                        newArr[i + 2] = vertices[i + 2] + chunkZ;

                        //texture coords unchanged (can probably add texture coords to separate array for efficiency)
                        newArr[i + 3] = vertices[i + 3];
                        newArr[i + 4] = vertices[i + 4];
                    }

                    for (int i = 0; i < 30; i++)
                        coordsList->push_back(newArr[i]);

                }

                // activeblocklist => (blockData & BLOCK_BOTTOM) != 0
                // !firstVisited => (blockData & FIRST_VISITED) == 0

                //TOP LAYER
                if ((blockData[row][layer][col] & BLOCK_TOP) != 0 && (blockData[row][layer][col] & SECOND_VISITED) == 0) {
                    int row_temp = row;
                    int col_temp = col;
                    bool endFlag = false;

                    //iterates right until end of chunk or visited block or inactive block reached
                    while (row_temp < CHUNK_SIZE && (blockData[row_temp][layer][col] & SECOND_VISITED) == 0 && (blockData[row_temp][layer][col] & BLOCK_TOP) != 0)
                        row_temp++;


                    //iterate upwards until incomplete row or end of chunk reached
                    while (col_temp < CHUNK_SIZE) {
                        for (int i = row; i < row_temp; i++) {
                            //makes sure every element in rectangle is visited

                            //end if already visited or inactive block
                            if ((blockData[i][layer][col_temp] & SECOND_VISITED) != 0 || (blockData[i][layer][col_temp] & BLOCK_TOP) == 0) {
                                endFlag = true;
                            }


                            if (endFlag)
                                break;
                        }


                        if (endFlag)
                            break;
                        else {
                            for (int i = row; i < row_temp; i++) {
                                blockData[i][layer][col_temp] |= SECOND_VISITED;
                            }
                        }

                        col_temp++;
                    }



                    //vertices and texture mappings
                    float vertices[] = {
                        row, 0, col,                        0, 0,                                          //bottom left
                        row_temp, 0, col,                   row_temp - row, 0,                        //bottom right
                        row_temp, 0, col_temp,              row_temp - row, col_temp - col,      //top right
                        row, 0, col,                        0, 0,                                          //bottom left
                        row, 0, col_temp,                   0, col_temp - col,                        //top left
                        row_temp, 0, col_temp,              row_temp - row, col_temp - col       //top right
                    };

                    float newArr[30] = { 0 };

                    //translates block faces to their correct positions
                    for (int i = 0; i < 30; i += 5) {

                        //block positions translated
                        newArr[i] = vertices[i] + chunkX;
                        newArr[i + 1] = vertices[i + 1] + layer + 1;
                        newArr[i + 2] = vertices[i + 2] + chunkZ;

                        //texture coords unchanged (can probably add texture coords to separate array for efficiency)
                        newArr[i + 3] = vertices[i + 3];
                        newArr[i + 4] = vertices[i + 4];
                    }

                    for (int i = 0; i < 30; i++)
                        coordsList->push_back(newArr[i]);

                }
            }
        }
    }
}

void Chunk::greedyMesherLeftToRight(std::vector<float>* coordsList) {

    int chunkX = chunkPos.x;
    int chunkZ = chunkPos.y;


}
void Chunk::greedyMesherBackToFront(std::vector<float>* coordsList) {

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

void Chunk::updateBlock(int xpos, int ypos, int zpos, bool isActive) {
	//update blockData and faces around it
}

bool Chunk::isActive(int xpos, int ypos, int zpos)
{
	return false;
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