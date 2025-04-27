#include <sstream>
#include <iostream>
#include <fstream>
#define print(x) std::cout << x << std::endl


class Shader {
public:
    unsigned int programId;

    Shader(const char* vertexPath, const char* fragmentPath) {
        // 1. Read shader files
        std::string vertexCode;
        std::string fragmentCode;
        std::ifstream vShaderFile;
        std::ifstream fShaderFile;

        vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

        try {
            // Open files
            vShaderFile.open(vertexPath);
            fShaderFile.open(fragmentPath);
            
            std::stringstream vShaderStream, fShaderStream;
            
            vShaderStream << vShaderFile.rdbuf(); // Fixed typo: rdbf → rdbuf
            fShaderStream << fShaderFile.rdbuf();
            
            vShaderFile.close();
            fShaderFile.close();
            
            vertexCode = vShaderStream.str();
            fragmentCode = fShaderStream.str();
        }
        catch(std::ifstream::failure& e) {
            print("ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ");
        }

        const char* vShaderCode = vertexCode.c_str();
        const char* fShaderCode = fragmentCode.c_str(); // Fixed: was shaderCode

        // 2. Compile shaders
        unsigned int vertexId, fragmentId;
        int success;
        char infoLog[512];

        // Vertex shader
        vertexId = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertexId, 1, &vShaderCode, NULL);
        glCompileShader(vertexId);
        glGetShaderiv(vertexId, GL_COMPILE_STATUS, &success); // Fixed: STATUS not SUCCESS
        if(!success) {
            glGetShaderInfoLog(vertexId, 512, NULL, infoLog);
            print("ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog);
        }

        // Fragment shader - Fixed: was GL_VERTEX_SHADER
        fragmentId = glCreateShader(GL_FRAGMENT_SHADER); 
        glShaderSource(fragmentId, 1, &fShaderCode, NULL);
        glCompileShader(fragmentId);
        glGetShaderiv(fragmentId, GL_COMPILE_STATUS, &success); // Fixed: comma not dot
        if(!success) {
            glGetShaderInfoLog(fragmentId, 512, NULL, infoLog);
            print("ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog);
        }

        // Shader program
        programId = glCreateProgram();
        glAttachShader(programId, vertexId);
        glAttachShader(programId, fragmentId);
        glLinkProgram(programId);
        glGetProgramiv(programId, GL_LINK_STATUS, &success);
        if(!success) {
            glGetProgramInfoLog(programId, 512, NULL, infoLog);
            print("ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog);
        }
        
        glDeleteShader(vertexId);
        glDeleteShader(fragmentId);
    }

    void use() {
        glUseProgram(programId);
    }

    void setFloat(const std::string &name, float value) const {
        glUniform1f(glGetUniformLocation(programId, name.c_str()), value);
    }

    void setInt(const std::string &name, int value) const {
        glUniform1i(glGetUniformLocation(programId, name.c_str()), value);
    }
};