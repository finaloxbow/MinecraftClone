#ifndef Shader_H
#define Shader_H

#include <glad/glad.h>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

struct ShaderProgramSource {
    std::string VertexSource;
    std::string FragmentSource;
};

enum class ShaderType {
    NONE = -1,
    VERTEX = 0,
    FRAGMENT = 1
};

class Shader {
public:
    //shader program ID
    unsigned int ID;
    
    //creates Shader Program
    Shader(const std::string& shaderFilePath);

    //helper function to compile a shader of some type
    static unsigned int CompileShader(unsigned int type, const std::string& source);

    //reads in shaders from file and returns struct consisting of vertex and fragment shader source code
    static ShaderProgramSource ParseShader(const std::string & filepath);



};

#endif
