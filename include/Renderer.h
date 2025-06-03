// include/Renderer.h
#ifndef RENDERER_H
#define RENDERER_H

struct SDL_Window; typedef void* SDL_GLContext;
#include <glm/glm.hpp>

// Forward declare classes used by pointer/reference
class Shader;
class Mesh; // <-- Forward declare Mesh

class Renderer {
public:
    Renderer(); ~Renderer();
    bool Initialize(SDL_Window* window); void Shutdown();
    void Clear() const;
    // Change parameter type to Mesh
    void PrepareDraw(const Shader& shader, const Mesh& mesh, const glm::mat4& mvpMatrix); // <-- Change type
    void DrawPrepared() const;
    void Present(SDL_Window* window);
    SDL_GLContext GetGLContext() const { return m_Context; }
    Renderer(const Renderer&) = delete; Renderer& operator=(const Renderer&) = delete; Renderer(Renderer&&) = delete; Renderer& operator=(Renderer&&) = delete;
private:
    SDL_GLContext m_Context = nullptr;
    const Shader* m_CurrentShader = nullptr;
    const Mesh* m_CurrentMesh = nullptr; // <-- Change type and name
};
#endif // RENDERER_H