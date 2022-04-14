#include "Chunk.h"

//static vars
VertexBufferLayout Chunk::layout;
Shader Chunk::shader;
Texture Chunk::texture;
bool Chunk::staticInit = false;
Noise Chunk::noiseGen;

Chunk::Chunk()
{
    pos.xpos = 0;
    pos.zpos = 0;
    camera = nullptr;

    //sets activeBlockList to all true
    for (int x = 0; x < CHUNK_SIZE; x++) {
        for (int y = 0; y < CHUNK_HEIGHT; y++) {
            for (int z = 0; z < CHUNK_SIZE; z++) {
                activeBlockList[x][y][z] = false;
            }
        }
    }

    //sets height map to all 0
    for (int x = 0; x < CHUNK_SIZE; x++) {
        for (int z = 0; z < CHUNK_SIZE; z++) {
            heightMap[x][z] = 0;
        }
    }

    //sets block face list to 0
    for (int i = 0; i < CHUNK_SIZE; i++) {
        for (int j = 0; j < CHUNK_HEIGHT; j++) {
            for (int k = 0; k < CHUNK_SIZE; k++) {
                blockFaceList[i][j][k] = 0;
            }
        }
    }
}


Chunk::Chunk(Camera* cameraIn, int xpos, int zpos)
    : camera(cameraIn)
{
    if (!staticInit) {
        layout.Push<float>(3);
        layout.Push<float>(2);

        shader.Set_Data("res/shaders/Basic.shader");
        texture.Set_Data("res/textures/concreteTexture.png");

        staticInit = true;
    }

    pos.xpos = xpos;
    pos.zpos = zpos;

    //sets height map
    for (int x = 0; x < CHUNK_SIZE; x++) {
        for (int z = 0; z < CHUNK_SIZE; z++) {
            //heightMap[x][z] = (int)((CHUNK_HEIGHT - 2) / 2) * (noiseGen.getNoise(pos.xpos + x, pos.zpos + z) + 1) + 1;
            heightMap[x][z] = heightMapGenerator(pos.xpos, pos.zpos, x, z);
        }
    }

    //fill activeBlockList with initial values
    for (int x = 0; x < CHUNK_SIZE; x++) {
        for (int z = 0; z < CHUNK_SIZE; z++) {
            
            //within range [1, CHUNK_SIZE - 1]
            int height = heightMap[x][z];
            
            for (int y = 0; y < height; y++) {
                activeBlockList[x][y][z] = true;
            }
        }
    }

    
}

void Chunk::SetData() {

    //rendering starting info
    std::vector<float> verts;
    float tempArr[30];

    //generates which block faces to render
    blockFaceGenerator();

    //precompile all necessary vertex data for a 
    // single flat chunk into a single vector
    for (int x = 0; x < CHUNK_SIZE; x++) {
        for (int y = 0; y < CHUNK_HEIGHT; y++) {
            for (int z = 0; z < CHUNK_SIZE; z++) {
                if (activeBlockList[x][y][z]) {
                    int faceData = blockFaceList[x][y][z];

                    //right
                    if ((faceData & BLOCK_RIGHT) == BLOCK_RIGHT) {
                        transFace(blockRight, tempArr, 30, x, y, z);
                        for (int i = 0; i < 30; i++)
                            verts.push_back(tempArr[i]);
                    }

                    //left
                    if ((faceData & BLOCK_LEFT) == BLOCK_LEFT) {
                        transFace(blockLeft, tempArr, 30, x, y, z);
                        for (int i = 0; i < 30; i++)
                            verts.push_back(tempArr[i]);
                    }

                    //back
                    if ((faceData & BLOCK_BACK) == BLOCK_BACK) {
                        transFace(blockBack, tempArr, 30, x, y, z);
                        for (int i = 0; i < 30; i++)
                            verts.push_back(tempArr[i]);
                    }

                    //front
                    if ((faceData & BLOCK_FRONT) == BLOCK_FRONT) {
                        transFace(blockFront, tempArr, 30, x, y, z);
                        for (int i = 0; i < 30; i++)
                            verts.push_back(tempArr[i]);
                    }

                    //top
                    if ((faceData & BLOCK_TOP) == BLOCK_TOP) {
                        transFace(blockTop, tempArr, 30, x, y, z);
                        for (int i = 0; i < 30; i++)
                            verts.push_back(tempArr[i]);
                    }

                    //bottom
                    if ((faceData & BLOCK_BOTTOM) == BLOCK_BOTTOM) {
                        transFace(blockBottom, tempArr, 30, x, y, z);
                        for (int i = 0; i < 30; i++)
                            verts.push_back(tempArr[i]);
                    }
                }
            }
        }
    }

    vb.Set_Data(&verts[0], (unsigned int)verts.size() * sizeof(verts[0]));

    //buffer layouts
    va.AddBuffer(vb, layout);

    //creates and starts program
    shader.Bind();

    //loads texture and binds it to texture slot 0
    texture.Bind();
    shader.SetUniform1i("u_Texture", 0);

}


