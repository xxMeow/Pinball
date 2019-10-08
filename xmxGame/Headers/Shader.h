//
//  Shader.h
//  xmxGame
//
//  Created by XMX on 2019/09/16.
//  Copyright © 2019 XMX. All rights reserved.
//

#ifndef Shader_h
#define Shader_h

#include <glad/glad.h>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include <unistd.h> // To use getcwd()

class Shader
{
public:
    // ID of program
    unsigned int ID;
    
    // Read file and construct shader program
    Shader(const char *vsFilePath, const char *fsFilePath)
    {
        /** 1. Read file **/
        
        std::ifstream vsFile, fsFile;
        // Make sure that ifstream object can throw exceptions
        vsFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);
        fsFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);
        
        std::string vsSrc, fsSrc;
        try {
            // XCode may have special working directory, check it
            char currPath[256];
            char *currPathPtr = getcwd(currPath, sizeof(currPath));
            if (currPathPtr) {
                std::cout << "Working at: " << currPath << std::endl;
            }
            // Open file
            vsFile.open(vsFilePath);
            fsFile.open(fsFilePath);
            std::stringstream vsStream, fsStream;
            // Read file to data stream
            vsStream << vsFile.rdbuf();
            fsStream << fsFile.rdbuf();
            // Close file processor
            vsFile.close();
            fsFile.close();
            // Convert data stream to string
            vsSrc = vsStream.str();
            fsSrc = fsStream.str();
        } catch (std::ifstream::failure e) {
            std::cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ" << std::endl;
        }
        
        const char *vsCode = vsSrc.c_str();
        const char *fsCode = fsSrc.c_str();
        
        /** 2. Compile shader **/
        
        // Compile info
        int cFlag; // If compile successed
        char cLog[512]; // Compile log
        
        unsigned int vs, fs; // Shader ID
        
        // Vertex shader
        vs = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vs, 1, &vsCode, NULL);
        glCompileShader(vs);
        // Check for compilation error
        glGetShaderiv(vs, GL_COMPILE_STATUS, &cFlag);
        if (!cFlag) {
            glGetShaderInfoLog(vs, 512, NULL, cLog);
            std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << cLog << std::endl;
        }
        
        // Fragment shader
        fs = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fs, 1, &fsCode, NULL);
        glCompileShader(fs);
        // Check for compilation error
        glGetShaderiv(fs, GL_COMPILE_STATUS, &cFlag);
        if (!cFlag) {
            glGetShaderInfoLog(fs, 512, NULL, cLog);
            std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << cLog << std::endl;
        }
        
        // Shader program
        ID = glCreateProgram();
        glAttachShader(ID, vs);
        glAttachShader(ID, fs);
        glLinkProgram(ID);
        // Check for linking error
        glGetProgramiv(ID, GL_LINK_STATUS, &cFlag);
        if (!cFlag) {
            glGetProgramInfoLog(ID, 512, NULL, cLog);
            std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << cLog << std::endl;
        }
        
        // Clean linked shaders (What we actually need is the shader program)
        glDeleteShader(vs);
        glDeleteShader(fs);
    }
    
    /** Enable program **/
    void use() {
        glUseProgram(ID);
    }
    
    /** Uniform tool functions: Search the location of a uniform by its name and set it as value **/
    // glUniform — specify the value of a uniform variable for the current program object
    // GLint glGetUniformLocation(GLuint program, const GLchar *name);
    void setBool(const std::string &name, bool value) const {
        glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
    }
    void setInt(const std::string &name, int value) const {
        glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
    }
    void setFloat(const std::string &name, float value) const {
        glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
    }
    void setVec2(const std::string &name, const glm::vec2 &value) const {
        glUniform2fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
    }
    void setVec2(const std::string &name, float x, float y) const {
        glUniform2f(glGetUniformLocation(ID, name.c_str()), x, y);
    }
    void setVec3(const std::string &name, const glm::vec3 &value) const {
        glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
    }
    void setVec3(const std::string &name, float x, float y, float z) const {
        glUniform3f(glGetUniformLocation(ID, name.c_str()), x, y, z);
    }
    void setVec4(const std::string &name, const glm::vec4 &value) const {
        glUniform4fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
    }
    void setVec4(const std::string &name, float x, float y, float z, float w) const {
        glUniform4f(glGetUniformLocation(ID, name.c_str()), x, y, z, w);
    }
    void setMat2(const std::string &name, const glm::mat2 &mat) const {
        glUniformMatrix2fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }
    void setMat3(const std::string &name, const glm::mat3 &mat) const {
        glUniformMatrix3fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }
    void setMat4(const std::string &name, const glm::mat4 &mat) const {
        glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }
};

#endif /* Shader_h */
