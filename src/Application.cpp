// src/Application.cpp
#include "Application.h" // Includes forward declarations
#include "Renderer.h"    // <-- ADD/ENSURE THIS (Provides full Renderer definition)
#include "Shader.h"
#include "FileUtils.h"
#include "Mesh.h"        // <-- ADD/ENSURE THIS (Provides full Mesh definition)
#include "Texture.h"
#include "VertexArray.h" // For Vertex struct definition

// ImGui Includes
#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_opengl3.h"

// Standard/Lib Includes
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_mouse.h>
#include <iostream>
#include <fstream> // <-- ADD THIS LINE
#include <memory>
#include <vector>
#include <cmath>

// GLM
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// --- Constants ---
const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;
const char* WINDOW_TITLE = "Model Viewer!";
const float OBJECT_ROTATION_SPEED = 0.5f; // Radians per second

Application::Application() :
    m_Window(nullptr),
    m_Renderer(nullptr),
    m_LitTexturedShader(nullptr), // Renamed from m_SimpleShader
    m_LoadedMesh(nullptr),        // Initialize new pointers
    m_DiffuseTexture(nullptr),
    m_IsRunning(false),
    m_TickCountLast(0),
    m_RotationAngle(0.0f),
    m_TestSound(nullptr),
    m_MixerInitialized(false),
    m_SoundLoaded(false),
    m_MusicChannel(-1), // <-- ADD THIS
    m_CurrentState(GameState::Playing)
{}

Application::~Application() {
    Shutdown();
}