void Chunk::Render()
{
    //abstracted camera into separate class
    glm::mat4 proj = camera->getPerspectiveMatrix();
    glm::mat4 view = camera->getViewMatrix();
    glm::mat4 model = glm::mat4(1.0f);

    glm::mat4 MVP = proj * view * model;
    //model view projection matrix sent to shader
    shader.SetUniformMat4f("u_MVP", MVP);

    
    shader.Bind();
    va.Bind();

    //We are drawing CHUNK_SIZE^3 blocks
    glDrawArrays(GL_TRIANGLES, 0, CHUNK_SIZE * CHUNK_HEIGHT * CHUNK_SIZE * VERTICES_COUNT);
}

//updates whether a block is being rendered
void Chunk::UpdateBlock(int xpos, int ypos, int zpos, bool isActive)
{
    activeBlockList[xpos][ypos][zpos] = isActive;
    
    this->SetData();
}

bool Chunk::isActive(int xpos, int ypos, int zpos)
{
	return activeBlockList[xpos][ypos][zpos];
}

//translates elements of arr are put into newArr
//specifically designed for the static block vertices arrays
void Chunk::transFace(const float arr[], float newArr[], int size, int x, int y, int z)
{
    for (int i = 0; i < size; i += 5) {
        //block positions translated
        newArr[i] = arr[i] + x + pos.xpos;
        newArr[i+1] = arr[i+1] + y;
        newArr[i + 2] = arr[i + 2] + z + pos.zpos;

        //texture coords unchanged (can probably add texture coords to separate array for efficiency)
        newArr[i + 3] = arr[i + 3];
        newArr[i + 4] = arr[i + 4];
    }
}

