# --- START OF FILE CMakeLists.txt ---

cmake_minimum_required(VERSION 3.16)
project(MyEngineProject VERSION 1.0 LANGUAGES CXX C) # Keep C for glad

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 17)
set_property(GLOBAL PROPERTY USE_FOLDERS ON) # Optional: Organizes files in IDEs

# --- Include Directories (Globally accessible for find_package etc.) ---
# Note: target_include_directories is generally preferred over include_directories()
# We will set includes per-target below. You might keep vendor here if find_package needs it.
include_directories(
    include
    vendor
    vendor/libs # For stb_image, tiny_obj_loader headers
)

# --- Find Packages ---
find_package(SDL2 REQUIRED)
find_package(SDL2_mixer REQUIRED) # Keep find_package, but link explicitly below
find_package(OpenGL REQUIRED)
find_package(glm REQUIRED)

# --- Add ImGui Library Target ---
# Build ImGui sources into a static library
add_library(imgui_lib STATIC
    vendor/imgui/imgui.cpp
    vendor/imgui/imgui_draw.cpp
    vendor/imgui/imgui_tables.cpp
    vendor/imgui/imgui_widgets.cpp
    vendor/imgui/backends/imgui_impl_sdl2.cpp
    vendor/imgui/backends/imgui_impl_opengl3.cpp
)
# Set include directories needed TO BUILD imgui_lib and needed BY OTHERS linking imgui_lib
target_include_directories(imgui_lib PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/vendor/imgui"          # ImGui Core
    "${CMAKE_CURRENT_SOURCE_DIR}/vendor/imgui/backends" # ImGui Backends
    ${SDL2_INCLUDE_DIRS}                               # Needed by imgui_impl_sdl2
)
# Set compile definitions needed TO BUILD imgui_lib and needed BY OTHERS linking imgui_lib
target_compile_definitions(imgui_lib PUBLIC
    IMGUI_IMPL_OPENGL_LOADER_GLAD                     # Tell backend to use glad
)
# Group ImGui sources in IDE (Optional)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/vendor/imgui FILES
    vendor/imgui/imgui.cpp
    vendor/imgui/imgui_draw.cpp
    vendor/imgui/imgui_tables.cpp
    vendor/imgui/imgui_widgets.cpp
    vendor/imgui/backends/imgui_impl_sdl2.cpp
    vendor/imgui/backends/imgui_impl_opengl3.cpp
)
# --- End ImGui Library Target ---


# --- Add Executable and Sources ---
add_executable(MyEngineApp
    src/main.cpp
    src/Application.cpp
    src/Renderer.cpp
    src/Shader.cpp
    # src/VertexArray.cpp # Make sure this is removed if not used
    src/Mesh.cpp
    src/Texture.cpp
    src/FileUtils.cpp
    src/glad.c
    # Note: ImGui sources are NOT listed here anymore
)
# Group project sources in IDE (Optional)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/src FILES
    src/main.cpp src/Application.cpp src/Renderer.cpp src/Shader.cpp
    src/Mesh.cpp src/Texture.cpp src/FileUtils.cpp src/glad.c
)

# ----> SET BUNDLE PROPERTY <----
if(APPLE)
    set_target_properties(MyEngineApp PROPERTIES MACOSX_BUNDLE True)
endif()

# --- Handle macOS Bundle Resources ---
# (Using the corrected block from before)
if(APPLE)
    set(RESOURCE_DESTINATION "$<TARGET_BUNDLE_CONTENT_DIR:MyEngineApp>/Resources")
    set(ASSET_SRC_DIR "${CMAKE_SOURCE_DIR}/assets")
    set(SHADER_SRC_DIR "${CMAKE_SOURCE_DIR}/shaders")
    set(ASSET_DEST_DIR "${RESOURCE_DESTINATION}/assets")
    set(SHADER_DEST_DIR "${RESOURCE_DESTINATION}/shaders")

    if(EXISTS "${ASSET_SRC_DIR}")
        add_custom_command(TARGET MyEngineApp POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E make_directory "${ASSET_DEST_DIR}"
            COMMAND ${CMAKE_COMMAND} -E copy_directory "${ASSET_SRC_DIR}" "${ASSET_DEST_DIR}"
            COMMENT "Copying assets to bundle" VERBATIM)
        message(STATUS "Added command to copy assets directory.")
    else()
        message(WARNING "Source assets directory not found: ${ASSET_SRC_DIR}. Skipping asset copy.")
    endif()

    if(EXISTS "${SHADER_SRC_DIR}")
         add_custom_command(TARGET MyEngineApp POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E make_directory "${SHADER_DEST_DIR}"
            COMMAND ${CMAKE_COMMAND} -E copy_directory "${SHADER_SRC_DIR}" "${SHADER_DEST_DIR}"
            COMMENT "Copying shaders to bundle" VERBATIM)
        message(STATUS "Added command to copy shaders directory.")
    else()
         message(WARNING "Source shaders directory not found: ${SHADER_SRC_DIR}. Skipping shader copy.")
    endif()
else()
     message(STATUS "Not on Apple, skipping bundle resource copy commands.")
endif()
# --- END OF Resource Copying ---


# --- Target Include Directories (for MyEngineApp) ---
target_include_directories(MyEngineApp PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/include"     # Project includes
    "${CMAKE_CURRENT_SOURCE_DIR}/vendor/libs"  # For stb_image, tiny_obj_loader
    # No need for vendor/imgui paths here, inherited from imgui_lib via PUBLIC
    ${SDL2_INCLUDE_DIRS}                     # Inherited via SDL2::SDL2 target usually
    "/opt/homebrew/opt/sdl2_mixer/include"   # Mixer explicit include (if find_package didn't work well)
    ${GLM_INCLUDE_DIRS}                      # Inherited via glm::glm target usually
)

# --- Target Compile Definitions (for MyEngineApp) ---
# No ImGui definitions needed here directly, inherited from imgui_lib
# target_compile_definitions(MyEngineApp PRIVATE
#    # TINYOBJLOADER_USE_MAPBOX_EARCUT # Optional
# )


# --- Target Link Libraries (for MyEngineApp) ---
target_link_libraries(MyEngineApp PRIVATE
    imgui_lib       # <-- Link the ImGui static library we built
    SDL2::SDL2
    "/opt/homebrew/opt/sdl2_mixer/lib/libSDL2_mixer.dylib" # Explicit mixer link
    glm::glm
    ${OPENGL_LIBRARIES} # From find_package(OpenGL)
)

# Add macOS Frameworks (for MyEngineApp)
if(APPLE)
    target_link_libraries(MyEngineApp PRIVATE
        "-framework Cocoa"
        "-framework IOKit"
        "-framework CoreVideo"
        "-framework CoreAudio"
        "-framework AudioToolbox"
        "-framework Metal"
        "-framework OpenGL" # Link OpenGL framework
    )
endif()

# --- END OF FILE CMakeLists.txt ---