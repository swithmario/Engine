#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out vec3 FragPos;    // Output vertex position in World Space
out vec3 Normal;     // Output normal in World Space
out vec2 TexCoords;  // Pass through texture coordinates

uniform mat4 uModel; // Model matrix (transforms to world space)
uniform mat4 uMVP;   // Combined Model-View-Projection matrix

void main()
{
    gl_Position = uMVP * vec4(aPos, 1.0); // Calculate final clip space position

    // Calculate world space position for lighting calculation
    FragPos = vec3(uModel * vec4(aPos, 1.0));

    // Transform normal to world space (assuming no non-uniform scaling for now)
    // Using the normal matrix (inverse transpose of model's upper 3x3) is more robust
    mat3 normalMatrix = transpose(inverse(mat3(uModel)));
    Normal = normalize(normalMatrix * aNormal);
    // Simplified (less robust) if only rotation/uniform scale: Normal = normalize(mat3(uModel) * aNormal);

    TexCoords = aTexCoords; // Pass through UVs
}