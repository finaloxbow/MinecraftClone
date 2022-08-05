#include "Chunk.h"

Chunk::Chunk(Camera* cameraIn, StaticChunkData* chunkDataIn, int xpos, int ypos) {
	
	camera = cameraIn;
	chunkPos = { xpos, ypos};
    chunkData = chunkDataIn;

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

    //TEST CODE======================================================
    //init activeBlockList bit based on height map
    for (int x = 0; x < CHUNK_SIZE; x++) {
        for (int z = 0; z < CHUNK_SIZE; z++) {
            int xpos = chunkPos.x;
            int zpos = chunkPos.y;
            int height = heightMapGenerator(x, z, xpos, zpos);

            //change height back to 16
            for (int y = 0; y < height; y++) {
                //sets blocks up to height as active
                blockData[x][y][z] |= IS_ACTIVE;
            }
        }
    }

    generateFaceData();

    
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
	va.AddBuffer(vb, chunkData->layout);

	//creates and starts shader program
    chunkData->shader.Bind();

	//loads texture and binds to slot 0
    chunkData->texture.Bind();
    chunkData->shader.SetUniform1i("u_Texture", 0);
    chunkData->shader.SetUniform1i("u_Texture", 0);
}

void Chunk::generateMesh(std::vector<float>* coordsList) {
	
    //meshing helper functions
    greedyMesherBottomToTop(coordsList);
    greedyMesherBackToFront(coordsList);
    greedyMesherLeftToRight(coordsList);


    printf("Number of vertices with all sides: %I64u\n", verts.size());

}


void Chunk::generateFaceData() {
    //generate face data
    for (int x = 0; x < CHUNK_SIZE; x++) {
        for (int z = 0; z < CHUNK_SIZE; z++) {
            int xpos = chunkPos.x;
            int zpos = chunkPos.y;
            int height = heightMapGenerator(x, z, xpos, zpos);

            for (int y = 0; y < CHUNK_HEIGHT; y++) {
                blockData[x][y][z] &= ~BLOCK_BOTTOM;
                blockData[x][y][z] &= ~BLOCK_TOP;
                blockData[x][y][z] &= ~BLOCK_RIGHT;
                blockData[x][y][z] &= ~BLOCK_LEFT;
                blockData[x][y][z] &= ~BLOCK_FRONT;
                blockData[x][y][z] &= ~BLOCK_BACK;
            }

            for (int y = 0; y < CHUNK_HEIGHT; y++) {
                //blockData[x][y][z] & IS_ACTIVE != 0
                if ((blockData[x][y][z] & IS_ACTIVE) != 0) {
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
                    if (x == CHUNK_SIZE - 1 || (x < CHUNK_SIZE - 1 && (blockData[x + 1][y][z] & IS_ACTIVE) == 0))
                        blockData[x][y][z] |= BLOCK_RIGHT;
                    //back face (-z)
                    if (z == 0 || (blockData[x][y][z - 1] & IS_ACTIVE) == 0)
                        blockData[x][y][z] |= BLOCK_BACK;
                    //front face (+z)
                    if (z == CHUNK_SIZE - 1 || (z < CHUNK_SIZE - 1 && (blockData[x][y][z + 1] & IS_ACTIVE) == 0))
                        blockData[x][y][z] |= BLOCK_FRONT;
                }
            }
        }
    }
}

