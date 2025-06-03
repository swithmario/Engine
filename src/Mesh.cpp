// src/Mesh.cpp

#include "Mesh.h"       // Include the header for this implementation file
#include <glad/glad.h>  // Include GLAD for OpenGL functions
#include <iostream>     // For logging output (optional)
#include <cstddef>      // For offsetof macro

// Constructor: Takes vertex data and indices, initializes index count, calls setup
Mesh::Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices) {
    // Basic validation
    if (vertices.empty()) { // Indices can technically be empty for glDrawArrays, but usually not for Mesh class
        std::cerr << "ERROR::MESH::Cannot create mesh with empty vertices." << std::endl;
        // Initialize members to zero to indicate an invalid state
        m_VAO = 0; m_VBO = 0; m_EBO = 0; m_IndexCount = 0;
        return;
    }
    // We'll always use indices with this Mesh class design
    if (indices.empty()) {
         std::cerr << "ERROR::MESH::Cannot create mesh with empty indices (use glDrawElements)." << std::endl;
         m_VAO = 0; m_VBO = 0; m_EBO = 0; m_IndexCount = 0;
         return;
    }

    m_IndexCount = static_cast<GLsizei>(indices.size());
    SetupMesh(vertices, indices); // Call the private setup function
}

// Destructor: Cleans up OpenGL buffer objects and vertex array object
Mesh::~Mesh() {
    // Check if IDs are valid before deleting
    if (m_VBO != 0) {
        glDeleteBuffers(1, &m_VBO);
        // std::cout << "INFO::MESH::Deleted VBO (ID: " << m_VBO << ")" << std::endl; // Optional log
    }
    if (m_EBO != 0) {
        glDeleteBuffers(1, &m_EBO);
        // std::cout << "INFO::MESH::Deleted EBO (ID: " << m_EBO << ")" << std::endl; // Optional log
    }
    if (m_VAO != 0) {
        glDeleteVertexArrays(1, &m_VAO);
        // std::cout << "INFO::MESH::Deleted VAO (ID: " << m_VAO << ")" << std::endl; // Optional log
    }
}

// SetupMesh: Configures the VAO, VBO, EBO, and vertex attributes
void Mesh::SetupMesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices) {
    // 1. Create buffers/arrays
    glGenVertexArrays(1, &m_VAO);
    glGenBuffers(1, &m_VBO);
    glGenBuffers(1, &m_EBO);

    // 2. Bind the Vertex Array Object first
    glBindVertexArray(m_VAO);

    // 3. Bind and load vertex data into Vertex Buffer Object (VBO)
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    // Use vector's size() and data() for buffer size and data pointer
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

    // 4. Bind and load index data into Element Buffer Object (EBO)
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
    // Use vector's size() and data() for buffer size and data pointer
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    // 5. Set the vertex attribute pointers
    // Position attribute (location = 0)
    glEnableVertexAttribArray(0);
    //                   index, size, type,      normalized, stride,         pointer offset
    glVertexAttribPointer(0,     3,    GL_FLOAT, GL_FALSE,   sizeof(Vertex), (void*)offsetof(Vertex, Position));

    // Normal attribute (location = 1)
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1,     3,    GL_FLOAT, GL_FALSE,   sizeof(Vertex), (void*)offsetof(Vertex, Normal));

    // Texture coordinate attribute (location = 2)
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2,     2,    GL_FLOAT, GL_FALSE,   sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));

    // 6. Unbind the VAO (NOT the EBO, VAO retains the EBO binding)
    glBindVertexArray(0);
    // Can also unbind the GL_ARRAY_BUFFER, though the VAO state takes precedence when VAO is bound
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // std::cout << "INFO::MESH::Setup complete (VAO: " << m_VAO << ", Verts: " << vertices.size() << ", Indices: " << m_IndexCount << ")" << std::endl; // Optional log
}

// Bind: Binds the Vertex Array Object for rendering
void Mesh::Bind() const {
    // Only bind if VAO is valid
    if (m_VAO != 0) {
        glBindVertexArray(m_VAO);
    } else {
        std::cerr << "WARN::MESH::Attempting to bind invalid mesh VAO." << std::endl;
    }
}

// Unbind: Unbinds the Vertex Array Object
void Mesh::Unbind() const {
    // It's standard practice to unbind VAO 0, effectively unbinding any current VAO
    glBindVertexArray(0);
}

// Draw: Renders the mesh using its indices
void Mesh::Draw() const {
    // Check if VAO and index count are valid before drawing
    if (m_VAO != 0 && m_IndexCount > 0) {
        // Assumes VAO is already bound via Mesh::Bind() before calling Draw()
        // Draw using the indices stored in the EBO bound to the VAO
        glDrawElements(GL_TRIANGLES,        // Mode
                       m_IndexCount,        // Count of indices
                       GL_UNSIGNED_INT,     // Type of indices
                       0);                  // Offset (usually 0)
    } else {
        if (m_VAO == 0) std::cerr << "WARN::MESH::Attempting to draw invalid mesh VAO." << std::endl;
        if (m_IndexCount == 0) std::cerr << "WARN::MESH::Attempting to draw mesh with zero indices." << std::endl;
    }
}