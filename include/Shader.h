#ifndef SHADER_H
#define SHADER_H

#include <string>
// Before: #include <glad/glad.h>
#include "glad/glad.h" // <-- Use quotes// Include GLAD before GLM
#include <glm/glm.hpp>

class Shader {
public:
    // Constructor: Reads and builds the shader
    Shader(const std::string& vertexPath, const std::string& fragmentPath);
    // Destructor: Deletes the shader program
    ~Shader();

    // Activates the shader program
    void Use() const;

    // Utility uniform functions
    void SetBool(const std::string& name, bool value) const;
    void SetInt(const std::string& name, int value) const;
    void SetFloat(const std::string& name, float value) const;
    void SetMat4(const std::string& name, const glm::mat4& mat) const;
    void SetVec3(const std::string& name, const glm::vec3& value) const;
     // Add more setters as needed (Vec2, Vec4, Mat3 etc.)

    GLuint GetProgramID() const { return m_ProgramID; }

private:
    GLuint m_ProgramID = 0; // Handle to the shader program

    // Utility function for checking shader compilation/linking errors.
    static void CheckCompileErrors(GLuint shader, std::string type);
};

#endif // SHADER_H