bool Application::Initialize() {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0) {
        std::cerr << "ERROR::APP::SDL_Init failed: " << SDL_GetError() << std::endl;
        return false;
    }
    std::cout << "INFO::APP::SDL initialized." << std::endl;

    m_Window = SDL_CreateWindow(WINDOW_TITLE, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
    if (!m_Window) { std::cerr << "ERROR::APP::Window creation failed: " << SDL_GetError() << std::endl; SDL_Quit(); return false; }
    std::cout << "INFO::APP::Window created." << std::endl;

    // --- Mouse Setup ---
    if (m_CurrentState == GameState::Playing) { SDL_SetRelativeMouseMode(SDL_TRUE); }
    int initialMouseX, initialMouseY;
    SDL_GetMouseState(&initialMouseX, &initialMouseY);
    m_LastMouseX = (float)initialMouseX;
    m_LastMouseY = (float)initialMouseY;
    m_FirstMouse = true;

    m_Renderer = std::make_unique<Renderer>();
    if (!m_Renderer->Initialize(m_Window)) { std::cerr << "ERROR::APP::Renderer init failed." << std::endl; SDL_DestroyWindow(m_Window); SDL_Quit(); return false; }
    std::cout << "INFO::APP::Renderer initialized." << std::endl;

    // --- Initialize ImGui ---
    IMGUI_CHECKVERSION(); ImGui::CreateContext(); ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();
    if (!m_Renderer || !m_Renderer->GetGLContext()) { std::cerr << "ERROR::APP::ImGui init failed - no renderer/context." << std::endl; return false; }
    ImGui_ImplSDL2_InitForOpenGL(m_Window, m_Renderer->GetGLContext());
    ImGui_ImplOpenGL3_Init("#version 330 core");
    std::cout << "INFO::APP::Dear ImGui initialized." << std::endl;

    // --- Load Shader ---
    // Construct full relative paths for each shader file
    std::string vertPath = FileUtils::GetResourcePath("shaders/lit_textured.vert");
    std::string fragPath = FileUtils::GetResourcePath("shaders/lit_textured.frag");

    // Check if paths were resolved
    if (vertPath.empty() || fragPath.empty()) {
        std::cerr << "ERROR::APP::Could not get shader resource path(s)." << std::endl;
        if (vertPath.empty()) std::cerr << "  - Vertex shader path failed." << std::endl;
        if (fragPath.empty()) std::cerr << "  - Fragment shader path failed." << std::endl;
        return false;
    }

    // Create the shader using the full paths
    m_LitTexturedShader = std::make_unique<Shader>(vertPath, fragPath);
    if (!m_LitTexturedShader || m_LitTexturedShader->GetProgramID() == 0) {
        std::cerr << "ERROR::APP::Failed to load or link lit_textured shader." << std::endl;
        // Shader constructor likely printed details already
        return false;
    }
    std::cout << "INFO::APP::Lit Textured Shader loaded." << std::endl;

    // --- Load Texture ---
    std::string textureFilename = "your_texture.png"; // <-- Ensure this file exists in assets/textures
    // Pass the full relative path to GetResourcePath
    std::string texturePath = FileUtils::GetResourcePath("assets/textures/" + textureFilename); // <-- CORRECT PATH CONSTRUCTION
    if (texturePath.empty()) { std::cerr << "ERROR::APP::Could not get texture path for: " << textureFilename << std::endl; return false; } // Improved error message
    m_DiffuseTexture = std::make_unique<Texture>();
    if (!m_DiffuseTexture->Load(texturePath)) {
        std::cerr << "ERROR::APP::Failed to load texture: " << texturePath << std::endl;
        m_DiffuseTexture.reset(); // Release if failed
        // return false; // Decide if texture is mandatory
    } else {
        std::cout << "INFO::APP::Texture loaded: " << textureFilename << std::endl;
    }


    // --- Load Model ---
    std::string modelFilename = "monkey.obj"; // <-- Ensure this file exists in assets/models
    // Pass the full relative path to GetResourcePath
    std::string modelPath = FileUtils::GetResourcePath("assets/models/" + modelFilename); // <-- CORRECT PATH CONSTRUCTION
    if (modelPath.empty()) { std::cerr << "ERROR::APP::Could not get model path for: " << modelFilename << std::endl; return false; } // Improved error message

    std::vector<Vertex> loadedVertices;
    std::vector<unsigned int> loadedIndices;
    if (FileUtils::LoadObjModel(modelPath, loadedVertices, loadedIndices)) {
        m_LoadedMesh = std::make_unique<Mesh>(loadedVertices, loadedIndices);
        if (!m_LoadedMesh) {
             std::cerr << "ERROR::APP::Failed to create Mesh object from loaded data." << std::endl;
             return false;
        }
         std::cout << "INFO::APP::Model loaded and mesh created: " << modelFilename << std::endl;
    } else {
        // Error message from LoadObjModel is already printed
        // std::cerr << "ERROR::APP::Failed to load model: " << modelPath << std::endl; // Redundant
        return false; // Cannot continue without model
    }

    // --- Initialize Audio (Optional) ---
    if (!LoadAudio()) { std::cout << "WARN::APP::Audio failed to load." << std::endl; } else { PlaySound(); }

    m_IsRunning = true;
    m_TickCountLast = SDL_GetTicks64();
    return true;
}

void Application::Run() {
    // ... (Run loop logic remains the same) ...
     std::cout << "INFO::APP::Entering main loop..." << std::endl;
    while (m_IsRunning) {
        Uint64 tickCountNow = SDL_GetTicks64();
        float deltaTime = (tickCountNow - m_TickCountLast) / 1000.0f;
        m_TickCountLast = tickCountNow;
        deltaTime = (deltaTime > 0.1f) ? 0.1f : deltaTime; // Cap deltaTime

        ProcessEvents();
        // Only update game logic if playing
        if (m_CurrentState == GameState::Playing) {
            Update(deltaTime);
        }
        Render();
    }
     std::cout << "INFO::APP::Exited main loop." << std::endl;
}

