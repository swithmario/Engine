#ifndef FILEUTILS_H
#define FILEUTILS_H
#include <string>
#include <vector>
#include "VertexArray.h" // <-- Include for Vertex struct

namespace FileUtils { // Use namespace instead of static class
    // No need for static keyword here
    void DetermineBaseResourcePath();
    std::string GetResourcePath(const std::string& relativePath);
    std::string ReadFileToString(const std::string& filePath); // <-- Ensure this name
    // Make LoadObjModel part of the namespace too
    bool LoadObjModel(const std::string& filePath,
                      std::vector<Vertex>& outVertices,
                      std::vector<unsigned int>& outIndices);

    // Declare the static member if needed *within* the namespace scope?
    // Better to handle base path internally without exposing static member.
    // Remove: static std::string sBasePath;
}
#endif // FILEUTILS_H