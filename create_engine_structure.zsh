#!/bin/zsh

echo "Creating Engine directory structure..."
echo ""

# --- Root Engine directory ---
ENGINE_DIR_NAME="Engine" # <--- CHANGED
mkdir -p "$ENGINE_DIR_NAME"
cd "$ENGINE_DIR_NAME" || exit
echo "Created and changed to $ENGINE_DIR_NAME/"
echo ""

# --- Core Source and Include Directories ---
echo "Creating include/ and src/ structures..."
mkdir -p include/core
mkdir -p include/audio

mkdir -p src/core
mkdir -p src/audio
echo "  Done."
echo ""

# --- Assets Directory ---
echo "Creating assets/ structure..."
mkdir -p assets/models
mkdir -p assets/textures
mkdir -p assets/shaders
mkdir -p assets/audio_probes  # For MVP stubbed audio data files, if not embedded
echo "  Done."
echo ""

# --- Top-Level Shaders Directory ---
# (If distinct from assets/shaders for source code)
echo "Creating top-level shaders/ directory (for shader source if not in assets)..."
mkdir -p shaders
echo "  Done."
echo ""


# --- External/Vendor Libraries Directory ---
echo "Creating external/ directory..."
mkdir -p external
# Subdirectories for common libraries like ImGui, GLM can be added manually
# or by Git submodules later. For example:
# mkdir -p external/imgui
# mkdir -p external/glm
echo "  Done."
echo ""

# --- Placeholder for root files (user will add these or generate them) ---
echo "Reminder: Add CMakeLists.txt, README.md, .gitignore to the root."
echo ""

echo "Engine directory structure created successfully!"
echo "You can find it in: $(pwd)"
echo "Next steps: Consider 'git init', copy your existing engine files, and adapt your CMakeLists.txt."

# Go back to the original directory (optional, if script is run from outside)
# cd ..