int Chunk::heightMapGenerator(int xpos, int zpos, int chunkX, int chunkZ) {
    
    //-1.5 to 1.5
    float elev = (noiseGen.getNoise(xpos + chunkX, zpos + chunkZ)
            + 0.5 * noiseGen.getNoise(2*(xpos + chunkX),2*( zpos + chunkZ))
            + 0.25 * noiseGen.getNoise(4*(xpos + chunkX),4*(zpos + chunkZ)));
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

void Chunk::blockFaceGenerator()
{
    for (int x = 0; x < CHUNK_SIZE; x++) {
        for (int y = 0; y < CHUNK_HEIGHT; y++) {
            for (int z = 0; z < CHUNK_SIZE; z++) {
                if (activeBlockList[x][y][z]) {
                    //if other block not rendered, render face

                    /*@TODO: check whether block to side is active
                                - if so, do not render current block*/

                    if ((x > 0 && !activeBlockList[x - 1][y][z]) || x == 0) {
                        //render right
                        blockFaceList[x][y][z] += BLOCK_RIGHT;
                    }
                    if ((x < CHUNK_SIZE - 1 && !activeBlockList[x + 1][y][z]) || x == CHUNK_SIZE - 1) {
                        //render left
                        blockFaceList[x][y][z] += BLOCK_LEFT;
                    }
                    if ((y > 0 && !activeBlockList[x][y - 1][z]) || y == 0) {
                        //render bottom
                        blockFaceList[x][y][z] += BLOCK_BOTTOM;
                    }
                    if ((y < CHUNK_HEIGHT - 1 && !activeBlockList[x][y + 1][z]) || y == CHUNK_HEIGHT - 1) {
                        //render top
                        blockFaceList[x][y][z] += BLOCK_TOP;
                    }
                    if ((z > 0 && !activeBlockList[x][y][z - 1]) || z == 0) {
                        //render front
                        blockFaceList[x][y][z] += BLOCK_FRONT;
                    }
                    if ((z < CHUNK_SIZE - 1 && !activeBlockList[x][y][z + 1]) || z == CHUNK_SIZE - 1) {
                        //render back
                        blockFaceList[x][y][z] += BLOCK_BACK;
                    }
                }
            }
        }
    }
}

void Chunk::GreedyMeshGenerator(std::vector<float> vertices) {
    int i, j, k, l, w, h, u, v, n, side = 0;
    int x[3]{ 0,0,0 };
    int q[3]{ 0,0,0 };
    int du[3]{ 0,0,0 };
    int dv[3]{ 0,0,0 };

    int mask[CHUNK_SIZE*CHUNK_HEIGHT];

    for (bool backFace = true, b = false; b != backFace; backFace = backFace && b, b = !b) {
        for (int d = 0; d < 3; d++) {
            u = (d + 1) % 3;
            v = (d + 2) % 3;

            x[0] = x[1] = x[2] = 0;
            q[0] = q[1] = q[2] = 0;
            q[d] = 1;

            if (d == 0) { side = backFace ? BLOCK_RIGHT : BLOCK_LEFT; }
            else if (d == 1) { side = backFace ? BLOCK_BOTTOM : BLOCK_TOP; }
            else if (d == 2) { side = backFace ? BLOCK_FRONT : BLOCK_BACK; }

            for (x[d] = -1; x[d] < CHUNK_SIZE;) {
                
                n = 0;

                for (x[v] = 0; x[v] < CHUNK_HEIGHT; x[v]++) {
                    for (x[u] = 0; x[u] < CHUNK_SIZE; x[u]++) {
                        
                        int voxelFace1 = (x[d] >= 0) ? blockFaceList[x[0]][x[1]][x[2]] : 0;
                        int voxelFace2 = (x[d] < CHUNK_SIZE - 1) ?
                            blockFaceList[x[0] + q[0]][x[1] + q[1]][x[2] + q[2]] : 0;
                        
                        mask[n++] = ((voxelFace1 != 0 && voxelFace2 != 0)) ? 0
                            : backFace ? voxelFace1 : voxelFace2;
                    }
                }

                x[d]++;

                n = 0;

                for (j = 0; j < CHUNK_HEIGHT; j++) {
                    for (i = 0; i < CHUNK_SIZE;) {
                        if (mask[n] != 0) {
                            for(w = 1; i + w < CHUNK_SIZE && mask[n + w] != 0 &&
                                mask[n+w] == mask[n]; w++){ }

                            bool done = false;

                            for (h = 1; j + h < CHUNK_HEIGHT; h++) {
                                for (k = 0; k < w; k++) {
                                    if (mask[n + k + h * CHUNK_SIZE] == 0 ||
                                        mask[n + k + h * CHUNK_SIZE] != mask[n])
                                        done = true;
                                }

                                if (done)
                                    break;
                            }

                            //render quad

                            pushQuad(
                                glm::vec3(x[0], x[1], x[2]),
                                glm::vec3(x[0] + du[0], x[1] + du[1], x[2] + du[2]),
                                glm::vec3(x[0] + du[0] + dv[0], x[1] + du[1] + dv[1], x[2] + du[2] + dv[2]),
                                glm::vec3(x[0] + dv[0], x[1] + dv[1], x[2] + dv[2]),
                                vertices
                            );
                            

                            for (l = 0; l < h; ++l) {
                                for (k = 0; k < w; ++k) {
                                    mask[n + k + l * CHUNK_SIZE] = 0;
                                }
                            }

                            i += w;
                            n += w;
                        }
                        else {
                            i++;
                            n++;
                        }
                    }
                }
            }
        }
    }
}

void Chunk::pushQuad(glm::vec3 bottomLeft, glm::vec3 topLeft, glm::vec3 topRight, glm::vec3 bottomRight,
    std::vector<float> vertices) {

}