void Application::ProcessEvents() {
    // ... (ProcessEvents logic remains the same, handling ImGui and state) ...
    SDL_PumpEvents();
    const Uint8* keyboardState = SDL_GetKeyboardState(NULL); // Get state for Update
    ImGuiIO& io = ImGui::GetIO();

    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        ImGui_ImplSDL2_ProcessEvent(&event);
        if (event.type == SDL_QUIT) m_IsRunning = false;
        // src/Application.cpp -> ProcessEvents() -> SDLK_ESCAPE block
        else if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE) {
            if (m_CurrentState == GameState::Playing) {
                m_CurrentState = GameState::Paused;
                SDL_SetRelativeMouseMode(SDL_FALSE); // Free mouse
                if (m_MusicChannel != -1) Mix_Pause(m_MusicChannel); // <-- PAUSE MUSIC
            } else if (m_CurrentState == GameState::Paused) {
                m_CurrentState = GameState::Playing;
                SDL_SetRelativeMouseMode(SDL_TRUE); // Capture mouse
                m_FirstMouse = true; // Reset first mouse flag
                if (m_MusicChannel != -1) Mix_Resume(m_MusicChannel); // <-- RESUME MUSIC
            } else if (m_CurrentState == GameState::ShowingHelp) {
                m_CurrentState = GameState::Paused; // Go back to pause menu
                // Music remains paused from when we entered Paused state before Help
            }
        }
        else if (m_CurrentState == GameState::Playing && !io.WantCaptureMouse && event.type == SDL_MOUSEMOTION) {
            if (m_FirstMouse) {
                int currentMouseX, currentMouseY; SDL_GetMouseState(&currentMouseX, &currentMouseY); // <-- FIX: Use &currentMouseX and &currentMouseY
                m_LastMouseX = (float)currentMouseX; m_LastMouseY = (float)currentMouseY;
                m_FirstMouse = false;
            }
             float xoffset = (float)event.motion.xrel; float yoffset = -(float)event.motion.yrel;
             HandleMouseInput(xoffset, yoffset);
        }
    }
}

void Application::HandleKeyboardInput(const Uint8* keyboardState, float deltaTime) {
    // ... (Keyboard input logic remains the same) ...
    float currentSpeed = m_CameraBaseSpeed;
    if (keyboardState[SDL_SCANCODE_LSHIFT] || keyboardState[SDL_SCANCODE_RSHIFT]) { currentSpeed *= m_CameraSpeedMultiplier; }
    float velocity = currentSpeed * deltaTime;
    if (keyboardState[SDL_SCANCODE_W]) m_CameraPos += m_CameraFront * velocity;
    if (keyboardState[SDL_SCANCODE_S]) m_CameraPos -= m_CameraFront * velocity;
    glm::vec3 worldUp = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::vec3 cameraRight = glm::normalize(glm::cross(m_CameraFront, worldUp));
    if (keyboardState[SDL_SCANCODE_A]) m_CameraPos -= cameraRight * velocity;
    if (keyboardState[SDL_SCANCODE_D]) m_CameraPos += cameraRight * velocity;
}

void Application::HandleMouseInput(float xoffset, float yoffset) {
    // ... (Mouse input logic remains the same) ...
    xoffset *= m_MouseSensitivity; yoffset *= m_MouseSensitivity;
    m_CameraYaw += xoffset; m_CameraPitch += yoffset;
    if (m_CameraPitch > 89.0f) m_CameraPitch = 89.0f; if (m_CameraPitch < -89.0f) m_CameraPitch = -89.0f;
    glm::vec3 front;
    front.x = cos(glm::radians(m_CameraYaw)) * cos(glm::radians(m_CameraPitch));
    front.y = sin(glm::radians(m_CameraPitch));
    front.z = sin(glm::radians(m_CameraYaw)) * cos(glm::radians(m_CameraPitch));
    m_CameraFront = glm::normalize(front);
}


void Application::Update(float deltaTime) {
    // Only update rotation if not paused
    if (m_CurrentState == GameState::Playing) {
        m_RotationAngle += OBJECT_ROTATION_SPEED * deltaTime;
        m_RotationAngle = fmod(m_RotationAngle, 2.0f * M_PI);

        const Uint8* keyboardState = SDL_GetKeyboardState(NULL);
        HandleKeyboardInput(keyboardState, deltaTime);
    }
}


