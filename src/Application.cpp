#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "Camera.h"
#include "BlockRenderer.h"

/*@TODO:
    - fix error where textures are not showing up
    - replace drawArrays() in BlockRenderer with DrawElements() using IndexBuffer class
*/

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

// settings
unsigned int SCR_WIDTH = 800;
unsigned int SCR_HEIGHT = 600;
float fov = 45.0f;
Camera camera;


// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
   
    //close window
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    //camera controls (move into camera class and call here)
    camera.keyboard_input(window);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
    SCR_WIDTH = width;
    SCR_HEIGHT = height;
}

//zoom in controls with scroll
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    fov -= (float)yoffset;
    if (fov < 1.0f)
        fov = 1.0f;
    if (fov > 45.0f)
        fov = 45.0f;
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
    
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);


    //setting window size change function pointer
    //-------------------------------------------
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    //sets cursor callback func
    camera.set_cursor_callback(window, &camera);
    //sets scroll callback func
    glfwSetScrollCallback(window, scroll_callback);

    //sets fov correctly
    camera.Set_Data(fov);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    //tests whether fragments in front of other fragments
    glEnable(GL_DEPTH_TEST);

    //rendering done here
    //-------------------
    BlockRenderer blockRender(&camera, 0.0f, 0.0f, 0.0f);

    //- generating buffers on GPU from vertex data
    //- VAO = Vertex Array Object, stores multiple buffers
    //- is required to render objects using OpenGL
    //----------------------------------------------------------
    

    //binding the buffer for use
    //specifying the type, size, actual data, and mode of the vertex data


    // creating the index buffer object, binding it to the element array buffer slot, and filling with index data

    

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
    

    //shaders written in GLSL to run on the GPU
    //takes in vec4 as input because of requirements
    //opengl automatically casts our vertices to vec4s

    //creates and starts program
    //creates renderer which handles draw calls
    



    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        //calculating time logic each frame (move into camera class)
        float currentFrame = static_cast<float>(glfwGetTime());
        
        camera.calc_time(currentFrame);

        // input
        // -----
        processInput(window);

        // render
        // ------
        glClearColor(0.46f, 0.89f, 0.51f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        //binds vertex array and index buffers
        blockRender.drawBlock();

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}
