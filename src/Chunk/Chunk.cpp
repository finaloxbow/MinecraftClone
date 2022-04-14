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

    //precompile all necessary vertex data for a 
    // single flat chunk into a single vector
    for (int x = 0; x < CHUNK_SIZE; x++) {
        for (int y = 0; y < CHUNK_HEIGHT; y++) {
            for (int z = 0; z < CHUNK_SIZE; z++) {
                if (activeBlockList[x][y][z]) {
                    //if other block not rendered, render face

                    /*@TODO: check whether block to side is active
                                - if so, do not render current block*/

                    if ((x > 0 && !activeBlockList[x - 1][y][z]) || x == 0) {
                        //render right
                        transFace(blockRight, tempArr, 30, x, y, z);
                        for (int i = 0; i < 30; i++)
                            verts.push_back(tempArr[i]);
                    }
                    if ((x < CHUNK_SIZE - 1 && !activeBlockList[x + 1][y][z]) || x == CHUNK_SIZE - 1) {
                        //render left
                        transFace(blockLeft, tempArr, 30, x, y, z);
                        for (int i = 0; i < 30; i++)
                            verts.push_back(tempArr[i]);
                    }
                    if ((y > 0 && !activeBlockList[x][y - 1][z]) || y == 0) {
                        //render bottom
                        transFace(blockBottom, tempArr, 30, x, y, z);
                        for (int i = 0; i < 30; i++)
                            verts.push_back(tempArr[i]);
                    }
                    if ((y < CHUNK_HEIGHT - 1 && !activeBlockList[x][y + 1][z]) || y == CHUNK_HEIGHT - 1) {
                        //render top
                        transFace(blockTop, tempArr, 30, x, y, z);
                        for (int i = 0; i < 30; i++)
                            verts.push_back(tempArr[i]);
                    }
                    if ((z > 0 && !activeBlockList[x][y][z - 1]) || z == 0) {
                        //render front
                        transFace(blockFront, tempArr, 30, x, y, z);
                        for (int i = 0; i < 30; i++)
                            verts.push_back(tempArr[i]);
                    }
                    if ((z < CHUNK_SIZE - 1 && !activeBlockList[x][y][z + 1]) || z == CHUNK_SIZE - 1) {
                        //render back
                        transFace(blockBack, tempArr, 30, x, y, z);
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