void Application::Render() {
    // Start ImGui Frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();

    // Render 3D Scene
    m_Renderer->Clear();

    // Calculate View/Projection
    glm::vec3 worldUp = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::vec3 cameraRight = glm::normalize(glm::cross(m_CameraFront, worldUp));
    glm::vec3 cameraActualUp = glm::normalize(glm::cross(cameraRight, m_CameraFront));
    glm::mat4 view = glm::lookAt(m_CameraPos, m_CameraPos + m_CameraFront, cameraActualUp);
    glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT, 0.1f, 100.0f);

    // Draw the Loaded Model// Draw the Loaded Model
    if (m_LitTexturedShader && m_LoadedMesh && m_Renderer) {
        m_LitTexturedShader->Use(); // Activate the shader

        // Calculate model matrix (still rotating)
        glm::mat4 model = glm::rotate(glm::mat4(1.0f), m_RotationAngle, glm::vec3(0.0f, 1.0f, 0.0f));
        glm::mat4 mvp = projection * view * model;

        // Set common shader uniforms
        m_LitTexturedShader->SetMat4("uMVP", mvp);
        m_LitTexturedShader->SetMat4("uModel", model);
        m_LitTexturedShader->SetVec3("uViewPos", m_CameraPos);

        // Set lighting uniforms (example values)
        m_LitTexturedShader->SetVec3("uLightDir", glm::vec3(0.5f, -1.0f, -0.5f));
        m_LitTexturedShader->SetVec3("uLightColor", glm::vec3(1.0f, 1.0f, 1.0f));

        // Bind Texture (if loaded) to texture unit 0
        if (m_DiffuseTexture) {
            m_DiffuseTexture->Bind(0);
            m_LitTexturedShader->SetInt("uTextureDiffuse", 0);
        }

        // --- CORRECTED MESH DRAWING ---
        m_LoadedMesh->Bind();   // Bind the mesh's VAO
        m_LoadedMesh->Draw();   // Draw using the mesh's EBO
        m_LoadedMesh->Unbind(); // Unbind the mesh's VAO
        // --- END CORRECTION ---


        // Unbind texture (optional, good practice)
        if (m_DiffuseTexture) {
            m_DiffuseTexture->Unbind();
        }

    } else {
        // Handle case where shader or mesh isn't loaded
        if (!m_LitTexturedShader) std::cerr << "WARN::RENDER::Shader not loaded!" << std::endl;
        if (!m_LoadedMesh) std::cerr << "WARN::RENDER::Mesh not loaded!" << std::endl;
    }


    // Render UI
    RenderUI();

    // End ImGui Frame & Render
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    // Present final frame
    m_Renderer->Present(m_Window);
}

