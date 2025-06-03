// src/Shader.cpp

#include "Shader.h"      // Header for this implementation file
#include "FileUtils.h"   // Needed for FileUtils::ReadFile

#include <glad/glad.h>   // Needed for GL types (GLuint, GLint) and functions
#include <glm/gtc/type_ptr.hpp> // Needed for glm::value_ptr

#include <string>        // Needed for std::string parameter in CheckCompileErrors
#include <iostream>      // Needed for std::cout, std::cerr, std::endl
#include <vector>        // Not directly used by implementation, but often included with GLM/OpenGL
// #include <glm/glm.hpp> // Already included via Shader.h -> glm/glm.hpp

// --- Helper Function Declarations (within Shader.cpp) ---
// ... (rest of the file remains the same) ...


// --- Helper Function Declarations (within Shader.cpp) ---
namespace { // Use an anonymous namespace for internal linkage
    GLuint CompileShader(GLenum type, const char* source, const std::string& shaderName);
    GLuint LinkProgram(GLuint vertexShader, GLuint fragmentShader);
}

// --- Shader Class Implementation ---

Shader::Shader(const std::string& vertexPath, const std::string& fragmentPath) {
    // 1. Retrieve the vertex/fragment source code from filePath
    std::string vertexCode = FileUtils::ReadFileToString(vertexPath);   // <-- Use ReadFileToString
    std::string fragmentCode = FileUtils::ReadFileToString(fragmentPath); // <-- Use ReadFileToString
    if (vertexCode.empty() || fragmentCode.empty()) {
        // Error message already printed by ReadFile
        m_ProgramID = 0; // Indicate failure
        return;
    }
    const char* vShaderCode = vertexCode.c_str();
    const char* fShaderCode = fragmentCode.c_str();

    // 2. Compile shaders
    GLuint vertex = CompileShader(GL_VERTEX_SHADER, vShaderCode, vertexPath);
    GLuint fragment = CompileShader(GL_FRAGMENT_SHADER, fShaderCode, fragmentPath);

    if (vertex == 0 || fragment == 0) {
         m_ProgramID = 0; // Indicate failure
         // Cleanup potentially created shader object
         if (vertex != 0) glDeleteShader(vertex);
         if (fragment != 0) glDeleteShader(fragment);
         return;
    }

    // 3. Link shader Program
    m_ProgramID = LinkProgram(vertex, fragment);

    // Delete the shaders as they're linked into our program now and no longer necessary
    glDeleteShader(vertex);
    glDeleteShader(fragment);

    if (m_ProgramID != 0) {
        std::cout << "INFO::SHADER::Program linked successfully (ID: " << m_ProgramID << ")" << std::endl;
    }
}

Shader::~Shader() {
    if (m_ProgramID != 0) {
        glDeleteProgram(m_ProgramID);
         std::cout << "INFO::SHADER::Program deleted (ID: " << m_ProgramID << ")" << std::endl;
    }
}

void Shader::Use() const {
    if (m_ProgramID != 0) {
        glUseProgram(m_ProgramID);
    } else {
        // Maybe log a warning here if trying to use an invalid shader
    }
}

// --- Uniform Setters ---
// Cache uniform locations in a map for performance if needed later

void Shader::SetBool(const std::string& name, bool value) const {
    if (m_ProgramID == 0) return;
    glUniform1i(glGetUniformLocation(m_ProgramID, name.c_str()), (int)value);
}

void Shader::SetInt(const std::string& name, int value) const {
     if (m_ProgramID == 0) return;
    glUniform1i(glGetUniformLocation(m_ProgramID, name.c_str()), value);
}

void Shader::SetFloat(const std::string& name, float value) const {
    if (m_ProgramID == 0) return;
    glUniform1f(glGetUniformLocation(m_ProgramID, name.c_str()), value);
}

void Shader::SetMat4(const std::string& name, const glm::mat4& mat) const {
    if (m_ProgramID == 0) return;
    glUniformMatrix4fv(glGetUniformLocation(m_ProgramID, name.c_str()), 1, GL_FALSE, glm::value_ptr(mat));
}
void Shader::SetVec3(const std::string& name, const glm::vec3& value) const {
    if (m_ProgramID == 0) return;
    glUniform3fv(glGetUniformLocation(m_ProgramID, name.c_str()), 1, glm::value_ptr(value));
}


// --- Static Error Checking Method ---
void Shader::CheckCompileErrors(GLuint shader, std::string type) {
    GLint success;
    GLchar infoLog[1024];
    if (type != "PROGRAM") {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(shader, 1024, NULL, infoLog);
            std::cerr << "ERROR::SHADER::COMPILATION_FAILED of type: " << type << "\n" << infoLog
                      << "\n -- --------------------------------------------------- -- " << std::endl;
        }
    } else {
        glGetProgramiv(shader, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(shader, 1024, NULL, infoLog);
            std::cerr << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog
                      << "\n -- --------------------------------------------------- -- " << std::endl;
        }
    }
}


// --- Helper Function Implementations (within anonymous namespace) ---
namespace {

GLuint CompileShader(GLenum type, const char* source, const std::string& shaderName) {
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, NULL);
    glCompileShader(shader);

    // Check for compile errors
    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        GLchar infoLog[1024];
        glGetShaderInfoLog(shader, 1024, NULL, infoLog);
        std::cerr << "ERROR::SHADER::COMPILATION_FAILED (" << shaderName << ")\n" << infoLog << std::endl;
        glDeleteShader(shader); // Don't leak the shader
        return 0;
    }
     std::cout << "INFO::SHADER::Compiled successfully (" << shaderName << ")" << std::endl;
    return shader;
}

GLuint LinkProgram(GLuint vertexShader, GLuint fragmentShader) {
    GLuint program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);

    // Check for linking errors
    GLint success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        GLchar infoLog[1024];
        glGetProgramInfoLog(program, 1024, NULL, infoLog);
        std::cerr << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
        glDeleteProgram(program); // Don't leak the program
        return 0;
    }
    return program;
}

} // end anonymous namespace