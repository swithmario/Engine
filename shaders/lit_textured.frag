#version 330 core
out vec4 FragColor;

in vec3 FragPos;   // Interpolated fragment position in World Space
in vec3 Normal;    // Interpolated normal in World Space
in vec2 TexCoords; // Interpolated texture coordinates

uniform sampler2D uTextureDiffuse; // The texture sampler

uniform vec3 uLightDir;    // Light direction (in World Space, pointing FROM light)
uniform vec3 uLightColor;  // Light color
uniform vec3 uViewPos;     // Camera position (World Space) - for specular later

void main()
{
    // --- Ambient ---
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * uLightColor;

    // --- Diffuse ---
    vec3 norm = normalize(Normal); // Ensure normal is unit length
    vec3 lightDir = normalize(-uLightDir); // Normalize light direction (ensure pointing TO light)
    float diff = max(dot(norm, lightDir), 0.0); // Calculate diffuse intensity (clamp negative)
    vec3 diffuse = diff * uLightColor;

    // --- Specular (Basic Phong) --- Optional for now
    // float specularStrength = 0.5;
    // vec3 viewDir = normalize(uViewPos - FragPos);
    // vec3 reflectDir = reflect(-lightDir, norm);
    // float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32); // Shininess factor = 32
    // vec3 specular = specularStrength * spec * uLightColor;

    // --- Combine ---
    // vec3 lighting = ambient + diffuse + specular; // If using specular
    vec3 lighting = ambient + diffuse;
    vec3 objectColor = texture(uTextureDiffuse, TexCoords).rgb; // Get color from texture

    FragColor = vec4(lighting * objectColor, 1.0); // Combine lighting and texture color
}