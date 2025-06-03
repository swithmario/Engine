#ifndef VERTEX_STRUCT_H
#define VERTEX_STRUCT_H
#include <cstddef>
#include <cstring>
#include <functional>


struct Vertex { float Position[3]; float Normal[3]; float TexCoords[2]; };

// Define operator== for Vertex
inline bool operator==(const Vertex& lhs, const Vertex& rhs) {
    return memcmp(&lhs, &rhs, sizeof(Vertex)) == 0;
}

// Define hash specialization for Vertex
namespace std {
    template<> struct hash<Vertex> {
        size_t operator()(Vertex const& vertex) const noexcept {
            size_t seed = 0;
            auto hash_combine = [&seed](float val) {
                seed ^= std::hash<float>{}(val) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
            };
            hash_combine(vertex.Position[0]); hash_combine(vertex.Position[1]); hash_combine(vertex.Position[2]);
            hash_combine(vertex.Normal[0]);   hash_combine(vertex.Normal[1]);   hash_combine(vertex.Normal[2]);
            hash_combine(vertex.TexCoords[0]);hash_combine(vertex.TexCoords[1]);
            return seed;
        }
    };
} // namespace std

#endif // VERTEX_STRUCT_H