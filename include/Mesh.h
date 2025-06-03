// include/Mesh.h
#ifndef MESH_H
#define MESH_H
#include "VertexArray.h" // <-- Includes Vertex struct now
#include <glad/glad.h>
#include <vector>
#include <cstddef>
class Mesh {
public:
    Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices);
    ~Mesh();
    void Bind() const;
    void Unbind() const;
    void Draw() const;
    Mesh(const Mesh&) = delete; Mesh& operator=(const Mesh&) = delete; Mesh(Mesh&&) = delete; Mesh& operator=(Mesh&&) = delete;
private:
    GLuint m_VAO = 0, m_VBO = 0, m_EBO = 0; GLsizei m_IndexCount = 0;
    void SetupMesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices);
};
#endif // MESH_H