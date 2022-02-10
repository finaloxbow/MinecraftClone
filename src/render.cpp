#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

struct ShaderProgramSource {
    std::string VertexSource;
    std::string FragmentSource;
};

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

//helper function to compile a shader of some type
static unsigned int CompileShader(unsigned int type, const std::string& source) {
    unsigned int id = glCreateShader(type);
    const char* src = source.c_str();
    glShaderSource(id, 1, &src, nullptr);
    glCompileShader(id);

    return id;
}

//creates the shader program
static unsigned int CreateShader(const std::string& vertexShader, const std::string& fragmentShader) {

    //creates program and compiles shaders from their source code
    unsigned int program = glCreateProgram();
    unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
    unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);
    glValidateProgram(program);

    //shader cleanup
    glDeleteShader(vs);
    glDeleteShader(fs);

    return program;
}

static ShaderProgramSource ParseShader(const std::string& filepath) {

    std::ifstream stream(filepath);

    enum class ShaderType {
        NONE = -1,
        VERTEX = 0,
        FRAGMENT = 1
    };

    std::string line;
    std::stringstream ss[2];
    ShaderType type = ShaderType::NONE;

    while (getline(stream, line)) {
        if (line.find("#shader") != std::string::npos) {
            if (line.find("vertex") != std::string::npos)
                type = ShaderType::VERTEX;
            else if (line.find("fragment") != std::string::npos)
                type = ShaderType::FRAGMENT;
        }
        else {
            //makes sure garbage isnt read in from shader file
            if (type != ShaderType::NONE)
                ss[(int)type] << line << '\n';
        }
    }

    return { ss[0].str(), ss[1].str() };
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
    float positions[6] = {
        -0.5f, -0.5f,
        0.0f, 0.5f,
        0.5f, -0.5f
    };

    //generating buffers on GPU from vertex data
    //TODO: find out why VAO required here for objects to render
    //----------------------------------------------------------
    unsigned int buffer, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &buffer);
    glBindVertexArray(VAO);

    //binding the buffer for use
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    //specifying the type, size, actual data, and mode of the vertex data
    glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(float), positions, GL_STATIC_DRAW);

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
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);

    //shaders written in GLSL to run on the GPU
    //takes in vec4 as input because of requirements
    //opengl automatically casts our vertices to vec4s

    ShaderProgramSource source = ParseShader("res/shaders/Basic.shader");
    std::cout << "----------Vertex Shader------------------" << std::endl;
    std::cout << source.VertexSource << std::endl;
    std::cout << "--------- Fragment Shader ---------------" << std::endl;
    std::cout << source.FragmentSource << std::endl;

    //creates the shader program to use
    unsigned int shader = CreateShader(source.VertexSource, source.FragmentSource);

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
        glUseProgram(shader);
        glDrawArrays(GL_TRIANGLES, 0, 3);


        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glDeleteProgram(shader);
    glfwTerminate();
    return 0;
}
