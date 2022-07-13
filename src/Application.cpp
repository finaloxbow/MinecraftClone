#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "Camera.h"
#include "ChunkManager.h"

//ENTIRE CODE: MADE BY ZIPPY


void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

// settings
unsigned int SCR_WIDTH = 1920;
unsigned int SCR_HEIGHT = 1080;
float fov = 60.0f;
Camera camera;

/*TODO:
    - render chunk in a single draw call (DONE)
    - make it so covered faces are not rendered
*/

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

    glfwSetWindowPos(window, 0, 0);
    glfwSwapInterval(0);

    //makes window context current
    //----------------------------
    glfwMakeContextCurrent(window);
    
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
    //Chunk testChunk(&camera, 0, 0);
    ChunkManager chunkMgr(&camera);

    //-------turns on wireframe mode
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    int numCount = 1;
    int total = 0;
    
    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        //calculating time logic each frame (move into camera class)
        float currentFrame = static_cast<float>(glfwGetTime());
        
        camera.calc_time(currentFrame);
        float fps = 1 / (camera.getFrameTime());
        std::cout << "FPS: " << fps << std::endl;

        total += fps;
        numCount++;

        // input
        // -----
        processInput(window);

        // render
        // ------
        glClearColor(0.455f, 0.702f, 0.820f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        /*testChunk.render();
        testChunk.setData();*/
        


        chunkMgr.Update_Loaded_Chunks();
        chunkMgr.Render_Chunks();


        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    
    printf("Avg: %f\n", (float)total / numCount);


    glfwTerminate();
    return 0;
}
