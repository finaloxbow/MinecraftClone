#include <glad/glad.h>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include "Shader.h"


//creates Shader Program
Shader::Shader(const std::string& shaderFilePath) {

    //create shader file path var as arg to replace the two curr args
    ShaderProgramSource source = Shader::ParseShader(shaderFilePath);
        
    //prints out shader source file
    std::cout << "----------Vertex Shader------------------" << std::endl;
    std::cout << source.VertexSource << std::endl;
    std::cout << "--------- Fragment Shader ---------------" << std::endl;
    std::cout << source.FragmentSource << std::endl;

        
    //creates program and compiles shaders from their source code
    unsigned int program = glCreateProgram();
    unsigned int vs = CompileShader(GL_VERTEX_SHADER, source.VertexSource);
    unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, source.FragmentSource);

    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);
    glValidateProgram(program);

    //shader cleanup
    glDeleteShader(vs);
    glDeleteShader(fs);

    ID = program;
}

//helper function to compile a shader of some type
unsigned int Shader::CompileShader(unsigned int type, const std::string& source) {
    unsigned int idShader = glCreateShader(type);
    const char* src = source.c_str();
    glShaderSource(idShader, 1, &src, nullptr);
    glCompileShader(idShader);

    return idShader;
}

//reads in shaders from file and returns struct consisting of vertex and fragment shader source code
ShaderProgramSource Shader::ParseShader(const std::string& filepath) {
        
    std::ifstream stream(filepath);

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

