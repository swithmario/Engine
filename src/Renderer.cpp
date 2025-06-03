#include "Renderer.h"
#include "Shader.h"
#include "Mesh.h" // <-- Include Mesh

#include <SDL2/SDL.h>
#include <glad/glad.h>
#include <SDL2/SDL_opengl.h>
#include <iostream>

Renderer::Renderer() : m_Context(nullptr) {}

Renderer::~Renderer() {
    // Ensure Shutdown is called, although Application should manage this
    Shutdown();
}

bool Renderer::Initialize(SDL_Window* window) {
    if (!window) {
        std::cerr << "ERROR::RENDERER::Window pointer is null." << std::endl;
        return false;
    }

    // --- Set OpenGL Attributes (MUST be done BEFORE creating context) ---
    // Use OpenGL 3.3 Core profile (or higher specified in GLAD generation)
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1); // Enable double buffering
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);  // Depth buffer

    // --- Create OpenGL Context ---
    m_Context = SDL_GL_CreateContext(window);
    if (!m_Context) {
        std::cerr << "ERROR::RENDERER::SDL_GL_CreateContext failed: " << SDL_GetError() << std::endl;
        return false;
    }
    std::cout << "INFO::RENDERER::OpenGL context created." << std::endl;

    // --- Initialize GLAD ---
    if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) {
        std::cerr << "ERROR::RENDERER::Failed to initialize GLAD." << std::endl;
        SDL_GL_DeleteContext(m_Context); // Clean up context if GLAD fails
        m_Context = nullptr;
        return false;
    }
    std::cout << "INFO::RENDERER::GLAD initialized." << std::endl;
    std::cout << "INFO::RENDERER::OpenGL Version: " << glGetString(GL_VERSION) << std::endl;
    std::cout << "INFO::RENDERER::GLSL Version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
    std::cout << "INFO::RENDERER::Vendor: " << glGetString(GL_VENDOR) << std::endl;
    std::cout << "INFO::RENDERER::Renderer: " << glGetString(GL_RENDERER) << std::endl;


    // --- Basic GL Setup ---
    glEnable(GL_DEPTH_TEST); // Enable depth testing
    // Enable Culling later if needed:
    // glEnable(GL_CULL_FACE);
    // glCullFace(GL_BACK);
    // glFrontFace(GL_CCW); // Standard counter-clockwise winding order

    // Set initial clear color (background) - Dark blue-grey
    glClearColor(0.235f, 0.235f, 0.353f, 1.0f);

    // Set initial viewport (can be updated on window resize)
    int width, height;
    SDL_GetWindowSize(window, &width, &height);
    glViewport(0, 0, width, height);


    // Enable VSync (optional, but good for reducing tearing)
    if (SDL_GL_SetSwapInterval(1) < 0) {
        std::cout << "WARN::RENDERER: Unable to set VSync! SDL Error: " << SDL_GetError() << std::endl;
    }


    return true;
}

void Renderer::Shutdown() {
    if (m_Context) {
        SDL_GL_DeleteContext(m_Context);
        m_Context = nullptr;
        std::cout << "INFO::RENDERER::OpenGL context destroyed." << std::endl;
    }
}

void Renderer::Clear() const {
    // Clear the color buffer and depth buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

// Change parameter type to Mesh
void Renderer::PrepareDraw(const Shader& shader, const Mesh& mesh, const glm::mat4& mvpMatrix) {
    shader.Use();
    m_CurrentShader = &shader;

    // Set MVP uniform (assuming shader has SetMat4)
    shader.SetMat4("uMVP", mvpMatrix); // Assuming PrepareDraw still sets MVP

    // Bind the Mesh's VAO
    mesh.Bind();            // <-- Call Mesh::Bind()
    m_CurrentMesh = &mesh; // <-- Store pointer to Mesh
}

void Renderer::DrawPrepared() const {
    if (m_CurrentMesh) {
         m_CurrentMesh->Draw(); // <-- Call Mesh::Draw()
    } else {
        std::cerr << "WARN::RENDERER::DrawPrepared called without a prepared mesh." << std::endl;
    }
}


void Renderer::Present(SDL_Window* window) {
    if (window && m_Context) {
       SDL_GL_SwapWindow(window);
       m_CurrentShader = nullptr;
       m_CurrentMesh = nullptr; // <-- Reset m_CurrentMesh
    }
}