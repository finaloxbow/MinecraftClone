#ifndef Shader_H
#define Shader_H

#include <glad/glad.h>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <unordered_map>

#include "glm/glm.hpp"

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
    unsigned int m_RendererID;
    std::string m_FilePath;
    std::unordered_map<std::string, int> m_UniformLocationCache;
    
    //creates/destroys Shader Program
    Shader(const std::string& shaderFilePath);
    ~Shader();

    

    //bind shaders to use in program
    void Bind() const;
    void Unbind() const;

    //set uniforms
    void SetUniform4f(const std::string& name, float v0, float v1, float v2, float v3);
    void SetUniform1i(const std::string& name, int i);
    void SetUniformMat4f(const std::string& name, const glm::mat4 matrix);

private:
    //gets the location of the uniform
    int GetUniformLocation(const std::string& name);

    //helper function to compile a shader of some type
    static unsigned int CompileShader(unsigned int type, const std::string& source);

    //reads in shaders from file and returns struct consisting of vertex and fragment shader source code
    static ShaderProgramSource ParseShader(const std::string& filepath);
    
    //creates shader program
    unsigned int CreateShader(const std::string& vertexShader, const std::string& fragmentShader);

};

#endif
