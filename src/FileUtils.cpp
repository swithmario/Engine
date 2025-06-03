#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

#include "FileUtils.h"
#include "VertexArray.h" // For Vertex struct
#include <unordered_map>
#include <SDL2/SDL.h> // For SDL_GetBasePath, SDL_free, SDL_GetError
#include <iostream>
#include <glm/gtc/type_ptr.hpp>
#include <fstream>
#include <sstream>
#include <filesystem> // For path joining

namespace FileUtils {

    // Internal static variable for base path caching
    static std::string sBasePathCache = "";

    void DetermineBaseResourcePathIfNeeded() {
        if (!sBasePathCache.empty()) return; // Already determined

        char* sdlBasePath = SDL_GetBasePath();
        if (sdlBasePath) {
            sBasePathCache = std::string(sdlBasePath);
            SDL_free(sdlBasePath);
            #ifdef __APPLE__
                // Basic bundle path adjustment
                size_t appPos = sBasePathCache.rfind(".app/");
                if (appPos != std::string::npos) {
                     sBasePathCache.resize(appPos + 5); // Path up to .app/
                     sBasePathCache += "Contents/Resources/";
                } else {
                     // Assume running relative to build dir? Risky.
                     sBasePathCache += "../Resources/"; // Adjust if needed
                }
            #else
                sBasePathCache += "Resources/"; // Adjust if needed
            #endif
            std::cout << "INFO::FILEUTILS::Base resource path determined: " << sBasePathCache << std::endl;
        } else {
            std::cerr << "ERROR::FILEUTILS::Failed to get SDL base path: " << SDL_GetError() << std::endl;
            sBasePathCache = "../Resources/"; // Fallback
            std::cerr << "WARN::FILEUTILS::Falling back to relative resource path: " << sBasePathCache << std::endl;
        }
    }

    std::string GetResourcePath(const std::string& relativePath) {
        DetermineBaseResourcePathIfNeeded(); // Ensure cache is populated
        if (sBasePathCache.empty()) {
            std::cerr << "ERROR::FILEUTILS::Base resource path not determined." << std::endl;
            return "";
        }
        try {
            std::filesystem::path combinedPath = std::filesystem::path(sBasePathCache) / std::filesystem::path(relativePath);
            // combinedPath = std::filesystem::weakly_canonical(combinedPath); // Optional normalization
            return combinedPath.string();
        } catch (const std::exception& e) {
            std::cerr << "ERROR::FILEUTILS::Filesystem path error for '" << relativePath << "': " << e.what() << std::endl;
            return "";
        }
    }

    std::string ReadFileToString(const std::string& filePath) { // <-- Ensure this name
        std::ifstream fileStream(filePath);
        if (!fileStream.is_open()) {
            std::cerr << "ERROR::FILEUTILS::FILE_NOT_SUCCESFULLY_READ: " << filePath << std::endl;
            return "";
        }
        std::stringstream buffer; buffer << fileStream.rdbuf(); fileStream.close();
        return buffer.str();
    }

    bool LoadObjModel(const std::string& filePath, std::vector<Vertex>& outVertices, std::vector<unsigned int>& outIndices) {
        tinyobj::attrib_t attrib; std::vector<tinyobj::shape_t> shapes; std::vector<tinyobj::material_t> materials;
        std::string warn, err; std::cout << "INFO::MODEL::Loading OBJ file: " << filePath << std::endl;
        std::filesystem::path pathObj(filePath); std::string mtlBaseDir = pathObj.parent_path().string() + "/";
        bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, filePath.c_str(), mtlBaseDir.c_str());
        if (!warn.empty()) { std::cout << "WARN::MODEL::TinyObjLoader: " << warn << std::endl; }
        if (!err.empty()) { std::cerr << "ERROR::MODEL::TinyObjLoader: " << err << std::endl; }
        if (!ret) { std::cerr << "ERROR::MODEL::Failed to load OBJ file: " << filePath << std::endl; return false; }

        outVertices.clear(); outIndices.clear();
        std::unordered_map<Vertex, uint32_t> uniqueVertices{};
        for (const auto& shape : shapes) {
            for (const auto& index : shape.mesh.indices) {
                Vertex vertex{};
                vertex.Position[0] = attrib.vertices[3 * index.vertex_index + 0]; vertex.Position[1] = attrib.vertices[3 * index.vertex_index + 1]; vertex.Position[2] = attrib.vertices[3 * index.vertex_index + 2];
                if (index.normal_index >= 0) { vertex.Normal[0] = attrib.normals[3 * index.normal_index + 0]; vertex.Normal[1] = attrib.normals[3 * index.normal_index + 1]; vertex.Normal[2] = attrib.normals[3 * index.normal_index + 2]; } else { vertex.Normal[0]=0; vertex.Normal[1]=0; vertex.Normal[2]=0; }
                if (index.texcoord_index >= 0) { vertex.TexCoords[0] = attrib.texcoords[2 * index.texcoord_index + 0]; vertex.TexCoords[1] = 1.0f - attrib.texcoords[2 * index.texcoord_index + 1]; } else { vertex.TexCoords[0]=0; vertex.TexCoords[1]=0; }
                // REMOVED: vertex.Color = {1.0f, 1.0f, 1.0f};
                if (uniqueVertices.count(vertex) == 0) { uniqueVertices[vertex] = static_cast<uint32_t>(outVertices.size()); outVertices.push_back(vertex); }
                outIndices.push_back(uniqueVertices[vertex]);
            }
        }
        std::cout << "INFO::MODEL::Loaded " << outVertices.size() << " vertices, " << outIndices.size() << " indices." << std::endl;
        return true;
    }

} // namespace FileUtils