#include "Chunk.h"

//resolve weird code dup here

Chunk::Chunk(Camera* cameraIn)
    : camera(cameraIn)
{
    //fill activeBlockList with initial values
    for (int x = 0; x < CHUNK_SIZE; x++) {
        for (int y = 0; y < CHUNK_SIZE; y++) {
            for (int z = 0; z < CHUNK_SIZE; z++) {
                activeBlockList[x][y][z] = true;
            }
        }
    }

    //rendering starting info
    std::vector<float> verts;

    //precompile all necessary vertex data for a 
    // single flat chunk into a single vector
    for (int x = 0; x < 16; x++) {
        for (int y = 0; y < 16; y++) {
            for (int z = 0; z < 16; z++) {
                if (activeBlockList[x][y][z]) {
                    for (int t = 0; t < 5 * 6 * 6; t += 5) {
                        verts.push_back(positions[t] + x);
                        verts.push_back(positions[t + 1] + y);
                        verts.push_back(positions[t + 2] + z);
                        verts.push_back(positions[t + 3]);
                        verts.push_back(positions[t + 4]);
                    }
                }
            }
        }
    }

    vb.Set_Data(&verts[0], verts.size() * sizeof(verts[0]));

    //buffer layouts
    layout.Push<float>(3);
    layout.Push<float>(2);
    va.AddBuffer(vb, layout);

    //creates and starts program
    shader.Set_Data("res/shaders/Basic.shader");
    shader.Bind();

    //loads texture and binds it to texture slot 0
    texture.Set_Data("res/textures/texturelol.jpg");
    texture.Bind();
    shader.SetUniform1i("u_Texture", 0);
	
}

void Chunk::SetData() {
    //rendering starting info
    std::vector<float> verts;

    //precompile all necessary vertex data for a 
    // single flat chunk into a single vector
    for (int x = 0; x < 16; x++) {
        for (int y = 0; y < 16; y++) {
            for (int z = 0; z < 16; z++) {
                if (activeBlockList[x][y][z]) {
                    //adds all translated block positions to verts
                    for (int t = 0; t < 5 * 6 * 6; t += 5) {
                        verts.push_back(positions[t] + x);
                        verts.push_back(positions[t + 1] + y);
                        verts.push_back(positions[t + 2] + z);
                        verts.push_back(positions[t + 3]);
                        verts.push_back(positions[t + 4]);
                    }
                }
            }
        }
    }

    vb.Set_Data(&verts[0], verts.size() * sizeof(verts[0]));

    va.AddBuffer(vb, layout);
    
}

//delete activeBlockList array here
Chunk::~Chunk()
{
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
    glDrawArrays(GL_TRIANGLES, 0, CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE * VERTICES_COUNT);
}

void Chunk::UpdateBlock(int xpos, int ypos, int zpos, bool isActive)
{
    activeBlockList[xpos][ypos][zpos] = isActive;
    
    //inefficient?
    this->SetData();
}

bool Chunk::isActive(int xpos, int ypos, int zpos)
{
	return activeBlockList[xpos][ypos][zpos];
}
