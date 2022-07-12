#include "ChunkDeprec.h"

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

    firstVisited = NULL;
    secondVisited = NULL;
}

Chunk::~Chunk()
{
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

    //allocates mem for activeBlockList
    activeBlockList = new bool** [CHUNK_SIZE];
    for (int i = 0; i < CHUNK_SIZE; i++) {
        activeBlockList[i] = new bool* [CHUNK_HEIGHT];
        for (int j = 0; j < CHUNK_HEIGHT; j++) {
            activeBlockList[i][j] = new bool[CHUNK_SIZE];
        }
    }
    
    //fills activeBlockList with default "false" value
    for (int i = 0; i < CHUNK_SIZE; i++) {
        for (int j = 0; j < CHUNK_HEIGHT; j++) {
            for (int h = 0; h < CHUNK_SIZE; h++) {
                activeBlockList[i][j][h] = false;
            }
        }
    }

    //allocates mem for height map
    heightMap = new int* [CHUNK_SIZE];
    for (int i = 0; i < CHUNK_SIZE; i++) {
        heightMap[i] = new int[CHUNK_SIZE];
    }

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
    
    //inits first visited 3d arr
    firstVisited = (bool***)malloc(CHUNK_SIZE * sizeof(bool**));
    if (firstVisited != NULL) {
        for (int i = 0; i < CHUNK_SIZE; i++) {
            firstVisited[i] = (bool**)malloc(CHUNK_HEIGHT * sizeof(bool*));
            if (firstVisited[i] != NULL) {
                for (int j = 0; j < CHUNK_HEIGHT; j++) {
                    firstVisited[i][j] = (bool*)malloc(CHUNK_SIZE * sizeof(bool));
                }
            }
        }
    }
    //inits second visited 3d arr
    secondVisited = (bool***)malloc(CHUNK_SIZE * sizeof(bool**));
    if (secondVisited != NULL) {
        for (int i = 0; i < CHUNK_SIZE; i++) {
            secondVisited[i] = (bool**)malloc(CHUNK_HEIGHT * sizeof(bool*));
            if (secondVisited[i] != NULL) {
                for (int j = 0; j < CHUNK_HEIGHT; j++) {
                    secondVisited[i][j] = (bool*)malloc(CHUNK_SIZE * sizeof(bool));
                }
            }
        }
    }

    //allocates mem for activeBlockList
    blockFaceList = new int** [CHUNK_SIZE];
    for (int i = 0; i < CHUNK_SIZE; i++) {
        blockFaceList[i] = new int* [CHUNK_HEIGHT];
        for (int j = 0; j < CHUNK_HEIGHT; j++) {
            blockFaceList[i][j] = new int[CHUNK_SIZE];
        }
    }

    chunkHasUpdated = true;
}

void Chunk::SetData() {
    verts.clear();
    
    
    GreedyMeshGeneratorBottomToTop(&verts);
    GreedyMeshGeneratorBackToFront(&verts);
    GreedyMeshGeneratorLeftToRight(&verts);

    chunkHasUpdated = false;

}

