#ifndef APPLICATION_H
#define APPLICATION_H

#include <SDL2/SDL.h>
#include <memory>
#include <glm/glm.hpp>

// Include new class headers
#include "Mesh.h"
#include "Texture.h"

// Forward declarations
class Renderer;
class Shader;
// Removed forward decl for Vertex as we use Mesh now
typedef struct Mix_Chunk Mix_Chunk;

enum class GameState {
    Playing,
    Paused,
    ShowingHelp
};

class Application {
public:
    Application();
    ~Application();

    bool Initialize();
    void Run();
    void Shutdown();

    Application(const Application&) = delete;
    Application& operator=(const Application&) = delete;
    Application(Application&&) = delete;
    Application& operator=(Application&&) = delete;

private:
    void ProcessEvents();
    void HandleKeyboardInput(const Uint8* keyboardState, float deltaTime);
    void HandleMouseInput(float xoffset, float yoffset);
    void Update(float deltaTime);
    void Render();
    void RenderUI();

    // --- Core Components ---
    SDL_Window* m_Window = nullptr;
    std::unique_ptr<Renderer> m_Renderer;

    // --- Scene / Game Objects ---
    // Renamed shader, added Mesh and Texture
    std::unique_ptr<Shader> m_LitTexturedShader;
    std::unique_ptr<Mesh> m_LoadedMesh;
    std::unique_ptr<Texture> m_DiffuseTexture;

    // --- Camera State ---
    glm::vec3 m_CameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
    glm::vec3 m_CameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 m_CameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
    float m_CameraYaw = -90.0f;
    float m_CameraPitch = 0.0f;
    float m_CameraBaseSpeed = 2.5f;
    float m_CameraSpeedMultiplier = 2.0f;
    float m_MouseSensitivity = 0.1f;
    bool m_FirstMouse = true;
    float m_LastMouseX = 0.0f;
    float m_LastMouseY = 0.0f;

    // --- State ---
    GameState m_CurrentState = GameState::Playing;
    bool m_IsRunning = false;
    Uint64 m_TickCountLast = 0;
    float m_RotationAngle = 0.0f; // Keep object rotation for now


    bool m_MixerInitialized = false;
    bool m_SoundLoaded = false;
    int m_MusicChannel = -1; // <-- ADD THIS LINE (-1 means not playing)
    Mix_Chunk* m_TestSound = nullptr;
    bool LoadAudio();
    void CloseAudio();
    void PlaySound();
    void RestartAudio();
};

#endif // APPLICATION_H