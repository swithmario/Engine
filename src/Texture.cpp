#include "Texture.h"
#include "stb_image.h" // Use stb_image for loading
#include <iostream>

Texture::Texture() : m_TextureID(0), m_Width(0), m_Height(0), m_Channels(0) {}

Texture::~Texture() {
    if (m_TextureID != 0) {
        glDeleteTextures(1, &m_TextureID);
        std::cout << "INFO::TEXTURE::Deleted texture (ID: " << m_TextureID << ")" << std::endl;
    }
}

bool Texture::Load(const std::string& filePath) {
    // Load image data using stb_image
    stbi_set_flip_vertically_on_load(true); // Flip UVs for OpenGL convention
    unsigned char* data = stbi_load(filePath.c_str(), &m_Width, &m_Height, &m_Channels, 0);
    if (!data) {
        std::cerr << "ERROR::TEXTURE::Failed to load texture file: " << filePath << std::endl;
        stbi_image_free(data); // Free data even if null, just in case
        return false;
    }
     std::cout << "INFO::TEXTURE::Loaded texture file: " << filePath << " (" << m_Width << "x" << m_Height << ", " << m_Channels << " channels)" << std::endl;


    // Determine OpenGL format based on channels
    GLenum internalFormat = GL_RGB8; // Default
    GLenum dataFormat = GL_RGB;
    if (m_Channels == 4) {
        internalFormat = GL_RGBA8;
        dataFormat = GL_RGBA;
    } else if (m_Channels == 3) {
        internalFormat = GL_RGB8;
        dataFormat = GL_RGB;
    } else if (m_Channels == 1) {
        internalFormat = GL_R8;
        dataFormat = GL_RED;
    } else {
        std::cerr << "ERROR::TEXTURE::Unsupported number of channels: " << m_Channels << " in file: " << filePath << std::endl;
        stbi_image_free(data);
        return false;
    }


    // Generate and configure OpenGL texture
    glGenTextures(1, &m_TextureID);
    glBindTexture(GL_TEXTURE_2D, m_TextureID);

    // Set texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // Set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); // Use mipmaps for minification
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Upload texture data
    glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, m_Width, m_Height, 0, dataFormat, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D); // Generate mipmaps

    // Free image data from CPU memory
    stbi_image_free(data);
    glBindTexture(GL_TEXTURE_2D, 0); // Unbind

    std::cout << "INFO::TEXTURE::Created OpenGL texture (ID: " << m_TextureID << ")" << std::endl;
    return true;
}

void Texture::Bind(unsigned int unit) const {
    if (m_TextureID != 0) {
        // Ensure unit is within reasonable bounds (e.g., 0-31)
        if (unit >= GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS) {
             std::cerr << "WARN::TEXTURE::Texture unit " << unit << " might be invalid." << std::endl;
        }
        glActiveTexture(GL_TEXTURE0 + unit);
        glBindTexture(GL_TEXTURE_2D, m_TextureID);
    }
}

void Texture::Unbind() const {
     // Note: Unbinding GL_TEXTURE_2D usually binds texture 0, which is fine
     // You might need more specific unbinding if using multiple texture types
     glBindTexture(GL_TEXTURE_2D, 0);
}