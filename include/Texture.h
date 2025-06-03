#ifndef TEXTURE_H
#define TEXTURE_H

#include <glad/glad.h>
#include <string>

class Texture {
public:
    Texture();
    ~Texture();

    // Load texture from file
    bool Load(const std::string& filePath);

    // Bind texture to a specific texture unit (e.g., 0 for GL_TEXTURE0)
    void Bind(unsigned int unit = 0) const;
    void Unbind() const; // Unbind from currently active unit

    GLuint GetID() const { return m_TextureID; }

    // Disable copy/move for simplicity
    Texture(const Texture&) = delete;
    Texture& operator=(const Texture&) = delete;
    Texture(Texture&&) = delete;
    Texture& operator=(Texture&&) = delete;

private:
    GLuint m_TextureID = 0;
    int m_Width = 0;
    int m_Height = 0;
    int m_Channels = 0;
};

#endif // TEXTURE_H