#include <glad/glad.h>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include "Shader.h"


//creates Shader Program
Shader::Shader(const std::string& shaderFilePath)
    : m_FilePath(shaderFilePath), m_RendererID(0)
{
    //create shader file path var as arg to replace the two curr args
    ShaderProgramSource source = ParseShader(shaderFilePath);
    
    //prints out shader source file
    std::cout << "----------Vertex Shader------------------" << std::endl;
    std::cout << source.VertexSource << std::endl;
    std::cout << "--------- Fragment Shader ---------------" << std::endl;
    std::cout << source.FragmentSource << std::endl;

    m_RendererID = CreateShader(source.VertexSource, source.FragmentSource);
}

Shader::~Shader()
{
    glDeleteProgram(m_RendererID);
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

//creates the shader program
unsigned int Shader::CreateShader(const std::string& vertexShader, const std::string& fragmentShader)
{
    //creates program and compiles shaders from their source code
    unsigned int program = glCreateProgram();
    unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
    unsigned int fs = CompileShader(GL_FRAGMENT_SHADER,fragmentShader);

    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);
    glValidateProgram(program);

    //shader cleanup
    glDeleteShader(vs);
    glDeleteShader(fs);

    return program;
}

void Shader::Bind() const
{
    glUseProgram(m_RendererID);
}

void Shader::Unbind() const
{
    glUseProgram(0);
}

void Shader::SetUniform4f(const std::string& name, float v0, float v1, float v2, float v3)
{
    glUniform4f(GetUniformLocation(name), v0, v1, v2, v3);
}

void Shader::SetUniform1i(const std::string& name, int i)
{
    glUniform1i(GetUniformLocation(name), i);
}

int Shader::GetUniformLocation(const std::string& name)
{
    if (m_UniformLocationCache.find(name) != m_UniformLocationCache.end())
        return m_UniformLocationCache[name];

    int location = glGetUniformLocation(m_RendererID, name.c_str());
    if (location == -1)
        std::cout << "Warning: uniform '" << name << "' doesn't exist." << std::endl;
        
    m_UniformLocationCache[name] = location;
    return location;
}