void Chunk::greedyMesherBottomToTop(std::vector<float>* coordsList){
    int chunkX = chunkPos.x;
    int chunkZ = chunkPos.y;

    //fills entire array with false entries
    for (int i = 0; i < CHUNK_SIZE; i++) {
        for (int j = 0; j < CHUNK_HEIGHT; j++) {
            for (int h = 0; h < CHUNK_SIZE; h++) {
                //sets visited bits to 0
                blockData[i][j][h] &= ~(FIRST_VISITED);
                blockData[i][j][h] &= ~(SECOND_VISITED);
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

    //fills entire array with false entries
    for (int i = 0; i < CHUNK_SIZE; i++) {
        for (int j = 0; j < CHUNK_HEIGHT; j++) {
            for (int h = 0; h < CHUNK_SIZE; h++) {
                blockData[i][j][h] &= ~FIRST_VISITED;
                blockData[i][j][h] &= ~SECOND_VISITED;
            }
        }
    }

    for (int row = 0; row < CHUNK_SIZE; row++) {
        for (int layer = 0; layer < CHUNK_HEIGHT; layer++) {
            for (int col = 0; col < CHUNK_SIZE; col++) {

                //LEFT LAYER
                if (blockHasFace(row, layer, col, BLOCK_LEFT) && !firstVisited(row, layer, col)
                    ) {
                    int layer_temp = layer;
                    int col_temp = col;
                    bool endFlag = false;

                    while (layer_temp < CHUNK_HEIGHT && !firstVisited(row, layer_temp, col) && blockHasFace(row, layer_temp, col, BLOCK_LEFT))
                        layer_temp++;

                    while (col_temp < CHUNK_SIZE) {
                        for (int i = layer; i < layer_temp; i++) {

                            if (firstVisited(row, i, col_temp) || !blockHasFace(row, i, col_temp, BLOCK_LEFT)) {
                                endFlag = true;
                            }



                            if (endFlag)
                                break;
                        }

                        if (endFlag)
                            break;
                        else {
                            for (int i = layer; i < layer_temp; i++) {
                                blockData[row][i][col_temp] |= FIRST_VISITED;
                            }
                        }

                        col_temp++;
                    }

                    //vertices and texture mappings
                    float vertices[] = {
                        0, layer, col,                        0, 0,                                          //bottom left
                        0, layer_temp, col,                   layer_temp - layer, 0,                        //bottom right
                        0, layer_temp, col_temp,              layer_temp - layer, col_temp - col,      //top right
                        0, layer, col,                        0, 0,                                          //bottom left
                        0, layer, col_temp,                   0, col_temp - col,                        //top left
                        0, layer_temp, col_temp,              layer_temp - layer, col_temp - col       //top right
                    };

                    float newArr[30] = { 0 };

                    //translates block faces to their correct positions
                    for (int i = 0; i < 30; i += 5) {

                        /*newArr[i] = arr[i] + x + pos.xpos;
                       newArr[i + 1] = arr[i + 1] + y;
                       newArr[i + 2] = arr[i + 2] + z + pos.zpos;*/

                       //block positions translated
                        newArr[i] = vertices[i] + row + chunkX;
                        newArr[i + 1] = vertices[i + 1];
                        newArr[i + 2] = vertices[i + 2] + chunkZ;

                        //texture coords unchanged (can probably add texture coords to separate array for efficiency)
                        newArr[i + 3] = vertices[i + 3];
                        newArr[i + 4] = vertices[i + 4];
                    }

                    for (int i = 0; i < 30; i++)
                        coordsList->push_back(newArr[i]);

                }

                //RIGHT LAYER
                if (blockHasFace(row, layer, col, BLOCK_RIGHT) && !secondVisited(row, layer, col)
                    ) {
                    int layer_temp = layer;
                    int col_temp = col;
                    bool endFlag = false;

                    while (layer_temp < CHUNK_HEIGHT && !secondVisited(row, layer_temp, col) && blockHasFace(row, layer_temp, col, BLOCK_RIGHT))
                        layer_temp++;

                    while (col_temp < CHUNK_SIZE) {
                        for (int i = layer; i < layer_temp; i++) {

                            if (secondVisited(row, i, col_temp) || !blockHasFace(row, i, col_temp, BLOCK_RIGHT)) {
                                endFlag = true;
                            }



                            if (endFlag)
                                break;
                        }

                        if (endFlag)
                            break;
                        else {
                            for (int i = layer; i < layer_temp; i++) {
                                blockData[row][i][col_temp] |= SECOND_VISITED;
                            }
                        }

                        col_temp++;
                    }

                    //vertices and texture mappings
                    float vertices[] = {
                        0, layer, col,                        0, 0,                                          //bottom left
                        0, layer_temp, col,                   layer_temp - layer, 0,                        //bottom right
                        0, layer_temp, col_temp,              layer_temp - layer, col_temp - col,      //top right
                        0, layer, col,                        0, 0,                                          //bottom left
                        0, layer, col_temp,                   0, col_temp - col,                        //top left
                        0, layer_temp, col_temp,              layer_temp - layer, col_temp - col       //top right
                    };

                    float newArr[30] = { 0 };

                    //translates block faces to their correct positions
                    for (int i = 0; i < 30; i += 5) {

                        /*newArr[i] = arr[i] + x + pos.xpos;
                       newArr[i + 1] = arr[i + 1] + y;
                       newArr[i + 2] = arr[i + 2] + z + pos.zpos;*/

                       //block positions translated
                        newArr[i] = vertices[i] + row + 1 + chunkX;
                        newArr[i + 1] = vertices[i + 1];
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




void Chunk::greedyMesherBackToFront(std::vector<float>* coordsList) {

    int chunkX = chunkPos.x;
    int chunkZ = chunkPos.y;

    //fills entire array with false entries
    for (int i = 0; i < CHUNK_SIZE; i++) {
        for (int j = 0; j < CHUNK_HEIGHT; j++) {
            for (int h = 0; h < CHUNK_SIZE; h++) {
                blockData[i][j][h] &= ~FIRST_VISITED;
                blockData[i][j][h] &= ~SECOND_VISITED;
            }
        }
    }

    //left to right
    for (int col = 0; col < CHUNK_SIZE; col++) {
        for (int row = 0; row < CHUNK_SIZE; row++) {
            for (int layer = 0; layer < CHUNK_HEIGHT; layer++) {

                //BACK LAYER
                if (blockHasFace(row, layer, col, BLOCK_BACK) && !firstVisited(row, layer, col)
                    ) {
                    int row_temp = row;
                    int layer_temp = layer;
                    bool endFlag = false;

                    //iterates right until end of chunk or visited block or inactive block reached
                    while (row_temp < CHUNK_SIZE && !firstVisited(row_temp, layer, col) && blockHasFace(row_temp, layer, col, BLOCK_BACK))
                        row_temp++;


                    //iterate upwards until incomplete row or end of chunk reached
                    while (layer_temp < CHUNK_HEIGHT) {
                        for (int i = row; i < row_temp; i++) {
                            //makes sure every element in rectangle is visited

                            //end if already visited or inactive block
                            if (firstVisited(i, layer_temp, col) || !blockHasFace(i, layer_temp, col, BLOCK_BACK)) {
                                endFlag = true;
                            }



                            if (endFlag)
                                break;
                        }


                        if (endFlag)
                            break;
                        else {
                            for (int i = row; i < row_temp; i++)
                                blockData[i][layer_temp][col] |= FIRST_VISITED;
                        }

                        layer_temp++;
                    }



                    //vertices and texture mappings
                    float vertices[] = {
                        row, layer, 0,                        0, 0,                                          //bottom left
                        row_temp, layer, 0,                   row_temp - row, 0,                        //bottom right
                        row_temp, layer_temp, 0,              row_temp - row, layer_temp - layer,      //top right
                        row, layer, 0,                        0, 0,                                          //bottom left
                        row, layer_temp, 0,                   0, layer_temp - layer,                        //top left
                        row_temp, layer_temp, 0,              row_temp - row, layer_temp - layer       //top right
                    };

                    float newArr[30] = { 0 };

                    //translates block faces to their correct positions
                    for (int i = 0; i < 30; i += 5) {

                        /*newArr[i] = arr[i] + x + pos.xpos;
                       newArr[i + 1] = arr[i + 1] + y;
                       newArr[i + 2] = arr[i + 2] + z + pos.zpos;*/

                       //block positions translated
                        newArr[i] = vertices[i] + chunkX;
                        newArr[i + 1] = vertices[i + 1];
                        newArr[i + 2] = vertices[i + 2] + col + chunkZ;

                        //texture coords unchanged (can probably add texture coords to separate array for efficiency)
                        newArr[i + 3] = vertices[i + 3];
                        newArr[i + 4] = vertices[i + 4];
                    }

                    for (int i = 0; i < 30; i++)
                        coordsList->push_back(newArr[i]);

                }

                //FRONT LAYER
                if (blockHasFace(row, layer, col, BLOCK_FRONT) && !secondVisited(row, layer, col)
                    ) {
                    int row_temp = row;
                    int layer_temp = layer;
                    bool endFlag = false;

                    //iterates right until end of chunk or visited block or inactive block reached
                    while (row_temp < CHUNK_SIZE && !secondVisited(row_temp, layer, col) && blockHasFace(row_temp, layer, col, BLOCK_FRONT))
                        row_temp++;


                    //iterate upwards until incomplete row or end of chunk reached
                    while (layer_temp < CHUNK_HEIGHT) {
                        for (int i = row; i < row_temp; i++) {
                            //makes sure every element in rectangle is visited

                            //end if already visited or inactive block
                            if (secondVisited(i, layer_temp, col) || !blockHasFace(i, layer_temp, col, BLOCK_FRONT)) {
                                endFlag = true;
                            }



                            if (endFlag)
                                break;
                        }


                        if (endFlag)
                            break;
                        else {
                            for (int i = row; i < row_temp; i++) {
                                blockData[i][layer_temp][col] |= SECOND_VISITED;
                            }
                        }

                        layer_temp++;
                    }



                    //vertices and texture mappings
                    float vertices[] = {
                        row, layer, 0,                        0, 0,                                          //bottom left
                        row_temp, layer, 0,                   row_temp - row, 0,                        //bottom right
                        row_temp, layer_temp, 0,              row_temp - row, layer_temp - layer,      //top right
                        row, layer, 0,                        0, 0,                                          //bottom left
                        row, layer_temp, 0,                   0, layer_temp - layer,                        //top left
                        row_temp, layer_temp, 0,              row_temp - row, layer_temp - layer       //top right
                    };

                    float newArr[30] = { 0 };

                    //translates block faces to their correct positions
                    for (int i = 0; i < 30; i += 5) {

                        /*newArr[i] = arr[i] + x + pos.xpos;
                       newArr[i + 1] = arr[i + 1] + y;
                       newArr[i + 2] = arr[i + 2] + z + pos.zpos;*/

                       //block positions translated
                        newArr[i] = vertices[i] + chunkX;
                        newArr[i + 1] = vertices[i + 1];
                        newArr[i + 2] = vertices[i + 2] + col + 1 + chunkZ;

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

long Chunk::numFloats()
{
    return verts.size();
}

void Chunk::render() {
	//sets model-view-projection matrix and sends to shader
	//MVP = proj * view * model
    chunkData->shader.SetUniformMat4f("u_MVP",
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
    
    //if block is being added
    if (isActive) {
        //set active
        blockData[xpos][ypos][zpos] |= IS_ACTIVE;
        generateFaceData();
    }
    else {
        blockData[xpos][ypos][zpos] &= ~IS_ACTIVE;
        generateFaceData();
    }

}

//true if block position is active, false otherwise
bool Chunk::isActive(int xpos, int ypos, int zpos)
{
	return (blockData[xpos][ypos][zpos] & IS_ACTIVE) != 0;
}

//return true if block visited by first branch
bool Chunk::firstVisited(int xpos, int ypos, int zpos){
    return (blockData[xpos][ypos][zpos] & FIRST_VISITED) != 0;
}

//return true if block visited by second branch
bool Chunk::secondVisited(int xpos, int ypos, int zpos) {
    return (blockData[xpos][ypos][zpos] & SECOND_VISITED) != 0;
}

//returns true if block has a certain face
bool Chunk::blockHasFace(int xpos, int ypos, int zpos, blockFace face)
{
    return (blockData[xpos][ypos][zpos] & face) != 0;
}

int Chunk::heightMapGenerator(int xpos, int zpos, int chunkX, int chunkZ) {
	
	//out of bounds noise check
	if (xpos < 0 || xpos >= CHUNK_SIZE || zpos < 0 || zpos >= CHUNK_SIZE)
		return 0;

	//-1.5 to 1.5
	float elev = (chunkData->noiseGen.getNoise(xpos + chunkX, zpos + chunkZ)
		+ 0.5 * chunkData->noiseGen.getNoise(2 * (xpos + chunkX), 2 * (zpos + chunkZ))
		+ 0.25 * chunkData->noiseGen.getNoise(4 * (xpos + chunkX), 4 * (zpos + chunkZ)));
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