void Application::RenderUI() {
    // ... (RenderUI logic remains the same) ...
     if (m_CurrentState == GameState::Paused) {
        ImGui::SetNextWindowPos(ImVec2(ImGui::GetIO().DisplaySize.x * 0.5f, ImGui::GetIO().DisplaySize.y * 0.5f), ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
        ImGui::Begin("Pause Menu", nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove);
        if (ImGui::Button("Resume", ImVec2(120, 0))) { // Wider button maybe
            m_CurrentState = GameState::Playing;
            SDL_SetRelativeMouseMode(SDL_TRUE);
            m_FirstMouse = true;
            if (m_MusicChannel != -1) Mix_Resume(m_MusicChannel); // <-- RESUME MUSIC
        }
        if (ImGui::Button("Help", ImVec2(120, 0))) {
            m_CurrentState = GameState::ShowingHelp;
        }
    
        // --- Add Pause/Resume Music Button ---
        if (m_MixerInitialized && m_SoundLoaded) { // Only show if sound system is ready
            bool isMusicPaused = (m_MusicChannel != -1) && Mix_Paused(m_MusicChannel);
            // Check if actually playing OR paused (channel might be -1 if stopped)
            bool isMusicActive = (m_MusicChannel != -1) && (Mix_Playing(m_MusicChannel) || isMusicPaused);
    
            if (isMusicActive) { // Only show pause/resume if channel is valid
                 if (ImGui::Button(isMusicPaused ? "Resume Music" : "Pause Music", ImVec2(120, 0))) {
                     if (isMusicPaused) {
                         Mix_Resume(m_MusicChannel);
                     } else {
                         Mix_Pause(m_MusicChannel);
                     }
                 }
             } else {
                 // Optional: Show a disabled button or nothing if music isn't active
                 // ImGui::BeginDisabled(); ImGui::Button("Music Paused", ImVec2(120, 0)); ImGui::EndDisabled();
             }
        }
        // --- End Pause/Resume Music Button ---
    
         if (ImGui::Button("Restart Song", ImVec2(120, 0))) {
            RestartAudio();
        }
        if (ImGui::Button("Quit", ImVec2(120, 0))) {
            m_IsRunning = false;
        }
        ImGui::End();
    } else if (m_CurrentState == GameState::ShowingHelp) {
        ImGui::SetNextWindowPos(ImVec2(ImGui::GetIO().DisplaySize.x * 0.5f, ImGui::GetIO().DisplaySize.y * 0.5f), ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
        ImGui::Begin("Help", nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove);
        ImGui::Text("Controls:"); ImGui::Separator();
        ImGui::Text("WASD    : Move Camera"); ImGui::Text("Mouse   : Look Around");
        ImGui::Text("L Shift : Move Faster"); ImGui::Text("Escape  : Pause / Resume");
        ImGui::Separator();
        if (ImGui::Button("Back", ImVec2(100, 0))) { m_CurrentState = GameState::Paused; }
        ImGui::End();
    }
}
void Application::Shutdown() {
    // ... (Shutdown logic with idempotency checks remains the same) ...
     // Check if already shut down partially or fully
    if (!m_IsRunning && !m_Window && !m_Renderer) { return; } // Avoid redundant shutdowns if possible
    std::cout << "INFO::APP::Shutting down..." << std::endl;

    // Shutdown ImGui
    if (ImGui::GetCurrentContext() != nullptr) {
        ImGuiIO& io = ImGui::GetIO();
        if (io.BackendPlatformUserData != NULL) { // Check if SDL backend seems initialized
             ImGui_ImplOpenGL3_Shutdown();
             ImGui_ImplSDL2_Shutdown();
             std::cout << "INFO::APP::Dear ImGui shutdown." << std::endl;
        } else { std::cout << "INFO::APP::Dear ImGui backend already shut down." << std::endl; }
        ImGui::DestroyContext();
    } else { std::cout << "INFO::APP::Dear ImGui context already destroyed." << std::endl; }

    SDL_SetRelativeMouseMode(SDL_FALSE);
    CloseAudio();

    // Reset resources (safe to reset null pointers)
    m_LoadedMesh.reset();
    m_DiffuseTexture.reset();
    m_LitTexturedShader.reset(); // Renamed from m_SimpleShader

    if (m_Renderer) { m_Renderer->Shutdown(); m_Renderer.reset(); }
    if (m_Window) { SDL_DestroyWindow(m_Window); m_Window = nullptr; std::cout << "INFO::APP::Window destroyed." << std::endl; }
    SDL_Quit();
    std::cout << "INFO::APP::SDL quit called." << std::endl;
    m_IsRunning = false;
}


// --- New Function to Restart Audio ---
// src/Application.cpp -> RestartAudio()
void Application::RestartAudio() {
    if (m_SoundLoaded && m_TestSound && m_MixerInitialized) {
        std::cout << "INFO:APP: Attempting to restart audio." << std::endl;
        if (m_MusicChannel != -1) { // Check if we have a valid channel playing/paused
            Mix_HaltChannel(m_MusicChannel); // Stop playback on that channel
            std::cout << "DEBUG::APP: Halted channel " << m_MusicChannel << std::endl;
            m_MusicChannel = -1; // Reset our tracker
        }
        // Now play it again (PlaySound will find a new channel and update m_MusicChannel)
        PlaySound();
    } else {
         std::cout << "INFO:APP: Audio not loaded or mixer not initialized, cannot restart." << std::endl;
    }
}
// --- End New Audio Function ---


// --- LoadAudio, CloseAudio, PlaySound remain mostly the same ---
// ... (Ensure these are using m_MixerInitialized flag correctly) ...
// src/Application.cpp -> LoadAudio()
bool Application::LoadAudio() {
    if (!m_MixerInitialized) {
         if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
             std::cerr << "ERROR::APP::SDL_mixer could not initialize! Error: " << Mix_GetError() << std::endl;
             return false;
         }
         m_MixerInitialized = true;
         std::cout << "INFO::APP::SDL_mixer initialized." << std::endl;
    }

    std::string soundFilename = "test_sound.wav"; // <-- Ensure this matches your file
    // Construct the full path directly using GetResourcePath
    std::string soundFilePath = FileUtils::GetResourcePath("assets/" + soundFilename);
    if (soundFilePath.empty()) {
        std::cerr << "ERROR::APP::Could not get resource path for audio: " << soundFilename << std::endl;
        // Don't quit mixer here, allow running without sound
        return false;
    }
    std::cout << "DEBUG::APP::Attempting to load audio from: [" << soundFilePath << "]" << std::endl;

    // *** ADD FILE CHECK ***
    std::ifstream audioFileStream(soundFilePath, std::ios::binary); // Use binary mode for safety
    if (!audioFileStream.is_open()) {
        std::cerr << "ERROR::APP::std::ifstream failed to open audio file: " << soundFilePath << std::endl;
        // perror(soundFilePath.c_str()); // Optional: OS-level error detail
        return false; // Cannot load if file doesn't open
    } else {
         std::cout << "DEBUG::APP::std::ifstream opened audio file OK." << std::endl;
         audioFileStream.close();
    }
    // *** END FILE CHECK ***


    m_TestSound = Mix_LoadWAV(soundFilePath.c_str()); // Try loading again
    if (!m_TestSound) {
        // Use Mix_GetError() which is specific to SDL_mixer functions
        std::cerr << "ERROR::APP::Mix_LoadWAV failed: " << Mix_GetError() << std::endl;
        m_SoundLoaded = false;
        return false;
    }

    std::cout << "INFO::APP::Sound loaded successfully: " << soundFilename << std::endl;
    m_SoundLoaded = true;
    return true;
}
// src/Application.cpp -> CloseAudio()
void Application::CloseAudio() {
    if (m_SoundLoaded && m_TestSound) { Mix_FreeChunk(m_TestSound); m_TestSound = NULL; std::cout << "INFO::APP::Sound chunk freed." << std::endl; }
    if (m_MixerInitialized) { Mix_Quit(); m_MixerInitialized = false; m_SoundLoaded = false; std::cout << "INFO::APP::SDL_mixer quit." << std::endl; }
    m_MusicChannel = -1; // <-- ENSURE THIS IS RESET
}
// src/Application.cpp -> PlaySound()
void Application::PlaySound() {
    if (m_SoundLoaded && m_TestSound && m_MixerInitialized) {
        // Play on first available channel, loops 0 times (play once)
        m_MusicChannel = Mix_PlayChannel(-1, m_TestSound, 0); // <-- Store channel ID
        if (m_MusicChannel == -1) { // Check if playback failed
             std::cerr << "ERROR::APP::Mix_PlayChannel failed: " << Mix_GetError() << std::endl;
        } else {
             std::cout << "INFO::APP::Played sound once on channel " << m_MusicChannel << std::endl;
        }
    }
    // ... existing error messages for not initialized/loaded ...
    else if (!m_MixerInitialized) { std::cerr << "ERROR::APP::Cannot play sound, mixer not initialized." << std::endl;}
    else if (!m_SoundLoaded) { std::cerr << "ERROR::APP::Cannot play sound, not loaded." << std::endl;}
}