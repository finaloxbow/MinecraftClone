#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "Shader.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "VertexArray.h"
#include "VerterBufferLayout.h"

#include <iostream>


void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
    //close window
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}


int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwSetWindowPos(window, 560, 240);

    //makes window context current
    //----------------------------
    glfwMakeContextCurrent(window);

    //syncs with monitor refresh rate
    glfwSwapInterval(1);

    //setting window size change function pointer
    //-------------------------------------------
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    //rendering done here
    //-------------------
    float positions[] = {
        -0.5f, -0.5f,   // 0
         0.5f, -0.5f,   // 1
         0.5f,  0.5f,   // 2
        -0.5f,  0.5f    // 3
    };

    //order of how to draw indices
    unsigned int indices[] = {
        0, 1 ,2,    // first triangle
        2, 3, 0     // second triangle
    };

    //- generating buffers on GPU from vertex data
    //- VAO = Vertex Array Object, stores multiple buffers
    //- is required to render objects using OpenGL
    //----------------------------------------------------------
    VertexArray va;
    

    //binding the buffer for use
    //specifying the type, size, actual data, and mode of the vertex data
    VertexBuffer vb(positions, sizeof(positions));

    // creating the index buffer object, binding it to the element array buffer slot, and filling with index data
    IndexBuffer ib(indices, 6);

    

    /*
    - specifying how the vertex data is structured
    - first param is "index" of the first vertex position attrib. since this is the first one, we use 1
    - second param is the size of the vertex attribute. for us, it is 2 floats, so use 2.
    - third param is the type of the vertex data
    - fourth param is whether to normalize the given data to between -1 and 1. Since we already start with normalized data, it is false
    - fifth param is the distance from the start of the first vertex attribute to the second one. Since the size of each vertex attribute (position
    in this case) is 2 floats, the distance from the start of the first attribute to the start of the second is 2 floats
    - last param is the offset of the first vertex attribute from the beginning of the data

    */
    VertexBufferLayout layout;
    layout.Push<float>(2);
    va.AddBuffer(vb, layout);
    


    //shaders written in GLSL to run on the GPU
    //takes in vec4 as input because of requirements
    //opengl automatically casts our vertices to vec4s

    //creates and starts program
    Shader shader("res/shaders/Basic.shader");
    glUseProgram(shader.ID);

    //finding uniform in shader program and setting color from program
    //instead of GLSL file
    int location = glGetUniformLocation(shader.ID, "u_Color");
    
    float r = 0.0f;
    float increment = 0.05f;


    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // input
        // -----
        processInput(window);

        // render
        // ------
        glClearColor(0.46f, 0.89f, 0.51f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        //using the shader program
        glUseProgram(shader.ID);
        //sets the value of the uniform
        glUniform4f(location, r, 0.3f, 0.8f, 1.0f);

        va.Bind();
        ib.Bind();
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        //updates the color of the rectangle
        if (r > 1.0f)
            increment = -0.05f;
        else if (r < 0.0f)
            increment = 0.05f;

        r += increment;
        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glDeleteProgram(shader.ID);
    glfwTerminate();
    return 0;
}
