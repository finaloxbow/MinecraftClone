#include "BlockRenderer.h"

//@TODO change drawArrays to drawElements using index buffer

static float positions[] = {
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,  
     0.5f, -0.5f, -0.5f,  1.0f, 0.0f,   
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,   

    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

    -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
};

BlockRenderer::BlockRenderer(Camera* camera_ptr)
    :camera(camera_ptr)
{
    std::vector<float> verts;
    
    
    for (int x = 0; x < 16; x++) {
        for (int y = 0; y < 16; y++) {
            for (int z = 0; z < 16; z++) {
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

void BlockRenderer::drawBlock(float xpos, float ypos, float zpos)
{

        //abstracted camera into separate class
        glm::mat4 proj = camera->getPerspectiveMatrix();
        glm::mat4 view = camera->getViewMatrix();
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(xpos, ypos, zpos));

        glm::mat4 MVP = proj * view * model;
        //model view projection matrix sent to shader
        shader.SetUniformMat4f("u_MVP", MVP);


        shader.Bind();
        va.Bind();
        glDrawArrays(GL_TRIANGLES, 0, 16*16*16*VERTICES_COUNT);

   
}