void Chunk::setRendering() {

    if (chunkHasUpdated) {
        SetData();
        chunkHasUpdated = false;
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

bool Chunk::chunkUpdated()
{
    return chunkHasUpdated;
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

    
    //shader.Bind();
    va.Bind();

    //We are drawing CHUNK_SIZE^2*CHUNK_HEIGHT blocks
    glDrawArrays(GL_TRIANGLES, 0, verts.size() / 5);
}

//updates whether a block is being rendered
void Chunk::UpdateBlock(int xpos, int ypos, int zpos, bool isActive)
{


    activeBlockList[xpos][ypos][zpos] = isActive;
    chunkHasUpdated = true;

    //this->SetData();
}

void Chunk::updateBlock(int xpos, int ypos, int zpos, bool isActive)
{
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
 
                //resets list of faces to render
                blockFaceList[x][y][z] = 0;
                
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

/*
IDEAS TO MAKE CODE MORE EFFICIENT:
    - make it so inner layers are not rendered if the block is not active
    - make it so the mesher actually WORKS!!!
*/
void Chunk::GreedyMeshGeneratorBottomToTop(std::vector<float>* coordsList)
{

    //fills entire array with false entries
    for (int i = 0; i < CHUNK_SIZE; i++) {
        for (int j = 0; j < CHUNK_HEIGHT; j++) {
            for (int h = 0; h < CHUNK_SIZE; h++) {
                firstVisited[i][j][h] = false;
                secondVisited[i][j][h] = false;
            }
        }
    }

    for (int layer = 0; layer < CHUNK_HEIGHT; layer++) {
        for (int row = 0; row < CHUNK_SIZE; row++) {
            for (int col = 0; col < CHUNK_SIZE; col++) {


                //BOTTOM LAYER
                if (activeBlockList[row][layer][col] && !firstVisited[row][layer][col]) {
                    int row_temp = row;
                    int col_temp = col;
                    bool endFlag = false;

                    //iterates right until end of chunk or visited block or inactive block reached
                    while (row_temp < CHUNK_SIZE && !firstVisited[row_temp][layer][col] && activeBlockList[row_temp][layer][col])
                        row_temp++;


                    //iterate upwards until incomplete row or end of chunk reached
                    while (col_temp < CHUNK_SIZE) {
                        for (int i = row; i < row_temp; i++) {
                            //makes sure every element in rectangle is visited

                            //end if already visited or inactive block
                            if (firstVisited[i][layer][col_temp] || !activeBlockList[i][layer][col_temp]) {
                                endFlag = true;
                            }


                            if (endFlag)
                                break;
                        }


                        if (endFlag)
                            break;
                        else {
                            for (int i = row; i < row_temp; i++) {
                                firstVisited[i][layer][col_temp] = true;
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
                        newArr[i] = vertices[i] + pos.xpos;
                        newArr[i + 1] = vertices[i + 1] + layer;
                        newArr[i + 2] = vertices[i + 2] + pos.zpos;

                        //texture coords unchanged (can probably add texture coords to separate array for efficiency)
                        newArr[i + 3] = vertices[i + 3];
                        newArr[i + 4] = vertices[i + 4];
                    }

                    for (int i = 0; i < 30; i++)
                        coordsList->push_back(newArr[i]);

                }

                //TOP LAYER
                if (activeBlockList[row][layer][col] && !secondVisited[row][layer][col]) {
                    int row_temp = row;
                    int col_temp = col;
                    bool endFlag = false;

                    //iterates right until end of chunk or visited block or inactive block reached
                    while (row_temp < CHUNK_SIZE && !secondVisited[row_temp][layer][col] && activeBlockList[row_temp][layer][col])
                        row_temp++;


                    //iterate upwards until incomplete row or end of chunk reached
                    while (col_temp < CHUNK_SIZE) {
                        for (int i = row; i < row_temp; i++) {
                            //makes sure every element in rectangle is visited

                            //end if already visited or inactive block
                            if (secondVisited[i][layer][col_temp] || !activeBlockList[i][layer][col_temp]) {
                                endFlag = true;
                            }


                            if (endFlag)
                                break;
                        }


                        if (endFlag)
                            break;
                        else {
                            for (int i = row; i < row_temp; i++) {
                                secondVisited[i][layer][col_temp] = true;
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
                        newArr[i] = vertices[i] + pos.xpos;
                        newArr[i + 1] = vertices[i + 1] + layer + 1;
                        newArr[i + 2] = vertices[i + 2] + pos.zpos;

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

void Chunk::GreedyMeshGeneratorBackToFront(std::vector<float>* coordsList)
{

    //fills entire array with false entries
    for (int i = 0; i < CHUNK_SIZE; i++) {
        for (int j = 0; j < CHUNK_HEIGHT; j++) {
            for (int h = 0; h < CHUNK_SIZE; h++) {
                firstVisited[i][j][h] = false;
                secondVisited[i][j][h] = false;
            }
        }
    }

    //left to right
    for (int col = 0; col < CHUNK_SIZE; col++) {
        for (int row = 0; row < CHUNK_SIZE; row++) {
            for (int layer = 0; layer < CHUNK_HEIGHT; layer++) {

                //BACK LAYER
                if (activeBlockList[row][layer][col] && !firstVisited[row][layer][col]
                    ) {
                    int row_temp = row;
                    int layer_temp = layer;
                    bool endFlag = false;

                    //iterates right until end of chunk or visited block or inactive block reached
                    while (row_temp < CHUNK_SIZE && !firstVisited[row_temp][layer][col] && activeBlockList[row_temp][layer][col])
                        row_temp++;


                    //iterate upwards until incomplete row or end of chunk reached
                    while (layer_temp < CHUNK_HEIGHT) {
                        for (int i = row; i < row_temp; i++) {
                            //makes sure every element in rectangle is visited

                            //end if already visited or inactive block
                            if (firstVisited[i][layer_temp][col] || !activeBlockList[i][layer_temp][col]) {
                                endFlag = true;
                            }

                            

                            if (endFlag)
                                break;
                        }


                        if (endFlag)
                            break;
                        else {
                            for(int i = row; i < row_temp; i++)
                                firstVisited[i][layer_temp][col] = true;
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
                        newArr[i] = vertices[i] + pos.xpos;
                        newArr[i + 1] = vertices[i + 1];
                        newArr[i + 2] = vertices[i + 2] + col + pos.zpos;

                        //texture coords unchanged (can probably add texture coords to separate array for efficiency)
                        newArr[i + 3] = vertices[i + 3];
                        newArr[i + 4] = vertices[i + 4];
                    }

                    for (int i = 0; i < 30; i++)
                        coordsList->push_back(newArr[i]);

                }

                //FRONT LAYER
                if (activeBlockList[row][layer][col] && !secondVisited[row][layer][col]
                    ) {
                    int row_temp = row;
                    int layer_temp = layer;
                    bool endFlag = false;

                    //iterates right until end of chunk or visited block or inactive block reached
                    while (row_temp < CHUNK_SIZE && !secondVisited[row_temp][layer][col] && activeBlockList[row_temp][layer][col])
                        row_temp++;


                    //iterate upwards until incomplete row or end of chunk reached
                    while (layer_temp < CHUNK_HEIGHT) {
                        for (int i = row; i < row_temp; i++) {
                            //makes sure every element in rectangle is visited

                            //end if already visited or inactive block
                            if (secondVisited[i][layer_temp][col] || !activeBlockList[i][layer_temp][col]) {
                                endFlag = true;
                            }

                            

                            if (endFlag)
                                break;
                        }


                        if (endFlag)
                            break;
                        else {
                            for (int i = row; i < row_temp; i++) {
                                secondVisited[i][layer_temp][col] = true;
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
                        newArr[i] = vertices[i] + pos.xpos;
                        newArr[i + 1] = vertices[i + 1];
                        newArr[i + 2] = vertices[i + 2] + col + 1 + pos.zpos;

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

void Chunk::GreedyMeshGeneratorLeftToRight(std::vector<float>* coordsList)
{
    //fills entire array with false entries
    for (int i = 0; i < CHUNK_SIZE; i++) {
        for (int j = 0; j < CHUNK_HEIGHT; j++) {
            for (int h = 0; h < CHUNK_SIZE; h++) {
                firstVisited[i][j][h] = false;
                secondVisited[i][j][h] = false;
            }
        }
    }

    for (int row = 0; row < CHUNK_SIZE; row++) {
        for (int layer = 0; layer < CHUNK_HEIGHT; layer++) {
            for (int col = 0; col < CHUNK_SIZE; col++) {

                //LEFT LAYER
                if (activeBlockList[row][layer][col] && !firstVisited[row][layer][col]
                    ) {
                    int layer_temp = layer;
                    int col_temp = col;
                    bool endFlag = false;

                    while (layer_temp < CHUNK_HEIGHT && !firstVisited[row][layer_temp][col] && activeBlockList[row][layer_temp][col])
                        layer_temp++;

                    while (col_temp < CHUNK_SIZE) {
                        for (int i = layer; i < layer_temp; i++) {

                            if (firstVisited[row][i][col_temp] || !activeBlockList[row][i][col_temp]) {
                                endFlag = true;
                            }



                            if (endFlag)
                                break;
                        }

                        if (endFlag)
                            break;
                        else {
                            for (int i = layer; i < layer_temp; i++) {
                                firstVisited[row][i][col_temp] = true;
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
                        newArr[i] = vertices[i] + row + pos.xpos;
                        newArr[i + 1] = vertices[i + 1];
                        newArr[i + 2] = vertices[i + 2] + pos.zpos;

                        //texture coords unchanged (can probably add texture coords to separate array for efficiency)
                        newArr[i + 3] = vertices[i + 3];
                        newArr[i + 4] = vertices[i + 4];
                    }

                    for (int i = 0; i < 30; i++)
                        coordsList->push_back(newArr[i]);

                }

                //RIGHT LAYER
                if (activeBlockList[row][layer][col] && !secondVisited[row][layer][col]
                    ){
                    int layer_temp = layer;
                    int col_temp = col;
                    bool endFlag = false;

                    while (layer_temp < CHUNK_HEIGHT && !secondVisited[row][layer_temp][col] && activeBlockList[row][layer_temp][col])
                        layer_temp++;

                    while (col_temp < CHUNK_SIZE) {
                        for (int i = layer; i < layer_temp; i++) {

                            if (secondVisited[row][i][col_temp] || !activeBlockList[row][i][col_temp]) {
                                endFlag = true;
                            }

                            

                            if (endFlag)
                                break;
                        }

                        if (endFlag)
                            break;
                        else {
                            for (int i = layer; i < layer_temp; i++) {
                                secondVisited[row][i][col_temp] = true;
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
                        newArr[i] = vertices[i] + row + 1 + pos.xpos;
                        newArr[i + 1] = vertices[i + 1];
                        newArr[i + 2] = vertices[i + 2] + pos.zpos;

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

